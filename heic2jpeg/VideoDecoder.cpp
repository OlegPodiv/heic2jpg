//
//Created by TerryWang on 11/21/17
//Copyright (c) 2017 EaseUS.All rights reserved.
//
#include "stdafx.h"
#include <Windows.h>
#include <list>
#include "VideoDecoder.h"
#include "EuRequisites.h"
#include "Heic2jpegMacroDef.h"

VideoDecoder::VideoDecoder(void)
:m_videoStream(NULL),m_videoCodecContext(NULL),m_videoCodec(NULL),m_swsContext(NULL), m_rgbFrame(NULL), m_streamIndex(-1)
,m_rgbDataBuf(NULL),m_wndWidth(0),m_wndHeight(0),m_videoWidth(0),m_videoHeight(0),m_frameRate(30)
,m_frameDelay(AV_TIME_BASE/30),m_srcPixelFormat(AV_PIX_FMT_YUV420P),m_videoCacheCount(0),m_startTs(.0f)
{	
	InitFfmpeg();

	memset(&m_bitmapInfo,0,sizeof(BITMAPINFOHEADER));
	m_bitmapInfo.biBitCount		= 24;
	m_bitmapInfo.biPlanes		= 1;
	m_bitmapInfo.biCompression	= BI_RGB;
	m_bitmapInfo.biSize			= sizeof(BITMAPINFOHEADER);
	m_rgbFrame					= av_frame_alloc();
}

VideoDecoder::~VideoDecoder(void)
{
	StopDecode();
	if(m_rgbFrame)
	{
		av_frame_free(&m_rgbFrame);
	}
	m_rgbFrame = NULL;	
}

void VideoDecoder::StopDecode()
{
	FlushCodec();

	if (m_swsContext)
		sws_freeContext(m_swsContext);
	m_swsContext = NULL;

	if (m_videoCodecContext)
		avcodec_free_context(&m_videoCodecContext);
	m_videoCodecContext = NULL;

	ARRYDELETE(m_rgbDataBuf);
	m_videoStream		= NULL;
	m_startTs			= .0f;
	m_streamIndex		= -1;
	m_videoWidth		= 0;
}

int VideoDecoder::SetDecodeId(int codecId)
{
	StopDecode();

	m_videoCodec = avcodec_find_decoder((AVCodecID)codecId);
	if (NULL == m_videoCodec) return -1;

	m_videoCodecContext = avcodec_alloc_context3(NULL);
	if (NULL == m_videoCodecContext) return -2;

	return avcodec_open2(m_videoCodecContext, m_videoCodec, NULL) < 0 ? -3 :0;
}

double VideoDecoder::OnGotVideoStream(AVStream* avStream)
{	
	if (NULL == avStream || m_videoStream) return 0.0f;

	StopDecode();

	m_videoStream		= avStream;
	m_streamIndex		= m_videoStream->index;
	m_videoCodecContext = avcodec_alloc_context3(NULL);
	if (NULL == m_videoCodecContext ) return 0.0f;
	int result	= avcodec_parameters_to_context(m_videoCodecContext,avStream->codecpar);
	if (result < 0) return 0.0f;

	m_videoCodec	= avcodec_find_decoder(m_videoCodecContext->codec_id);
	if (NULL == m_videoCodec) return 0.0f;

	av_codec_set_pkt_timebase(m_videoCodecContext,m_videoStream->time_base);
	result			= avcodec_open2(m_videoCodecContext,m_videoCodec,NULL);
	if (result < 0) return 0.0f;

	AVCodecParameters* codecParameter = m_videoStream->codecpar;
	if (NULL == codecParameter) return 0.0f;
	m_wndWidth		= m_videoWidth	= codecParameter->width;
	m_wndHeight		= m_videoHeight	= codecParameter->height;
	if (codecParameter->format != AV_PIX_FMT_NONE)
		m_srcPixelFormat = codecParameter->format;
	CreateSwsContent(m_wndWidth,m_wndHeight);
	if (m_videoStream->avg_frame_rate.den <= 0) return 0.0f;

	m_frameRate		= (int)av_q2d(m_videoStream->r_frame_rate);
	m_frameDelay	= AV_TIME_BASE/av_q2d(m_videoStream->r_frame_rate);
#define AV_NOPTS_VALUE1 0x8000000000000000
	if (AV_NOPTS_VALUE1 != m_videoStream->start_time)
		m_startTs	= m_videoStream->start_time*av_q2d(m_videoStream->time_base);
	return m_videoStream->duration*av_q2d(m_videoStream->time_base);
}

