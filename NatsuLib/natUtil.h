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

		///	@brief	���ֽ�תUnicode
		std::wstring MultibyteToUnicode(ncStr Str, nuInt CodePage = DefaultCodePage);
		///	@brief	���ַ�ת���ֽ�
		std::string WidecharToMultibyte(ncWStr Str, nuInt CodePage = DefaultCodePage);

		///	@brief	�����Դ�ַ���
		///	@param[in]	ResourceID	��ԴID
		///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
		nString GetResourceString(DWORD ResourceID, HINSTANCE hInstance = NULL);

		///	@brief	�����Դ����
		///	@param[in]	ResourceID	��ԴID
		///	@param[in]	lpType		��Դ����
		///	@param[in]	hInstance	ʵ�������Ĭ��ΪNULL
		std::vector<nByte> GetResourceData(DWORD ResourceID, LPCTSTR lpType, HINSTANCE hInstance = NULL);
#endif
	}
}
