#pragma once
#include "句柄管理.h"
#include <CommCtrl.h>
#include <OleCtl.h>

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"ole32.lib")

struct EXP 颜色 {
	union {
		struct { //内存排布是r在最低位、g,b在第17,24位
			byte r;
			byte g;
			byte b;
			byte _unused; //对齐COLORREF
		};
		COLORREF val;
	};
	operator COLORREF()const
	{
		return val;
	}

	颜色() : val(0) {}
	颜色(COLORREF c) : val(c) {}
	颜色(byte r, byte g, byte b)
		: r(r), g(g), b(b), _unused(0) {}

	enum {
		默认底色 = RGB(240, 240, 240),
		黑色 = 0,
		藏青 = 8388608,
		墨绿 = 32768,
		深青 = 8421376,
		红褐 = 128,
		紫红 = 8388736,
		褐绿 = 32896,
		浅灰 = 12632256,
		灰色 = 8421504,
		蓝色 = 16711680,
		绿色 = 65280,
		艳青 = 16776960,
		红色 = 255,
		品红 = 16711935,
		黄色 = 65535,
		白色 = 16777215,
		蓝灰 = 16744576,
		藏蓝 = 14692440,
		嫩绿 = 57472,
		青绿 = 8445952,
		黄褐 = 24768,
		粉红 = 16754943,
		嫩黄 = 55512,
		银白 = 15527148,
		紫色 = 16711824,
		天蓝 = 16746496,
		灰绿 = 8429696,
		青蓝 = 12607488,
		橙黄 = 33023,
		桃红 = 8409343,
		芙红 = 12615935,
		深灰 = 6316128,
		透明 = -1,
	};
};


//白易1.0核心库暂不实现GDI+ ARGB和PNG图像的支持（请移至GdiPlus模块）
//struct EXP 真彩色{
//	union {
//		struct { //内存排布是b在最低位、a在最高位
//			byte b;
//			byte g;
//			byte r;
//			byte a;
//		};
//		DWORD val;
//	};	operator DWORD(){ return val; }
//
//	真彩色(DWORD c) : val(c) {} //从GDI+色那边直接赋值
//	真彩色() : r(r), g(g), b(b), a(255) {}
//	真彩色(byte r, byte g, byte b, byte a = 255)
//		: r(r), g(g), b(b), a(a) {}
//};
//DWORD RGBToBGRA(COLORREF c, byte a = 255){
//	byte r = (byte)(c & 0xFF);
//	byte g = (byte)((c >> 8) & 0xFF);
//	byte b = (byte)((c >> 16) & 0xFF);
//	return ((DWORD)a << 24) |
//		((DWORD)b << 16) |((DWORD)g << 8)  |((DWORD)r);
//}

struct EXP 位图
{
	HBITMAP h;
	bool _接管生命周期;

	位图() { _接管生命周期 = false; }

	/**位图
	 * @param h
	 * @param 是否接管生命周期=false 如果接管则本对象销毁时将位图句柄销毁
	 */
	位图(HBITMAP h, bool 是否接管生命周期 = false) {
		挂接(h, 是否接管生命周期);
	}

	/**位图 移动构造必须接管
	 * @param hbp
	 * @param 是否接管生命周期=true 一定接管
	 */
	位图(HandleBITMAP&& hbp) {
		挂接((HandleBITMAP&&)hbp);
	}

	~位图() {
		销毁();
	}
	void 挂接(HBITMAP h, bool 是否接管生命周期 = false) {
		this->h = h; _接管生命周期 = 是否接管生命周期;
	}
	void 挂接(HandleBITMAP&& hbp) {
		h = hbp._move(); _接管生命周期 = true;
	}
	void 销毁() { if (_接管生命周期)DeleteObject(h); }
	operator HBITMAP() { return h; }

	/**将位图原位缩放到指定尺寸（高质量缩放，旧句柄自动释放）
	 * @param 目标宽度
	 * @param 目标高度
	 */
	void 缩放(float 目标宽度, float 目标高度, 位图& 目标) {
		if (!h || 目标宽度 <= 0 || 目标高度 <= 0) return;
		SIZE sz = 取尺寸();
		if (sz.cx <= 0 || sz.cy <= 0) return;

		HandleCDC s(0), d(0);
		s.select(h);
		HandleBITMAP target = CreateCompatibleBitmap(s, (int)目标宽度, (int)目标高度);
		d.select(target);

		BITMAP bm = {};
		GetObject(h, sizeof(bm), &bm);
		int stretchMode = (bm.bmBitsPixel <= 8) ? COLORONCOLOR : HALFTONE;
		SetStretchBltMode(d, stretchMode);
		if (stretchMode == HALFTONE) {
			SetBrushOrgEx(d, 0, 0, 0);
		}

		if (!StretchBlt(d, 0, 0, (int)目标宽度, (int)目标高度, s, 0, 0, sz.cx, sz.cy, SRCCOPY)) {
			return;
		}

		目标.销毁();
		目标.挂接((HandleBITMAP&&)target);
	}

