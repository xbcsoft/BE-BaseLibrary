#ifndef _BEWin32UIProj
#include <BEWin32UI/VSDllSDK.hpp>
#else
#include "../../VSDllSDK.hpp"
#endif
#include "../超级列表框.h"

超级列表框* UI_超级列表框_创建(byte* fb, 容器类* 父窗口)
{
	if (fb==nullptr){
		UI_销毁控件(父窗口); //当参数1给出空指针时此方法变为销毁此投入的参数2实例对象
		return 0;
	}
	超级列表框::参数 cs;
	cs.左边 = gFBf(fb, 0);
	cs.顶边 = gFBf(fb, 1);
	cs.宽度 = gFBf(fb, 2);
	cs.高度 = gFBf(fb, 3);
	cs.可视 = gFBb(fb, 4);
	cs.禁止 = gFBb(fb, 5);
	cs.可停留焦点 = gFBb(fb, 6);
	cs.停留顺序 = gFBi(fb, 7);
	cs.边框 = gFBi(fb, 8);
	cs.文本颜色 = gFBi(fb, 9);
	cs.文本背景色 = gFBi(fb, 10);
	cs.背景颜色 = gFBi(fb, 11);
	cs.字体 = gFBpo<字体>(fb, 12);
	cs.类型 = gFBi(fb, 13);
	cs.图标对齐方式 = gFBi(fb, 14);
	cs.自动排列图标 = gFBb(fb, 15);
	cs.标题自动换行 = gFBb(fb, 16);
	cs.报表列 = gFB(fb, 17);
	cs.无表头 = gFBb(fb, 18);
	cs.表头图片组 = gFB(fb, 19);
	cs.表头可单击 = gFBb(fb, 20);
	cs.整行选择 = gFBb(fb, 21);
	cs.显示表格线 = gFBb(fb, 22);
	cs.表列可拖动 = gFBb(fb, 23);
	cs.图片组 = gFB(fb, 24);
	cs.状态图片组 = gFB(fb, 25);
	cs.热点跟踪 = gFBb(fb, 26);
	cs.手形指针 = gFBb(fb, 27);
	cs.标注非热点 = gFBb(fb, 28);
	cs.标注热点 = gFBb(fb, 29);
	cs.允许编辑 = gFBb(fb, 30);
	cs.排序方式 = gFBi(fb, 31);
	cs.是否有检查框 = gFBb(fb, 32);
	cs.平面滚动条 = gFBb(fb, 33);
	cs.单一选择 = gFBb(fb, 34);
	cs.现行选中项 = gFBi(fb, 35);
	cs.始终显示选择项 = gFBb(fb, 36);
	cs.表项 = gFB(fb, 37);
	超级列表框* ui = new 超级列表框;
	ui->创建(cs, 父窗口);
	return ui;
}

