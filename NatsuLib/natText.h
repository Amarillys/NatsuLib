#pragma once
#include "natConfig.h"
#include "natString.h"
#include "natRefObj.h"

namespace NatsuLib
{
	template <StringType encoding>
	struct TextReader
		: natRefObj
	{
		typedef typename StringEncodingTrait<encoding>::CharType CharType;

		// ���Ǽ���һ��nuInt���������κ�NatsuLib֧�ֵı���ĵ�����Ч�ַ�
		// ע�⣺�������һ��CodePoint������raw���ַ����
		virtual nBool Read(nuInt& Char) = 0;
		virtual nBool Peek(nuInt& Char) = 0;

		virtual void SetNewLine(StringView<encoding> const& newLine)
		{
			m_NewLine.clear();
			EncodingResult result;
			nuInt codePoint;
			const auto charCount = newLine.GetCharCount();
			for (size_t i = 0; i < charCount; ++i)
			{
				std::tie(result, std::ignore) = detail_::EncodingCodePoint<encoding>::Decode(newLine.Slice(i, -1), codePoint);
				if (result != EncodingResult::Accept)
				{
					nat_Throw(natException, "Not an available new line string."_nv);
				}

				m_NewLine.push_back(codePoint);
			}
		}

		virtual String<encoding> ReadUntil(std::initializer_list<nuInt> terminatorChars)
		{
			nuInt currentChar;
			String<encoding> result;
			while (Read(currentChar)
				&& std::find(terminatorChars.begin(), terminatorChars.end(), currentChar) == terminatorChars.end()
				&& detail_::EncodingCodePoint<encoding>::Encode(result, currentChar) == EncodingResult::Accept)
			{
			}
			return result;
		}

		virtual String<encoding> ReadLine()
		{
			nuInt currentChar;
			nuInt matchedLength{};
			String<encoding> result;
			while (matchedLength < m_NewLine.size() && Read(currentChar))
			{
				if (currentChar == m_NewLine[matchedLength])
				{
					++matchedLength;
				}
				else
				{
					matchedLength = 0;
				}

				if (detail_::EncodingCodePoint<encoding>::Encode(result, currentChar) != EncodingResult::Accept)
				{
					break;
				}
			}
			return result;
		}

		virtual String<encoding> ReadToEnd()
		{
			return ReadUntil({});
		}

	protected:
		TextReader()
			: m_NewLine{ '\n' }
		{
		}

		std::vector<nuInt> m_NewLine;
	};

	template <StringType encoding>
	struct TextWriter
		: natRefObj
	{
		typedef typename StringEncodingTrait<encoding>::CharType CharType;

		virtual nBool Write(nuInt Char) = 0;

		virtual void SetNewLine(StringView<encoding> const& newLine)
		{
			EncodingResult result;
			std::tie(result, std::ignore) = detail_::EncodingCodePoint<encoding>::Decode(newLine, m_NewLine);
			if (result != EncodingResult::Accept)
			{
				nat_Throw(natException, "Not an available new line string."_nv);
			}
		}

		virtual size_t Write(StringView<encoding> const& str)
		{
			size_t writtenChars{};
			nuInt codePoint;
			EncodingResult result;
			size_t moveStep;
			size_t currentPos{};

			while (true)
			{
				std::tie(result, moveStep) = detail_::EncodingCodePoint<encoding>::Decode(str.Slice(static_cast<std::ptrdiff_t>(currentPos), -1), codePoint);
				
				if (result != EncodingResult::Accept || !Write(codePoint))
				{
					break;
				}

				currentPos += moveStep;
				++writtenChars;
			}

			return writtenChars;
		}

		virtual size_t WriteLine(StringView<encoding> const& str)
		{
			const auto size = Write(str);
			return size + Write(m_NewLine) ? 1 : 0;
		}

	protected:
		TextWriter()
			: m_NewLine{ '\n' }
		{
		}

		nuInt m_NewLine;
	};
}
