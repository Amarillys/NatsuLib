////////////////////////////////////////////////////////////////////////////////
///	@file	natMultiThread.h
///	@brief	���߳�֧��
////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef WIN32
#	include <Windows.h>
#endif
#include <unordered_map>
#include <memory>
#include <functional>
#include <atomic>
#include <queue>
#include <future>

#pragma push_macro("max")
#ifdef max
#	undef max
#endif

namespace NatsuLib
{
	////////////////////////////////////////////////////////////////////////////////
	///	@addtogroup	ϵͳ�ײ�֧��
	///	@brief		�ṩ�漰ϵͳ�ں˲�����֧��
	///	@{

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	�̻߳���
	///	@note	��̳и��ಢ��дThreadJob��ʵ�ֶ��߳�\n
	///			��ȷ���߳�����ִ���ڼ䱣����Ч
	////////////////////////////////////////////////////////////////////////////////
	class natThread
	{
	public:
#ifdef WIN32
		typedef DWORD ThreadIdType;
		typedef DWORD ResultType;
		typedef HANDLE UnsafeHandle;
#else
		typedef std::thread::id ThreadIdType;
		typedef unsigned long ResultType;
		typedef nUnsafePtr<void> UnsafeHandle;
#endif

		enum : nuInt
		{
			Infinity = std::numeric_limits<nuInt>::max(),
		};

		///	@brief	���캯��
		///	@param[in]	Pause	�����߳�ʱ��ͣ
		explicit natThread(nBool Pause = true);
		virtual ~natThread();

		///	@brief		����߳̾��
		///	@warning	�����ֶ�ɾ��
		UnsafeHandle GetHandle() noexcept;

		ThreadIdType GetThreadId() const noexcept;

		///	@brief	�����߳�ִ��
		///	@return	�Ƿ�ɹ�
		nBool Resume() noexcept;

		///	@brief	�����߳�ִ��
		///	@return	�Ƿ�ɹ�
		nBool Suspend() noexcept;

		///	@brief	�ȴ��߳�ִ��
		///	@param[in]	WaitTime	�ȴ�ʱ��
		///	@return	����ʱ�Ƿ�δ��ʱ
		nBool Wait(nuInt WaitTime = Infinity) noexcept;

		///	@brief	�����߳�
		///	@param[in]	ExitCode	�˳���
		///	@return	�Ƿ�ɹ�
		nBool Terminate(nuInt ExitCode = nuInt(-1)) noexcept;

		///	@brief	����˳���
		nuInt GetExitCode();
	protected:
		///	@brief	��д�˷�����ʵ���̹߳���
		virtual ResultType ThreadJob() = 0;

	private:
#ifdef WIN32
		///	@brief	ִ���̵߳İ�װ����
		///	@param[in]	p	ָ��Thread���ָ��
		static ResultType CALLBACK execute(void* p);
		UnsafeHandle m_hThread;
		ThreadIdType m_hThreadID;
#else
		std::future<ResultType> m_Result;
		std::mutex m_Mutex;
		std::condition_variable m_Pause;
		std::thread m_Thread;
#endif
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	�ٽ���
	///	@note	���ڿ���Ϊ��������Խ�����̳߳�ͻ
	////////////////////////////////////////////////////////////////////////////////
	class natCriticalSection final
	{
	public:
		natCriticalSection();
		~natCriticalSection();

		///	@brief	�����ٽ���
		///	@note	������������
		void Lock();
		///	@brief	���������ٽ���
		///	@note	���������߳�
		///	@return	�Ƿ�ɹ�
		nBool TryLock();
		///	@brief	�����ٽ���
		void UnLock();
	private:
#ifdef WIN32
		CRITICAL_SECTION m_Section;
#else
		std::recursive_mutex m_Mutex;
#endif
	};

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	Windows��Event��װ��
	///	@note	ͨ��Event�Զ��̲߳�������ͬ��
	////////////////////////////////////////////////////////////////////////////////
	class natEventWrapper final
	{
	public:
#ifdef WIN32
		typedef HANDLE UnsafeHandle;
#else
		typedef nUnsafePtr<void> UnsafeHandle;
#endif
		