超级列表框::参数 超级列表框::_df;
static UI属性元信息 META_超级列表框[] = {
	//int 索引,LPWSTR 名称;DWORD 类型;union 默认值;LPWSTR 枚举值;LPWSTR 说明;BOOL 可空;BOOL 子项;
	_MK属性(0, L"左边", UI属性类型::数值, 超级列表框::_df.左边, NULL, L"", false, false),
	_MK属性(1, L"顶边", UI属性类型::数值, 超级列表框::_df.顶边, NULL, L"", false, false),
	_MK属性(2, L"宽度", UI属性类型::数值, 超级列表框::_df.宽度, NULL, L"", false, false),
	_MK属性(3, L"高度", UI属性类型::数值, 超级列表框::_df.高度, NULL, L"", false, false),
	_MK属性(4, L"可视", UI属性类型::布尔, 超级列表框::_df.可视, NULL, L"", false, false),
	_MK属性(5, L"禁止", UI属性类型::布尔, 超级列表框::_df.禁止, NULL, L"", false, false),
	_MK属性(6, L"可停留焦点", UI属性类型::布尔, 超级列表框::_df.可停留焦点, NULL, L"", false, false),
	_MK属性(7, L"停留顺序", UI属性类型::整数, 超级列表框::_df.停留顺序, NULL, L"", false, false),
	_MK属性(8, L"边框", UI属性类型::枚举, (int)超级列表框::_df.边框, L"0.无边框|1.凹入式|2.凸出式|3.浅凹入式|4.镜框式|5.单线边框式", L"", false, false),
	_MK属性(9, L"文本颜色", UI属性类型::颜色, (int)超级列表框::_df.文本颜色, NULL, L"", false, false),
	_MK属性(10, L"文本背景色", UI属性类型::颜色, (int)超级列表框::_df.文本背景色, NULL, L"", false, false),
	_MK属性(11, L"背景颜色", UI属性类型::颜色, (int)超级列表框::_df.背景颜色, NULL, L"", false, false),
	_MK属性(12, L"字体", UI属性类型::字体, &超级列表框::_df.字体, NULL, L"", false, false),
	_MK属性(13, L"类型", UI属性类型::枚举, (int)超级列表框::_df.类型, L"0.大图标列表框|1.小图标列表框|2.普通列表框|3.报表列表框", L"", false, false),
	_MK属性(14, L"图标对齐方式", UI属性类型::枚举, (int)超级列表框::_df.图标对齐方式, L"0.顶部对齐|1.左边对齐", L"", false, true),
	_MK属性(15, L"自动排列图标", UI属性类型::布尔, 超级列表框::_df.自动排列图标, NULL, L"", false, true),
	_MK属性(16, L"标题自动换行", UI属性类型::布尔, 超级列表框::_df.标题自动换行, NULL, L"", false, true),
	_MK属性(17, L"报表列", UI属性类型::自定数据, &超级列表框::_df.报表列, NULL, L"仅报表列表框类型可用", false, true),
	_MK属性(18, L"无表头", UI属性类型::布尔, 超级列表框::_df.无表头, NULL, L"", false, true),
	_MK属性(19, L"表头图片组", UI属性类型::自定数据, &超级列表框::_df.表头图片组, NULL, L"", false, true),
	_MK属性(20, L"表头可单击", UI属性类型::布尔, 超级列表框::_df.表头可单击, NULL, L"", false, true),
	_MK属性(21, L"整行选择", UI属性类型::布尔, 超级列表框::_df.整行选择, NULL, L"", false, true),
	_MK属性(22, L"显示表格线", UI属性类型::布尔, 超级列表框::_df.显示表格线, NULL, L"", false, true),
	_MK属性(23, L"表列可拖动", UI属性类型::布尔, 超级列表框::_df.表列可拖动, NULL, L"", false, true),
	_MK属性(24, L"图片组", UI属性类型::自定数据, &超级列表框::_df.图片组, NULL, L"", false, false),
	_MK属性(25, L"状态图片组", UI属性类型::自定数据, &超级列表框::_df.状态图片组, NULL, L"", false, false),
	_MK属性(26, L"热点跟踪", UI属性类型::布尔, 超级列表框::_df.热点跟踪, NULL, L"", false, false),
	_MK属性(27, L"手形指针", UI属性类型::布尔, 超级列表框::_df.手形指针, NULL, L"", false, false),
	_MK属性(28, L"标注非热点", UI属性类型::布尔, 超级列表框::_df.标注非热点, NULL, L"", false, false),
	_MK属性(29, L"标注热点", UI属性类型::布尔, 超级列表框::_df.标注热点, NULL, L"", false, false),
	_MK属性(30, L"允许编辑", UI属性类型::布尔, 超级列表框::_df.允许编辑, NULL, L"", false, false),
	_MK属性(31, L"排序方式", UI属性类型::枚举, (int)超级列表框::_df.排序方式, L"0.不排序|1.正向排序|2.逆向排序", L"", false, false),
	_MK属性(32, L"是否有检查框", UI属性类型::布尔, 超级列表框::_df.是否有检查框, NULL, L"", false, false),
	_MK属性(33, L"平面滚动条", UI属性类型::布尔, 超级列表框::_df.平面滚动条, NULL, L"", false, false),
	_MK属性(34, L"单一选择", UI属性类型::布尔, 超级列表框::_df.单一选择, NULL, L"", false, false),
	_MK属性(35, L"现行选中项", UI属性类型::整数, 超级列表框::_df.现行选中项, NULL, L"", false, false),
	_MK属性(36, L"始终显示选择项", UI属性类型::布尔, 超级列表框::_df.始终显示选择项, NULL, L"", false, false),
	_MK属性(37, L"表项", UI属性类型::自定数据, &超级列表框::_df.表项, NULL, L"", false, false),
};

