#pragma once
#include "Heic2jpegErrorMacroDef.h"

ITL_ERT EuHeic2jpeg(const wchar_t *wzSrcFilePath, const wchar_t *wzDstJpgPath);
ITL_ERT EuHeic2Thumbnail(const wchar_t *wzSrcFilePath, const wchar_t *wzDstThumbnailPath);
