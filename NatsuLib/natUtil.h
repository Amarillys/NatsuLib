#pragma once
#include "natType.h"
#include "natStringUtil.h"

#ifdef WIN32
#include <Windows.h>
#endif
#include <vector>

namespace NatsuLib
{
	namespace natUtil
	{
		enum : nuInt
		{
			DefaultCodePage
#ifdef WIN32
				= CP_INSTALLED
#endif
		};

		///	@brief	stringתwstring
		inline std::wstring C2Wstr(std::string const& str)
		{
			return std::wstring(str.begin(), str.end());
		}
		///	@brief	wstringתstring
		inline std::string W2Cstr(std::wstring const& str)
		{
			return std::string(str.begin(), str.end());
		}

		///	@brief	���ֽ�תUnicode
		std::wstring MultibyteToUnicode(ncStr Str, nuInt CodePage = DefaultCodePage);
		///	@brief	���ַ�ת���ֽ�
		std::string WidecharToMultibyte(ncWStr Str, nuInt CodePage = DefaultCodePage);

#ifdef WIN32
		///	@brief	�����Դ�ַ���
		///	@param[in]	ResourceID	��ԴID
		///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
		nTString GetResourceString(DWORD ResourceID, HINSTANCE hInstance = NULL);

		///	@brief	�����Դ����
		///	@param[in]	ResourceID	��ԴID
		///	@param[in]	lpType		��Դ����
		///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
		std::vector<nByte> GetResourceData(DWORD ResourceID, ncTStr lpType, HINSTANCE hInstance = NULL);
#endif
	}
}