int VideoDecoder::CreateSwsContent(int wndWidth,int wndHeight)
{
	wndWidth					= wndWidth&(~(0x10-1));
	m_bitmapInfo.biWidth		= m_wndWidth	= wndWidth;
	m_bitmapInfo.biHeight		= m_wndHeight	= wndHeight;
	m_bitmapInfo.biSizeImage	= m_wndWidth*m_wndHeight*3;

	if (m_swsContext)
		sws_freeContext(m_swsContext);
	
	int swsFlags	= m_videoWidth < m_wndWidth ? SWS_GAUSS : SWS_BICUBIC; 
	m_swsContext	= sws_getContext(m_videoWidth,m_videoHeight,(AVPixelFormat)m_srcPixelFormat
						,m_wndWidth,m_wndHeight,AV_PIX_FMT_BGR24,swsFlags,NULL,NULL,NULL);

	ARRYDELETE(m_rgbDataBuf);
	m_rgbDataBuf	= new BYTE[m_wndWidth*m_wndHeight*4];
	if (NULL == m_rgbDataBuf) return -1;
	memset(m_rgbDataBuf, 0, m_wndWidth*m_wndHeight*4);
	av_image_fill_arrays(m_rgbFrame->data,m_rgbFrame->linesize,m_rgbDataBuf
						,AV_PIX_FMT_BGR24,m_wndWidth,m_wndHeight,1);
	return m_swsContext ? 0 : -1;
}

int VideoDecoder::SendVideoData(BYTE* data, int dataCount)
{
	AVPacket* packet	= av_packet_alloc();
	av_init_packet(packet); 
	packet->data = &data[0]; 
	packet->size = dataCount;
	//int result			= av_packet_from_data(packet, data, dataCount);
	int result				= SendVideoPacket(packet);
	av_packet_free(&packet);
	return result;
}

int VideoDecoder::GetRgb24Data(BYTE** data, int& dataCount)
{
	for (int i = 0; i < 0x20 && 0 == m_videoCacheCount; ++i)
		ReceiveVideoFrame();

	double  currentTs = .0f; 
	bool needCacheFrame = false;
	AVFrame* rgbFrame = GetRGBFrame(currentTs,needCacheFrame);
	if (NULL == rgbFrame) return -1;

	*data = rgbFrame->data[0];
	dataCount = m_bitmapInfo.biSizeImage;
	return 0;
}

int VideoDecoder::SendVideoPacket(AVPacket* avPacket)
{
	if (NULL == m_videoCodecContext) return -1;

	int result = avcodec_send_packet(m_videoCodecContext,avPacket);
	for(int i = 0; i < 0x20 && EAGAIN == result;++i)
		result = avcodec_send_packet(m_videoCodecContext,avPacket);
	return result;
}

int VideoDecoder::ReceiveVideoFrame()
{
	if (NULL == m_videoCodecContext) return -1;
	AVFrame* videoFrame	= av_frame_alloc();
	if (NULL == videoFrame) return -1;

	int result = avcodec_receive_frame(m_videoCodecContext,videoFrame);
	if (0 != result)
	{
		av_frame_free(&videoFrame);
	}
	else
	{
		m_videoFrameList.push_back(videoFrame);
		++ m_videoCacheCount;
	}
	return m_videoCacheCount;
}

AVFrame* VideoDecoder::GetRGBFrame(double& currentTs,bool needCacheFrame)
{
	AVFrame* videoFrame = NULL;
	if (!needCacheFrame && m_videoCacheCount)
	{
		if (m_videoFrameList.empty()) return m_rgbFrame;

		videoFrame = *m_videoFrameList.begin();
		m_videoFrameList.pop_front();
		-- m_videoCacheCount;
	}

	if (videoFrame && videoFrame->width && ( NULL == m_swsContext || videoFrame->width != m_videoWidth))
	{
		m_videoWidth	= videoFrame->width;
		m_videoHeight	= videoFrame->height;
		CreateSwsContent(m_videoWidth,m_videoHeight);
	}

	if (m_swsContext && videoFrame && videoFrame->data[0] && videoFrame->linesize[0])
	{
		int result		= sws_scale(m_swsContext,videoFrame->data,videoFrame->linesize
								,0,m_videoHeight,m_rgbFrame->data,m_rgbFrame->linesize);	
		if (m_videoStream)
		{
			currentTs = av_frame_get_best_effort_timestamp(videoFrame)* av_q2d(m_videoStream->time_base) - m_startTs;
			if (m_videoStream->avg_frame_rate.den)
			{
				m_frameDelay = AV_TIME_BASE / av_q2d(m_videoStream->r_frame_rate);
				m_frameDelay += videoFrame->repeat_pict * (m_frameDelay * 0.5);
			}
		}
		av_frame_free(&videoFrame);
	}
	return m_rgbFrame;
}

int VideoDecoder::GetStreamIndex()
{
	return m_streamIndex;
}

double VideoDecoder::GetFrameDelay()
{
	return m_frameDelay;
}