namespace UI超级列表框二维编辑器实现 {

class 编辑窗口;

struct 编辑器列表框 : 列表框 {
    编辑窗口* 主窗口 = NULL;
    void 事件_列表项被选择() override;
    void 事件_双击选择() override;
};

struct 编辑器标题框 : 编辑框 {
    编辑窗口* 主窗口 = NULL;
    void 事件_内容被改变() override;
};

struct 编辑器按钮 : 按钮 {
    enum 操作类型 { 确定, 取消, 插入主项, 向后插入, 插入子项, 删除, 选择图片, 选择状态图片 } 操作;
    编辑窗口* 主窗口 = NULL;
    void 事件_被单击() override;
};

class 编辑窗口 : public 窗口 {
public:
    bool 表项模式 = false;
    bool 已确认 = false;
    bool 正在同步 = false;
    int 当前行 = -1, 当前子项 = -1;
    Bytes 编辑结果;
    BEIDEFN IDE回调 = NULL;

    编辑器列表框 项目列表;
    编辑器标题框 标题框;
    编辑框 宽度框, 对齐框, 图片框, 状态框, 缩进框, 数值框;
    选择框 图片居右框;
    标签 标题标签, 宽度标签, 对齐标签, 图片标签, 状态标签, 缩进标签, 数值标签, 说明标签;
    编辑器按钮 插入主项按钮, 向后插入按钮, 插入子项按钮, 删除按钮, 确定按钮, 取消按钮, 选择图片按钮, 选择状态图片按钮;

    Arraybe<StrW> 列标题;
    Arraybe<int> 列宽, 列对齐, 列图片;
    Arraybe<bool> 列图片居右;
    Arraybe<Arraybe<StrW>> 文本;
    Arraybe<Arraybe<int>> 图片;
    Arraybe<int> 状态, 缩进, 数值;

    void 解析(const Bytes* dat) {
        if (!dat || !dat->buf || dat->size <= 0) return;
        if (!表项模式) deFB(*dat, 列标题, 列宽, 列对齐, 列图片, 列图片居右);
        else deFB(*dat, 文本, 图片, 状态, 缩进, 数值);
    }
    static int 取整数(编辑框& e, int 默认值 = 0) {
        StrW s = e._取内容();
        return s._size() ? _wtoi((const wchar_t*)s) : 默认值;
    }
    static void 置整数(编辑框& e, int v) {
        wchar_t buf[32] = { 0 };
        swprintf_s(buf, L"%d", v);
        e.内容_(buf);
    }
    static int 编码项目(int 行, int 子项) { return (行 & 0xffff) | ((子项 & 0xffff) << 16); }
    static void 解码项目(int v, int& 行, int& 子项) { 行 = v & 0xffff; 子项 = (v >> 16) & 0xffff; }

    void 保存当前() {
        if (正在同步 || 当前行 < 0) return;
        if (!表项模式) {
            if (当前行 >= 列标题.count) return;
            列标题[当前行] = 标题框._取内容();
            列宽[当前行] = 取整数(宽度框, 50);
            列对齐[当前行] = 取整数(对齐框, 0);
            列图片[当前行] = 取整数(图片框, -1);
            列图片居右[当前行] = 图片居右框._取选中();
        } else {
            if (当前行 >= 文本.count || 当前子项 < 0 || 当前子项 >= 文本[当前行].count) return;
            文本[当前行][当前子项] = 标题框._取内容();
            图片[当前行][当前子项] = 取整数(图片框, -1);
            if (当前子项 == 0) {
                状态[当前行] = 取整数(状态框, -1);
                缩进[当前行] = 取整数(缩进框, 0);
                数值[当前行] = 取整数(数值框, 0);
            }
        }
    }

    void 标题即时改变() {
        if (正在同步 || 当前行 < 0) return;
        StrW 新标题 = 标题框._取内容();
        if (!表项模式) {
            if (当前行 >= 列标题.count) return;
            列标题[当前行] = 新标题;
        } else {
            if (当前行 >= 文本.count || 当前子项 < 0 || 当前子项 >= 文本[当前行].count) return;
            文本[当前行][当前子项] = 新标题;
        }
        int sel = 项目列表._取现行选中项();
        if (sel < 0) return;
		StrW 显示标题;
        if (表项模式 && 当前子项 > 0) 显示标题 = L"    ";
        显示标题 += 新标题;
        正在同步 = true;
        项目列表.置项目文本(sel, 显示标题);
        项目列表.现行选中项_(sel);
        正在同步 = false;
    }

