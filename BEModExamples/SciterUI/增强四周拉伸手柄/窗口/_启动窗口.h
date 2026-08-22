#pragma once
#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕();

#pragma region 组件成员
	struct _st :SciterUI {
		/** 该事件是为了让拉伸的范围变得更大更跟手！
		 *
		 * @param x, y               鼠标在窗口内部的相对坐标 (0 <= x <= w, 0 <= y <= h)
		 * @param w, h               窗口当前的实际宽度与高度
		 * @param inLeft, inRight    默认计算出的左/右窄边热区命中状态
		 * @param inTop, inBottom    默认计算出的上/下窄边热区命中状态
		 * @param inCornerLeft       默认计算出的左侧角区 (左上/左下) 命中状态
		 * @param inCornerRight      默认计算出的右侧角区 (右上/右下) 命中状态
		 * @param inCornerTop        默认计算出的顶部角区 (左上/右上) 命中状态
		 * @param inCornerBottom     默认计算出的底部角区 (左下/右下) 命中状态
		 *
		 * @return 可空<char>        返回 nil 表示不进行自定义，沿用系统默认判定；
		 *                           返回具体的 Win32 HitTest 码 (如 HTTOPLEFT=13, HTRIGHT=11, HTCLIENT=1 等)
		 *                           则直接作为 WM_NCHITTEST 的响应返回值。
		 */
		可空<char> 事件_窗口外围被拉伸(int x, int y, int w, int h, bool inLeft, bool inRight, bool inTop, bool inBottom, bool inCornerLeft, bool inCornerRight, bool inCornerTop, bool inCornerBottom);
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
