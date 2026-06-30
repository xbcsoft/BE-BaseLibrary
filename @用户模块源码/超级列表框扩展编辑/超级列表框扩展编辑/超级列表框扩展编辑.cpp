#include "stdafx.h"
#include <扩展界面库一/超级列表框.h>

// 回调函数类型定义：返回 false 阻止编辑，返回 true 允许编辑
typedef bool(*超级列表框扩展编辑回调)(HWND 列表框句柄, int 准备编辑行, int 准备编辑列);

namespace {
class 超级列表框扩展编辑
{
public:
	超级列表框扩展编辑() {}
	~超级列表框扩展编辑()
	{
		if (!_列表框) return;

		// 如果正在编辑，强制结束
		结束编辑(false);

		// 移除子类化
		RemoveWindowSubclass(_列表框->窗口句柄, 列表框子类化过程, (UINT_PTR)this);
	}

	bool 启用(超级列表框& 列表框, 超级列表框扩展编辑回调 回调 = nullptr)
	{
		_列表框 = &列表框;
		_回调 = 回调;

		// 对超级列表框进行子类化
		if (!SetWindowSubclass(_列表框->窗口句柄, 列表框子类化过程, (UINT_PTR)this, (DWORD_PTR)this)) {
			_列表框 = nullptr;
			_回调 = nullptr;
			return false;
		}

		return true;
	}

	void 开始编辑(int 行, int 列)
	{
		if (!_列表框) return;

		// 触发回调确认是否允许编辑
		if (_回调 && !_回调(_列表框->窗口句柄, 行, 列)) {
			return;
		}

		// 【黑科技核心】如果已经在编辑，不调用结束编辑！
		// 只静默保存旧内容（置标题会自动局部刷新该表项），不触发任何重画/隐藏/焦点变化
		// 因为编辑框是同一个HWND，SetFocus是no-op，不会产生WM_KILLFOCUS
		if (_编辑行 != -1 && _编辑框.窗口句柄) {
			StrW text = _编辑框._取内容();
			_列表框->置标题(_编辑行, _编辑列, text);
		}

		_编辑行 = 行;
		_编辑列 = 列;
		_是否保存 = true;

		// 获取子表项的屏幕物理矩形
		RECT rect = { 0 };
		rect.top = 列;
		// 第一列使用 LVIR_LABEL 避免返回整行，其他子列使用 LVIR_BOUNDS 获取整个单元格宽度
		rect.left = (列 == 0) ? LVIR_LABEL : LVIR_BOUNDS;
		if (!SendMessageW(_列表框->窗口句柄, LVM_GETSUBITEMRECT, 行, (LPARAM)&rect)) {
			_编辑行 = -1;
			_编辑列 = -1;
			return;
		}

		// 保证高度足够以正常显示编辑框的凹入式边框，不进行上下收缩，解决双击时有时无边框的问题
		// 针对第一列左移3像素，宽度增加1像素；其他列左边收缩1像素，右边不收缩（宽度增加1像素以完美契合）
		if (列 == 0) {
			rect.left -= 3;
			rect.right += 1;
		} else {
			rect.left += 1;
		}

		// 将超级列表框客户区坐标转换为父窗口客户区坐标
		POINT pt1 = { rect.left, rect.top };
		POINT pt2 = { rect.right, rect.bottom };
		ClientToScreen(_列表框->窗口句柄, &pt1);
		ClientToScreen(_列表框->窗口句柄, &pt2);
		ScreenToClient(_列表框->父窗口句柄, &pt1);
		ScreenToClient(_列表框->父窗口句柄, &pt2);

		int x = pt1.x;
		int y = pt1.y;
		int w = pt2.x - pt1.x;
		int h = pt2.y - pt1.y;

		// 如果编辑框尚未创建，则进行创建 and 初始化（无需 new，直接初始化成员）
		if (!_编辑框.窗口句柄) {
			// 使用 rdpi 将物理像素转换为白易虚拟像素 (vpx)
			编辑框::参数 cs{ rdpi((float)x), rdpi((float)y), rdpi((float)w), rdpi((float)h) };
			cs.内容 = _列表框->取标题(行, 列);
			cs.字体 = _列表框->取字体();
			_编辑框.创建(cs, NULL, _列表框->父窗口句柄);

			// 对编辑框进行子类化，以拦截焦点丢失及回车/ESC按键
			SetWindowSubclass(_编辑框.窗口句柄, 编辑框子类化过程, (UINT_PTR)this, (DWORD_PTR)this);
		}

		_编辑框.内容_(_列表框->取标题(行, 列));
		_编辑框.置被选择字符数(-1);
		SetWindowPos(_编辑框.窗口句柄, HWND_TOP, x, y, w, h, SWP_SHOWWINDOW);
	}

	void 结束编辑(bool 保存内容)
	{
		if (!_编辑框.窗口句柄) return;
		if (_编辑行 == -1 || _编辑列 == -1) return;

		if (保存内容 && _列表框) {
			StrW text = _编辑框._取内容();
			_列表框->置标题(_编辑行, _编辑列, text);
		}

		// 隐藏编辑框
		HWND hwndEdit = _编辑框.窗口句柄;
		if (hwndEdit) {
			ShowWindow(hwndEdit, SW_HIDE);
		}

		_编辑行 = -1;
		_编辑列 = -1;
	}

