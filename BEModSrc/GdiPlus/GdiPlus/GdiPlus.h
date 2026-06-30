#pragma once
/**@ModuleTitle: GdiPlus测试版
*  @version:     beta0.2
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description: 该模块尚未完成，仅提供当下必要的图片格式转换功能
*/
#include "stdafx.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// 保证整个进程使用 GDI+ 前自动 startup
struct __GdiPlusAutoInit {
	ULONG_PTR token = 0;
	__GdiPlusAutoInit() {
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartup(&token, &input, NULL);
	}
	~__GdiPlusAutoInit() {
		if (token) Gdiplus::GdiplusShutdown(token);
	}
}; extern __GdiPlusAutoInit __g_gdiplus_auto_init;

struct 真彩色 {
	union {
		struct { //内存排布是b在最低位、a在最高位
			byte b;
			byte g;
			byte r;
			byte a;
		};
		Gdiplus::ARGB val;
	};	operator Gdiplus::ARGB(){ return val; }

	真彩色(Gdiplus::ARGB c) : val(c) {} //从GDI+色那边直接赋值
	真彩色() : r(r), g(g), b(b), a(255) {}
	真彩色(byte r, byte g, byte b, byte a = 255)
		: r(r), g(g), b(b), a(a) {}
};
DWORD RGBToBGRA(COLORREF c, byte a = 255);

//支持常见的图片格式，带透明度的PNG/GIF
struct 真彩图 {

	static HandleBITMAP 从内存创建(const Bytes& data, 可空<HandleBITMAP&> 导出掩码位图 = nil) {
		if (data.size == 0) return NULL;

		IStream* pStream = NULL;
		if (FAILED(CreateStreamOnHGlobal(NULL, TRUE, &pStream))) return NULL;

		ULONG written;
		pStream->Write(data.buf, data.size, &written);
		LARGE_INTEGER li = { 0 };
		pStream->Seek(li, STREAM_SEEK_SET, NULL);

		Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromStream(pStream);

		if (!bmp || bmp->GetLastStatus() != Gdiplus::Ok) {
			if (bmp) delete bmp;
			pStream->Release();
			return NULL;
		}

		int width = bmp->GetWidth();
		int height = bmp->GetHeight();

		// 创建 32 位 DIB Section (带 Alpha)
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height; // Top-down
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		void* pBits = nullptr;
		HBITMAP hRet = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
		bool success = false;
		if (hRet && pBits) {
			Gdiplus::Rect rect(0, 0, width, height);
			Gdiplus::BitmapData bmpData;
			bmpData.Width = width;
			bmpData.Height = height;
			bmpData.Stride = width * 4;
			bmpData.PixelFormat = PixelFormat32bppPARGB;
			bmpData.Scan0 = pBits;

			if (bmp->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeUserInputBuf, PixelFormat32bppPARGB, &bmpData) == Gdiplus::Ok) {
				bmp->UnlockBits(&bmpData);
				success = true;
			} else {
				DeleteObject(hRet);
				hRet = NULL;
			}
		}

		// 导出掩码位图
		if (success && 导出掩码位图 != nil) {
			// 再次 Lock 为 32bpp ARGB 以提取原始 Alpha 字节制作单色 Mask
			Gdiplus::Rect rect(0, 0, width, height);
			Gdiplus::BitmapData bmpData;
			if (bmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData) == Gdiplus::Ok) {
				int lineByteWidth = ((width + 15) & ~15) / 8;
				unsigned char* maskBits = (unsigned char*)malloc(lineByteWidth * height);
				if (maskBits) {
					memset(maskBits, 0, lineByteWidth * height);
					for (int y = 0; y < height; ++y) {
						unsigned int* pSrcRow = (unsigned int*)((unsigned char*)bmpData.Scan0 + y * bmpData.Stride);
						unsigned char* pDstRow = maskBits + y * lineByteWidth;
						for (int x = 0; x < width; ++x) {
							unsigned char alpha = (unsigned char)((pSrcRow[x] >> 24) & 0xFF);
							if (alpha < 128) {
								pDstRow[x / 8] |= (0x80 >> (x % 8));
							}
						}
					}
					*导出掩码位图.p = CreateBitmap(width, height, 1, 1, maskBits);
					free(maskBits);
				}
				bmp->UnlockBits(&bmpData);
			}
		}

		delete bmp;
		pStream->Release();
		return hRet;
	}
};
