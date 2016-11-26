#include "stdafx.h"
#include "natStream.h"
#include "natException.h"
#include <algorithm>
#include <cstring>

using namespace NatsuLib;

#ifdef _WIN32
natFileStream::natFileStream(ncTStr lpFilename, nBool bReadable, nBool bWritable)
	: m_hMappedFile(NULL), m_Filename(lpFilename), m_bReadable(bReadable), m_bWritable(bWritable)
{
	m_hFile = CreateFile(
		lpFilename,
		(bReadable ? GENERIC_READ : 0) | (bWritable ? GENERIC_WRITE : 0),
		FILE_SHARE_READ,
		NULL,
		bWritable ? OPEN_ALWAYS : OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (!m_hFile || m_hFile == INVALID_HANDLE_VALUE)
	{
		nat_Throw(natWinException, _T("Open file \"%s\" failed"), lpFilename);
	}
}

natFileStream::natFileStream(UnsafeHandle hFile, nBool bReadable, nBool bWritable)
	: m_hFile(hFile), m_hMappedFile(NULL), m_bReadable(bReadable), m_bWritable(bWritable)
{
	if (!m_hFile || m_hFile == INVALID_HANDLE_VALUE)
	{
		nat_Throw(natErrException, NatErr_InvalidArg, _T("Invalid hFile."));
	}
}

nBool natFileStream::CanWrite() const
{
	return m_bWritable;
}

nBool natFileStream::CanRead() const
{
	return m_bReadable;
}

nBool natFileStream::CanResize() const
{
	return m_bWritable;
}

nBool natFileStream::CanSeek() const
{
	return true;
}

nBool natFileStream::IsEndOfStream() const
{
	return GetSize() == GetPosition();
}

nLen natFileStream::GetSize() const
{
	return GetFileSize(m_hFile, NULL);
}

void natFileStream::SetSize(nLen Size)
{
	if (!m_bWritable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not writable."));
	}

	LARGE_INTEGER tVal;
	tVal.QuadPart = Size;

	if (SetFilePointer(m_hFile, tVal.LowPart, &tVal.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		nat_Throw(natWinException, _T("SetFilePointer failed."));
	}

	if (SetEndOfFile(m_hFile) == FALSE)
	{
		nat_Throw(natWinException, _T("SetEndOfFile failed."));
	}

	SetPosition(NatSeek::Beg, 0l);
}

nLen natFileStream::GetPosition() const
{
	LARGE_INTEGER tVal;
	tVal.QuadPart = 0;

	tVal.LowPart = SetFilePointer(m_hFile, tVal.LowPart, &tVal.HighPart, FILE_CURRENT);
	return tVal.QuadPart;
}

void natFileStream::SetPosition(NatSeek Origin, nLong Offset)
{
	nuInt tOrigin;
	switch (Origin)
	{
	case NatSeek::Beg:
		tOrigin = FILE_BEGIN;
		break;
	case NatSeek::Cur:
		tOrigin = FILE_CURRENT;
		break;
	case NatSeek::End:
		tOrigin = FILE_END;
		break;
	default:
		nat_Throw(natErrException, NatErr_InvalidArg, _T("Origin is not a valid NatSeek."));
	}

	LARGE_INTEGER tVal;
	tVal.QuadPart = Offset;

	if (SetFilePointer(m_hFile, tVal.LowPart, &tVal.HighPart, tOrigin) == INVALID_SET_FILE_POINTER)
	{
		nat_Throw(natWinException, _T("SetFilePointer failed."));
	}
}

nByte natFileStream::ReadByte()
{
	nByte byte;
	if (ReadBytes(&byte, 1) == 1)
	{
		return byte;
	}

	nat_Throw(natErrException, NatErr_InternalErr, _T("Unable to read byte."));
}

nLen natFileStream::ReadBytes(nData pData, nLen Length)
{
	DWORD tReadBytes = 0ul;
	if (!m_bReadable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not readable."));
	}

	if (Length == 0ul)
	{
		return tReadBytes;
	}

	if (pData == nullptr)
	{
		nat_Throw(natErrException, NatErr_InvalidArg, _T("pData cannot be nullptr."));
	}

	if (ReadFile(m_hFile, pData, static_cast<DWORD>(Length), &tReadBytes, NULL) == FALSE)
	{
		nat_Throw(natWinException, _T("ReadFile failed."));
	}

	return tReadBytes;
}

std::future<nLen> natFileStream::ReadBytesAsync(nData pData, nLen Length)
{
	return std::async([=]()
	{
		return ReadBytes(pData, Length);
	});
}

void natFileStream::WriteByte(nByte byte)
{
	if (WriteBytes(&byte, 1) != 1)
	{
		nat_Throw(natErrException, NatErr_InternalErr, _T("Unable to write byte."));
	}
}

nLen natFileStream::WriteBytes(ncData pData, nLen Length)
{
	DWORD tWriteBytes = 0ul;
	if (!m_bWritable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not writable."));
	}

	if (Length == 0ul)
	{
		return tWriteBytes;
	}

	if (pData == nullptr)
	{
		nat_Throw(natErrException, NatErr_InvalidArg, _T("pData cannot be nullptr."));
	}

	if (WriteFile(m_hFile, pData, static_cast<DWORD>(Length), &tWriteBytes, NULL) == FALSE)
	{
		nat_Throw(natWinException, _T("WriteFile failed."));
	}

	return tWriteBytes;
}

std::future<nLen> natFileStream::WriteBytesAsync(ncData pData, nLen Length)
{
	return std::async([=]()
	{
		return WriteBytes(pData, Length);
	});
}

void natFileStream::Flush()
{
	if (m_pMappedFile)
	{
		FlushViewOfFile(m_pMappedFile->GetInternalBuffer(), static_cast<SIZE_T>(GetSize()));
	}

	FlushFileBuffers(m_hFile);
}

ncTStr natFileStream::GetFilename() const noexcept
{
	return m_Filename.c_str();
}

natFileStream::UnsafeHandle natFileStream::GetUnsafeHandle() const noexcept
{
	return m_hFile;
}

natRefPointer<natMemoryStream> natFileStream::MapToMemoryStream()
{
	if (m_pMappedFile)
	{
		return m_pMappedFile;
	}

	m_hMappedFile = CreateFileMapping(m_hFile, NULL, m_bWritable ? PAGE_READWRITE : PAGE_READONLY, NULL, NULL, NULL);
	if (m_hMappedFile && m_hMappedFile != INVALID_HANDLE_VALUE)
	{
		auto pFile = MapViewOfFile(m_hMappedFile, (m_bReadable ? FILE_MAP_READ : 0) | (m_bWritable ? FILE_MAP_WRITE : 0), NULL, NULL, NULL);
		if (pFile)
		{
			m_pMappedFile = std::move(natMemoryStream::CreateFromExternMemory(reinterpret_cast<nData>(pFile), GetSize(), m_bReadable, m_bWritable));
		}
		else
		{
			nat_Throw(natWinException, _T("MapViewOfFile failed."));
		}
	}
	else
	{
		nat_Throw(natWinException, _T("CreateFileMapping failed."));
	}

	return m_pMappedFile;
}

natFileStream::~natFileStream()
{
	CloseHandle(m_hMappedFile);
	CloseHandle(m_hFile);
}
#else
natFileStream::natFileStream(ncTStr lpFilename, nBool bReadable, nBool bWritable)
	: m_CurrentPos(0), m_Filename(lpFilename), m_bReadable(bReadable), m_bWritable(bWritable)
{
	std::ios_base::openmode openmode{};
	if (bReadable)
	{
		openmode |= std::ios_base::in;
	}
	if (bWritable)
	{
		openmode |= std::ios_base::out;
	}

	m_File.open(lpFilename, openmode);
	if (!m_File.is_open())
	{
		nat_Throw(natErrException, NatErr_InternalErr, _T("Cannot open file \"{0}\"."), lpFilename);
	}

	auto current = m_File.tellg();
	m_File.seekg(0, std::ios_base::end);
	m_Size = static_cast<nLen>(m_File.tellg() - current);
	m_File.seekg(current);
}

nBool natFileStream::CanWrite() const
{
	return m_bWritable;
}

nBool natFileStream::CanRead() const
{
	return m_bReadable;
}

nBool natFileStream::CanResize() const
{
	return m_bWritable;
}

nBool natFileStream::CanSeek() const
{
	return true;
}

nBool natFileStream::IsEndOfStream() const
{
	return m_File.eof();
}

nLen natFileStream::GetSize() const
{
	return m_Size;
}

void natFileStream::SetSize(nLen Size)
{
	if (!m_bWritable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not writable."));
	}

	if (Size < m_Size)
	{
		nat_Throw(natErrException, NatErr_InvalidArg, _T("Cannot truncate file."));
	}

	if (Size == m_Size)
	{
		return;
	}

	auto current = m_File.tellp();
	m_File.seekp(Size);
	m_File.seekp(current);
	m_Size = Size;
}

nLen natFileStream::GetPosition() const
{
	return m_CurrentPos;
}

void natFileStream::SetPosition(NatSeek Origin, nLong Offset)
{
	std::ios_base::seekdir tOrigin;
	switch (Origin)
	{
	case NatSeek::Beg:
		tOrigin = std::ios_base::beg;
		break;
	case NatSeek::Cur:
		tOrigin = std::ios_base::cur;
		break;
	case NatSeek::End:
		tOrigin = std::ios_base::end;
		break;
	default:
		nat_Throw(natErrException, NatErr_InvalidArg, _T("Origin is not a valid NatSeek."));
	}

	m_File.seekp(Offset, tOrigin);
	m_CurrentPos = static_cast<nLen>(m_File.tellp());
}

nLen natFileStream::ReadBytes(nData pData, nLen Length)
{
	if (!m_bReadable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not readable."));
	}

	if (Length == 0ul)
	{
		return 0;
	}

	if (pData == nullptr)
	{
		nat_Throw(natErrException, NatErr_InvalidArg, _T("pData cannot be nullptr."));
	}

	auto size = m_File.readsome(reinterpret_cast<char*>(pData), static_cast<std::streamsize>(Length));
	if (m_File.fail())
	{
		nat_Throw(natErrException, NatErr_InternalErr, _T("Reading file failed after readed {0} bytes."), size);
	}

	m_CurrentPos = static_cast<nLen>(m_File.tellg());
	return size;
}

std::future<nLen> natFileStream::ReadBytesAsync(nData pData, nLen Length)
{
	return std::async([=]()
	{
		return ReadBytes(pData, Length);
	});
}

nLen natFileStream::WriteBytes(ncData pData, nLen Length)
{
	if (!m_bWritable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not writable."));
	}

	if (Length == 0ul)
	{
		return 0;
	}

	if (pData == nullptr)
	{
		nat_Throw(natErrException, NatErr_InvalidArg, _T("pData cannot be nullptr."));
	}

	m_File.write(reinterpret_cast<const char*>(pData), static_cast<std::streamsize>(Length));
	auto current = m_File.tellp();
	auto size = current - static_cast<std::streamoff>(m_CurrentPos);

	if (m_File.fail())
	{
		nat_Throw(natErrException, NatErr_InternalErr, _T("Writing file failed after wrote {0} bytes."), size);
	}

	m_CurrentPos = static_cast<nLen>(current);
	return size;
}

std::future<nLen> natFileStream::WriteBytesAsync(ncData pData, nLen Length)
{
	return std::async([=]()
	{
		return WriteBytes(pData, Length);
	});
}

void natFileStream::Flush()
{
	m_File.flush();
}

ncTStr natFileStream::GetFilename() const noexcept
{
	return m_Filename.c_str();
}

natFileStream::~natFileStream()
{
}

#endif

natMemoryStream::natMemoryStream(ncData pData, nLen Length, nBool bReadable, nBool bWritable, nBool bResizable)
	: m_pData(nullptr), m_Length(0ul), m_CurPos(0u), m_bReadable(bReadable), m_bWritable(bWritable), m_bResizable(bResizable), m_bExtern(false)
{
	try
	{
		m_pData = new nByte[static_cast<nuInt>(Length)]{};
		m_Length = Length;
		if (pData)
		{
#ifdef _MSC_VER
			memcpy_s(m_pData, static_cast<rsize_t>(Length), pData, static_cast<rsize_t>(Length));
#else
			memcpy(m_pData, pData, static_cast<size_t>(Length));
#endif
		}
	}
	catch (std::bad_alloc&)
	{
		nat_Throw(natException, _T("Failed to allocate memory"));
	}
}

nBool natMemoryStream::CanWrite() const
{
	return m_bWritable;
}

nBool natMemoryStream::CanRead() const
{
	return m_bReadable;
}

nBool natMemoryStream::CanResize() const
{
	return m_bResizable;
}

nBool natMemoryStream::CanSeek() const
{
	return true;
}

nBool natMemoryStream::IsEndOfStream() const
{
	return m_CurPos >= m_Length;
}

nLen natMemoryStream::GetSize() const
{
	return m_Length;
}

void natMemoryStream::SetSize(nLen Size)
{
	if (!m_bResizable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not resizable."));
	}

	if (Size == m_Length)
	{
		return;
	}

	nData tpData;
	tpData = new nByte[static_cast<nuInt>(Size)];
	for (auto i = 0u; i < Size; ++i)
	{
		tpData[i] = i < m_Length ? m_pData[i] : nByte(0u);
	}

	SafeDelArr(m_pData);
	m_pData = tpData;
	m_Length = Size;

	if (m_CurPos > m_Length)
	{
		m_CurPos = m_Length;
	}
}

nLen natMemoryStream::GetPosition() const
{
	return m_CurPos;
}

void natMemoryStream::SetPosition(NatSeek Origin, nLong Offset)
{
	switch (Origin)
	{
	case NatSeek::Beg:
		if (Offset < 0 || m_Length < static_cast<nLen>(Offset))
			nat_Throw(natErrException, NatErr_OutOfRange, _T("Out of range."));
		m_CurPos = Offset;
		break;
	case NatSeek::Cur:
		if ((Offset < 0 && m_CurPos < static_cast<nLen>(-Offset)) || m_Length > static_cast<nLen>(m_CurPos + Offset))
			nat_Throw(natErrException, NatErr_OutOfRange, _T("Out of range."));
		m_CurPos += Offset;
		break;
	case NatSeek::End:
		if (Offset > 0 || m_Length < static_cast<nLen>(-Offset))
			nat_Throw(natErrException, NatErr_OutOfRange, _T("Out of range."));
		m_CurPos = m_Length + Offset;
		break;
	default:
		nat_Throw(natErrException, NatErr_OutOfRange, _T("Out of range."));
	}
}

nByte natMemoryStream::ReadByte()
{
	return m_pData[m_CurPos++];
}

nLen natMemoryStream::ReadBytes(nData pData, nLen Length)
{
	nLen tReadBytes = 0ul;

	if (!m_bReadable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not readable."));
	}

	if (Length == 0ul)
	{
		return tReadBytes;
	}

	if (pData == nullptr)
	{
		nat_Throw(natErrException, NatErr_InvalidArg, _T("pData cannot be nullptr."));
	}

	tReadBytes = std::min(Length, m_Length - m_CurPos);
#ifdef _MSC_VER
	memcpy_s(pData + m_CurPos, static_cast<rsize_t>(m_Length - m_CurPos), m_pData, static_cast<rsize_t>(tReadBytes));
#else
	memcpy(pData + m_CurPos, m_pData, static_cast<size_t>(tReadBytes));
#endif
	
	m_CurPos += tReadBytes;

	return tReadBytes;
}

std::future<nLen> natMemoryStream::ReadBytesAsync(nData pData, nLen Length)
{
	return std::async([=]()
	{
		natRefScopeGuard<natCriticalSection> guard(m_CriSection);
		return ReadBytes(pData, Length);
	});
}

void natMemoryStream::WriteByte(nByte byte)
{
	if (m_CurPos >= m_Length)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("End of stream."));
	}

	m_pData[m_CurPos++] = byte;
}

