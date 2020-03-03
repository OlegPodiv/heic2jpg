//
//Created by TerryWang on 11/21/17
//Copyright (c) 2017 EaseUS.All rights reserved.
//
#pragma once
struct AVCodec;
struct AVStream;
struct AVFrame;
struct AVCodecContext;
struct SwsContext;
struct AVPacket;
class VideoDecoder
{
	friend class MediaGraph;

public:
	VideoDecoder(void);
	~VideoDecoder(void);

	static int WriteData2BmpFile(const char* filePath,BITMAPINFOHEADER* bi,BYTE* data);	
	static void InitFfmpeg();

	int		SetDecodeId(int codecId);
	int     SendVideoData(BYTE* data, int dataCount);
	int		GetRgb24Data(BYTE** data, int& dataCount);
	bool    WriteJPEG(AVFrame* pFrame, int width, int height, const char* filename);

private:
	int		GetStreamIndex();
	void	StopDecode();
	bool	CanPullDemuxPacket();
	double	OnGotVideoStream(AVStream* avStream);
	double	GetFrameDelay();
	int		GetFrameRate();
	BITMAPINFOHEADER* GetBitmapInfo();
	int		SendVideoPacket(AVPacket* avPacket);
	int		ReceiveVideoFrame();
	AVFrame*  GetRGBFrame(double& currentTs, bool needCacheFrame);
	void	FlushCodec();
	int	CreateSwsContent(int wndWidth,int wndHeight);

public:
	int						m_videoWidth;
	int						m_videoHeight;

private:
	AVCodec*				    m_videoCodec;
	AVStream*				m_videoStream;
	AVCodecContext*	m_videoCodecContext;
	SwsContext*			m_swsContext;
	AVFrame*				m_rgbFrame;
	BYTE*					m_rgbDataBuf;
	int						m_wndWidth;
	int						m_wndHeight;
	int						m_streamIndex;
	int						m_srcPixelFormat;
	int						m_frameRate;
	double					m_frameDelay;
	double					m_startTs;
	BITMAPINFOHEADER		m_bitmapInfo;
	int						m_videoCacheCount;
	std::list<AVFrame*>		m_videoFrameList;
};