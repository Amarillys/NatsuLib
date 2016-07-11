////////////////////////////////////////////////////////////////////////////////
///	@file	natException.h
///	@brief	�쳣���ͷ�ļ�
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "natType.h"
#include <Windows.h>

namespace NatsuLib
{
	namespace natUtil
	{
		template <typename... Args>
		nTString FormatString(ncTStr lpStr, Args&&... args);
		inline std::wstring C2Wstr(std::string const& str);
		inline std::string W2Cstr(std::wstring const& str);
	}

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	NatsuLib�쳣����
	///	@note	�쳣�ɴ�������������ʹ�ÿ����׳��쳣�Ĵ���
	////////////////////////////////////////////////////////////////////////////////
	class natException
		: public std::exception
	{
	public:
		template <typename... Args>
		natException(ncTStr Src, ncTStr File, nuInt Line, ncTStr Desc, Args&&... args) noexcept
			: exception(natUtil::W2Cstr(natUtil::FormatString(Desc, std::forward<Args>(args)...)).c_str()), m_Time(GetTickCount()), m_File(File), m_Line(Line), m_Source(Src), m_Description(natUtil::C2Wstr(exception::what()))
		{
		}

		virtual ~natException() = default;

		nuInt GetTime() const noexcept
		{
			return m_Time;
		}

		ncTStr GetFile() const noexcept
		{
			return m_File.c_str();
		}

		nuInt GetLine() const noexcept
		{
			return m_Line;
		}

		ncTStr GetSource() const noexcept
		{
			return m_Source.c_str();
		}

		ncTStr GetDesc() const noexcept
		{
			return m_Description.c_str();
		}

	protected:
		nuInt m_Time;
		nTString m_File;
		nuInt m_Line;
		nTString m_Source;
		nTString m_Description;
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	NatsuLib WinAPI�����쳣
	///	@note	�����Զ�����LastErr��Ϣ
	////////////////////////////////////////////////////////////////////////////////
	class natWinException
		: public natException
	{
	public:
		template <typename... Args>
		natWinException(ncTStr Src, ncTStr File, nuInt Line, ncTStr Desc, Args&&... args) noexcept
			: natWinException(Src, File, Line, GetLastError(), Desc, std::forward<Args>(args)...)
		{
		}

		template <typename... Args>
		natWinException(ncTStr Src, ncTStr File, nuInt Line, DWORD LastErr, ncTStr Desc, Args&&... args) noexcept
			: natException(Src, File, Line, Desc, std::forward<Args>(args)...), m_LastErr(LastErr)
		{
			m_Description = move(natUtil::FormatString((m_Description + _T(" (LastErr = %ul)")).c_str(), m_LastErr));
		}

		DWORD GetErrNo() const noexcept
		{
			return m_LastErr;
		}

		nTString GetErrMsg() const noexcept
		{
			LPVOID pBuf = nullptr;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, m_LastErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<nTStr>(&pBuf), 0, nullptr);
			if (!pBuf)
			{
				return nTString();
			}

			nTString ret(static_cast<ncTStr>(pBuf));
			LocalFree(pBuf);
			return move(ret);
		}

	private:
		DWORD m_LastErr;
	};

	class natErrException
		: public natException
	{
	public:
		template <typename... Args>
		natErrException(ncTStr Src, ncTStr File, nuInt Line, NatErr ErrNo, ncTStr Desc, Args&&... args) noexcept
			: natException(Src, File, Line, Desc, std::forward<Args>(args)...), m_Errno(ErrNo)
		{
			m_Description = move(natUtil::FormatString((m_Description + _T(" (Errno = {0})")).c_str(), m_Errno));
		}

		NatErr GetErrNo() const noexcept
		{
			return m_Errno;
		}

		ncTStr GetErrMsg() const noexcept
		{
			return GetErrDescription(m_Errno);
		}

	private:
		NatErr m_Errno;

		static ncTStr GetErrDescription(NatErr Errno)
		{
			switch (Errno)
			{
			case NatErr_Interrupted:
				return _T("Interrupted");
			case NatErr_OK:
				return _T("Success");
			case NatErr_Unknown:
				return _T("Unknown error");
			case NatErr_IllegalState:
				return _T("Illegal state");
			case NatErr_InvalidArg:
				return _T("Invalid argument");
			case NatErr_InternalErr:
				return _T("Internal error");
			case NatErr_OutOfRange:
				return _T("Out of range");
			case NatErr_NotImpl:
				return _T("Not implemented");
			case NatErr_NotSupport:
				return _T("Not supported");
			default:
				return _T("No description");
			}
		}
	};
}

#define nat_Throw(ExceptionClass, ...) do { throw ExceptionClass(_T(__FUNCTION__), _T(__FILE__), __LINE__, __VA_ARGS__); } while (false)

#include "natStringUtil.h"