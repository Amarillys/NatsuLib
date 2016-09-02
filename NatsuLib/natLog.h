////////////////////////////////////////////////////////////////////////////////
///	@file	natLog.h
///	@brief	��־���
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <chrono>
#include <iostream>

#include "natEvent.h"

namespace NatsuLib
{
	////////////////////////////////////////////////////////////////////////////////
	///	@brief	��־��ʵ��
	///	@see	n2dGlobal::Logfile
	////////////////////////////////////////////////////////////////////////////////
	class natLog final
	{
	public:
		class EventLogUpdated final
			: public natEventBase
		{
		public:
			explicit EventLogUpdated(nuInt logType, std::chrono::system_clock::time_point const& time, ncTStr data)
				: m_LogType(logType), m_Time(time), m_Data(data)
			{
			}

			nBool CanCancel() const noexcept override
			{
				return false;
			}

			nuInt GetLogType() const noexcept
			{
				return m_LogType;
			}

			std::chrono::system_clock::time_point const& GetTime() const noexcept
			{
				return m_Time;
			}

			ncTStr GetData() const noexcept
			{
				return m_Data;
			}

		private:
			nuInt m_LogType;
			std::chrono::system_clock::time_point m_Time;
			ncTStr m_Data;
		};

		///	@brief	Ԥ����־����
		enum LogType
		{
			Msg,	///< @brief	��Ϣ
			Err,	///< @brief	����
			Warn	///< @brief	����
		};

		explicit natLog(natEventBus& eventBus);
		~natLog();

		///	@brief	��¼��Ϣ
		template <typename ...Arg>
		void LogMsg(ncTStr content, Arg &&... arg)
		{
			Log(Msg, content, std::forward<Arg>(arg)...);
		}

		///	@brief	��¼����
		template <typename ... Arg>
		void LogErr(ncTStr content, Arg &&... arg)
		{
			Log(Err, content, std::forward<Arg>(arg)...);
		}

		///	@brief	��¼����
		template <typename ... Arg>
		void LogWarn(ncTStr content, Arg &&... arg)
		{
			Log(Warn, content, std::forward<Arg>(arg)...);
		}

		///	@brief	��¼
		template <typename ... Arg>
		void Log(nuInt type, ncTStr content, Arg &&... arg)
		{
			UpdateLog(type, std::move(natUtil::FormatString(content, std::forward<Arg>(arg)...)));
		}

		///	@brief	ע����־�����¼�������
		void RegisterLogUpdateEventFunc(natEventBus::EventListenerDelegate func);

	private:
		struct OutputToOStream
		{
			template <typename... RestChar_t>
			static void Impl(ncTStr str, std::basic_ostream<nChar>& currentOStream, std::basic_ostream<RestChar_t>&... _ostreams)
			{
				currentOStream << natUtil::W2Cstr(str) << std::endl;
				Impl(str, std::forward<std::basic_ostream<RestChar_t>>(_ostreams)...);
			}

			template <typename... RestChar_t>
			static void Impl(ncTStr str, std::basic_ostream<nWChar>& currentOStream, std::basic_ostream<RestChar_t>&... _ostreams)
			{
				currentOStream << str << std::endl;
				Impl(str, std::forward<std::basic_ostream<RestChar_t>>(_ostreams)...);
			}

			static void Impl(ncTStr /*str*/)
			{
			}
		};

	public:
		template <typename... Char_t>
		void UseDefaultAction(std::basic_ostream<Char_t>&... ostreams)
		{
			RegisterLogUpdateEventFunc([&ostreams...](natEventBase& event)
			{
				auto&& eventLogUpdated(static_cast<EventLogUpdated&>(event));
				auto time = std::chrono::system_clock::to_time_t(eventLogUpdated.GetTime());
				tm timeStruct;
				localtime_s(&timeStruct, &time);
				auto logType = static_cast<LogType>(eventLogUpdated.GetLogType());
				auto logStr = natUtil::FormatString(_T("[{0}] [{1}] {2}"), std::put_time(&timeStruct, _T("%F %T")), GetDefaultLogTypeName(logType), eventLogUpdated.GetData());
				switch (logType)
				{
				case Msg:
				case Warn:
					OutputToOStream::Impl(logStr.c_str(), std::wclog, ostreams...);
					break;
				case Err:
					OutputToOStream::Impl(logStr.c_str(), std::wcerr, ostreams...);
					break;
				default:
					break;
				}
			});
		}

		static ncTStr GetDefaultLogTypeName(LogType logtype);

	private:
		void UpdateLog(nuInt type, nTString&& log);
		natEventBus& m_EventBus;
	};
}
