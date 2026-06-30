#include "子容器.hpp"
#include "helper/位图.h"

定义_枚举型(图片框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5, 渐变镜框式 = 6
);

定义_枚举型(图片框显示方式, char,
	图片居左上 = 0, 缩放图片 = 1, 居中显示 = 2
);

class EXP 图片框 : public 子容器<true>
{
public:
	struct 参数 : public 子容器<true>::参数 {
		定义_子组件通用构造方法;
		Bytes 图片;
		bool 是否贮存图片 = false;
		图片框边框 边框 = 图片框边框::无边框;
		颜色 背景颜色 = 颜色::默认底色;
		图片框显示方式 显示方式 = 图片框显示方式::图片居左上;
	} static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL) {
		if (!子容器<true>::创建(cs, 父窗口, 父句柄)) return false;

		// 动态添加 SS_NOTIFY 风格，使 STATIC 控件能接收并触发鼠标消息
		LONG_PTR style = GetWindowLongPtrW(窗口句柄, GWL_STYLE);
		SetWindowLongPtrW(窗口句柄, GWL_STYLE, style | SS_NOTIFY);

		边框 = cs.边框;
		显示方式 = cs.显示方式;
		是否贮存图片_ = cs.是否贮存图片;
		
		背景颜色_(cs.背景颜色, false);
		if (cs.图片) {
			置图片(cs.图片);
		}
		容器类::_子类化();
		return true;
	}

	/**在控件上显示位图
	 * @param imgData 仅支持bmp,jpg,gif(但不支持动图，且也不支持gif自动透明)
	 * @param 是否贮存图片=false 是否在图片框内存储一份原始字节集（半公开成员为 _图片）
	 */
	void 置图片(const Bytes& imgData) {
		if (是否贮存图片_) {
			_图片 = imgData;
		} else {
			_图片.reset();
		}

		_hdcMem.init(); _hdcMask.init();
		_掩码句柄.destroy();

		_图片句柄 = 位图::从内存创建(imgData, _掩码句柄);

		if (_掩码句柄) {
			_hdcMask.select(_掩码句柄);
		} else {
			_hdcMask.destroy();
		}

		if (_图片句柄) {
			SIZE sz = 位图::取尺寸(_图片句柄);
			_图片宽度 = sz.cx; _图片高度 = sz.cy;
			_hdcMem.select(_图片句柄);
		} else {
			_图片宽度 = 0; _图片高度 = 0;
			_hdcMem.destroy(); _hdcMask.destroy();
		}

		重画();
	}

	/**在控件上显示位图
	 * @param hBmp 位图句柄
	 * @param maskBmp 掩码位图句柄，如果投入则按照此掩码进行背景透明
	 */
	void 置图片(HBITMAP hBmp, HBITMAP maskBmp = NULL) {
		_图片.reset();

		_hdcMem.init(); _hdcMask.init();
		_图片句柄.destroy();
		_掩码句柄.destroy();

		_图片句柄 = (HBITMAP)CopyImage(hBmp, IMAGE_BITMAP, 0, 0, 0);
		_掩码句柄 = (HBITMAP)CopyImage(maskBmp, IMAGE_BITMAP, 0, 0, 0);

		if (_掩码句柄) {
			_hdcMask.select(_掩码句柄);
		} else {
			_hdcMask.destroy();
		}

		if (_图片句柄) {
			SIZE sz = 位图::取尺寸(_图片句柄);
			_图片宽度 = sz.cx; _图片高度 = sz.cy;
			_hdcMem.select(_图片句柄);
		} else {
			_图片宽度 = 0; _图片高度 = 0;
			_hdcMem.destroy(); _hdcMask.destroy();
		}

		重画();
	}

	void 边框_(图片框边框 bor) {
		边框 = bor;
		重画();
	}

	void 显示方式_(图片框显示方式 mode) {
		显示方式 = mode;
		重画();
	}

public:
	bool 通用事件_窗口重画(PAINTSTRUCT& ps);
public:
	图片框边框 边框;
	定义_背景颜色成员;
	图片框显示方式 显示方式;
	Bytes _图片;
	bool 是否贮存图片_;

private:
	HandleBITMAP _图片句柄;
	HandleBITMAP _掩码句柄;
	HandleCDC _hdcMem;
	HandleCDC _hdcMask;
	int _图片宽度 = 0;
	int _图片高度 = 0;
};

