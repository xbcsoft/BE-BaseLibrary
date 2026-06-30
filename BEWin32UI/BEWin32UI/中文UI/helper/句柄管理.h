#pragma once
#include "../stdafx.h"

class HandleDC {
public:
	HWND hwnd;
	HDC hdc;
	HandleDC(HWND hwnd);
	~HandleDC();
	//禁止拷贝
	HandleDC(const HandleDC&) = delete;
	HandleDC& operator=(const HandleDC&) = delete;
	//移动构造
	HandleDC(HandleDC&& other);
	operator HDC() const;
};

class HandleCDC {
public:
	HDC hdc = NULL;
	HGDIOBJ hOldObj = NULL;

	HandleCDC() = default;
	HandleCDC(HDC refHdc);
	~HandleCDC();
	
	void init(HDC refHdc = NULL);

	void destroy();

	HGDIOBJ select(HGDIOBJ obj);

	//禁止拷贝
	HandleCDC(const HandleCDC&) = delete;
	HandleCDC& operator=(const HandleCDC&) = delete;
	//移动构造
	HandleCDC(HandleCDC&& other);
	//移动赋值
	HandleCDC& operator=(HandleCDC&& other);
	operator HDC() const;
};


#pragma region 通用GDI句柄管理

enum class HandleKind { GDI, Icon, Cursor };

//通用traits：默认用 DeleteObject
template<typename HandleType, HandleKind kind = HandleKind::GDI>
struct TraitsHandle {
	static BOOL destroy(HandleType h) {
		return DeleteObject((HGDIOBJ)h);
	}
};

template<>
struct TraitsHandle<HICON, HandleKind::Icon> {
	static BOOL destroy(HICON h) {
		return DestroyIcon(h);
	}
};

template<>
struct TraitsHandle<HCURSOR, HandleKind::Cursor> {
	static BOOL destroy(HCURSOR h) {
		return DestroyCursor(h);
	}
};

template<typename HandleType>
class HandleRes {
protected:
	HandleType h = nullptr;
public:
	HandleRes() = default;
	HandleRes(HandleType h) : h(h) {}

	void destroy(){
		if (h){
			TraitsHandle<HandleType>::destroy(h);
			h = nullptr;
		}
	}

	~HandleRes() { destroy(); }

	// 禁止拷贝
	HandleRes(const HandleRes&) = delete;
	HandleRes& operator=(const HandleRes&) = delete;

	// 移动构造
	HandleRes(HandleRes&& other) {
		h = other.h;
		other.h = nullptr;
	}
	// 移动赋值
	HandleRes& operator=(HandleRes&& other) {
		if (this != &other) {
			if (h) destroy();
			h = other.h;
			other.h = nullptr;
		}
		return *this;
	}

	// 显式移动
	HandleType _move(){
		HandleType r = h;
		h = nullptr;
		return r;
	}

	//赋值运算符，直接接管原生句柄
	HandleRes& operator=(HandleType hh) {
		if (h != hh) {
			destroy();
			h = hh;
		}
		return *this;
	}

	operator HandleType() const { return h; }
	operator LONG_PTR() const { return (LONG_PTR)(h); }
};

using HandleICON = HandleRes<HICON>;
using HandleCURSOR = HandleRes<HCURSOR>;
using HandleBRUSH = HandleRes<HBRUSH>;
using HandlePEN = HandleRes<HPEN>;
using HandleFONT = HandleRes<HFONT>;
using HandleBITMAP = HandleRes<HBITMAP>;

#pragma endregion
