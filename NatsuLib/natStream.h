////////////////////////////////////////////////////////////////////////////////
///	@file	natStream.h
///	@brief	NatsuLib��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "natRefObj.h"
#include "natMultiThread.h"
#include "natString.h"

#ifndef _WIN32
#	include <fstream>
#endif

namespace NatsuLib
{
	////////////////////////////////////////////////////////////////////////////////
	///	@brief	Ѱַ�ο�λ��
	////////////////////////////////////////////////////////////////////////////////
	enum class NatSeek
	{
		Beg,	///< @brief	����ͷ
		Cur,	///< @brief	��ǰ��λ��
		End		///< @brief	����β
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	��
	////////////////////////////////////////////////////////////////////////////////
	struct natStream
		: natRefObj
	{
		virtual ~natStream() = default;

		///	@brief		���Ƿ��д
		virtual nBool CanWrite() const = 0;

		///	@brief		���Ƿ�ɶ�
		virtual nBool CanRead() const = 0;

		///	@brief		���Ƿ�����·����С
		virtual nBool CanResize() const = 0;

		///	@brief		���Ƿ��Ѱַ
		virtual nBool CanSeek() const = 0;

		///	@brief		���Ƿ��ѵ����β
		virtual nBool IsEndOfStream() const = 0;

		///	@brief		������Ĵ�С
		virtual nLen GetSize() const = 0;

		///	@brief		���·������Ĵ�С
		///	@note		���·���ɹ��Ժ�ָ��λ�����Ŀ�ͷ
		virtual void SetSize(nLen Size) = 0;

		///	@brief		��ö�дָ���λ��
		virtual nLen GetPosition() const = 0;

		///	@brief		���ö�дָ���λ��
		///	@param[in]	Origin	Ѱַ�ο�λ��
		///	@param[in]	Offset	ƫ��
		virtual void SetPosition(NatSeek Origin, nLong Offset) = 0;

		/// @brief		�����ж�ȡһ���ֽ�
		virtual nByte ReadByte() = 0;

		///	@brief		��ȡ�ֽ�����
		///	@param[out]	pData	���ݻ�����
		///	@param[in]	Length	��ȡ�ĳ���
		///	@return		ʵ�ʶ�ȡ����
		virtual nLen ReadBytes(nData pData, nLen Length) = 0;

		/// @brief		�첽��ȡ�ֽ�����
		/// @param[out]	pData	���ݻ�����
		/// @param[in]	Length	��ȡ�ĳ���
		/// @return		ʵ�ʶ�ȡ����
		virtual std::future<nLen> ReadBytesAsync(nData pData, nLen Length) = 0;

		/// @brief		������д��һ���ֽ�
		virtual void WriteByte(nByte byte) = 0;

		///	@brief		д���ֽ�����
		///	@param[in]	pData	���ݻ�����
		///	@param[in]	Length	д��ĳ���
		///	@return		ʵ��д�볤��
		virtual nLen WriteBytes(ncData pData, nLen Length) = 0;

		///	@brief		�첽д���ֽ�����
		///	@param[in]	pData	���ݻ�����
		///	@param[in]	Length	д��ĳ���
		///	@return		ʵ��д�볤��
		virtual std::future<nLen> WriteBytesAsync(ncData pData, nLen Length) = 0;

		///	@brief		ˢ����
		///	@note		�����л�����Ƶ�����Ч��������
		virtual void Flush() = 0;
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	NatsuLib�ڴ���ʵ��
	////////////////////////////////////////////////////////////////////////////////
	class natMemoryStream
		: public natRefObjImpl<natStream>
	{
	public:
		natMemoryStream();
		natMemoryStream(ncData pData, nLen Length, nBool bReadable, nBool bWritable, nBool bResizable);
		~natMemoryStream();

		static natRefPointer<natMemoryStream> CreateFromExternMemory(nData pData, nLen Length, nBool bReadable, nBool bWritable);

		nBool CanWrite() const override;
		nBool CanRead() const override;
		nBool CanResize() const override;
		nBool CanSeek() const override;
		nBool IsEndOfStream() const override;
		nLen GetSize() const override;
		void SetSize(nLen Size) override;
		nLen GetPosition() const override;
		void SetPosition(NatSeek Origin, nLong Offset) override;
		nByte ReadByte() override;
		nLen ReadBytes(nData pData, nLen Length) override;
		std::future<nLen> ReadBytesAsync(nData pData, nLen Length) override;
		void WriteByte(nByte byte) override;
		nLen WriteBytes(ncData pData, nLen Length) override;
		std::future<nLen> WriteBytesAsync(ncData pData, nLen Length) override;
		void Flush() override;

		nData GetInternalBuffer() noexcept;
		ncData GetInternalBuffer() const noexcept;

	private:
		natCriticalSection m_CriSection;

		nData m_pData;
		nLen m_Length;
		nLen m_CurPos;
		nBool m_bReadable;
		nBool m_bWritable;
		nBool m_bResizable;
		nBool m_bExtern;
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	NatsuLib�ļ���ʵ��
	////////////////////////////////////////////////////////////////////////////////
	class natFileStream
		: public natRefObjImpl<natStream>
	{
	public:
#ifdef _WIN32
		typedef HANDLE UnsafeHandle;
#endif

		natFileStream(ncTStr lpFilename, nBool bReadable, nBool bWritable);
#ifdef _WIN32
		natFileStream(UnsafeHandle hFile, nBool bReadable, nBool bWritable);
#endif

		~natFileStream();

		nBool CanWrite() const override;
		nBool CanRead() const override;
		nBool CanResize() const override;
		nBool CanSeek() const override;
		nBool IsEndOfStream() const override;
		nLen GetSize() const override;
		void SetSize(nLen Size) override;
		nLen GetPosition() const override;
		void SetPosition(NatSeek Origin, nLong Offset) override;
		nByte ReadByte() override;
		nLen ReadBytes(nData pData, nLen Length) override;
		std::future<nLen> ReadBytesAsync(nData pData, nLen Length) override;
		void WriteByte(nByte byte) override;
		nLen WriteBytes(ncData pData, nLen Length) override;
		std::future<nLen> WriteBytesAsync(ncData pData, nLen Length) override;
		void Flush() override;

		ncTStr GetFilename() const noexcept;

#ifdef _WIN32
		UnsafeHandle GetUnsafeHandle() const noexcept;
		natRefPointer<natMemoryStream> MapToMemoryStream();
#endif

	private:
#ifdef _WIN32
		UnsafeHandle m_hFile, m_hMappedFile;
		natRefPointer<natMemoryStream> m_pMappedFile;
#else
		std::fstream m_File;
		nLen m_Size, m_CurrentPos;
#endif
		
		nTString m_Filename;
		nBool m_bReadable, m_bWritable;
	};

	/*template <StringType encoding>
	class natConsoleStream final
		: public natRefObjImpl<natStream>
	{
	public:
#ifdef _WIN32
		typedef HANDLE NativeHandle;

		natConsoleStream()
			: m_StdIn(GetStdHandle(STD_INPUT_HANDLE)), m_StdOut(GetStdHandle(STD_OUTPUT_HANDLE)), m_StdErr(GetStdHandle(STD_ERROR_HANDLE))
		{
		}
#else
		typedef FILE* NativeHandle;

		natConsoleStream()
			: m_StdIn(stdin), m_StdOut(stdout), m_StdErr(stderr)
		{
		}
#endif

		~natConsoleStream()
		{
		}

		nBool CanWrite() const override
		{
			return true;
		}

		nBool CanRead() const override
		{
			return true;
		}

		nBool CanResize() const override
		{
			return false;
		}

		nBool CanSeek() const override
		{
			return false;
		}

		nBool IsEndOfStream() const override
		{
			return false;
		}

		nLen GetSize() const override
		{
			return 0;
		}

		void SetSize(nLen Size) override
		{
			nat_Throw(natErrException, NatErr_NotSupport, _T("This stream cannot set size."));
		}

		nLen GetPosition() const override
		{
			return 0;
		}

		void SetPosition(NatSeek Origin, nLong Offset) override
		{
			nat_Throw(natErrException, NatErr_NotSupport, _T("This stream cannot set position."));
		}

		nByte ReadByte() override
		{
		}

		nLen ReadBytes(nData pData, nLen Length) override
		{
#ifdef _WIN32
			SetCP();
			String<encoding> tmpStr;
#else
#endif
		}

		std::future<nLen> ReadBytesAsync(nData pData, nLen Length) override
		{
			return std::async([this]
			{
				return ReadBytes(pData, Length);
			});
		}

		void WriteByte(nByte byte) override
		{
		}

		nLen WriteBytes(ncData pData, nLen Length) override
		{
		}

		std::future<nLen> WriteBytesAsync(ncData pData, nLen Length) override
		{
			return std::async([this]
			{
				return WriteBytes(pData, Length);
			});
		}

		void Flush() override
		{
			FlushConsoleInputBuffer(m_StdIn);
		}

#ifdef _WIN32
		static void SetCP()
		{
			SetConsoleCP(encoding == StringType::Ansi ? CP_ACP : CP_UTF8);
		}
#endif

	private:
		NativeHandle m_StdIn, m_StdOut, m_StdErr;
	};*/
}
