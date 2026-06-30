#pragma once
#include "句柄管理.h"
#include "窗口辅助核心.h"

struct 字体
{
	StrW 字体名称;
	int 字体大小 = 9; //pt为单位
	bool 下划线 = false;
	bool 删除线 = false;
	bool 倾斜 = false;
	bool 加粗 = false;
	int 角度 = 0;

	字体();
	字体(可空<c_StrW> 字体名称, int 字体大小 = 9, bool 下划线 = false, bool 删除线 = false,
		bool 倾斜 = false, bool 加粗 = false, int 角度 = 0);

	int _字体像素; //-px为单位，内部使用
	//注：该px是字体实际字形对应的原生设备像素（若计算整体行距的情况不要使用此值，还需加上内部行距）

	static LOGFONT 获取默认逻辑字体();

	/**到字体句柄
	 * @return 务必使用HandleFONT去接
	 */
	HandleFONT 到字体句柄() const;

	void 从LOGFONT创建(LOGFONT& lf);
	LOGFONT 到LOGFONT() const;

	/**取字体高度（虚拟单位）
	 * @param 是否为净高=false 为假则为包括字体内行距在内的高度，为真则净高度（字体大小的像素版）
	 * @return
	 */
	int 取字体高度(bool 是否为净高 = true) const;

	template <class D>
	int 取字体高度(bool 是否为净高 = true) const {
		return _取字体高度(是否为净高);
	}

	int _取字体高度(bool 是否为净高 = true) const;

	static int 取字体高度(HFONT hFont, bool 是否为净高 = true);

	template <class D>
	static int 取字体高度(HFONT hFont, bool 是否为净高 = true) {
		return _取字体高度(hFont, 是否为净高);
	}

	static int _取字体高度(HFONT hFont, bool 是否为净高 = true);
	static HDC _hdc;
};

HFONT 窗口_取字体句柄(HWND hwnd);
字体 窗口_取字体(HWND hwnd);

struct 文本测量 {
	HandleCDC hdc;
	HFONT hfont; HFONT hOld;
	文本测量(HFONT hfont);
	文本测量(const 字体& font);
	SIZEF 测量(const StrW& text);

	template <class D>
	SIZE 测量(const StrW& text) {
		return _测量(text);
	}

	SIZE _测量(const StrW& text);
	~文本测量();
};
