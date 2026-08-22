#include <BEMod.h>

struct __启动窗口 : 窗口
{
	void 事件_创建完毕()
	{
		//SciterUI\MahApps.Metro\@可复用组件拆分
		//建议让AI参考此文件夹提取自己想要的组件
	}

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
		可空<char> 事件_窗口外围被拉伸(int x, int y, int w, int h, bool inLeft, bool inRight, bool inTop, bool inBottom, bool inCornerLeft, bool inCornerRight, bool inCornerTop, bool inCornerBottom)
		{
			// 【示范】：将拉伸热区向窗口内部进一步延伸放大（例如内部扩展 15~25px），让鼠标更容易抓取拉伸
			int innerMargin = dpi(10);   // 四条边拉伸热区向内延伸 10px
			int cornerSize = dpi(25);   // 四个角斜向拉伸大热区扩展为 25x25px

			// 1. 优先判定四个角（向内扩展到 25px 区域）
			if (x >= 0 && x < cornerSize && y >= 0 && y < cornerSize)
				return (char)HTTOPLEFT;
			if (x >= w - cornerSize && x < w && y >= 0 && y < cornerSize)
				return (char)HTTOPRIGHT;
			if (x >= 0 && x < cornerSize && y >= h - cornerSize && y < h)
				return (char)HTBOTTOMLEFT;
			if (x >= w - cornerSize && x < w && y >= h - cornerSize && y < h)
				return (char)HTBOTTOMRIGHT;

			// 2. 判定四条边（向内扩展到 15px 区域）
			if (x >= 0 && x < innerMargin)
				return (char)HTLEFT;
			if (x >= w - innerMargin && x < w)
				return (char)HTRIGHT;
			if (y >= 0 && y < innerMargin)
				return (char)HTTOP;
			if (y >= h - innerMargin && y < h)
				return (char)HTBOTTOM;

			// 3. 其余内部区域返回 nil，表示无需额外定制，沿用默认逻辑
			return nil;
		}
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;