	/**取尺寸 无参的则是成员方法，有参的则是静态方法
	 * @return
	 */
	SIZE 取尺寸() {
		return 取尺寸(h);
	}

	static SIZE 取尺寸(HBITMAP hBmp) {
		BITMAP bm = {}; GetObject(hBmp, sizeof(bm), &bm);
		return { bm.bmWidth, bm.bmHeight };
	}

	/**从资源载入位图（仅支持BMP）
	 * @param hInstance
	 * @param uResourceID 资源ID
	 * @return
	 */
	static HandleBITMAP 从资源创建(HINSTANCE hInstance, UINT uResourceID)
	{
		return (HBITMAP)LoadImageW(hInstance, MAKEINTRESOURCEW(uResourceID),
			IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	}

	/**从内存创建
	 * @param data
	 * @param 导出掩码位图<可空>=nil 仅
	 * @return
	 */
	static HandleBITMAP 从内存创建(const Bytes& data, 可空<HandleBITMAP&> 导出掩码位图 = nil)
	{
		if (data.size <= 0) return NULL;

		// 检查是否为 ICO 或 CUR 格式
		struct ICONDIR {
			WORD idReserved; // 必须为0
			WORD idType;     // 1=ICON, 2=CURSOR
			WORD idCount;    // 图标/光标数量
		};
		struct ICONDIRENTRY {
			BYTE bWidth;
			BYTE bHeight;
			BYTE bColorCount;
			BYTE bReserved;
			WORD wPlanes;
			WORD wBitCount;
			DWORD dwBytesInRes;
			DWORD dwImageOffset;
		};

		if (data.size >= (int)sizeof(ICONDIR))
		{
			const ICONDIR* dir = (const ICONDIR*)data.buf;
			if (dir->idReserved == 0 && (dir->idType == 1 || dir->idType == 2))
			{
				if (data.size >= (int)(sizeof(ICONDIR) + dir->idCount * sizeof(ICONDIRENTRY)))
				{
					const ICONDIRENTRY* entries = (const ICONDIRENTRY*)(data.buf + sizeof(ICONDIR));
					const ICONDIRENTRY* best = nullptr;
					int bestDiff = 0x7fffffff;

					for (int i = 0; i < dir->idCount; ++i)
					{
						int w = entries[i].bWidth == 0 ? 256 : entries[i].bWidth;
						int diff = -w;  // 选择最大图像

						if (!best || diff < bestDiff)
						{
							best = &entries[i];
							bestDiff = diff;
						}
					}

					if (best && best->dwImageOffset + best->dwBytesInRes <= (DWORD)data.size)
					{
						int w = (best->bWidth == 0) ? 256 : best->bWidth;

						HICON hicon = NULL;
						if (dir->idType == 2) // 光标 (CURSOR)
						{
							// 光标数据必须在资源 bits 前缀 4 字节的 Hotspot 坐标 (x, y)
							DWORD newSize = 4 + best->dwBytesInRes;
							BYTE* pBuf = (BYTE*)malloc(newSize);
							if (pBuf)
							{
								*(WORD*)(pBuf + 0) = best->wPlanes;     // XHotspot
								*(WORD*)(pBuf + 2) = best->wBitCount;   // YHotspot
								memcpy(pBuf + 4, data.buf + best->dwImageOffset, best->dwBytesInRes);

								hicon = CreateIconFromResourceEx(
									pBuf,
									newSize,
									FALSE, // FALSE 为光标
									0x00030000,
									w,
									w,
									LR_DEFAULTCOLOR
								);
								free(pBuf);
							}
						}
						else // 图标 (ICON)
						{
							hicon = CreateIconFromResourceEx(
								(PBYTE)(data.buf + best->dwImageOffset),
								best->dwBytesInRes,
								TRUE, // TRUE 为图标
								0x00030000,
								w,
								w,
								LR_DEFAULTCOLOR
							);
						}

						if (hicon)
						{
							HBITMAP hRet = NULL;
							ICONINFO info;
							if (GetIconInfo(hicon, &info))
							{
								HBITMAP hBmpSrc = info.hbmColor;
								if (!hBmpSrc)
								{
									hBmpSrc = info.hbmMask;
								}
								if (hBmpSrc)
								{
									hRet = (HBITMAP)CopyImage(hBmpSrc, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
								}
								if (导出掩码位图 != nil && info.hbmMask)
								{
									HBITMAP hMask = (HBITMAP)CopyImage(info.hbmMask, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
									*导出掩码位图.p = hMask;
								}
								if (info.hbmColor) DeleteObject(info.hbmColor);
								if (info.hbmMask) DeleteObject(info.hbmMask);
							}
							DestroyIcon(hicon);
							if (hRet) return hRet;
						}
					}
				}
			}
		}

		IStream* pStream = NULL;
		if (FAILED(CreateStreamOnHGlobal(NULL, TRUE, &pStream)))
			return NULL;

		ULONG written = 0;
		pStream->Write(data.buf, data.size, &written);
		LARGE_INTEGER li = { 0 };
		pStream->Seek(li, STREAM_SEEK_SET, NULL);

		IPicture* pPicture = NULL;
		if (SUCCEEDED(OleLoadPicture(pStream, 0, TRUE, IID_IPicture, (void**)&pPicture)))
		{
			HBITMAP hBmp = NULL;
			pPicture->get_Handle((unsigned int*)&hBmp);
			// 拷贝一份位图，因为 IPicture 释放后句柄可能失效
			HBITMAP hRet = (HBITMAP)CopyImage(hBmp, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			pPicture->Release();
			pStream->Release();
			return hRet;
		}

		pStream->Release();
		return NULL;
	}

	/**从文件载入位图（仅支持 JPG, BMP, GIF，不支持PNG）
	 * @param 文件名
	 * @return
	 */
	static HandleBITMAP 从文件创建(c_StrX 文件名)
	{
		return 从内存创建(读入文件(文件名));
	}

	/**根据源位图上的各点像素是否为透明色生成一个与源图尺寸一致的单色掩码位图
	 * @param 源位图
	 * @param 透明色
	 * @param 相似度 1-100（100为必须精确匹配，80则允许各分量允许有20%的色差）
	 * @return
	 */
	static HandleBITMAP 创建掩码位图(HBITMAP 源位图, COLORREF 透明色, int 相似度 = 100) {
		HandleBITMAP hb;
		if (!源位图) return hb;
		BITMAP bm;
		GetObject(源位图, sizeof(bm), &bm);
		int w = bm.bmWidth;
		int h = bm.bmHeight;

		// 1. 设置我们想要的标准化 DIB 提取格式 (32位，负高代表顶端朝下的正常扫描线)
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = w;
		bmi.bmiHeader.biHeight = -h;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		// 2. 直接用 GetDIBits 暴力导出原图的标准化像素，彻底省去创建缓冲DC和缓冲位图
		DWORD* pSrcBits = (DWORD*)malloc(w * h * 4);
		HDC hdcRef = GetDC(NULL);
		GetDIBits(hdcRef, 源位图, 0, h, pSrcBits, &bmi, DIB_RGB_COLORS);
		ReleaseDC(NULL, hdcRef);

		// 3. 准备完全兼容 GDI 光栅运算的 1bpp 单色掩码内存数据
		int lineByteWidth = ((w + 15) & ~15) / 8; // 每行必须是2字节对齐
		unsigned char* maskBits = (unsigned char*)malloc(lineByteWidth * h);
		memset(maskBits, 0, lineByteWidth * h);

		int trR = GetRValue(透明色);
		int trG = GetGValue(透明色);
		int trB = GetBValue(透明色);

		// 计算容差：100%相似度 = 0容差；80%相似度 = 20%的误差(255*0.2 = 51)
		int tol = 255 * (100 - 相似度) / 100;
		if (tol < 0) tol = 0;
		if (tol > 255) tol = 255;

		// 4. 内存极速遍历比对
		if (tol == 0) {
			// 精确匹配分支，最高效
			DWORD trColor = (trR << 16) | (trG << 8) | trB;
			for (int y = 0; y < h; ++y) {
				DWORD* pSrcRow = pSrcBits + y * w;
				unsigned char* pDstRow = maskBits + y * lineByteWidth;

				for (int x = 0; x < w; ++x) {
					if ((pSrcRow[x] & 0x00FFFFFF) == trColor) {
						pDstRow[x / 8] |= (0x80 >> (x % 8)); // 匹配到透明，该位置 1
					}
				}
			}
		} else {
			// 容差匹配分支
			for (int y = 0; y < h; ++y) {
				DWORD* pSrcRow = pSrcBits + y * w;
				unsigned char* pDstRow = maskBits + y * lineByteWidth;

				for (int x = 0; x < w; ++x) {
					DWORD c = pSrcRow[x];
					int r = (c >> 16) & 0xFF;
					int g = (c >> 8) & 0xFF;
					int b = c & 0xFF;

					// 严格忽略 Alpha 通道，比对 RGB 的容差
					if (abs(r - trR) <= tol && abs(g - trG) <= tol && abs(b - trB) <= tol) {
						pDstRow[x / 8] |= (0x80 >> (x % 8));
					}
				}
			}
		}

		// 5. 生成标准 1bpp 单色掩码并清理
		HBITMAP hMask = CreateBitmap(w, h, 1, 1, maskBits);
		free(pSrcBits);
		free(maskBits);

		hb = hMask;
		return hb;
	}

	static HandleBITMAP 创建掩码位图(const Bytes& imgData, COLORREF 透明色, int 相似度 = 100) {
		HandleBITMAP hBmp = 从内存创建(imgData);
		if (!hBmp) return NULL;
		return 创建掩码位图(hBmp, 透明色, 相似度);
	}

};

struct EXP 图标
{
	HICON h;
	bool _接管生命周期;

	图标() { h = NULL; _接管生命周期 = false; }

	/**图标
	 * @param h
	 * @param 是否接管生命周期=false 如果接管则本对象销毁时将图标句柄销毁
	 */
	图标(HICON h, bool 是否接管生命周期 = false) {
		挂接(h, 是否接管生命周期);
	}

	/**图标 移动构造必须接管
	 * @param hbp
	 * @param 是否接管生命周期=true 一定接管
	 */
	图标(HandleICON&& hbp, bool 是否接管生命周期 = true) { 挂接((HandleICON&&)hbp); }

	~图标() {
		销毁();
	}
	void 挂接(HICON h, bool 是否接管生命周期 = false) {
		this->h = h; _接管生命周期 = 是否接管生命周期;
	}
	void 挂接(HandleICON&& hbp) {
		h = hbp._move(); _接管生命周期 = true;
	}
	void 销毁() { if (_接管生命周期 && h) { DestroyIcon(h); h = NULL; } }
	operator HICON() { return h; }

	/**从内存载入图标（最终得到的HICON只有图标资源组中的一副）
	 * @param icoData 图标字节集
	 * @param baseSize=0 如果没指定尺寸，就选择最大图像
	 * @return
	 */
	static HandleICON 从内存创建(const Bytes& icoData, int baseSize = 0)
	{
		struct ICONDIR {
			WORD idReserved; // 必须为0
			WORD idType;     // 1=ICON
			WORD idCount;    // 图标数量
		};

		struct ICONDIRENTRY {
			BYTE bWidth;
			BYTE bHeight;
			BYTE bColorCount;
			BYTE bReserved;
			WORD wPlanes;
			WORD wBitCount;
			DWORD dwBytesInRes;
			DWORD dwImageOffset;
		};

		if (!icoData.buf || icoData.size < sizeof(ICONDIR)) return NULL;
		const ICONDIR* dir = (const ICONDIR*)icoData.buf;
		if (dir->idReserved != 0 || dir->idType != 1) return NULL;

		const ICONDIRENTRY* entries = (const ICONDIRENTRY*)(icoData.buf + sizeof(ICONDIR));

		const ICONDIRENTRY* best = nullptr;
		int bestDiff = 0x7fffffff;

		for (int i = 0; i < dir->idCount; ++i)
		{
			int w = entries[i].bWidth == 0 ? 256 : entries[i].bWidth;
			int diff = 0;
			if (baseSize)
				diff = abs(w - baseSize) + abs(w - baseSize);
			else
				diff = -w;  // 如果没指定尺寸，就选择最大图像

			if (!best || diff < bestDiff)
			{
				best = &entries[i];
				bestDiff = diff;
			}
		}

		if (!best) return NULL;
		if (best->dwImageOffset + best->dwBytesInRes >(DWORD)icoData.size) return NULL;

		int w = baseSize ? baseSize : ((best->bWidth == 0) ? 256 : best->bWidth);

		HICON hicon = CreateIconFromResourceEx(
			(PBYTE)(icoData.buf + best->dwImageOffset),
			best->dwBytesInRes,
			TRUE,
			0x00030000,
			w,
			w,
			LR_DEFAULTCOLOR
		);
		return hicon;
	}

	/**从内存载入图标（最终得到的HICON只有图标资源组中的一副）
	 * @param icoData 图标字节集
	 * @param baseSize=0 如果没指定尺寸，就选择最大图像
	 * @return
	 */
	static HandleICON 从文件创建(c_StrX 文件名, int baseSize = 0)
	{
		return 从内存创建(读入文件(文件名), baseSize);
	}

	/**从资源载入图标（最终得到的HICON只有图标资源组中的一副）
	 * @param hInstance
	 * @param uIconID 图标字节集
	 * @param baseSize=0 如果没指定尺寸，就选择最大图像
	 * @return
	 */
	static HandleICON 从资源创建(HINSTANCE hInstance, UINT uIconID, int baseSize = 0)
	{
		// 预定义的标准图标尺寸列表
		const int stdSize[] = { 16, 24, 32, 48, 64, 128, 256 };
		const int numSizes = sizeof(stdSize) / sizeof(stdSize[0]);
		if (baseSize==0) { baseSize = 256; }

		// 找到最接近 nBaseSize 的索引
		int baseIndex = 0;
		for (int i = 0; i < numSizes; i++) {
			if (stdSize[i] == baseSize) {
				baseIndex = i;
				break;
			}
		}
		HICON hIcon = NULL;

		// 根据 bSmallIcon 标志，使用两个独立的循环来确定搜索顺序
		if (baseSize!=256) {
			// 优先小图标：从 baseIndex 向下搜索
			for (int i = baseIndex; i >= 0; --i) {
				hIcon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(uIconID), IMAGE_ICON, stdSize[i], stdSize[i], 0);
				if (hIcon) return hIcon;
			}
		} else {
			// 优先大图标：从最大尺寸开始向下搜索
			for (int i = numSizes - 1; i >= 0; --i) {
				hIcon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(uIconID), IMAGE_ICON, stdSize[i], stdSize[i], 0);
				if (hIcon) return hIcon;
			}
		}
		return NULL; // 如果所有尺寸都找不到，返回 NULL
	}
};

struct EXP 光标
{
	HCURSOR h;
	bool _接管生命周期;

	光标() { h = NULL; _接管生命周期 = false; }

	/**光标
	 * @param h
	 * @param 是否接管生命周期=false 如果接管则本对象销毁时将光标句柄销毁
	 */
	光标(HCURSOR h, bool 是否接管生命周期 = false) {
		挂接(h, 是否接管生命周期);
	}

	/**光标 移动构造必须接管
	 * @param hbp
	 * @param 是否接管生命周期=true 一定接管
	 */
	光标(HandleCURSOR&& hbp, bool 是否接管生命周期 = true) { 挂接((HandleCURSOR&&)hbp); }

	~光标() {
		销毁();
	}
	void 挂接(HCURSOR h, bool 是否接管生命周期 = false) {
		this->h = h; _接管生命周期 = 是否接管生命周期;
	}
	void 挂接(HandleCURSOR&& hbp) {
		h = hbp._move(); _接管生命周期 = true;
	}
	void 销毁() { if (_接管生命周期 && h) { DestroyCursor(h); h = NULL; } }
	operator HCURSOR() { return h; }

	/**从内存载入光标
	 * @param curData 光标字节集
	 * @param baseSize=0 如果没指定尺寸，就选择最大图像
	 * @return
	 */
	static HandleCURSOR 从内存创建(const Bytes& curData, int baseSize = 0)
	{
		struct ICONDIR {
			WORD idReserved; // 必须为0
			WORD idType;     // 2=CURSOR
			WORD idCount;    // 光标数量
		};

		struct ICONDIRENTRY {
			BYTE bWidth;
			BYTE bHeight;
			BYTE bColorCount;
			BYTE bReserved;
			WORD wPlanes;      // XHotspot
			WORD wBitCount;    // YHotspot
			DWORD dwBytesInRes;
			DWORD dwImageOffset;
		};

		if (!curData.buf || curData.size < sizeof(ICONDIR)) return NULL;
		const ICONDIR* dir = (const ICONDIR*)curData.buf;
		if (dir->idReserved != 0 || dir->idType != 2) return NULL;

		const ICONDIRENTRY* entries = (const ICONDIRENTRY*)(curData.buf + sizeof(ICONDIR));

		const ICONDIRENTRY* best = nullptr;
		int bestDiff = 0x7fffffff;

		for (int i = 0; i < dir->idCount; ++i)
		{
			int w = entries[i].bWidth == 0 ? 256 : entries[i].bWidth;
			int diff = 0;
			if (baseSize)
				diff = abs(w - baseSize) + abs(w - baseSize);
			else
				diff = -w;  // 如果没指定尺寸，就选择最大图像

			if (!best || diff < bestDiff)
			{
				best = &entries[i];
				bestDiff = diff;
			}
		}

		if (!best) return NULL;
		if (best->dwImageOffset + best->dwBytesInRes >(DWORD)curData.size) return NULL;

		int w = baseSize ? baseSize : ((best->bWidth == 0) ? 256 : best->bWidth);

		HCURSOR hcursor = NULL;
		// 光标数据必须在资源 bits 前缀 4 字节的 Hotspot 坐标 (x, y)
		DWORD newSize = 4 + best->dwBytesInRes;
		BYTE* pBuf = (BYTE*)malloc(newSize);
		if (pBuf)
		{
			*(WORD*)(pBuf + 0) = best->wPlanes;     // XHotspot
			*(WORD*)(pBuf + 2) = best->wBitCount;   // YHotspot
			memcpy(pBuf + 4, curData.buf + best->dwImageOffset, best->dwBytesInRes);

			hcursor = (HCURSOR)CreateIconFromResourceEx(
				pBuf,
				newSize,
				FALSE, // FALSE 为光标
				0x00030000,
				w,
				w,
				LR_DEFAULTCOLOR
			);
			free(pBuf);
		}
		return hcursor;
	}
};

struct EXP 图片组 {
	HIMAGELIST h = NULL;
	图片组() = default;            // 默认构造函数
	图片组(const 图片组&) = delete;  // 禁止拷贝构造
	图片组& operator=(const 图片组&) = delete; // 禁止拷贝赋值
	~图片组() { 销毁(); }
	/**
	 * @param 宽度
	 * @param 高度
	 * @param 标志=32 位深可选的值有4(16色),8,16,24,32，若要支持透明度需ILC_MASK
	 注：ILC_MASK指定后一个图片组中索引槽的图片就包含2张（一个是原位图一个是掩码图）
	 这个在函数添加图片文件()中会有指定额外参数添加进掩码图（它是一个单色位图用于指定透明和非透明）
	 对于32位的位图，ImageList并不会自动使用alpha通道做透明，
	 这是因为在传统GDI绘图的方法或Win32控件中：透明性仍然依赖mask而不是alpha通道
	 图片组仅是为了存储图片信息，所以还是以实际的渲染方法为主看其是否支持alpha通道
	 * @param 动态增长个数=2 当内部数组容量不足时增长的槽个数
	 * @return
	 */
	HIMAGELIST 创建(float 宽度, float 高度, int 标志 = 32, int 初始图片数量 = 0, int 动态增长个数 = 2) {
		return h = ImageList_Create((int)宽度, (int)高度, 标志, 初始图片数量, 动态增长个数);
	}
	void 销毁() {
		if (h) { ImageList_Destroy(h); h = 0; }
	}

	/**成功返回该图片在图片组中的索引，失败返回-1
	 * @param 图标句柄
	 * @return
	 * */
	int 添加图标(HICON 图标句柄) {
		return ImageList_AddIcon(h, 图标句柄);
	}

	/**成功返回该图片在图片组中的索引，失败返回-1
	 * @param 图片句柄
	 * @param 掩码位图=NULL 指向与hbmImage同尺寸的单色位图(1透明,0不透明)。如果为NULL，则不使用掩码。
	 * @param 自动横向切割=false
	 * @return
	 */
	int 添加位图(HBITMAP 图片句柄, HBITMAP 掩码位图 = NULL, bool 自动横向切割 = false) {
		位图 ddb(图片句柄);
		int 宽度 = 0, 高度 = 0;
		if (自动横向切割==false) { ImageList_GetIconSize(h, &宽度, &高度); }
		SIZE sz = ddb.取尺寸();
		if (sz.cx != 宽度 || sz.cy != 高度) {
			ddb.缩放(宽度, 高度, ddb);
		}
		return ImageList_Add(h, ddb, 掩码位图);
	}

	/**向图片组中添加一个位图，并根据指定的颜色自动生成透明掩码。
	 * @param 图片句柄
	 * @param 透明色 要作为透明处理的颜色。
	 * @return
	 * */
	int 添加位图_透明处理(HBITMAP 图片句柄, COLORREF 透明色) {
		return ImageList_AddMasked(h, 图片句柄, 透明色);
	}

	/**从图象文件里添加图象，成功返回该图片在图片组中的索引，失败返回-1
	 * @param 文件名
	 * @param 格式 IMAGE_BITMAP(位图)，IMAGE_ICON(图标)
	 * @param 自动横向切割 如果位图的宽度大于创建时单个图像宽度，函数会假定这张位图包含多个横向排列的图像。
	 * @param 图标baseSize=0 不指定则默认使用图标文件中最大的图标
	 * @param 掩码位图=NULL 指向与hbmImage同尺寸的单色位图(1透明,0不透明)，创建掩码位图()可得到
	 如果该参数为NULL，则ImageList会自动生成透明掩码（通常是自动识别图中某种颜色当作透明色）
	 此外，若在图片组创建时指定ILC_COLOR32则开启alpha通道，透明度由位图的高32位指定/则该掩码位图无效
	 * @return
	 */
	int 添加图片文件(c_StrX 文件名, int 格式, bool 自动横向切割 = false, int 图标baseSize = 0, HBITMAP 掩码位图 = NULL) {
		//该函数返回的句柄以及销毁方法根据格式而定
		if (格式==IMAGE_BITMAP) {
			HandleBITMAP ddb = 位图::从文件创建(文件名);
			if (!ddb) return -1;
			// 若图片组有指定尺寸（非自动切割模式），检查位图尺寸是否与槽天匹配
			// ImageList_Add 按「位图宽/槽宽」计算张数，小于槽宽则返回-1
			return 添加位图(ddb, 掩码位图, 自动横向切割);
		} else if (格式==IMAGE_ICON) {
			return 添加图标(图标::从文件创建(文件名, 图标baseSize));
		}
		return -1;
	}

	/**把源图片组指定索引的图片复制到另一图片组对应索引。
	 * @param 源图片索引
	 * @param 目标图片组
	 * @param 目标图片索引
	 * @param 复制标记
	 ILD_NORMAL或0	默认复制，仅复制图像本身（根据源ImageList内部存储），mask也会复制（如果源有）
	 ILD_TRANSPARENT	复制透明部分（Mask），可用于透明显示
	 ILD_MASK	只复制 mask 位图（透明掩码），不复制颜色位图。
	 ILD_IMAGE	只复制颜色图像，不复制 mask。
	 * @return
	 * */
	bool 复制图片(int 源图片索引, 图片组& 目标图片组, int 目标图片索引, int 复制标记 = 0) {
		return ImageList_Copy(目标图片组, 目标图片索引, h, 源图片索引, 复制标记)!=0;
	}

	void 从图片组复制(图片组& 源图片组) {
		销毁(); h = ImageList_Duplicate(源图片组);
	}

	bool 替换图标(int 被替换图标索引, HandleICON 图标句柄) {
		return ImageList_ReplaceIcon(h, 被替换图标索引, 图标句柄)!=0;
	}

	bool 替换位图(int 被替换图片索引, HBITMAP 图片句柄, HBITMAP 掩码位图) {
		return  ImageList_Replace(h, 被替换图片索引, 图片句柄, 掩码位图)!=0;
	}

	bool 删除图片(int 图片索引) {
		return (ImageList_Remove(h, 图片索引) != 0);
	}

	/**实际上当使用掩码位图搞了透明后用还会用图片组的背景色去填充
	   如果背景颜色值指定为透明色时才是真透明过去
	   注：如果没有调用置背景色，则ImageList创建时默认的背景色是CLR_NONE
	   * @param 颜色值 特殊值CLR_NONE(-1)为透明色
	   * @return
	   * */
	COLORREF 置背景色(COLORREF 颜色值 = -1) {
		return (ImageList_SetBkColor(h, 颜色值));
	}

	COLORREF 取背景色() {
		return (ImageList_GetBkColor(h));
	}

	/**从图片组的索引创建一个新HICON，调用者必须负责用DestroyIcon释放。
	 注：事实上ImageList在存储图片时已经不区分原始添加是位图还是图标
	 * @param 图片索引
	 * @param 标志
	 图标拷贝获取方式，取值为 ILD_ 系列标志之一或组合，例如：
	 • ILD_NORMAL ：正常获取图标
	 • ILD_TRANSPARENT ：使用 mask 透明
	 • ILD_MASK ：只复制 mask
	 • ILD_IMAGE ：只复制颜色图像
	 • ILD_SELECTED ：在选中状态下的图标（如果支持）
	 • ILD_FOCUS ：焦点状态图标
	 • ILD_BLEND25/50 ：25% / 50% 混合效果，用于淡化
	 * @return HICON
	 * */
	HICON 取图标句柄_拷贝(int 图片索引, int 标志) {
		return (ImageList_GetIcon(h, 图片索引, 标志));
	}

	SIZE 取图标尺寸() {
		SIZE sz;
		ImageList_GetIconSize(h, (int*)&sz.cx, (int*)&sz.cy);
		return sz;
	}
	bool 置图标尺寸(SIZE sz) {
		return (ImageList_SetIconSize(h, sz.cx, sz.cy) !=0);
	}

	int 取图片数() {
		return ImageList_GetImageCount(h);
	}
	bool 置图标数(int 数量) {
		return ImageList_SetImageCount(h, 数量)!=0;
	}

	/**取一个图片组内部的图片信息，当中导出的句柄仅是其引用不得调用DeleteObject
	   注：ImageList在存储图片时已经不区分原始添加是位图还是图标，
	   而每一个索引槽存储的只有两张位图（一个是原位图还有一个是掩码位图）
	 * @param 图片索引
	 * @param 导出图片信息
	 * @return
	 * */
	bool 取内部图片信息(int 图片索引, IMAGEINFO& 导出图片信息) {
		return ImageList_GetImageInfo(h, 图片索引, &导出图片信息)!=0;
	}

	/**当中导出的句柄仅是其引用不得调用DeleteObject
	 * @return HBITMAP
	 * */
	HBITMAP 取图片句柄(int 图片索引) {
		IMAGEINFO 导出图片信息;
		if (!取内部图片信息(图片索引, 导出图片信息))return 0;
		return 导出图片信息.hbmImage;
	}

	Bytes 导出图片组到字节集()
	{
		if (!h) return{};

		IStream* pStream = nullptr;
		CreateStreamOnHGlobal(NULL, TRUE, &pStream);
		if (!ImageList_Write(h, pStream)) { pStream->Release(); return{}; }

		HGLOBAL hMem = nullptr;
		GetHGlobalFromStream(pStream, &hMem);        // 获取内存句柄
		Bytes outBytes(GlobalLock(hMem), (int)GlobalSize(hMem));

		GlobalUnlock(hMem);
		pStream->Release();                          // 自动释放 HGLOBAL
		//若CreateStreamOnHGlobal的第二个参数 fDeleteOnRelease=TRUE 时：
		//当释放 IStream（pStream->Release()）时，会自动释放内部的 HGLOBAL 内存。
		//GlobalUnlock 只是解锁内存，解除指针访问限制，并不会释放内存。
		return outBytes;
	}


	HIMAGELIST 导入图片组字节集(const Bytes& 图片组字节集) {
		if (图片组字节集.buf == nullptr || 图片组字节集.size <= 0)
			return nullptr;

		// 1. 创建可移动 HGLOBAL，并拷贝字节流
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 图片组字节集.size);
		void* pData = GlobalLock(hMem);
		memcpy(pData, 图片组字节集.buf, 图片组字节集.size);
		GlobalUnlock(hMem);

		// 2. 创建内存流
		IStream* pStream = nullptr;
		if (FAILED(CreateStreamOnHGlobal(hMem, TRUE, &pStream))) {
			GlobalFree(hMem); // 失败释放
			return nullptr;
		}

		// 3. 读取 ImageList
		HIMAGELIST hImageList = ImageList_Read(pStream);

		// 4. 释放流（HGLOBAL 会自动释放，因为 fDeleteOnRelease = TRUE）
		pStream->Release();
		return hImageList; // 成功返回 ImageList 句柄
	}

	/**在指定窗口DC上画图片
	 * @param 图片索引
	 * @param 窗口DC
	 * @param x 目标DC的横坐标
	 * @param y 目标DC的纵坐标
	 * @param 标志 标志 "ILD_"系列：
	 • ILD_NORMAL ：正常绘制
	 • ILD_TRANSPARENT ：使用 mask 透明
	 • ILD_BLEND25/50 ：半透明淡化
	 • ILD_SELECTED ：选中状态
	 • ILD_MASK ：只绘制 mask
	 • ILD_IMAGE ：只绘制颜色图像
	 * @return
	 */
	bool 窗口画图片(int 图片索引, HDC 窗口DC, float x, float y, int 标志) {
		return ImageList_Draw(h, 图片索引, 窗口DC, (int)x, (int)y, 标志)!=0;
	}

	/*拖动功能解决方案
	 MouseDown->
	 ImageList_BeginDrag -> 初始化拖动（内存里记录哪个图像）
	 ImageList_DragEnter -> 在窗口指定初始位置显示拖动图像
	 MouseMove->ImageList_DragMove -> 随鼠标移动
	 MouseUp->ImageList_EndDrag -> 停止拖动
	 **/
	static bool 拖动_start(HIMAGELIST himl, HWND 窗口句柄, int 图片索引, float x, float y) { // 辅助块以防冲突
		return false;
	}
	static bool 拖动_开始(HIMAGELIST himl, HWND 窗口句柄, int 图片索引, float x, float y) {
		if (!himl || 图片索引 < 0) return false;

		int width = 0, height = 0;
		ImageList_GetIconSize(himl, &width, &height);

		// 准备所需图像准备开始拖动
		if (!ImageList_BeginDrag(himl, 图片索引, 0, 0))return false;

		// 捕获鼠标（让鼠标即便在该窗口外部也能响应消息队列~直到鼠标释放）
		SetCapture(窗口句柄);

		// 在指定窗口上的初始位置进行首次显示
		if (!ImageList_DragEnter(窗口句柄, (int)x, (int)y))
		{
			ImageList_EndDrag();
			ReleaseCapture();
			return false;
		}

		return true;
	}
	static bool 拖动_移动中(float x, float y) {
		return ImageList_DragMove((int)x, (int)y)!=0;
	}
	static bool 拖动_结束(HWND 窗口句柄) {
		BOOL r = ImageList_DragLeave(窗口句柄);
		ImageList_EndDrag();
		return r!=0;
	}

	operator HIMAGELIST() { return h; }
};
