#include "容器类.h"
#include <CommCtrl.h>

定义_枚举型(网格尺寸模式, char,
	固定 = 0, 比例 = 1
);

定义_枚举型(网格水平对齐, char,
	拉伸 = 0, 居左 = 1, 居中 = 2, 居右 = 3
);

定义_枚举型(网格垂直对齐, char,
	拉伸 = 0, 居顶 = 1, 居中 = 2, 居底 = 3
);

/*
	轨道尺寸规则
	------------
	1. 固定轨道：网格::像素(35) 表示始终占 35 个逻辑像素。
	2. 比例轨道：网格::比例(权重) 的权重必须是正整数，不是百分比，
	   也不是 0~1 的小数。例如 比例(1)、比例(2) 表示剩余空间按 1:2 分配。
	3. 布局顺序：先从客户区扣内边距、行/列间距和全部固定像素；
	   剩余空间才由全部比例轨道按“当前权重 / 权重总和”瓜分。
	4. 因而单独一个 比例(7) 仍会占满所有剩余空间；权重只在存在多个
	   比例轨道时体现相对大小。比例(1)、比例(2) 与 比例(10)、比例(20)
	   的结果完全相同。
	5. 设计器/XML 的对应写法为：固定像素写 35px，比例权重写 1、2、3 等
	   正整数；多条轨道用 '-' 分隔，例如 "35px-1-2-25px"。
*/
struct 网格 {
	网格尺寸模式 模式;
	float 像素值 = 0;
	int 比例权重 = 0;

	static 网格 像素(float vpx) {
		return { 网格尺寸模式::固定, vpx < 0 ? 0 : vpx, 0 };
	}

	// 权重使用 int，避免把比例误解为百分比或小数；非法值安全回退为 1。
	static 网格 比例(int 权重 = 1) {
		return { 网格尺寸模式::比例, 0, 权重 > 0 ? 权重 : 1 };
	}
};

struct 网格边距 {
	float 左 = 0, 顶 = 0, 右 = 0, 底 = 0;
};

class EXP 网格布局
{
public:
	~网格布局() {
		_解绑宿主子类化();
	}

	struct 项 {
		窗口基类* 控件 = nullptr;
		int 行 = 0, 列 = 0;
		int 跨行 = 1, 跨列 = 1;
		网格水平对齐 水平 = 网格水平对齐::拉伸;
		网格垂直对齐 垂直 = 网格垂直对齐::拉伸;
		网格边距 外边距;
	};

	// ========= 绑定宿主 =========
	void 绑定(容器类* 宿主) {
		_解绑宿主子类化();
		_宿主句柄 = 宿主 ? 宿主->窗口句柄 : NULL;
		if (!_宿主句柄) return;
		// 使用系统子类链，不占用 GWLP_USERDATA / GWLP_WNDPROC；白易自己的
		// HWND <-> C++ 对象映射及已有控件子类化完全不受影响。
		_已绑定宿主子类 = SetWindowSubclass(_宿主句柄, _宿主子类过程,
			(UINT_PTR)this, (DWORD_PTR)this) != FALSE;
	}

	void 绑定(HWND 宿主句柄) {
		_解绑宿主子类化();
		_宿主句柄 = 宿主句柄;
	}

	// ========= 间距配置 =========
	网格布局& 置内边距(float 统一) {
		_内边距 = 网格边距({ 统一, 统一, 统一, 统一 });
		return *this;
	}

	网格布局& 置内边距(float 左, float 顶, float 右, float 底) {
		_内边距 = { 左, 顶, 右, 底 };
		return *this;
	}

	网格布局& 置间距(float 统一) {
		_行间距 = 统一;
		_列间距 = 统一;
		return *this;
	}

	网格布局& 置间距(float 行间距, float 列间距) {
		_行间距 = 行间距;
		_列间距 = 列间距;
		return *this;
	}

	// ========= 行列定义 =========
	网格布局& 添加行(网格 尺寸) {
		_行集合.push(尺寸);
		return *this;
	}

	网格布局& 添加列(网格 尺寸) {
		_列集合.push(尺寸);
		return *this;
	}

	网格布局& 设置行(std::initializer_list<网格> rows) {
		_行集合.clear();
		for (auto& r : rows) _行集合.push(r);
		return *this;
	}

	网格布局& 设置列(std::initializer_list<网格> cols) {
		_列集合.clear();
		for (auto& c : cols) _列集合.push(c);
		return *this;
	}

