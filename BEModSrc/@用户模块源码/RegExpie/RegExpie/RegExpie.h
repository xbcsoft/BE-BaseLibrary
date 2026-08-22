#pragma once
/**@ModuleTitle: 正则表达式IE版
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description: 仅win32平台上采用IE9组件模拟的正则表达式类，本库并非高性能只为体积小，要求性能建议用C++原生regex
*/
#include "stdafx.h"


#pragma comment(lib, "jsrt.lib")


struct 子匹配结果 {
	Arraybe<StrU8> 子文本;
	Arraybe<int> 子文本位置;
};

using 替换回调函数 = StrU8(*)(const StrU8& 匹配文本, int 匹配文本所在位置, const 子匹配结果& 子匹配, size_t 附加参数);

class RegExpie {
public:
	RegExpie();

	~RegExpie();

	RegExpie(const RegExpie&) = delete;
	RegExpie& operator=(const RegExpie&) = delete;

	RegExpie(RegExpie&& other) noexcept;

	RegExpie& operator=(RegExpie&& other) noexcept;

	bool 创建(const StrU8& 正则文本, bool 是否不区分大小写 = false, const StrU8& 原文本 = StrU8());

	void 置原文本(const StrU8& 原文本);

	bool 匹配();

	int 搜索(int 搜索开始位置 = 0, 可空<StrU8&> 返回匹配文本 = nil, 可空<子匹配结果&> 返回匹配子结果 = nil);

	int 全部搜索(可空<Arraybe<StrU8>&> 返回匹配文本 = nil,
		可空<Arraybe<int>&> 返回匹配文本的位置 = nil,
		可空<Arraybe<子匹配结果>&> 返回子匹配结果 = nil);

	StrU8 替换(const StrU8& 默认替换文本 = "", 替换回调函数 替换回调 = nullptr, int 起始位置 = 0, int 替换次数 = -1, 可空<int&> 返回实际替换的次数 = nil, size_t 附加回调参数四 = 0);

private:
	static StrW _转义正则字符串(const StrW& w);

	// JSRT 的 Runtime/Context/Value 句柄在 SDK 中均为 void*，
	// 使用 ABI 兼容类型，避免生成头文件依赖 <jsrt.h>。
	void* m_runtime = nullptr;
	void* m_context = nullptr;
	StrU8 m_srcText;

	void* _Eval(const StrW& code);

	bool _IsNullOrUndefined(void* val);

	StrW _ToString(void* val);

	double _ToNumber(void* val);

	bool _ToBool(void* val);

	void* _GetProperty(void* obj, const StrW& propName);

	void* _GetIndexedProperty(void* obj, int index);

	int _GetArrayLength(void* arr);
};
