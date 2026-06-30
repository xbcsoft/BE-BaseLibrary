#pragma once
#include "子容器.hpp"
#include "helper/位图.h"

定义_枚举型(图片框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5, 渐变镜框式 = 6
);

定义_枚举型(图片框显示方式, char,
	图片居左上 = 0, 缩放图片 = 1, 居中显示 = 2
);

class 图片框 : public 子容器<true>
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

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	/**在控件上显示位图
	 * @param imgData 仅支持bmp,jpg,gif(但不支持动图，且也不支持gif自动透明)
	 * @param 是否贮存图片=false 是否在图片框内存储一份原始字节集（半公开成员为 _图片）
	 */
	void 置图片(const Bytes& imgData);

	/**在控件上显示位图
	 * @param hBmp 位图句柄
	 * @param maskBmp 掩码位图句柄，如果投入则按照此掩码进行背景透明
	 */
	void 置图片(HBITMAP hBmp, HBITMAP maskBmp = NULL);

	void 边框_(图片框边框 bor);

	void 显示方式_(图片框显示方式 mode);

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