    void 刷新列表(int 选中行 = -1, int 选中子项 = -1) {
        正在同步 = true;
        项目列表.清空();
        int 选中索引 = -1;
        if (!表项模式) {
            for (int i = 0; i < 列标题.count; ++i) {
                int p = 项目列表.加入项目(列标题[i], 编码项目(i, 0));
                if (i == 选中行) 选中索引 = p;
            }
        } else {
            for (int i = 0; i < 文本.count; ++i) {
                StrW t = 文本[i].count ? 文本[i][0] : StrW(L"");
                int p = 项目列表.加入项目(t, 编码项目(i, 0));
                if (i == 选中行 && 选中子项 == 0) 选中索引 = p;
                for (int j = 1; j < 文本[i].count; ++j) {
                    StrW s = L"    "; s += 文本[i][j];
                    p = 项目列表.加入项目(s, 编码项目(i, j));
                    if (i == 选中行 && j == 选中子项) 选中索引 = p;
                }
            }
        }
        if (选中索引 < 0 && 项目列表.取项目数() > 0) 选中索引 = 0;
        项目列表.现行选中项_(选中索引);
        正在同步 = false;
        载入选中();
    }

    void 载入选中() {
        int sel = 项目列表._取现行选中项();
        if (sel < 0 || sel >= 项目列表.取项目数()) {
            当前行 = 当前子项 = -1;
            return;
        }
        解码项目(项目列表.取项目数值(sel), 当前行, 当前子项);
        正在同步 = true;
        if (!表项模式) {
            标题框.内容_(列标题[当前行]);
            置整数(宽度框, 列宽[当前行]); 置整数(对齐框, 列对齐[当前行]); 置整数(图片框, 列图片[当前行]);
            图片居右框.选中_(列图片居右[当前行]);
        } else {
            标题框.内容_(文本[当前行][当前子项]); 置整数(图片框, 图片[当前行][当前子项]);
            bool root = 当前子项 == 0;
            状态框.禁止_(!root); 选择状态图片按钮.禁止_(!root); 缩进框.禁止_(!root); 数值框.禁止_(!root);
            if (root) { 置整数(状态框, 状态[当前行]); 置整数(缩进框, 缩进[当前行]); 置整数(数值框, 数值[当前行]); }
        }
        正在同步 = false;
    }

    void 列表选择改变() {
        if (正在同步) return;
        保存当前();
        载入选中();
        标题框.获取焦点();
        SendMessageW(标题框.窗口句柄, EM_SETSEL, 0, -1);
    }