	// ========= 放置控件 =========
	网格布局& 放置(窗口基类& 控件, int 行, int 列, int 跨行 = 1, int 跨列 = 1,
		可空<char> 水平对齐 = nil,
		可空<char> 垂直对齐 = nil,
		网格边距 外边距 = {})
	{
		if (水平对齐 == nil) {
			水平对齐 = 网格水平对齐::拉伸;
		}
		if (垂直对齐 == nil) {
			垂直对齐 = 网格垂直对齐::拉伸;
		}

		项 item;
		item.控件 = &控件;
		item.行 = 行;
		item.列 = 列;
		item.跨行 = 跨行;
		item.跨列 = 跨列;
		item.水平 = (char)水平对齐;
		item.垂直 = (char)垂直对齐;
		item.外边距 = 外边距;
		_项集合.push(item);
		return *this;
	}

	// ========= 取项数量 =========
	int 取项数() { return _项集合.count; }

	// ========= 核心：刷新布局 =========
	void 刷新布局() {
		if (!_宿主句柄 || _项集合.count == 0) return;
		if (_行集合.count == 0 || _列集合.count == 0) return;

		// 1. 取宿主客户区大小（虚拟像素）
		RECT rcClient;
		GetClientRect(_宿主句柄, &rcClient);
		float 宿主宽 = rdpi(rcClient.right);
		float 宿主高 = rdpi(rcClient.bottom);

		// 2. 扣除内边距与间距，求纯内容可分配空间
		float 纯内容宽 = 宿主宽 - (_内边距.左 + _内边距.右);
		if (_列集合.count > 1) 纯内容宽 -= (_列集合.count - 1) * _列间距;

		float 纯内容高 = 宿主高 - (_内边距.顶 + _内边距.底);
		if (_行集合.count > 1) 纯内容高 -= (_行集合.count - 1) * _行间距;

		if (纯内容宽 < 0) 纯内容宽 = 0;
		if (纯内容高 < 0) 纯内容高 = 0;

		// 3. 算出各行高、各列宽
		Arraybe<float> 列宽 = _计算轨道(_列集合, 纯内容宽);
		Arraybe<float> 行高 = _计算轨道(_行集合, 纯内容高);

		// 4. 计算各行列起始坐标（前缀和 + 间距）
		Arraybe<float> 列起点(_列集合.count);
		float curX = _内边距.左;
		for (int c = 0; c < _列集合.count; ++c) {
			列起点[c] = curX;
			curX += 列宽[c] + _列间距;
		}

		Arraybe<float> 行起点(_行集合.count);
		float curY = _内边距.顶;
		for (int r = 0; r < _行集合.count; ++r) {
			行起点[r] = curY;
			curY += 行高[r] + _行间距;
		}

		// 5. 使用 DeferWindowPos 批量原子重排（零闪烁）
		HDWP hdwp = BeginDeferWindowPos(_项集合.count);
		if (!hdwp) return;

		for (auto& item : _项集合) {
			if (!item.控件 || !item.控件->窗口句柄) continue;

			// 边界保护
			if (item.行 < 0 || item.行 >= _行集合.count) continue;
			if (item.列 < 0 || item.列 >= _列集合.count) continue;

			// 计算单元格起始位置
			float cellX = 列起点[item.列];
			float cellY = 行起点[item.行];

			// 计算跨度总尺寸（含跨越的间距）
			float cellW = _合并跨度(列宽, item.列, item.跨列, _列间距);
			float cellH = _合并跨度(行高, item.行, item.跨行, _行间距);

			// 扣除控件自身外边距
			float x = cellX + item.外边距.左;
			float y = cellY + item.外边距.顶;
			float w = cellW - (item.外边距.左 + item.外边距.右);
			float h = cellH - (item.外边距.顶 + item.外边距.底);

			// 根据对齐方式微调
			_应用对齐(item, x, y, w, h, cellX + item.外边距.左, cellY + item.外边距.顶,
				cellW - (item.外边距.左 + item.外边距.右),
				cellH - (item.外边距.顶 + item.外边距.底));

			if (w < 0) w = 0;
			if (h < 0) h = 0;

			int px = dpi(x), py = dpi(y), pw = dpi(w), ph = dpi(h);
			RECT 当前{};
			GetWindowRect(item.控件->窗口句柄, &当前);
			MapWindowPoints(NULL, _宿主句柄, (LPPOINT)&当前, 2);
			if (当前.left == px && 当前.top == py
				&& 当前.right - 当前.left == pw
				&& 当前.bottom - 当前.top == ph) {
				// 矩形未变化时不发送 WINDOWPOS/尺寸消息，避免原生控件无效重绘。
				item.控件->左边 = x; item.控件->顶边 = y;
				item.控件->宽度 = w; item.控件->高度 = h;
				continue;
			}

			HDWP ret = DeferWindowPos(hdwp, item.控件->窗口句柄, NULL,
				px, py, pw, ph,
				SWP_NOZORDER | SWP_NOACTIVATE);
			if (ret) {
				hdwp = ret;
				item.控件->左边 = x; item.控件->顶边 = y;
				item.控件->宽度 = w; item.控件->高度 = h;
			}
		}

		EndDeferWindowPos(hdwp);
	}

