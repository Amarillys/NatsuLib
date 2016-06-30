#pragma once
#include "natType.h"
#include <Windows.h>
#include <vector>
#include "natStringUtil.h"

namespace NatsuLib
{
	namespace natUtil
	{
		///	@brief	��ñ���ʱ��
		///	@return	����ʱ����Ϣ���ַ���
		inline nTString GetSysTime()
		{
			SYSTEMTIME st;
			GetLocalTime(&st);

			return FormatString(_T("%04d/%02d/%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}

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
		std::wstring MultibyteToUnicode(ncStr Str, nuInt CodePage = CP_INSTALLED);
		///	@brief	���ַ�ת���ֽ�
		std::string WidecharToMultibyte(ncWStr Str, nuInt CodePage = CP_INSTALLED);

		///	@brief	�����Դ�ַ���
		///	@param[in]	ResourceID	��ԴID
		///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
		nTString GetResourceString(DWORD ResourceID, HINSTANCE hInstance = NULL);

		///	@brief	�����Դ����
		///	@param[in]	ResourceID	��ԴID
		///	@param[in]	lpType		��Դ����
		///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
		std::vector<nByte> GetResourceData(DWORD ResourceID, ncTStr lpType, HINSTANCE hInstance = NULL);

		///	@brief	�ַ����ָ��
		///	@param[in]	str			Ҫ�ָ���ַ���
		///	@param[in]	pattern		�ָ��ַ�
		///	@param[in]	callableObject	�ɱ����õĶ��󣬽��ܲ���ΪnUnsafePtr<const Char_T>, size_t���ֱ�Ϊָ���ַ�����ʼλ�õ�ָ����ַ�������
		template <typename Char_T, typename CallableObject>
		void split(nUnsafePtr<const Char_T> str, size_t strLen, nUnsafePtr<const Char_T> pattern, size_t patternLen, CallableObject callableObject)
		{
			size_t pos = 0;

			for (size_t i = 0; i < strLen; ++i)
			{
				auto currentchar = str[i];
				for (size_t j = 0; j < patternLen; ++j)
				{
					if (str[i] == pattern[j])
					{
						callableObject(str + pos, i - pos);

						pos = i + 1;
						break;
					}
				}
			}

			if (pos != strLen)
			{
				callableObject(str + pos, strLen - pos);
			}
		}

		template <typename Char_T, typename CallableObject, size_t StrLen, size_t PatternLen>
		void split(const Char_T(&str)[StrLen], const Char_T(&pattern)[PatternLen], CallableObject callableObject)
		{
			split(str, StrLen - 1, pattern, PatternLen - 1, callableObject);
		}

		template <typename Char_T, typename CallableObject>
		void split(std::basic_string<Char_T> const& str, std::basic_string<Char_T> const& pattern, CallableObject callableObject)
		{
			split(str.c_str(), str.size(), pattern.c_str(), pattern.size(), callableObject);
		}
	}
}
