#pragma once
#include "控件类.h"

定义_枚举型(编辑框边框, char,
	无边框 = 0, 凹入式 = 1, 凸出式 = 2, 浅凹入式 = 3, 镜框式 = 4, 单线边框式 = 5
);

定义_枚举型(编辑框滚动条, char,
	无 = 0, 横向滚动条 = 1, 纵向滚动条 = 2, 横向及纵向滚动条 = 3
);

定义_枚举型(编辑框输入方式, char,
	通常方式 = 0, 只读方式 = 1, 密码输入 = 2, 整数文本输入 = 3, 小数文本输入 = 4,
	输入字节 = 5, 输入短整数 = 6, 输入整数 = 7, 输入长整数 = 8, 输入小数 = 9,
	输入双精度小数 = 10, 输入日期时间 = 11
);

定义_枚举型(编辑框转换方式, char,
	无 = 0, 大写到小写 = 1, 小写到大写 = 2
);

定义_枚举型(编辑框调节器方式, char,
	无调节器 = 0, 自动调节器 = 1, 手动调节器 = 2
);

static void _更新编辑框格式矩形(HWND hwnd);

class 编辑框 :public 控件类
{
public:
	struct 参数 : 控件类::参数 {
		定义_子组件通用构造方法;
		StrW 内容 = L"";
		编辑框边框 边框 = 编辑框边框::凹入式;
		字体 字体;
		颜色 文本颜色 = 颜色::黑色;
		颜色 背景颜色 = 颜色::白色;
		bool 隐藏选择 = true;
		int 最大允许长度 = 0;
		bool 是否允许多行 = false;
		编辑框滚动条 滚动条 = 编辑框滚动条::无;
		横向对齐 对齐方式 = 横向对齐::左对齐;
		编辑框输入方式 输入方式 = 编辑框输入方式::通常方式;
		StrW 密码遮盖字符 = L"*";
		编辑框转换方式 转换方式 = 编辑框转换方式::无;
		编辑框调节器方式 调节器方式 = 编辑框调节器方式::无调节器;
		int 调节器底限值 = -32767;
		int 调节器上限值 = 32767;
	}static _df;

	bool 创建(const 参数& cs, 容器类* 父窗口 = NULL, HWND 父句柄 = NULL);

	void 内容_(const StrW& txt);
	StrW _取内容();
	int 取内容长度();

	void 加入文本(const StrW& 欲加入文本);

	void 边框_(编辑框边框 v);
	void 文本颜色_(颜色 cr);

	颜色 _取背景颜色(颜色** out = 0);

	//编辑框的背景颜色较为特殊，需要采用背景画刷来实现
	void 背景颜色_(颜色 cr, bool 是否重画 = true);

	void 隐藏选择_(bool is);
	void 最大允许长度_(int len);
	void 是否允许多行_(bool is);
	void 滚动条_(编辑框滚动条 v);
	void 对齐方式_(横向对齐 v);
	void 输入方式_(编辑框输入方式 v);
	void 密码遮盖字符_(const StrW& v);
	void 转换方式_(编辑框转换方式 v);
	void 调节器方式_(编辑框调节器方式 v);
	void 调节器底限值_(int v);
	void 调节器上限值_(int v);

	void 置起始选择位置(int pos);
	int 取起始选择位置();

	/**置被选择字符数
	 * @param len 若给-1相当于全选，0相当于取消选择
	 * @param i<可空> 如果为空则从当前光标处开始
	 */
	void 置被选择字符数(int len, 可空<int> i = 空);
	int 取被选择字符数();

	/**替换当前被选中文本
	 * @param txt
	 */
	void 置被选择文本(const StrW& txt);
	/**获取当前被选中的文本内容
	 * @return 选中的文本，若无选择则返回空
	 */
	StrW 取被选择文本();

public:
	virtual void 事件_内容被改变();
	virtual void 事件_调节钮被按下(int 按钮值);
	virtual LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	定义_字体成员;
	StrW 内容;
	bool _不即时写内容属性_ = false;
	编辑框边框 边框;
	颜色 文本颜色;
	颜色 背景颜色;
	bool 隐藏选择;
	int 最大允许长度;
	bool 是否允许多行;
	编辑框滚动条 滚动条;
	横向对齐 对齐方式;
	编辑框输入方式 输入方式;
	StrW 密码遮盖字符;
	编辑框转换方式 转换方式;
	编辑框调节器方式 调节器方式;
	int 调节器底限值;
	int 调节器上限值;

protected:

	DWORD _取编辑风格();

	DWORD _取扩展风格();

	void _重建控件();

	void _同步样式();

	bool _校验输入字符(WPARAM wParam);
};