		enum : nuInt
		{
			Infinity = std::numeric_limits<nuInt>::max(),
		};

		natEventWrapper(nBool AutoReset, nBool InitialState);
		~natEventWrapper();

		///	@brief		��þ��
		///	@warning	�����ֶ��ͷ�
		UnsafeHandle GetHandle();

		///	@brief		����¼�
		///	@return		�Ƿ�ɹ�
		nBool Set();

		///	@brief		ȡ���¼����
		///	@return		�Ƿ�ɹ�
		nBool Reset();

		///	@brief		�¼�����
		///	@return		�Ƿ�ɹ�
		nBool Pulse();

		///	@brief		�ȴ��¼�
		///	@param[in]	WaitTime	�ȴ�ʱ��
		///	@return		�Ƿ�ɹ�
		nBool Wait(nuInt WaitTime = Infinity);
	private:
#ifdef WIN32
		UnsafeHandle m_hEvent;
#else
		std::mutex m_Mutex;
		std::condition_variable m_Condition;
#endif
	};

	class natThreadPool final
	{
		class WorkToken;
	public:
		typedef std::function<nuInt(void*)> WorkFunc;
		enum : nuInt
		{
			DefaultMaxThreadCount = 4,
			Infinity = std::numeric_limits<nuInt>::max(),
		};

		explicit natThreadPool(nuInt InitialThreadCount = 0, nuInt MaxThreadCount = DefaultMaxThreadCount);
		~natThreadPool();

		void KillIdleThreads(nBool Force = false);
		void KillAllThreads(nBool Force = false);
		std::future<WorkToken> QueueWork(WorkFunc workFunc, void* param = nullptr);
		natThread::ThreadIdType GetThreadId(nuInt Index) const;
		void WaitAllJobsFinish(nuInt WaitTime = Infinity);

	private:
		class WorkerThread final
			: public natThread
		{
		public:
			WorkerThread(natThreadPool* pPool, nuInt Index);
			WorkerThread(natThreadPool* pPool, nuInt Index, WorkFunc CallableObj, void* Param = nullptr);
			~WorkerThread() = default;

			WorkerThread(WorkerThread const&) = delete;
			WorkerThread& operator=(WorkerThread const&) = delete;

			nBool IsIdle() const;
			std::future<nuInt> SetWork(WorkFunc CallableObj, void* Param = nullptr);

			void RequestTerminate(nBool value = true);

		private:
			ResultType ThreadJob() override;

			natThreadPool* m_pPool;
			const nuInt m_Index;
			WorkFunc m_CallableObj;
			void* m_Arg;
			std::promise<nuInt> m_LastResult;

			std::atomic<nBool> m_Idle, m_ShouldTerminate;
		};

		class WorkToken final
		{
		public:
			WorkToken() = default;
			WorkToken(nuInt workThreadId, std::future<nuInt>&& result)
				: m_WorkThreadIndex(workThreadId), m_Result(move(result))
			{
			}
			WorkToken(WorkToken&& other)
				: m_WorkThreadIndex(other.m_WorkThreadIndex), m_Result(move(other.m_Result))
			{
			}
			~WorkToken() = default;

			WorkToken& operator=(WorkToken&& other)
			{
				m_WorkThreadIndex = other.m_WorkThreadIndex;
				m_Result = move(other.m_Result);
				return *this;
			}

			nuInt GetWorkThreadIndex() const noexcept
			{
				return m_WorkThreadIndex;
			}

			std::future<nuInt>& GetResult() noexcept
			{
				return m_Result;
			}

		private:
			nuInt m_WorkThreadIndex;
			std::future<nuInt> m_Result;
		};

		nuInt getNextAvailableIndex();
		nuInt getIdleThreadIndex();
		void onWorkerThreadIdle(nuInt Index);

		const nuInt m_MaxThreadCount;
		std::unordered_map<nuInt, std::unique_ptr<WorkerThread>> m_Threads;
		std::queue<std::tuple<WorkFunc, void*, std::promise<WorkToken>>> m_WorkQueue;
		natCriticalSection m_Section;
	};

	///	@}
}

#pragma pop_macro("max")
