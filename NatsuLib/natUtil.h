#pragma once
#include "natStringUtil.h"

#ifdef WIN32
#include <Windows.h>
#endif
#include <vector>
#include <locale>
#include <codecvt>

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
			,
		};

		///	@brief	stringתwstring
		std::wstring C2Wstr(ncStr str, size_t n);
		std::wstring C2Wstr(std::string const& str);
		template <size_t n>
		std::wstring C2Wstr(const nChar(&str)[n])
		{
			return C2Wstr(str, n);
		}
		std::wstring C2Wstr(ncStr str);
		///	@brief	wstringתstring
		std::string W2Cstr(ncWStr str, size_t n);
		std::string W2Cstr(std::wstring const& str);
		template <size_t n>
		std::string W2Cstr(const nWChar(&str)[n])
		{
			return W2Cstr(str, n);
		}
		std::string W2Cstr(ncWStr str);

#ifdef UNICODE
		nTString ToTString(ncStr str, size_t n);
		nTString ToTString(std::string const& str);
		template <size_t n>
		nTString ToTString(const nChar(&str)[n])
		{
			return C2Wstr(str);
		}
		nTString ToTString(ncStr str);

		nTString ToTString(ncWStr str, size_t n);
		nTString ToTString(std::wstring const& str);
		template <size_t n>
		nTString ToTString(const nWChar(&str)[n])
		{
			return ToTString(str, n);
		}
		nTString ToTString(ncWStr str);
#else
		nTString ToTString(ncStr str, size_t n);
		nTString ToTString(std::string const& str);
		template <size_t n>
		nTString ToTString(const nChar(&str)[n])
		{
			return ToTString(str, n);
		}
		nTString ToTString(ncStr str);

		nTString ToTString(ncWStr str, size_t n);
		nTString ToTString(std::wstring const& str);
		template <size_t n>
		nTString ToTString(const nWChar(&str)[n])
		{
			return W2Cstr(str);
		}
		nTString ToTString(ncWStr str);
#endif

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
