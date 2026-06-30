//此文件用于定义全局变量/常量符号(可赋初值)
//支持的定义格式：int val=1; const int val;
//支持类前置声明定义指针：class ABC;ABC* p;
//可选#include仅含声明的头文件 或inline内容
//保存此文件后将在"BEMod.h"模块形式写入声明
#include <BEWin32Base/BEWin32Base.h>

HINSTANCE g_hInst;
char* g_hInstEnd;
float g_dpi; float _g_dpi_bak;
int _g_wndCount;
class 窗口; 窗口* _g_interWnd;