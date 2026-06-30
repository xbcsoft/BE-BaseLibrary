#pragma once
#include "句柄管理.h"
#include <CommCtrl.h>
#include <OleCtl.h>

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"ole32.lib")

struct 颜色 {
	union {
		struct { //内存排布是r在最低位、g,b在第17,24位
			byte r;
			byte g;
			byte b;
			byte _unused; //对齐COLORREF
		};
		COLORREF val;
	};
	operator COLORREF()const;

	颜色();
	颜色(COLORREF c);
	颜色(byte r, byte g, byte b);

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
//struct 真彩色{
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

struct 位图
{
	HBITMAP h;
	bool _接管生命周期;

	位图();

	/**位图
	 * @param h
	 * @param 是否接管生命周期=false 如果接管则本对象销毁时将位图句柄销毁
	 */
	位图(HBITMAP h, bool 是否接管生命周期 = false);

	/**位图 移动构造必须接管
	 * @param hbp
	 * @param 是否接管生命周期=true 一定接管
	 */
	位图(HandleBITMAP&& hbp);

	~位图();
	void 挂接(HBITMAP h, bool 是否接管生命周期 = false);
	void 挂接(HandleBITMAP&& hbp);
	void 销毁();
	operator HBITMAP();

	/**将位图原位缩放到指定尺寸（高质量缩放，旧句柄自动释放）
	 * @param 目标宽度
	 * @param 目标高度
	 */
	void 缩放(float 目标宽度, float 目标高度, 位图& 目标);

	/**取尺寸 无参的则是成员方法，有参的则是静态方法
	 * @return
	 */
	SIZE 取尺寸();

	static SIZE 取尺寸(HBITMAP hBmp);

	/**从资源载入位图（仅支持BMP）
	 * @param hInstance
	 * @param uResourceID 资源ID
	 * @return
	 */
	static HandleBITMAP 从资源创建(HINSTANCE hInstance, UINT uResourceID);

	/**从内存创建
	 * @param data
	 * @param 导出掩码位图<可空>=nil 仅
	 * @return
	 */
	static HandleBITMAP 从内存创建(const Bytes& data, 可空<HandleBITMAP&> 导出掩码位图 = nil);

	/**从文件载入位图（仅支持 JPG, BMP, GIF，不支持PNG）
	 * @param 文件名
	 * @return
	 */
	static HandleBITMAP 从文件创建(c_StrX 文件名);

	/**根据源位图上的各点像素是否为透明色生成一个与源图尺寸一致的单色掩码位图
	 * @param 源位图
	 * @param 透明色
	 * @param 相似度 1-100（100为必须精确匹配，80则允许各分量允许有20%的色差）
	 * @return
	 */
	static HandleBITMAP 创建掩码位图(HBITMAP 源位图, COLORREF 透明色, int 相似度 = 100);

	static HandleBITMAP 创建掩码位图(const Bytes& imgData, COLORREF 透明色, int 相似度 = 100);

};

struct 图标
{
	HICON h;
	bool _接管生命周期;

	图标();

	/**图标
	 * @param h
	 * @param 是否接管生命周期=false 如果接管则本对象销毁时将图标句柄销毁
	 */
	图标(HICON h, bool 是否接管生命周期 = false);

	/**图标 移动构造必须接管
	 * @param hbp
	 * @param 是否接管生命周期=true 一定接管
	 */
	图标(HandleICON&& hbp, bool 是否接管生命周期 = true);

	~图标();
	void 挂接(HICON h, bool 是否接管生命周期 = false);
	void 挂接(HandleICON&& hbp);
	void 销毁();
	operator HICON();

	/**从内存载入图标（最终得到的HICON只有图标资源组中的一副）
	 * @param icoData 图标字节集
	 * @param baseSize=0 如果没指定尺寸，就选择最大图像
	 * @return
	 */
	static HandleICON 从内存创建(const Bytes& icoData, int baseSize = 0);

	/**从内存载入图标（最终得到的HICON只有图标资源组中的一副）
	 * @param icoData 图标字节集
	 * @param baseSize=0 如果没指定尺寸，就选择最大图像
	 * @return
	 */
	static HandleICON 从文件创建(c_StrX 文件名, int baseSize = 0);

	/**从资源载入图标（最终得到的HICON只有图标资源组中的一副）
	 * @param hInstance
	 * @param uIconID 图标字节集
	 * @param baseSize=0 如果没指定尺寸，就选择最大图像
	 * @return
	 */
	static HandleICON 从资源创建(HINSTANCE hInstance, UINT uIconID, int baseSize = 0);
};

struct 光标
{
	HCURSOR h;
	bool _接管生命周期;

	光标();

	/**光标
	 * @param h
	 * @param 是否接管生命周期=false 如果接管则本对象销毁时将光标句柄销毁
	 */
	光标(HCURSOR h, bool 是否接管生命周期 = false);

