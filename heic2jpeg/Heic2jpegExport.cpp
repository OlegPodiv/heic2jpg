#include "stdafx.h"
#include "Heic2jpegExport.h"
#include "Heic2jpeg.h"

#define EU_EXPORT extern

EU_EXPORT ITL_ERT EuHeic2jpeg(const wchar_t *wzSrcFilePath, const wchar_t *wzDstJpgPath)
{
	ITL_ERT eRet = ITL_ERT_SUCCESS;
	do
	{
		CHeic2jpeg heif2jpg;
		eRet = heif2jpg.heif2jpeg(wzSrcFilePath, wzDstJpgPath);
		//eRet = CHeic2jpeg::Instance().heif2jpeg(wzSrc, wzDst);
	} while (0);

	return eRet;
}

EU_EXPORT ITL_ERT EuHeic2Thumbnail(const wchar_t *wzSrcFilePath, const wchar_t *wzDstThumbnailPath)
{
	ITL_ERT eRet = ITL_ERT_SUCCESS;
	do
	{
		CHeic2jpeg heif2jpg;
		eRet = heif2jpg.heif2thumbnail(wzSrcFilePath, wzDstThumbnailPath);
		//eRet = CHeic2jpeg::Instance().heif2thumbnail(wzSrc, wzDst);
	} while (0);

	return eRet;
}
