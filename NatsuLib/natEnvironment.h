#pragma once
#include "natConfig.h"
#include "natString.h"

namespace NatsuLib
{
	namespace Environment
	{
		nStrView GetNewLine();

		enum class Endianness
		{
			BigEndian,
			LittleEndian,
			MiddleEndian,	// ����֧�֣�ֻ�Ƿ����������
		};

		Endianness GetEndianness();

		nString GetEnvironmentVar(nStrView name);
		void SetEnvironmentVar(nStrView name, nStrView value);
	}
}