    void 执行操作(编辑器按钮::操作类型 op) {
        if (op == 编辑器按钮::确定) {
            保存当前();
            编辑结果 = !表项模式 ? FB(列标题, 列宽, 列对齐, 列图片, 列图片居右) : FB(文本, 图片, 状态, 缩进, 数值);
            已确认 = true; 销毁(); return;
        }
        if (op == 编辑器按钮::取消) { 已确认 = false; 销毁(); return; }
        if (op == 编辑器按钮::选择图片) {
            if (!IDE回调) return;
            Bytes* 图片组数据 = IDE回调(IDE方法::取属性值, FB(L"图片组"));
            int v = 取整数(图片框, -1);
            if (图片组数据 && UI_图片组_选择图片(图片组数据, v, v, L"图片浏览选择", true))
                置整数(图片框, v);
            delete 图片组数据;
            return;
        }
        if (op == 编辑器按钮::选择状态图片) {
            if (!IDE回调) return;
            Bytes* 状态图片组数据 = IDE回调(IDE方法::取属性值, FB(L"状态图片组"));
            int v = 取整数(状态框, -1);
            if (状态图片组数据 && UI_图片组_选择图片(状态图片组数据, v, v, L"状态图片浏览选择", true))
                置整数(状态框, v);
            delete 状态图片组数据;
            return;
        }
        保存当前();
        if (op == 编辑器按钮::删除) {
            if (当前行 < 0) return;
            if (!表项模式) {
                列标题.del(当前行); 列宽.del(当前行); 列对齐.del(当前行); 列图片.del(当前行); 列图片居右.del(当前行);
                刷新列表(当前行 < 列标题.count ? 当前行 : 列标题.count - 1, 0);
            } else if (当前子项 > 0) {
                文本[当前行].del(当前子项); 图片[当前行].del(当前子项);
                刷新列表(当前行, 当前子项 < 文本[当前行].count ? 当前子项 : 文本[当前行].count - 1);
            } else {
                文本.del(当前行); 图片.del(当前行); 状态.del(当前行); 缩进.del(当前行); 数值.del(当前行);
                刷新列表(当前行 < 文本.count ? 当前行 : 文本.count - 1, 0);
            }
            return;
        }
        if (!表项模式) {
            int pos = 列标题.count;
            if (当前行 >= 0 && 当前行 < 列标题.count)
                pos = (op == 编辑器按钮::向后插入) ? 当前行 + 1 : 当前行;
            列标题.insert(pos, StrW(L"新列"));
            列宽.insert(pos, 50); 列对齐.insert(pos, 0); 列图片.insert(pos, -1); 列图片居右.insert(pos, false);
            刷新列表(pos, 0); return;
        }
        if (op == 编辑器按钮::插入子项 && 当前行 >= 0 && 当前行 < 文本.count) {
            int pos = 当前子项 >= 0 ? 当前子项 + 1 : 文本[当前行].count;
            if (pos < 1) pos = 1;
            if (pos > 文本[当前行].count) pos = 文本[当前行].count;
            文本[当前行].insert(pos, StrW(L"子项"));
            图片[当前行].insert(pos, -1);
            刷新列表(当前行, pos);
        } else {
            int pos = (当前行 >= 0 && 当前行 < 文本.count) ? 当前行 + 1 : 文本.count;
            Arraybe<StrW> ts; ts.push(StrW(L"新表项"));
            Arraybe<int> is; is.push(-1);
            文本.insert(pos, ts); 图片.insert(pos, is); 状态.insert(pos, -1); 缩进.insert(pos, 0); 数值.insert(pos, 0);
            刷新列表(pos, 0);
        }
    }

    bool 事件_被关闭() override { 已确认 = false; return true; }
    void 事件_快捷键(WORD cmdID) override { if (cmdID == 快捷键_取ID(VK_RETURN)) 执行操作(编辑器按钮::确定); }

