#include "../_启动窗口.h"

void __启动窗口::载入(窗口* 父窗, bool 模态)
{
	if (窗口句柄)return;
	窗口::创建();

	{
		位图 t(位图::从内存创建(R::图1)); //这是张BMP图
		SIZE sz = t.取尺寸();
		图片框::参数 cs(30, 10, rdpi(sz.cx), rdpi(sz.cy));
		cs.背景颜色 = 颜色::透明;
		图片框1.创建(cs, this);
		图片框1.置图片(t, 位图::创建掩码位图(R::图1, 颜色::品红));
	}

	{
		HandleBITMAP hmask;
		位图 t(真彩图::从内存创建(R::图2, hmask)); //这是张PNG图
		SIZE sz = t.取尺寸();
		图片框::参数 cs(100, 10, rdpi(sz.cx), rdpi(sz.cy));
		//要提取GIF的掩码位图需要依赖【GdiPlus.bec】
		cs.背景颜色 = 颜色::透明;
		图片框2.创建(cs, this);
		图片框2.置图片(t, hmask);
	}

	{   /*该图存在边缘部分透明，所以提取掩码并非绝对无锯齿（这也是目前图片框的局限！
			需要全面GDI+绘制后才可完美绘制，即真彩图片框在白易1.0期间不会实现）*/
		HandleBITMAP hmask;
		位图 t(真彩图::从内存创建(R::图3, hmask)); //这是张PNG图
		SIZE sz = t.取尺寸();
		图片框::参数 cs(300, 80, rdpi(sz.cx), rdpi(sz.cy));
		//要提取GIF的掩码位图需要依赖【GdiPlus.bec】
		cs.背景颜色 = 颜色::透明;
		图片框3.创建(cs, this);
		图片框3.置图片(t, hmask);
	}

	窗口::完毕(模态);
}