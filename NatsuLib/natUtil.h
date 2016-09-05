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
		inline std::wstring C2Wstr(ncStr str, size_t n)
		{
			return std::wstring_convert<std::codecvt_utf8<nWChar>>{}.from_bytes(str, str + n);
		}
		inline std::wstring C2Wstr(std::string const& str)
		{
			return C2Wstr(str.c_str(), str.size());
		}
		template <size_t n>
		std::wstring C2Wstr(const nChar(&str)[n])
		{
			return C2Wstr(str, n);
		}
		inline std::wstring C2Wstr(ncStr str)
		{
			return C2Wstr(str, std::char_traits<nChar>::length(str));
		}
		///	@brief	wstringתstring
		inline std::string W2Cstr(ncWStr str, size_t n)
		{
			return std::wstring_convert<std::codecvt_utf8<nWChar>>{}.to_bytes(str, str + n);
		}
		inline std::string W2Cstr(std::wstring const& str)
		{
			return W2Cstr(str.c_str(), str.size());
		}
		template <size_t n>
		std::string W2Cstr(const nWChar(&str)[n])
		{
			return W2Cstr(str, n);
		}
		inline std::string W2Cstr(ncWStr str)
		{
			return W2Cstr(str, std::char_traits<nWChar>::length(str));
		}

#ifdef UNICODE
		inline nTString ToTString(ncStr str, size_t n)
		{
			return C2Wstr(str, n);
		}
		inline nTString ToTString(std::string const& str)
		{
			return C2Wstr(str);
		}
		template <size_t n>
		nTString ToTString(const nChar(&str)[n])
		{
			return C2Wstr(str);
		}
		inline nTString ToTString(ncStr str)
		{
			return C2Wstr(str);
		}
		inline nTString ToTString(ncWStr str, size_t n)
		{
			return{ str, n };
		}
		inline nTString ToTString(std::wstring const& str)
		{
			return ToTString(str.c_str(), str.size());
		}
		template <size_t n>
		nTString ToTString(const nWChar(&str)[n])
		{
			return ToTString(str, n);
		}
		inline nTString ToTString(ncWStr str)
		{
			return ToTString(str, std::char_traits<nWChar>::length(str));
		}
#else
		inline nTString ToTString(ncStr str, size_t n)
		{
			return{ str, n };
		}
		inline nTString ToTString(std::string const& str)
		{
			return ToTString(str.c_str(), str.size());
		}
		template <size_t n>
		nTString ToTString(const nChar(&str)[n])
		{
			return ToTString(str, n);
		}
		inline nTString ToTString(ncStr str)
		{
			return ToTString(str, std::char_traits<nChar>::length(str));
			
		}
		inline nTString ToTString(ncWStr str, size_t n)
		{
			return W2Cstr(str, n);
		}
		inline nTString ToTString(std::wstring const& str)
		{
			return W2Cstr(str);
		}
		template <size_t n>
		nTString ToTString(const nWChar(&str)[n])
		{
			return W2Cstr(str);
		}
		inline nTString ToTString(ncWStr str)
		{
			return W2Cstr(str);
		}
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
