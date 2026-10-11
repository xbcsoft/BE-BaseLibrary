#pragma once
#include "容器类.h"

class 多夹接口 : public 容器类
{
public:
	virtual int 插入子夹(StrW 标题, 可空<int> 子夹索引 = nil) = 0;
	virtual void 子夹添加组件(int 索引, 窗口基类& a) = 0;
	virtual void 置子夹标题(int 索引, StrW 标题) = 0;
	virtual StrW 取子夹标题(int 索引) = 0;
	// 索引=-1时仅获取当前子夹；合法索引时切换并返回切换前索引；失败返回-1。
	virtual int 现行子夹_(int 索引) = 0;
	virtual void 子夹管理_(c_Bytes 数组) = 0; //以{0,0}作为分割组的连续W字符串组
	// 设计器在准备拖动多夹控件前调用。返回 false 继续设计器拖动；
	// 返回 true 表示该客户区坐标应交给多夹本体处理（例如点击子夹头）。
	virtual bool _判定客户鼠标拖动(int x, int y) = 0;
};
