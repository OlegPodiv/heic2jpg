/**
*
* Copyright (C) 2002-2018成都易我科技有限公司
* All rights reserved.
*
* @file   EuFFMepgRequisites.h
* @author qiaozhousan
* @date   2018-11-9
*
* @brief  
*
*/

#ifndef _EuRequisites_H__
#define _EuRequisites_H__

#include <string>
using namespace std;

#ifdef _WIN32  
//Windows  
extern "C"  
{  
#ifdef _WIN64
#include "./include/x64/libavutil/imgutils.h"
#include "./include/x64/libavcodec/avcodec.h"
#include "./include/x64/libswscale/swscale.h"
#include "./include/x64/libavformat/avformat.h"
#include "./include/x64/libavutil/avutil.h"
#include "./include/x64/libavutil/opt.h"
#else
#include "./include/x86/libavutil/imgutils.h"
#include "./include/x86/libavcodec/avcodec.h"
#include "./include/x86/libswscale/swscale.h"
#include "./include/x86/libavformat/avformat.h"
#include "./include/x86/libavutil/avutil.h"
#include "./include/x86/libavutil/opt.h"
#endif
};  
#else  
//Linux...\Mac...
#ifdef __cplusplus  
extern "C"  
{  
#endif  
#include <libavcodec/avcodec.h> 
#include <libavformat/avformat.h> 
#include <libswscale/swscale.h>  
#include <libavutil/imgutils.h> 
#include <libavutil/avutil.h> 
#include <libavutil/opt.h> 
#ifdef __cplusplus  
};  
#endif  
#endif 

//import static_lib
#ifdef _DEBUG
#pragma comment(lib,"opencv_core2413d.lib")
#pragma comment(lib,"opencv_highgui2413d.lib")
#pragma comment(lib,"libjpegd.lib")
#pragma comment(lib,"libpngd.lib")
#pragma comment(lib,"libtiffd.lib")
#pragma comment(lib,"libjasperd.lib")
#pragma comment(lib,"IlmImfd.lib")
#pragma comment(lib,"zlibd.lib")
#else											
#pragma comment(lib,"opencv_core2413.lib")
#pragma comment(lib,"opencv_highgui2413.lib")
#pragma comment(lib,"libjpeg.lib")
#pragma comment(lib,"libpng.lib")
#pragma comment(lib,"libtiff.lib")
#pragma comment(lib,"libjasper.lib")
#pragma comment(lib,"IlmImf.lib")
#pragma comment(lib,"zlib.lib")
#endif

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"../Output/x64/Debug/common.lib")
#pragma comment(lib,"../Output/x64/Debug/heif_static.lib")
#else
#pragma comment(lib,"../Output/x64/Release/common.lib")
#pragma comment(lib,"../Output/x64/Release/heif_static.lib")
#endif
#pragma comment(lib,"./lib/x64/avcodec.lib")
#pragma comment(lib,"./lib/x64/avutil.lib")
#pragma comment(lib,"./lib/x64/swscale.lib")
#pragma comment(lib,"./lib/x64/avformat.lib")
#else
#ifdef _DEBUG
#pragma comment(lib,"../Output/Win32/Debug/common.lib")
#pragma comment(lib,"../Output/Win32/Debug/heif_static.lib")
#else
#pragma comment(lib,"../Output/Win32/Release/common.lib")
#pragma comment(lib,"../Output/Win32/Release/heif_static.lib")
#endif
#pragma comment(lib,"./lib/x86/avcodec.lib")
#pragma comment(lib,"./lib/x86/avutil.lib")
#pragma comment(lib,"./lib/x86/swscale.lib")
#pragma comment(lib,"./lib/x86/avformat.lib")
#endif

#endif


