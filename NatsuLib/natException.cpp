#include "stdafx.h"
#include "natException.h"
#include "natMisc.h"

using namespace NatsuLib;

detail_::natExceptionStorage::natExceptionStorage(std::exception_ptr nestedException, std::chrono::system_clock::time_point const& time, nTString const& file, nuInt line, nTString const& src, nTString const& desc)
	: m_NestedException(nestedException), m_Time(time), m_File(file), m_Line(line), m_Source(src), m_Description(desc)
{
}

natException::~natException()
{
}

std::chrono::system_clock::time_point natException::GetTime() const noexcept
{
	return m_Time;
}

ncTStr natException::GetFile() const noexcept
{
	return m_File;
}

nuInt natException::GetLine() const noexcept
{
	return m_Line;
}

ncTStr natException::GetSource() const noexcept
{
	return m_Source;
}

ncTStr natException::GetDesc() const noexcept
{
	return m_Description;
}

std::exception_ptr natException::GetNestedException() const noexcept
{
	return m_NestedException;
}

#ifdef EnableExceptionStackTrace
natStackWalker const& natException::GetStackWalker() const noexcept
{
	return m_StackWalker;
}
#endif

ncStr natException::what() const noexcept
{
	return m_Description.data();
}

#ifdef _WIN32

DWORD natWinException::GetErrNo() const noexcept
{
	return m_LastErr;
}

ncTStr natWinException::GetErrMsg() const noexcept
{
	if (m_ErrMsg.empty())
	{
		LPVOID pBuf = nullptr;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, m_LastErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&pBuf), 0, nullptr);
		if (pBuf)
		{
			auto scopeExit = make_scope([pBuf]
			{
				LocalFree(pBuf);
			});
#ifdef UNICODE
			m_ErrMsg = WideStringView{ static_cast<LPCTSTR>(pBuf) };
#else
			m_ErrMsg = AnsiStringView{ static_cast<LPCTSTR>(pBuf) };
#endif
		}
	}

	return m_ErrMsg;
}

#endif

NatErr natErrException::GetErrNo() const noexcept
{
	return m_Errno;
}

ncTStr natErrException::GetErrMsg() const noexcept
{
	return GetErrDescription(m_Errno);
}