	超级列表框* 取列表框() const { return _列表框; }
	编辑框* 取编辑框() const { return _编辑框.窗口句柄 ? const_cast<编辑框*>(&_编辑框) : nullptr; }
	int 取编辑行() const { return _编辑行; }
	int 取编辑列() const { return _编辑列; }

private:
	超级列表框* _列表框 = nullptr;
	编辑框 _编辑框; // 直接作为对象成员，避免指针的 new/delete 动态分配开销
	超级列表框扩展编辑回调 _回调 = nullptr;
	int _编辑行 = -1;
	int _编辑列 = -1;
	bool _是否保存 = true;

	static LRESULT CALLBACK 列表框子类化过程(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		超级列表框扩展编辑* pThis = reinterpret_cast<超级列表框扩展编辑*>(dwRefData);
		if (!pThis) return DefSubclassProc(hwnd, msg, wParam, lParam);

		switch (msg) {
		case WM_LBUTTONDBLCLK: {
			LRESULT res = DefSubclassProc(hwnd, msg, wParam, lParam);

			short x = (short)LOWORD(lParam);
			short y = (short)HIWORD(lParam);
			LVHITTESTINFO ht = { 0 };
			ht.pt.x = x;
			ht.pt.y = y;
			SendMessageW(hwnd, LVM_SUBITEMHITTEST, 0, (LPARAM)&ht);
			if (ht.iItem != -1 && ht.iSubItem != -1) {
				pThis->开始编辑(ht.iItem, ht.iSubItem);

				// 【黑科技】DefSubclassProc 处理双击时会让 ListView 排队一个异步 WM_PAINT，
				// 这个延迟重绘会在消息循环中覆盖编辑框的蓝底选区。
				// 解决方案：先强制 ListView 立即完成所有挂起绘制（画在编辑框下面），
				// 然后强制编辑框无效化并立即重绘（画在最上面），确保蓝底选区始终可见。
				UpdateWindow(hwnd);
				if (pThis->_编辑框.窗口句柄) {
					RedrawWindow(pThis->_编辑框.窗口句柄, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
				}
			}
			return res;
		}
		case WM_VSCROLL:
		case WM_HSCROLL:
		case WM_MOUSEWHEEL: {
			if (pThis->_编辑框.窗口句柄 && pThis->_编辑行 != -1) {
				SetFocus(hwnd);
			}
			break;
		}
		case WM_DESTROY: {
			pThis->结束编辑(false);
			RemoveWindowSubclass(hwnd, 列表框子类化过程, uIdSubclass);

			// 移除属性并释放对象以防止内存泄漏
			超级列表框扩展编辑* pExt = (超级列表框扩展编辑*)RemovePropW(hwnd, L"Edit_Obj");
			if (pExt)delete pExt;
			break;
		}
		}

		return DefSubclassProc(hwnd, msg, wParam, lParam);
	}

	static LRESULT CALLBACK 编辑框子类化过程(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		超级列表框扩展编辑* pThis = reinterpret_cast<超级列表框扩展编辑*>(dwRefData);
		if (!pThis) return DefSubclassProc(hwnd, msg, wParam, lParam);

		switch (msg) {
		case WM_CHAR:
		case WM_IME_CHAR:
		case WM_PASTE:
		case WM_CUT:
		case WM_UNDO:
		case WM_CLEAR:
		case EM_REPLACESEL:
		case WM_KEYUP: {
			LRESULT res = DefSubclassProc(hwnd, msg, wParam, lParam);
			if (pThis->取编辑行() != -1 && pThis->取列表框()) {
				StrW text = pThis->取编辑框()->_取内容();
				pThis->取列表框()->置标题(pThis->取编辑行(), pThis->取编辑列(), text);

				// 解决边框被列表框重画覆盖的问题：
				// 1. 让列表框立即完成因为置标题排队的重绘
				UpdateWindow(pThis->取列表框()->窗口句柄);
				// 2. 强行使整个编辑框的非客户区（包括3D边框）和客户区无效并立即强制重画到最顶层
				RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
			}
			return res;
		}
		case WM_KILLFOCUS: {
			pThis->结束编辑(pThis->_是否保存);
			pThis->_是否保存 = true;
			break;
		}
		case WM_KEYDOWN: {
			if (wParam == VK_RETURN) {
				pThis->_是否保存 = true;
				SetFocus(pThis->_列表框->窗口句柄);
				return 0;
			} else if (wParam == VK_ESCAPE) {
				pThis->_是否保存 = false;
				SetFocus(pThis->_列表框->窗口句柄);
				return 0;
			}
			break;
		}
		case WM_DESTROY: {
			RemoveWindowSubclass(hwnd, 编辑框子类化过程, uIdSubclass);
			break;
		}
		}

		return DefSubclassProc(hwnd, msg, wParam, lParam);
	}
};

} // namespace


bool 超级列表框_置扩展编辑(HWND 列表框句柄, 超级列表框扩展编辑回调 回调)
{
	if (!列表框句柄) return false;

	超级列表框* pList = (超级列表框*)窗口_句柄取对象(列表框句柄);
	if (!pList) return false;

	超级列表框扩展编辑* pExt = (超级列表框扩展编辑*)GetPropW(列表框句柄, L"Edit_Obj");
	if (pExt) {
		delete pExt;
	}
	pExt = new 超级列表框扩展编辑();
	SetPropW(列表框句柄, L"Edit_Obj", (HANDLE)pExt);

	return pExt->启用(*pList, 回调);
}

bool 超级列表框_取消扩展编辑(HWND 列表框句柄)
{
	if (!列表框句柄) return false;

	超级列表框扩展编辑* pExt = (超级列表框扩展编辑*)GetPropW(列表框句柄, L"Edit_Obj");
	if (pExt) {
		delete pExt;
		RemovePropW(列表框句柄, L"Edit_Obj");
		return true;
	}
	return false;
}