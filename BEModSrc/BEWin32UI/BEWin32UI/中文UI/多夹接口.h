#pragma once
#include "容器类.h"

class 多夹接口 : public 容器类
{
public:
	virtual int 插入子夹(StrW 标题, 可空<int> 子夹索引 = nil) = 0;
	virtual void 子夹添加组件(int 索引, 窗口基类& a) = 0;
	virtual void 置子夹标题(int 索引, StrW 标题) = 0;
	virtual StrW 取子夹标题(int 索引) = 0;
};