	void _解绑宿主子类化()
	{
		if (_已绑定宿主子类 && _宿主句柄 && IsWindow(_宿主句柄))
			RemoveWindowSubclass(_宿主句柄, _宿主子类过程, (UINT_PTR)this);
		_已绑定宿主子类 = false;
	}
private:
	static LRESULT CALLBACK _宿主子类过程(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
		UINT_PTR subclassId, DWORD_PTR refData)
	{
		网格布局* self = (网格布局*)refData;
		if (msg == WM_NCDESTROY) {
			RemoveWindowSubclass(hwnd, _宿主子类过程, subclassId);
			if (self) {
				self->_已绑定宿主子类 = false;
				self->_宿主句柄 = NULL;
			}
			return DefSubclassProc(hwnd, msg, wParam, lParam);
		}

		LRESULT ret = DefSubclassProc(hwnd, msg, wParam, lParam);
		if (msg == WM_SIZE && self && self->_宿主句柄 == hwnd)
			self->刷新布局();
		return ret;
	}

	HWND _宿主句柄 = NULL;
	bool _已绑定宿主子类 = false;
	网格边距 _内边距 = { 8, 8, 8, 8 };
	float _行间距 = 6.0f;
	float _列间距 = 6.0f;
	Arraybe<网格> _行集合;
	Arraybe<网格> _列集合;
	Arraybe<项> _项集合;

	/*
		根据固定/比例模式分配各轨道尺寸。
		- 固定轨道先从总长度扣除；即使固定轨道总和超出可用长度，剩余也只会归零。
		- 比例权重均为正 int。若比例轨道为 {1, 2, 1}，剩余空间按 1/4、2/4、1/4 分配。
		- 这里的“比例”始终针对扣除固定轨道后的剩余空间，不表示总长度百分比。
	*/
	static Arraybe<float> _计算轨道(const Arraybe<网格>& tracks, float 总长度) {
		Arraybe<float> res(tracks.count);
		float 剩余 = 总长度;
		long long 权重和 = 0;

		// 第一轮：扣除固定像素，统计比例权重和
		for (int i = 0; i < tracks.count; ++i) {
			if (tracks[i].模式 == 网格尺寸模式::固定) {
				res[i] = tracks[i].像素值;
				剩余 -= tracks[i].像素值;
			} else {
				权重和 += tracks[i].比例权重;
				res[i] = 0;
			}
		}
		if (剩余 < 0) 剩余 = 0;

		// 第二轮：将剩余空间按权重瓜分给比例轨道
		if (权重和 > 0) {
			for (int i = 0; i < tracks.count; ++i) {
				if (tracks[i].模式 == 网格尺寸模式::比例) {
					res[i] = 剩余 * (static_cast<float>(tracks[i].比例权重)
						/ static_cast<float>(权重和));
				}
			}
		}
		return res;
	}

	// 合并跨度：n 个格子的尺寸 + (n-1) 个间距
	static float _合并跨度(const Arraybe<float>& sizes, int start, int span, float gap) {
		float total = 0.0f;
		int end = start + span;
		if (end > sizes.count) end = sizes.count;
		for (int i = start; i < end; ++i) {
			total += sizes[i];
		}
		if (span > 1) total += (span - 1) * gap;
		return total;
	}

	// 根据对齐方式调整控件的最终位置和尺寸
	static void _应用对齐(const 项& item, float& x, float& y, float& w, float& h,
		float areaX, float areaY, float areaW, float areaH)
	{
		float origW = item.控件->宽度;
		float origH = item.控件->高度;

		// 水平对齐
		switch (item.水平) {
		case 网格水平对齐::居左:
			x = areaX;
			w = origW;
			break;
		case 网格水平对齐::居中:
			w = origW;
			x = areaX + (areaW - origW) / 2;
			break;
		case 网格水平对齐::居右:
			w = origW;
			x = areaX + areaW - origW;
			break;
		default: // 拉伸
			x = areaX;
			w = areaW;
			break;
		}

		// 垂直对齐
		switch (item.垂直) {
		case 网格垂直对齐::居顶:
			y = areaY;
			h = origH;
			break;
		case 网格垂直对齐::居中:
			h = origH;
			y = areaY + (areaH - origH) / 2;
			break;
		case 网格垂直对齐::居底:
			h = origH;
			y = areaY + areaH - origH;
			break;
		default: // 拉伸
			y = areaY;
			h = areaH;
			break;
		}
	}
};
