////////////////////////////////////////////////////////////////////////////////
///	@file	natStopWatch.h
///	@brief	��ʱ��
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "natConfig.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <chrono>
#endif

namespace NatsuLib
{
	////////////////////////////////////////////////////////////////////////////////
	///	@addtogroup	����
	///	@{

	////////////////////////////////////////////////////////////////////////////////
	///	@brief	�߾���ͣ����
	////////////////////////////////////////////////////////////////////////////////
	class natStopWatch final
	{
	public:
		natStopWatch();

		///	@brief	��ͣ
		void Pause();

		///	@brief	����
		void Resume();

		///	@brief	����
		void Reset();

		///	@brief	�������ʱ��
		///	@note	��λΪ��
		nDouble GetElpased() const;
	private:
#ifdef WIN32
		LARGE_INTEGER	m_cFreq,	///< @brief	cpuƵ��
			m_cLast,	///< @brief	��һ��ʱ��
			m_cFixStart,///< @brief	��ͣʱ�����޸��Ĳ���
			m_cFixAll;	///< @brief	��ͣʱ�����޸��Ĳ���
#else
		std::chrono::high_resolution_clock::time_point m_Last, m_FixStart, m_FixAll;
#endif
	};

	///	@}
}
