#pragma once
#include "stdafx.h"

class HTTP键值对
{
public:
	HashTbe<StrA, StrA> map;

	void 添加(const StrA& key, const StrA& value, bool url解码 = true)
	{
		map[key] = url解码 ? BEdecode::URL(value) : value;
	}

	/**批量加入
	 * @param 文本 如"a=123&b=456"
	 * @param url解码=false
	 */
	void 添加_批量(const StrA& 文本, bool url解码 = true)
	{
		if (文本.len() == 0) return;

		for (BR pair : SplitV<StrA>(文本, "&")) {
			if (pair.size == 0) continue;

			const char* pStart = (const char*)pair.p;
			const char* pEqual = nullptr;
			for (size_t i = 0; i < pair.size; ++i) {
				if (pStart[i] == '=') {
					pEqual = pStart + i;
					break;
				}
			}

			if (pEqual) {
				Bytes k(pStart, BRef, pEqual - pStart);
				Bytes v(pEqual + 1, BRef, pStart + pair.size - pEqual - 1);
				//这里故意做引用的，hash表内部会进行拷贝构造所以依旧是安全的空终止
				添加((StrA&)k, (StrA&)v, url解码);
			} else {
				添加(StrA(pStart, pair.size), "", url解码);
			}
		}
	}

	StrA 取值(const StrA& key)
	{
		StrA* p = map.find(key);
		return p ? *p : StrA("");
	}

	void 获取Key数组(Arraybe<StrA>& arr)
	{
		arr.reset(0);
		for (auto& it : map) {
			arr.push(it.key);
		}
	}

	void 获取Value数组(Arraybe<StrA>& arr)
	{
		arr.reset(0);
		for (auto& it : map) {
			arr.push(it.value);
		}
	}

	void 清空()
	{
		map.clear();
	}
};