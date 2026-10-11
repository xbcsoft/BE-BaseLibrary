#pragma once
#include <BECore/中文核心.hpp>

class 容器类;

class 组件基类
{
public:
	struct 参数 {
		float 左边;
		float 顶边;
		float 宽度;
		float 高度;
	};
	virtual ~组件基类() {}
	virtual bool 销毁(bool 同步 = false) = 0;

	float 左边, 顶边, 宽度, 高度;
	容器类* 父容器 = NULL;

protected:
	void _子入父列表(bool 静态组件不入父 = true);
};
