#pragma once

#include "natConfig.h"
#include "natRefObj.h"

namespace NatsuLib
{
	namespace RelationalOperator
	{
		////////////////////////////////////////////////////////////////////////////////
		///	@brief	�ȽϽӿ�
		///	@tparam	T	Ҫ�Ƚϵ�����
		////////////////////////////////////////////////////////////////////////////////
		template <typename T>
		struct IComparable
			: natRefObj
		{
			 ///	@brief	��ʾ���������һ����Ĵ�С��ϵ
			 ///	@param	other	Ҫ�ȽϵĶ���
			 ///	@return	���ظ�����ʾ���������һ�����С��0��ʾ��ȣ�������ʾ���󣬾����ֵ���������⺬�壬���÷��ű�ʾ��С��ϵ
			 virtual nInt CompareTo(T const& other) const = 0;
		};

		template <typename T>
		nBool operator==(IComparable<T> const& a, T const& b)
		{
			return a.CompareTo(b) == 0;
		}

		template <typename T>
		nBool operator==(T const& a, IComparable<T> const& b)
		{
			return b == a;
		}

		template <typename T>
		nBool operator!=(IComparable<T> const& a, T const& b)
		{
			return a.CompareTo(b) != 0;
		}

		template <typename T>
		nBool operator!=(T const& a, IComparable<T> const& b)
		{
			return b != a;
		}

		template <typename T>
		nBool operator<(IComparable<T> const& a, T const& b)
		{
			return a.CompareTo(b) < 0;
		}

		template <typename T>
		nBool operator>(IComparable<T> const& a, T const& b)
		{
			return a.CompareTo(b) > 0;
		}

		template <typename T>
		nBool operator<=(IComparable<T> const& a, T const& b)
		{
			return a.CompareTo(b) <= 0;
		}

		template <typename T>
		nBool operator>=(IComparable<T> const& a, T const& b)
		{
			return a.CompareTo(b) >= 0;
		}

		template <typename T>
		nBool operator<(T const& a, IComparable<T> const& b)
		{
			return b > a;
		}

		template <typename T>
		nBool operator>(T const& a, IComparable<T> const& b)
		{
			return b < a;
		}

		template <typename T>
		nBool operator<=(T const& a, IComparable<T> const& b)
		{
			return b >= a;
		}

		template <typename T>
		nBool operator>=(T const& a, IComparable<T> const& b)
		{
			return b <= a;
		}
	}
}
