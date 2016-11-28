#pragma once
#include "natStringUtil.h"

#ifdef _WIN32
#include <Windows.h>
#endif
#include <vector>

namespace NatsuLib
{
	namespace natUtil
	{
#ifdef _WIN32
		enum : nuInt
		{
			DefaultCodePage = CP_INSTALLED
		};
#endif

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
		std::basic_string<TCHAR> ToTString(ncStr str, size_t n);
		std::basic_string<TCHAR> ToTString(std::string const& str);
		template <size_t n>
		std::basic_string<TCHAR> ToTString(const nChar(&str)[n])
		{
			return C2Wstr(str);
		}
		std::basic_string<TCHAR> ToTString(ncStr str);

		std::basic_string<TCHAR> ToTString(ncWStr str, size_t n);
		std::basic_string<TCHAR> ToTString(std::wstring const& str);
		template <size_t n>
		std::basic_string<TCHAR> ToTString(const nWChar(&str)[n])
		{
			return ToTString(str, n);
		}
		std::basic_string<TCHAR> ToTString(ncWStr str);
#else
		std::basic_string<TCHAR> ToTString(ncStr str, size_t n);
		std::basic_string<TCHAR> ToTString(std::string const& str);
		template <size_t n>
		std::basic_string<TCHAR> ToTString(const nChar(&str)[n])
		{
			return ToTString(str, n);
		}
		std::basic_string<TCHAR> ToTString(ncStr str);

		std::basic_string<TCHAR> ToTString(ncWStr str, size_t n);
		std::basic_string<TCHAR> ToTString(std::wstring const& str);
		template <size_t n>
		std::basic_string<TCHAR> ToTString(const nWChar(&str)[n])
		{
			return W2Cstr(str);
		}
		std::basic_string<TCHAR> ToTString(ncWStr str);
#endif

#ifdef _WIN32

		///	@brief	���ֽ�תUnicode
		std::wstring MultibyteToUnicode(ncStr Str, nuInt CodePage = DefaultCodePage);
		///	@brief	���ַ�ת���ֽ�
		std::string WidecharToMultibyte(ncWStr Str, nuInt CodePage = DefaultCodePage);

		///	@brief	�����Դ�ַ���
		///	@param[in]	ResourceID	��ԴID
		///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
		nTString GetResourceString(DWORD ResourceID, HINSTANCE hInstance = NULL);

		///	@brief	�����Դ����
		///	@param[in]	ResourceID	��ԴID
		///	@param[in]	lpType		��Դ����
		///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
		std::vector<nByte> GetResourceData(DWORD ResourceID, LPCTSTR lpType, HINSTANCE hInstance = NULL);
#endif
	}
}
