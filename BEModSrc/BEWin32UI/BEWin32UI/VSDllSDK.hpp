#pragma once
#include "BEWin32UI.h"

EXPC Bytes* CBytes_New(int size = 0) { return new Bytes(size); }
EXPC Bytes* CBytes_New2(void* p, int size = 0) { return new Bytes(p, size); }
EXPC byte* CBytes_GetPointer(Bytes* b) { return b->buf; }
EXPC byte* CBytes_GetPointer2(Bytes* b, int& size) { size = b->size; return b->buf; }
EXPC int CBytes_GetSize(Bytes* b) { return b->size; }
EXPC void CBytes_Free(Bytes* b) { delete b; }
EXPC void CBytes_Reset(Bytes* b, int len) { b->reset(len); }
EXPC void CBytes_CopyFrom(Bytes* b, void* p, int len) { b->copyFrom(p, len); }
EXPC void CBytes_CopyFrom2(Bytes* b, int startI, void* p, int len) { b->copyFrom(startI, p, len); }
EXPC 字体* CFont_New(charW* 名称, int 大小, bool 下划线, bool 删除线, bool 倾斜, bool 加粗, int 角度) {
	字体* f = new 字体;
	f->字体名称 = 名称;
	f->字体大小 = 大小;
	f->下划线 = 下划线;
	f->删除线 = 删除线;
	f->倾斜 = 倾斜;
	f->加粗 = 加粗;
	f->角度 = 角度;
	return f;
}
EXPC void CFont_Free(字体* f) { delete f; }
#include <FB/FB.cpp>

//_________________________________________________________________________

void _UI_多夹_插入子夹(多夹接口* ui, LPWSTR 标题, int 索引) {
	ui->插入子夹(标题, 索引);
}
void _UI_多夹_子夹添加组件(多夹接口* ui, int 索引, 窗口基类* a) {
	ui->子夹添加组件(索引, *a);
}
Bytes* _UI_多夹_取子夹标题(多夹接口* ui, int 索引) {
	StrW r = ui->取子夹标题(索引);
	return CBytes_New2(r, r._size());
}
void _UI_多夹_置子夹标题(多夹接口* ui, int 索引, LPWSTR 标题) {
	ui->置子夹标题(索引, 标题);
}

#pragma pack(1)
struct UI属性类型 {
	enum {
		枚举 = 0, 布尔 = 1, 颜色 = 2, 整数 = 3,
		数值 = 4, 数据 = 5, 文本 = 6, 字体 = 7,
		自定义 = 10
	};
};
struct UI属性元信息 {
	int 序号;
	LPWSTR 名称;
	DWORD 类型;
	union 默认
	{ //真值类型只有以下四种，其他一律采用指针看待
		int 整数;
		bool 布尔;
		double 数值;
		LPWSTR 文本;
		constexpr 默认(int 整数) :整数(整数) {}
		constexpr 默认(uint 整数) : 整数(整数) {}
		constexpr 默认(DWORD 整数) : 整数(整数) {}
		constexpr 默认(bool 布尔) : 布尔(布尔) {}
		constexpr 默认(double 数值) : 数值(数值) {}
		constexpr 默认(LPWSTR 文本) : 文本(文本) {}
		constexpr 默认(void* 指针) : 指针(指针) {}
		constexpr 默认(const StrW& 文本) : 文本((LPWSTR)文本.bytes.buf) {}
		template<class T> constexpr 默认(NilOpt<T> v) {}
		void* 指针;
	}默认值;
	LPWSTR 枚举值; //L"0.左对齐|1.居中|2.右对齐" //用"|"进行分割
	LPWSTR 说明;
	BOOL 可空;
	BOOL 子项;
};
constexpr UI属性元信息 _MK属性(int 序号, LPWSTR 名称, DWORD 类型, UI属性元信息::默认 默认值,
	LPWSTR 枚举值, LPWSTR 说明, BOOL 可空, BOOL 子项) {
	return { 序号, 名称, 类型, 默认值, 枚举值, 说明, 可空, 子项 };
}

struct UI类型 {
	enum {
		控件 = 0, 容器 = 1, 多夹 = 2,
		虚拟控件 = 3, 虚拟容器 = 4
	};
};
struct UI库组件信息 {
	LPWSTR 名称;
	int 组件类型; //默认为 UI类型::控件=0
	LPWSTR 特化分组名; //如果为NULL则使用默认分组名
	int 图标大小;
	byte* 图标;
	int 属性个数;
	void* 属性元信息;
	void* fn创建;
	void* fn置属性;
	void* fn编辑框;
	void* 保留[10];
};

struct UI多夹类型 {
	void* fn插入子夹 = _UI_多夹_插入子夹;
	void* fn子夹添加组件 = _UI_多夹_子夹添加组件;
	void* fn取子夹标题 = _UI_多夹_取子夹标题;
	void* fn置子夹标题 = _UI_多夹_置子夹标题;
}_fn多夹;

typedef Bytes*(*BEIDEFN)(int fnID, byte* fbDat);
namespace IDE方法 {
enum {
	启用禁用属性 = 1, //void fb(LPWSTR 属性名,bool 启用禁用)
	//属性的值与属性面板展示的相一致
	取属性值 = 2, //r = Bytes* fb(LPWSTR 属性名) !注：返回值需手动delete r;
	置属性值 = 3  //void fb(LPWSTR 属性名,LPWSTR 属性值)
};
};
typedef bool(*属性面板回调)(LPWSTR 组件名称,
	LPWSTR 属性名, LPWSTR 当前属性值, LPWSTR 即将改变到属性值, BEIDEFN ide方法);

struct _UI库信息 {
	DWORD SDK主版本 = 1;
	DWORD SDK次版本 = 0;
	LPSTR 模块名 = NULL;
	LPSTR 分组名 = NULL;
	LPSTR 作者 = NULL;
	LPSTR 日期 = NULL;
	LPSTR 其他 = NULL;
	UI多夹类型* fn多夹 = &_fn多夹;
	属性面板回调 fn全局属性面板 = NULL;
	void* 保留[10]{};

	//后续继承的【UI库信息】必须紧接着成员：
	//int 组件个数;
	//UI库组件信息 interfaceUI[x];
};
#pragma pack()