    bool 打开(const Bytes* dat, bool itemMode, BEIDEFN ide回调 = NULL) {
        表项模式 = itemMode;
        IDE回调 = ide回调;
        解析(dat);
        窗口::参数 cs;
        cs.标题 = itemMode ? L"超级列表框表项设置" : L"超级列表框报表列设置";
        cs.类名 = itemMode ? L"BESuperListItemEditor" : L"BESuperListColumnEditor";
        cs.宽度 = 485; cs.高度 = 360; cs.边框 = 窗口边框::普通固定边框;
        cs.控制按钮 = true; cs.最小化按钮 = false; cs.最大化按钮 = false;
        cs.Esc键关闭 = true; cs.子控件焦点导航 = true; cs.图标ID = 0; cs.可视 = false;
        cs.总在最前 = true; cs.在任务栏显示 = false;
        if (!窗口::创建(cs, NULL, false)) return false;
        if (_g_interWnd && _g_interWnd->窗口句柄) SetWindowLongPtrW(窗口句柄, GWLP_HWNDPARENT, (LONG_PTR)_g_interWnd->窗口句柄);
        SetWindowLongPtrW(窗口句柄, GWL_EXSTYLE, GetWindowLongPtrW(窗口句柄, GWL_EXSTYLE) & ~WS_EX_APPWINDOW);

        列表框::参数 lc; lc.左边 = 12; lc.顶边 = 12; lc.宽度 = 205; lc.高度 = 205; lc.可停留焦点 = true;
        项目列表.主窗口 = this; 项目列表.创建(lc, this);

        创建标签(标题标签, L"标题:", 230, 18, 62); 创建编辑框(标题框, 293, 16, 152, L""); 标题框.主窗口 = this;
        创建标签(图片标签, L"图片索引:", 230, 52, 62); 创建编辑框(图片框, 293, 50, 78, L"-1");
        创建按钮(选择图片按钮, 编辑器按钮::选择图片, L"...", 377, 49, 34);
        if (!itemMode) {
            创建标签(宽度标签, L"列宽:", 230, 86, 62); 创建编辑框(宽度框, 293, 84, 78, L"50");
            创建标签(对齐标签, L"对齐(0/1/2):", 230, 120, 90); 创建编辑框(对齐框, 321, 118, 50, L"0");
            选择框::参数 cc; cc.左边 = 230; cc.顶边 = 152; cc.宽度 = 120; cc.高度 = 24; cc.标题 = L"图片居右"; 图片居右框.创建(cc, this);
        } else {
            创建标签(状态标签, L"状态图片:", 230, 86, 62); 创建编辑框(状态框, 293, 84, 78, L"-1");
            创建按钮(选择状态图片按钮, 编辑器按钮::选择状态图片, L"...", 377, 83, 34);
            创建标签(缩进标签, L"缩进:", 230, 120, 62); 创建编辑框(缩进框, 293, 118, 78, L"0");
            创建标签(数值标签, L"项目数值:", 230, 154, 62); 创建编辑框(数值框, 293, 152, 78, L"0");
        }
        创建按钮(插入主项按钮, 编辑器按钮::插入主项, itemMode ? L"向后插入(&I)" : L"插入(&I)", 12, 228, 86);
        if (itemMode) {
            创建按钮(插入子项按钮, 编辑器按钮::插入子项, L"插入子项(&S)", 104, 228, 86);
        } else {
            创建按钮(向后插入按钮, 编辑器按钮::向后插入, L"向后插入(&N)", 104, 228, 86);
        }
        创建按钮(删除按钮, 编辑器按钮::删除, L"删除(&R)", 196, 228, 72);
        创建按钮(确定按钮, 编辑器按钮::确定, L"确定(&O)", 276, 228, 72);
        创建按钮(取消按钮, 编辑器按钮::取消, L"取消(&C)", 354, 228, 72);

        标签::参数 sc; sc.左边 = 12; sc.顶边 = 268; sc.宽度 = 433; sc.高度 = 52;
        sc.是否自动折行 = true;
        sc.标题 = itemMode
            ? L"说明：表项可包含多个子项。图片索引、状态图片均为 -1 时表示不指定图片；缩进和项目数值仅属于表项本身，子项只保存标题与图片索引。"
            : L"说明：报表列用于报表型超级列表框。列宽按逻辑像素保存；对齐方式 0=左对齐、1=居中、2=右对齐；图片索引 -1 表示不指定图片。";
        说明标签.创建(sc, this);
        窗口_添加风格(确定按钮.窗口句柄, BS_DEFPUSHBUTTON);

        快捷键_注册(VK_RETURN); 刷新列表(); _组件列表.clear(); 可视 = true;
        ShowWindow(窗口句柄, SW_SHOWNORMAL); SetForegroundWindow(窗口句柄);
        窗口::完毕(true);
        return 已确认;
    }

private:
    void 创建标签(标签& c, const charW* 文本, float 左, float 顶, float 宽) {
        标签::参数 p; p.左边 = 左; p.顶边 = 顶; p.宽度 = 宽; p.高度 = 22; p.标题 = 文本; c.创建(p, this);
    }
    void 创建编辑框(编辑框& c, float 左, float 顶, float 宽, const charW* 内容) {
        编辑框::参数 p; p.左边 = 左; p.顶边 = 顶; p.宽度 = 宽; p.高度 = 24; p.内容 = 内容; p.可停留焦点 = true; c.创建(p, this);
    }
    void 创建按钮(编辑器按钮& b, 编辑器按钮::操作类型 op, const charW* 文本, float 左, float 顶, float 宽) {
        b.主窗口 = this; b.操作 = op;
        按钮::参数 p; p.左边 = 左; p.顶边 = 顶; p.宽度 = 宽; p.高度 = 28; p.标题 = 文本; p.可停留焦点 = true; b.创建(p, this);
    }
};

inline void 编辑器列表框::事件_列表项被选择() { if (主窗口) 主窗口->列表选择改变(); }
inline void 编辑器列表框::事件_双击选择() { if (主窗口) 主窗口->列表选择改变(); }
inline void 编辑器标题框::事件_内容被改变() { if (主窗口) 主窗口->标题即时改变(); }
inline void 编辑器按钮::事件_被单击() { if (主窗口) 主窗口->执行操作(操作); }

inline Bytes* 编辑报表列(const Bytes* 当前数据, BEIDEFN ide方法) { 编辑窗口 w; if (!w.打开(当前数据, false, ide方法)) return NULL; return new Bytes(w.编辑结果); }
inline Bytes* 编辑表项(const Bytes* 当前数据, BEIDEFN ide方法) { 编辑窗口 w; if (!w.打开(当前数据, true, ide方法)) return NULL; return new Bytes(w.编辑结果); }

} // namespace UI超级列表框二维编辑器实现

