#pragma once
#include <BEMod.h>

struct _窗口00_JS基础 : 窗口
{
	SciterDom dom;
	void 事件_创建完毕();

	void 方法调用测试();

	void 字节集演示();


	//《实验结果》（按理来说T_MAP传入JS后会进行一次深拷贝，但JS对象的构造特别是多级时开销更大）：
	//【三级 MAP(T_MAP) 构造+投入】耗时 = | 2250 |  ms
	//【三级 JS对象(T_OBJECT) 构造+投入】耗时 = | 14016 |  ms
	void 实验_传MAP对象跟JS对象哪个性能好();

	void 多参的使用();

	void 数组的引用();

	void 对象的引用();

#pragma region 组件成员
	struct _st :SciterUI {
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern _窗口00_JS基础 窗口00_JS基础;
