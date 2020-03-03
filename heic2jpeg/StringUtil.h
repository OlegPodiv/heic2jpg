#pragma once

#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <functional>
#include <objbase.h>
#include "Heic2jpegMacroDef.h"
using namespace std;

inline GUID CreateGuid()
{
	GUID guid = { 0 };

#ifdef WIN32
	CoCreateGuid(&guid);
#else
	uuid_generate(reinterpret_cast<unsigned char *>(&guid));
#endif

	return guid;
}

inline std::wstring CreateGuidStr()
{
	std::wstring wzValue;

	do
	{
		GUID guid = CreateGuid();

		wchar_t wzGuid[BUFF_SIZE] = { 0 };
		swprintf_s(wzGuid, BUFF_SIZE, L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);

		wzValue = wzGuid;

	} while (0);

	return wzValue;
}

inline std::wstring CreateGuidStr2()
{
	std::wstring wzValue;

	do
	{
		GUID guid = CreateGuid();

		wchar_t wzGuid[BUFF_SIZE] = { 0 };
		swprintf_s(wzGuid, BUFF_SIZE, L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);

		wzValue = wzGuid;

	} while (0);

	return wzValue;
}

inline std::string ws2s(const std::wstring& ws)
{
	string curLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs");

	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = sizeof(wchar_t) * ws.size() + 1;

	vector<char> vecDest(_Dsize);
	char *_Dest = &vecDest[0];

	size_t converted = 0;
	wcstombs_s(&converted, _Dest, _Dsize, _Source, _TRUNCATE);
	string result = _Dest;

	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

inline std::wstring s2ws(const std::string& s)
{
	string curLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;

	size_t converted = 0;
	vector<wchar_t> vecDest(_Dsize);
	wchar_t *_Dest = &vecDest[0];

	mbstowcs_s(&converted, _Dest, _Dsize, _Source, _TRUNCATE);
	wstring result = _Dest;

	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

inline std::wstring utf82ws(const std::string& s)
{
	wstring result = L"";

	int _Dsize = ::MultiByteToWideChar(CP_UTF8, NULL, s.c_str(), (int)s.length(), NULL, 0);

	vector<wchar_t> vecDest(_Dsize + 1);
	wchar_t *_Dest = &vecDest[0];

	::MultiByteToWideChar(CP_UTF8, NULL, s.c_str(), (int)s.length(), _Dest, _Dsize);
	result = _Dest;

	return result;
}

inline std::string ws2utf8(const std::wstring& ws)
{
	string result = "";

	int _Dsize = ::WideCharToMultiByte(CP_UTF8, NULL, ws.c_str(), (int)ws.length(), NULL, 0, NULL, NULL);

	vector<char> vecDest(_Dsize + 1);
	char *_Dest = &vecDest[0];

	::WideCharToMultiByte(CP_UTF8, NULL, ws.c_str(), (int)ws.length(), _Dest, _Dsize, NULL, NULL);
	result = _Dest;

	return result;
}

inline std::wstring ToWString(const char* pszSrc)
{
	std::wstring wstrRes = L"";
	if (!IsNull(pszSrc))
	{
		string szSrc = pszSrc;
		wstrRes = s2ws(szSrc);
	}
	return wstrRes.c_str();
}

inline std::string ToString(const wchar_t* pszSrc)
{
	std::string strRes = "";
	if (!IsNull(pszSrc))
	{
		wstring wzSrc = pszSrc;
		strRes = ws2s(wzSrc);
	}
	return strRes.c_str();
}
