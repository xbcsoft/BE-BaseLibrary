#pragma once
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

class 网格布局
{
public:
	~网格布局();

	struct 项 {
		窗口基类* 控件 = nullptr;
		int 行 = 0, 列 = 0;
		int 跨行 = 1, 跨列 = 1;
		网格水平对齐 水平 = 网格水平对齐::拉伸;
		网格垂直对齐 垂直 = 网格垂直对齐::拉伸;
		网格边距 外边距;
	};

	// ========= 绑定宿主 =========
	void 绑定(容器类* 宿主);

	void 绑定(HWND 宿主句柄);

	// ========= 间距配置 =========
	网格布局& 置内边距(float 统一);

	网格布局& 置内边距(float 左, float 顶, float 右, float 底);

	网格布局& 置间距(float 统一);

	网格布局& 置间距(float 行间距, float 列间距);

	// ========= 行列定义 =========
	网格布局& 添加行(网格 尺寸);

	网格布局& 添加列(网格 尺寸);

	网格布局& 设置行(std::initializer_list<网格> rows);

	网格布局& 设置列(std::initializer_list<网格> cols);

	// ========= 放置控件 =========
	网格布局& 放置(窗口基类& 控件, int 行, int 列, int 跨行 = 1, int 跨列 = 1,
		可空<char> 水平对齐 = nil,
		可空<char> 垂直对齐 = nil,
		网格边距 外边距 = {});

	// ========= 取项数量 =========
	int 取项数();

	// ========= 核心：刷新布局 =========
	void 刷新布局();

	void _解绑宿主子类化();
private:
	static LRESULT CALLBACK _宿主子类过程(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
		UINT_PTR subclassId, DWORD_PTR refData);

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
	static Arraybe<float> _计算轨道(const Arraybe<网格>& tracks, float 总长度);

	// 合并跨度：n 个格子的尺寸 + (n-1) 个间距
	static float _合并跨度(const Arraybe<float>& sizes, int start, int span, float gap);

	// 根据对齐方式调整控件的最终位置和尺寸
	static void _应用对齐(const 项& item, float& x, float& y, float& w, float& h,
		float areaX, float areaY, float areaW, float areaH);
};