void UI_超级列表框_置属性(超级列表框* ui, int methodi, byte* fb)
{
	switch (methodi)
	{
	case 0:
		ui->移动(gFBf(fb, 0), 空, 空, 空);
		break;
	case 1:
		ui->移动(空, gFBf(fb, 0), 空, 空);
		break;
	case 2:
		ui->移动(空, 空, gFBf(fb, 0), 空);
		break;
	case 3:
		ui->移动(空, 空, 空, gFBf(fb, 0));
		break;
	case 4:
		//可视
		break;
	case 5:
		//禁止
		break;
	case 6:
		//可停留焦点
		break;
	case 7:
		//停留顺序
		break;
	case 8:
		ui->边框_(gFBi(fb, 0));
		break;
	case 9:
		ui->文本颜色_(gFBi(fb, 0));
		break;
	case 10:
		ui->文本背景色_(gFBi(fb, 0));
		break;
	case 11:
		ui->背景颜色_(gFBi(fb, 0));
		break;
	case 12:
		ui->置字体(gFBpo<字体>(fb, 0));
		break;
	case 13:
		ui->类型_(gFBi(fb, 0));
		break;
	case 14:
		ui->图标对齐方式_(gFBi(fb, 0));
		break;
	case 15:
		ui->自动排列图标_(gFBb(fb, 0));
		break;
	case 16:
		ui->标题自动换行_(gFBb(fb, 0));
		break;
	case 17:
		ui->报表列_(gFB(fb, 0));
		break;
	case 18:
		ui->无表头_(gFBb(fb, 0));
		break;
	case 19: {
		图片组 临时图片组;
		临时图片组.h = 临时图片组.导入图片组字节集(gFB(fb, 0));
		if (临时图片组.h) ui->置表头图片组(临时图片组);
		break;
	}
	case 20:
		ui->表头可单击_(gFBb(fb, 0));
		break;
	case 21:
		ui->整行选择_(gFBb(fb, 0));
		break;
	case 22:
		ui->显示表格线_(gFBb(fb, 0));
		break;
	case 23:
		ui->表列可拖动_(gFBb(fb, 0));
		break;
	case 24: {
		图片组 临时图片组;
		临时图片组.h = 临时图片组.导入图片组字节集(gFB(fb, 0));
		if (临时图片组.h) ui->置图片组(临时图片组);
		break;
	}
	case 25: {
		图片组 临时图片组;
		临时图片组.h = 临时图片组.导入图片组字节集(gFB(fb, 0));
		if (临时图片组.h) ui->置状态图片组(临时图片组);
		break;
	}
	case 26:
		ui->热点跟踪_(gFBb(fb, 0));
		break;
	case 27:
		ui->手形指针_(gFBb(fb, 0));
		break;
	case 28:
		ui->标注非热点_(gFBb(fb, 0));
		break;
	case 29:
		ui->标注热点_(gFBb(fb, 0));
		break;
	case 30:
		ui->允许编辑_(gFBb(fb, 0));
		break;
	case 31:
		ui->排序方式_(gFBi(fb, 0));
		break;
	case 32:
		ui->是否有检查框_(gFBb(fb, 0));
		break;
	case 33:
		ui->平面滚动条_(gFBb(fb, 0));
		break;
	case 34:
		ui->单一选择_(gFBb(fb, 0));
		break;
	case 35:
		ui->现行选中项_(gFBi(fb, 0));
		break;
	case 36:
		ui->始终显示选择项_(gFBb(fb, 0));
		break;
	case 37:
		ui->表项_(gFB(fb, 0));
		break;
	}
}
