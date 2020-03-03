#pragma once

#include <vector>
#include "Heic2jpegErrorMacroDef.h"
#include "EuRequisites.h"
#include "../api/reader/heifreader.h"
#include <opencv2/opencv.hpp> 
#include "Sync.h"

class CHeic2jpeg
{
public:
	CHeic2jpeg(void);
	~CHeic2jpeg(void);

public:
	ITL_ERT heif2jpeg(const wchar_t *wzSrc, const wchar_t *wzDst);
	ITL_ERT heif2thumbnail(const wchar_t *wzSrc, const wchar_t *wzDst);

private:
	ITL_ERT DecodeImageId2File(DWORD imgId, const char* outFilePath);
	int SetHeifFilePath(const char* heifPath);
	ITL_ERT ConvertGridItem2File(void* gridItem, const char* outFilePath);

private:
	HEIF::Reader*	m_heifReader;
	BYTE*				m_chunkData;
	CEuLock m_Datalock;
};

