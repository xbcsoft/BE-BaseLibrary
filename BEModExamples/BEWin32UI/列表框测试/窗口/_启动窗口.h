#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();

	void 事件_尺寸被改变();

	void _更新状态提示();

	void _列表框1_列表项被选择();

	void _列表框1_双击选择();

	void _添加按钮_被单击();

	void _删除按钮_被单击();

	void _清空按钮_被单击();

	void _多选切换按钮_被单击();

	void _排序切换按钮_被单击();

	void _颜色切换按钮_被单击();

	void _取选中列表按钮_被单击();

#pragma region 组件成员
	struct _列表框1 : 列表框 {
		void 事件_列表项被选择();
		void 事件_双击选择();
	} 列表框1;

	标签 提示标签;

	struct _添加按钮 : 按钮 {
		void 事件_被单击();
	} 添加按钮;

	struct _删除按钮 : 按钮 {
		void 事件_被单击();
	} 删除按钮;

	struct _清空按钮 : 按钮 {
		void 事件_被单击();
	} 清空按钮;

	struct _多选切换按钮 : 按钮 {
		void 事件_被单击();
	} 多选切换按钮;

	struct _排序切换按钮 : 按钮 {
		void 事件_被单击();
	} 排序切换按钮;

	struct _颜色切换按钮 : 按钮 {
		void 事件_被单击();
	} 颜色切换按钮;

	struct _取选中列表按钮 : 按钮 {
		void 事件_被单击();
	} 取选中列表按钮;
#pragma endregion

	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
