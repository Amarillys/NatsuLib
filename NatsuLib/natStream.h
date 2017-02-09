////////////////////////////////////////////////////////////////////////////////
///	@file	natStream.h
///	@brief	NatsuLib��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "natRefObj.h"
#include "natMultiThread.h"
#include "natString.h"
#include "natException.h"

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
		virtual ~natStream();

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
		virtual nByte ReadByte();

		///	@brief		��ȡ�ֽ�����
		///	@param[out]	pData	���ݻ�����
		///	@param[in]	Length	��ȡ�ĳ���
		///	@return		ʵ�ʶ�ȡ����
		virtual nLen ReadBytes(nData pData, nLen Length) = 0;

		///	@brief		ǿ�ƶ�ȡ�ֽ�����
		///	@param[out]	pData	���ݻ�����
		///	@param[in]	Length	��ȡ�ĳ���
		///	@note		�ظ���ȡ����ֱ���ɹ���ȡ���ֽ�����С��LengthΪֹ��ע�Ȿ�������������ѭ��
		virtual void ForceReadBytes(nData pData, nLen Length);

		/// @brief		�첽��ȡ�ֽ�����
		/// @param[out]	pData	���ݻ�����
		/// @param[in]	Length	��ȡ�ĳ���
		/// @return		ʵ�ʶ�ȡ����
		virtual std::future<nLen> ReadBytesAsync(nData pData, nLen Length);

		/// @brief		������д��һ���ֽ�
		virtual void WriteByte(nByte byte);

		///	@brief		д���ֽ�����
		///	@param[in]	pData	���ݻ�����
		///	@param[in]	Length	д��ĳ���
		///	@return		ʵ��д�볤��
		virtual nLen WriteBytes(ncData pData, nLen Length) = 0;

		///	@brief		ǿ��д���ֽ�����
		///	@param[in]	pData	���ݻ�����
		///	@param[in]	Length	д��ĳ���
		///	@note		�ظ�д�����ֱ���ɹ�д����ֽ�����С��LengthΪֹ��ע�Ȿ�������������ѭ��
		virtual void ForceWriteBytes(ncData pData, nLen Length);

		///	@brief		�첽д���ֽ�����
		///	@param[in]	pData	���ݻ�����
		///	@param[in]	Length	д��ĳ���
		///	@return		ʵ��д�볤��
		virtual std::future<nLen> WriteBytesAsync(ncData pData, nLen Length);

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
		static natRefPointer<natMemoryStream> CreateFromExternMemory(ncData pData, nLen Length, nBool bReadable);

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

#ifdef _WIN32
		natFileStream(nStrView filename, nBool bReadable, nBool bWritable, nBool isAsync = false);
		natFileStream(UnsafeHandle hFile, nBool bReadable, nBool bWritable, nBool transferOwner = false, nBool isAsync = false);
#else
		natFileStream(nStrView filename, nBool bReadable, nBool bWritable);
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
		void WriteByte(nByte byte) override;
		nLen WriteBytes(ncData pData, nLen Length) override;
#ifdef _WIN32
		std::future<nLen> ReadBytesAsync(nData pData, nLen Length) override;
		std::future<nLen> WriteBytesAsync(ncData pData, nLen Length) override;
#endif

		void Flush() override;

		nStrView GetFilename() const noexcept;

#ifdef _WIN32
		UnsafeHandle GetUnsafeHandle() const noexcept;
		natRefPointer<natMemoryStream> MapToMemoryStream();
#endif

	private:
#ifdef _WIN32
		UnsafeHandle m_hFile, m_hMappedFile;
		natRefPointer<natMemoryStream> m_pMappedFile;
		const nBool m_ShouldDispose;
		const nBool m_IsAsync;
#else
		std::fstream m_File;
		nLen m_Size, m_CurrentPos;
#endif
		
		nString m_Filename;
		nBool m_bReadable, m_bWritable;
	};

	class natSubStream
		: public natRefObjImpl<natStream>
	{
	public:
		natSubStream(natRefPointer<natStream> stream, nLen startPosition, nLen endPosition);
		~natSubStream();

		natRefPointer<natStream> GetUnderlyingStream() const noexcept;

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

	private:
		const natRefPointer<natStream> m_InternalStream;
		const nLen m_StartPosition;
		const nLen m_EndPosition;
		nLen m_CurrentPosition;

		void adjustPosition() const;
		void checkPosition() const;
	};

	class natStdStream
		: public natRefObjImpl<natStream>
	{
	public:
		enum StdStreamType
		{
			StdIn,
			StdOut,
			StdErr,
		};

#ifdef _WIN32
		typedef HANDLE NativeHandle;

		nBool UseFileApi() const noexcept;
#else
		typedef FILE* NativeHandle;
#endif

		explicit natStdStream(StdStreamType stdStreamType);
		~natStdStream();

		nBool CanWrite() const override;
		nBool CanRead() const override;
		nBool CanResize() const override;
		nBool CanSeek() const override;
		nBool IsEndOfStream() const override;
		nLen GetSize() const override;
		void SetSize(nLen /*Size*/) override;
		nLen GetPosition() const override;
		void SetPosition(NatSeek /*Origin*/, nLong /*Offset*/) override;
		nByte ReadByte() override;
		nLen ReadBytes(nData pData, nLen Length) override;
		std::future<nLen> ReadBytesAsync(nData pData, nLen Length) override;
		void WriteByte(nByte byte) override;
		nLen WriteBytes(ncData pData, nLen Length) override;
		std::future<nLen> WriteBytesAsync(ncData pData, nLen Length) override;
		void Flush() override;

	private:
		StdStreamType m_StdStreamType;
		NativeHandle m_StdHandle;
#ifdef _WIN32
		natRefPointer<natFileStream> m_InternalStream;
#endif
	};
}