	/**光标 移动构造必须接管
	 * @param hbp
	 * @param 是否接管生命周期=true 一定接管
	 */
	光标(HandleCURSOR&& hbp, bool 是否接管生命周期 = true);

	~光标();
	void 挂接(HCURSOR h, bool 是否接管生命周期 = false);
	void 挂接(HandleCURSOR&& hbp);
	void 销毁();
	operator HCURSOR();

	/**从内存载入光标
	 * @param curData 光标字节集
	 * @param baseSize=0 如果没指定尺寸，就选择最大图像
	 * @return
	 */
	static HandleCURSOR 从内存创建(const Bytes& curData, int baseSize = 0);
};

struct 图片组 {
	HIMAGELIST h = NULL;
	图片组() = default;            // 默认构造函数
	图片组(const 图片组&) = delete;  // 禁止拷贝构造
	图片组& operator=(const 图片组&) = delete; // 禁止拷贝赋值
	~图片组();
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
	HIMAGELIST 创建(float 宽度, float 高度, int 标志 = 32, int 初始图片数量 = 0, int 动态增长个数 = 2);
	void 销毁();

	/**成功返回该图片在图片组中的索引，失败返回-1
	 * @param 图标句柄
	 * @return
	 * */
	int 添加图标(HICON 图标句柄);

	/**成功返回该图片在图片组中的索引，失败返回-1
	 * @param 图片句柄
	 * @param 掩码位图=NULL 指向与hbmImage同尺寸的单色位图(1透明,0不透明)。如果为NULL，则不使用掩码。
	 * @param 自动横向切割=false
	 * @return
	 */
	int 添加位图(HBITMAP 图片句柄, HBITMAP 掩码位图 = NULL, bool 自动横向切割 = false);

	/**向图片组中添加一个位图，并根据指定的颜色自动生成透明掩码。
	 * @param 图片句柄
	 * @param 透明色 要作为透明处理的颜色。
	 * @return
	 * */
	int 添加位图_透明处理(HBITMAP 图片句柄, COLORREF 透明色);

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
	int 添加图片文件(c_StrX 文件名, int 格式, bool 自动横向切割 = false, int 图标baseSize = 0, HBITMAP 掩码位图 = NULL);

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
	bool 复制图片(int 源图片索引, 图片组& 目标图片组, int 目标图片索引, int 复制标记 = 0);

	void 从图片组复制(图片组& 源图片组);

	bool 替换图标(int 被替换图标索引, HandleICON 图标句柄);

	bool 替换位图(int 被替换图片索引, HBITMAP 图片句柄, HBITMAP 掩码位图);

	bool 删除图片(int 图片索引);

	/**实际上当使用掩码位图搞了透明后用还会用图片组的背景色去填充
	   如果背景颜色值指定为透明色时才是真透明过去
	   注：如果没有调用置背景色，则ImageList创建时默认的背景色是CLR_NONE
	   * @param 颜色值 特殊值CLR_NONE(-1)为透明色
	   * @return
	   * */
	COLORREF 置背景色(COLORREF 颜色值 = -1);

	COLORREF 取背景色();

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
	HICON 取图标句柄_拷贝(int 图片索引, int 标志);

	SIZE 取图标尺寸();
	bool 置图标尺寸(SIZE sz);

	int 取图片数();
	bool 置图标数(int 数量);

	/**取一个图片组内部的图片信息，当中导出的句柄仅是其引用不得调用DeleteObject
	   注：ImageList在存储图片时已经不区分原始添加是位图还是图标，
	   而每一个索引槽存储的只有两张位图（一个是原位图还有一个是掩码位图）
	 * @param 图片索引
	 * @param 导出图片信息
	 * @return
	 * */
	bool 取内部图片信息(int 图片索引, IMAGEINFO& 导出图片信息);

	/**当中导出的句柄仅是其引用不得调用DeleteObject
	 * @return HBITMAP
	 * */
	HBITMAP 取图片句柄(int 图片索引);

	Bytes 导出图片组到字节集();


	HIMAGELIST 导入图片组字节集(const Bytes& 图片组字节集);

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
	bool 窗口画图片(int 图片索引, HDC 窗口DC, float x, float y, int 标志);

	/*拖动功能解决方案
	 MouseDown->
	 ImageList_BeginDrag -> 初始化拖动（内存里记录哪个图像）
	 ImageList_DragEnter -> 在窗口指定初始位置显示拖动图像
	 MouseMove->ImageList_DragMove -> 随鼠标移动
	 MouseUp->ImageList_EndDrag -> 停止拖动
	 **/
	static bool 拖动_start(HIMAGELIST himl, HWND 窗口句柄, int 图片索引, float x, float y);
	static bool 拖动_开始(HIMAGELIST himl, HWND 窗口句柄, int 图片索引, float x, float y);
	static bool 拖动_移动中(float x, float y);
	static bool 拖动_结束(HWND 窗口句柄);

	operator HIMAGELIST();
};
