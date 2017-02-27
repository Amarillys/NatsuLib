#pragma once

#include "natRefObj.h"
#include <functional>

namespace NatsuLib
{
	struct natNode
		: natRefObj
	{
		virtual void AddChild(natNode* pChild) = 0;
		virtual nBool ChildExists(natNode* pChild) const noexcept = 0;
		virtual nBool ChildExists(nStrView Name) const noexcept = 0;
		virtual natNode* GetChildByName(nStrView Name) const = 0;

		template <typename T>
		T* GetChildByName(nStrView Name) const
		{
			return dynamic_cast<T*>(GetChildByName(Name));
		}

		/**
		* @brief ö���ӽڵ�
		* @param[in] Recursive �ݹ��ö��
		* @param[in] EnumCallback ö�ٻص�����������һ��������������ΪnatNode*����ʾ�˴�ö�ٵ��Ľڵ㣬����ֵΪbool������trueʱö��������ֹ
		* @return �Ƿ���ΪEnumCallback����true����ֹö��
		*/
		virtual nBool EnumChildNode(nBool Recursive, std::function<nBool(natNode*)> EnumCallback) = 0;

		virtual size_t GetChildCount() const noexcept = 0;
		virtual void SetParent(natNode* pParent) = 0;
		virtual natNode* GetParent() const noexcept = 0;
		virtual void RemoveChild(natNode* pnatNode) = 0;
		virtual void RemoveChildByName(nStrView Name) = 0;
		virtual void RemoveAllChild() = 0;

		virtual void SetName(nStrView Name) = 0;
		virtual nStrView GetName() const noexcept = 0;
	};
}