bool 图片框::通用事件_窗口重画(PAINTSTRUCT& ps) {
	HDC hdc = BeginPaint(窗口句柄, &ps);
	RECT rc; GetClientRect(窗口句柄, &rc);

	// 1. 绘制背景
	if (背景颜色 != 颜色::透明) {
		FillRect(hdc, &rc, _背景画刷);
	}

	// 2. 绘制图片
	if (_图片句柄) {
		HDC hdcMem = _hdcMem.hdc;
		HDC hdcMask = _掩码句柄 ? _hdcMask.hdc : NULL;

		// 获取去掉边框后的实际绘制区域
		RECT rcDraw = rc;
		if (边框 != 图片框边框::无边框) {
			int offset = 2; // 默认缩进
			if (边框 == 图片框边框::浅凹入式 || 边框 == 图片框边框::单线边框式) {
				offset = 1;
			}
			InflateRect(&rcDraw, -dpi(offset), -dpi(offset));
		}

		int drawX = rcDraw.left;
		int drawY = rcDraw.top;
		int drawW = rcDraw.right - rcDraw.left;
		int drawH = rcDraw.bottom - rcDraw.top;

		if (显示方式 == 图片框显示方式::居中显示) {
			drawX = rcDraw.left + (drawW - _图片宽度) / 2;
			drawY = rcDraw.top + (drawH - _图片高度) / 2;
			drawW = _图片宽度;
			drawH = _图片高度;
		} else if (显示方式 == 图片框显示方式::图片居左上) {
			drawW = _图片宽度;
			drawH = _图片高度;
		}

		BITMAP bm = {};
		GetObject(_图片句柄, sizeof(bm), &bm);
		bool hasAlpha = false;
		if (bm.bmBitsPixel == 32 && bm.bmBits) {
			DWORD* p = (DWORD*)bm.bmBits;
			int n = bm.bmWidth * bm.bmHeight;
			for (int i = 0; i < n; ++i) {
				if (p[i] & 0xFF000000) {
					hasAlpha = true;
					break;
				}
			}
		}

		typedef BOOL(WINAPI* LPFN_ALPHABLEND)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
		static LPFN_ALPHABLEND pfnAlphaBlend = (LPFN_ALPHABLEND)GetProcAddress(LoadLibraryW(L"msimg32.dll"), "AlphaBlend");

		if (hasAlpha && pfnAlphaBlend) {
			BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			pfnAlphaBlend(hdc, drawX, drawY, drawW, drawH, hdcMem, 0, 0, _图片宽度, _图片高度, bf);
		} else if (hdcMask) {
			// 【完美透明算法】: 利用 GDI 光栅运算瞬间完成背景透明
			// 等价公式： 目标 = (目标 AND 掩码) OR (源 AND NOT 掩码)
			// (掩码约定：1/白=透明，0/黑=不透明)

			// 1. 准备纯净源图 (透明区域强制为黑，不受任何原底色干扰)
			//    使用三元光栅码 0x00220326 (DSna = D AND NOT S)
			//    直接实现 源图 AND NOT(掩码)，无需单独创建反色掩码DC
			HandleCDC hdcTemp; hdcTemp.init();
			HBITMAP hbmTemp = CreateCompatibleBitmap(hdc, _图片宽度, _图片高度);
			HBITMAP hOldTemp = (HBITMAP)SelectObject(hdcTemp.hdc, hbmTemp);

			BitBlt(hdcTemp.hdc, 0, 0, _图片宽度, _图片高度, hdcMem, 0, 0, SRCCOPY);
			BitBlt(hdcTemp.hdc, 0, 0, _图片宽度, _图片高度, hdcMask, 0, 0, 0x00220326); // DSna

			// 2. 正式绘制到屏幕
			if (显示方式 == 图片框显示方式::缩放图片) {
				SetStretchBltMode(hdc, HALFTONE);
				// (目标 AND 掩码) ：在背景上"挖洞"
				StretchBlt(hdc, drawX, drawY, drawW, drawH, hdcMask, 0, 0, _图片宽度, _图片高度, SRCAND);
				// 叠加纯净源图 ：填充图像
				StretchBlt(hdc, drawX, drawY, drawW, drawH, hdcTemp.hdc, 0, 0, _图片宽度, _图片高度, SRCPAINT);
			} else {
				BitBlt(hdc, drawX, drawY, drawW, drawH, hdcMask, 0, 0, SRCAND);
				BitBlt(hdc, drawX, drawY, drawW, drawH, hdcTemp.hdc, 0, 0, SRCPAINT);
			}

			// 清理临时资源
			SelectObject(hdcTemp.hdc, hOldTemp); DeleteObject(hbmTemp); hdcTemp.destroy();
		} else {
			// 无掩码，直接拷贝
			if (显示方式 == 图片框显示方式::缩放图片) {
				SetStretchBltMode(hdc, HALFTONE);
				StretchBlt(hdc, drawX, drawY, drawW, drawH, hdcMem, 0, 0, _图片宽度, _图片高度, SRCCOPY);
			} else {
				BitBlt(hdc, drawX, drawY, drawW, drawH, hdcMem, 0, 0, SRCCOPY);
			}
		}
	}

	// 3. 绘制边框 (在图片上方绘制,避免图片遮挡内边框)
	if (边框 == 图片框边框::凹入式) {
		DrawEdge(hdc, &rc, EDGE_SUNKEN, BF_RECT);
	} else if (边框 == 图片框边框::浅凹入式) {
		DrawEdge(hdc, &rc, BDR_SUNKENOUTER, BF_RECT);
	} else if (边框 == 图片框边框::单线边框式) {
		HBRUSH hBr = CreateSolidBrush(RGB(128, 128, 128));
		FrameRect(hdc, &rc, hBr);
		DeleteObject(hBr);
	} else if (边框 == 图片框边框::镜框式) {
		DrawEdge(hdc, &rc, EDGE_ETCHED, BF_RECT);
	} else if (边框 == 图片框边框::凸出式) {
		DrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT);
	}

	EndPaint(窗口句柄, &ps);
	return false;
}