int VideoDecoder::GetFrameRate()
{
	return m_frameRate;
}

BITMAPINFOHEADER* VideoDecoder::GetBitmapInfo()
{
	return &m_bitmapInfo;
}

bool VideoDecoder::CanPullDemuxPacket()
{
	return m_videoCacheCount < m_frameRate;
}

void VideoDecoder::FlushCodec()
{
	if (m_videoCodecContext)
		avcodec_flush_buffers(m_videoCodecContext);

	while (!m_videoFrameList.empty())
	{
		AVFrame* videoFrame = *m_videoFrameList.begin();
		m_videoFrameList.pop_front();
		av_frame_free(&videoFrame);
	}
	m_videoCacheCount = 0;	
}

void VideoDecoder::InitFfmpeg()
{
	static bool g_initedFfmpeg = false;
	if (g_initedFfmpeg) return;

	g_initedFfmpeg = true;
	av_register_all();
	avcodec_register_all();
}

int VideoDecoder::WriteData2BmpFile(const char* filePath,BITMAPINFOHEADER* bi,BYTE* data)
{			
	if (NULL == filePath || NULL == data || NULL == bi ) return -1;

	BITMAPFILEHEADER   BmfHdr	= {};  
	BmfHdr.bfType				= 0x4D42;  // "BM"
	BmfHdr.bfSize				= bi->biSizeImage;
	BmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+ (DWORD)sizeof(BITMAPINFOHEADER);
	
	FILE* fp = NULL;
	 fopen_s(&fp, filePath, "wb");
	if (NULL == fp) return -2;

	size_t writeCount = fwrite((LPCVOID)&BmfHdr,1, sizeof(BITMAPFILEHEADER),fp);
	writeCount = fwrite((LPCVOID)bi,1, sizeof(BITMAPINFOHEADER),fp);
	writeCount = fwrite((LPCVOID)data,1, bi->biSizeImage,fp);
	fclose(fp);	
	fp = NULL;
	return (int)writeCount;
}

bool VideoDecoder::WriteJPEG(AVFrame* pFrame, int width, int height, const char* filename)
{
	bool bRet = true;
	do
	{
		CHECK_BREAK(NULL == pFrame || NULL == filename);
		// 分配AVFormatContext对象
		AVFormatContext* pFormatCtx = avformat_alloc_context();
		if (NULL == pFormatCtx)
		{
			bRet = false;
			break;
		}

		// 设置输出文件格式
		pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);
		// 创建并初始化一个和该url相关的AVIOContext
		if (avio_open(&pFormatCtx->pb, filename, AVIO_FLAG_READ_WRITE) < 0)
		{
			avformat_free_context(pFormatCtx);
			bRet = false;
			break;
		}

		// 构建一个新stream
		AVStream* pAVStream = avformat_new_stream(pFormatCtx, 0);
		if (NULL == pAVStream)
		{
			avio_close(pFormatCtx->pb);
			avformat_free_context(pFormatCtx);
			bRet = false;
			break;
		}

		// 设置该stream的信息
		AVCodecContext* pCodecCtx = pAVStream->codec;
		if (pFormatCtx->oformat)
		{
			pCodecCtx->codec_id = pFormatCtx->oformat->video_codec;
		}

		pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
		pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
		pCodecCtx->width = width;
		pCodecCtx->height = height;
		pCodecCtx->time_base.num = 1;
		pCodecCtx->time_base.den = 25;

		// Begin Output some information
		av_dump_format(pFormatCtx, 0, filename, 1);
		// End Output some information

		// 查找解码器
		AVCodec* pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
		if (NULL == pCodec)
		{
			if (pAVStream)
			{
				avcodec_close(pAVStream->codec);
			}
			avio_close(pFormatCtx->pb);
			avformat_free_context(pFormatCtx);
			bRet = false;
			break;
		}

		// 设置pCodecCtx的解码器为pCodec
		if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
		{
			bRet = false;
			break;
		}

		//Write Header
		avformat_write_header(pFormatCtx, NULL);
		int y_size = pCodecCtx->width * pCodecCtx->height;

		//Encode
		// 给AVPacket分配足够大的空间
		AVPacket pkt;
		av_new_packet(&pkt, y_size * 3);

		int got_picture = 0;
		int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
		if (ret < 0)
		{
			got_picture = 0;
			bRet = false;
		}

		if (got_picture == 1)
		{
			//pkt.stream_index = pAVStream->index;
			av_write_frame(pFormatCtx, &pkt);
		}

		av_free_packet(&pkt);

		//Write Trailer
		av_write_trailer(pFormatCtx);
		if (pAVStream)
		{
			avcodec_close(pAVStream->codec);
		}
		avio_close(pFormatCtx->pb);
		avformat_free_context(pFormatCtx);

	} while (0);

	return bRet;
}