nLen natMemoryStream::WriteBytes(ncData pData, nLen Length)
{
	nLen tWriteBytes = 0ul;

	if (!m_bWritable)
	{
		nat_Throw(natErrException, NatErr_IllegalState, _T("Stream is not writable."));
	}

	if (Length == 0ul)
	{
		return tWriteBytes;
	}

	if (pData == nullptr)
	{
		nat_Throw(natErrException, NatErr_InvalidArg, _T("pData cannot be nullptr."));
	}

	tWriteBytes = std::min(Length, m_Length - m_CurPos);
#ifdef _MSC_VER
	memcpy_s(m_pData, static_cast<rsize_t>(tWriteBytes), pData + m_CurPos, static_cast<rsize_t>(tWriteBytes));
#else
	memcpy(m_pData, pData + m_CurPos, static_cast<size_t>(tWriteBytes));
#endif
	m_CurPos += tWriteBytes;

	return tWriteBytes;
}

std::future<nLen> natMemoryStream::WriteBytesAsync(ncData pData, nLen Length)
{
	return std::async([=]()
	{
		natRefScopeGuard<natCriticalSection> guard(m_CriSection);
		return WriteBytes(pData, Length);
	});
}

void natMemoryStream::Flush()
{
}

nData natMemoryStream::GetInternalBuffer() noexcept
{
	return m_pData;
}

ncData natMemoryStream::GetInternalBuffer() const noexcept
{
	return m_pData;
}

natMemoryStream::natMemoryStream()
	: m_pData(nullptr), m_Length(0ul), m_CurPos(0u), m_bReadable(false), m_bWritable(false), m_bResizable(false), m_bExtern(false)
{
}

natMemoryStream::~natMemoryStream()
{
	if (!m_bExtern)
	{
		SafeDelArr(m_pData);
	}
}

natRefPointer<natMemoryStream> natMemoryStream::CreateFromExternMemory(nData pData, nLen Length, nBool bReadable, nBool bWritable)
{
	auto pStream = make_ref<natMemoryStream>();

	pStream->m_pData = pData;
	pStream->m_Length = Length;
	pStream->m_bReadable = bReadable;
	pStream->m_bWritable = bWritable;
	pStream->m_bExtern = true;

	return pStream;
}
