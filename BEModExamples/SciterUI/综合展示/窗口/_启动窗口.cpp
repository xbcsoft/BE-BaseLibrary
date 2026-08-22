#include <BEMod.h>

struct __启动窗口 : 窗口
{
	SciterDom dom;
	UINT_PTR timerId = 0;

	void 事件_创建本体()
	{
		SciterUI::全局初始化();
		SciterUI::参数 cs;
		cs.文件_html = "index.html";
		st.创建(cs, this);
		dom = st.取文档模型();
	}
	void 事件_创建完毕()
	{
		// 注入 4 个供前端调用的 C++ 原生能力函数
		dom.注入JS函数("getSysInfo", [this](SciterObj& arg) -> SciterObj {
			StrA sysName, ntVer;
			int buildVer = 取系统版本(sysName, ntVer);
			StrU8 result = sprintF("%s (NT %s, Build %d)", (char*)sysName, (char*)ntVer, buildVer);
			return result;
		});

		dom.注入JS函数("nativeMsgBox", [this](SciterObj& arg) -> SciterObj {
			信息框("这是来自 C++ 宿主的原生 Win32 消息框！", "原生交互演示", MB_OK | MB_ICONINFORMATION);
			return "已弹出原生消息框并确认";
		});

		dom.注入JS函数("openFileDialog", [this](SciterObj& arg) -> SciterObj {
			StrA filePath = 文件对话框_打开("选择文件", "所有文件(*.*)|*.*|文本文件(*.txt)|*.txt", 0, "", true, 窗口句柄);
			if (filePath.len() == 0) {
				return SciterObj("已取消文件选择");
			}
			StrU8 msg = "已选择文件: " + filePath;
			return msg;
		});

		dom.注入JS函数("toggleTopmost", [this](SciterObj& arg) -> SciterObj {
			总在最前_(!总在最前);
			if (总在最前) {
				return "置顶状态: 已开启 (总在最前)";
			} else {
				return "置顶状态: 已关闭";
			}
		});

		dom.注入JS函数("captureScreen", [this](SciterObj& arg) -> SciterObj {
			保存截图到文件();
			return "已触发完整截屏 (Ctrl+Shift+P)";
		});

		// 启动独立无句柄定时器（避免 SciterProcND 吞掉窗口 WM_TIMER 消息）
		timerId = SetTimer(NULL, 0, 1000, [](HWND, UINT, UINT_PTR, DWORD) {
			if (!_启动窗口.窗口句柄) return;
			SYSTEMTIME st;
			GetLocalTime(&st);
			StrU8 timeStr = sprintF("%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
			_启动窗口.dom.调用JS函数("updateClock", timeStr);
		});
	}

	void 事件_尺寸被改变()
	{
		StrU8 sizeStr = sprintF("%d × %d", (int)宽度, (int)高度);
		dom.调用JS函数("updateWindowSize", sizeStr);
	}

	void 事件_将被销毁()
	{
		if (timerId) {
			KillTimer(NULL, timerId);
			timerId = 0;
		}
	}

	bool 保存截图到文件()
	{
		StrX filename = sprintF("screenshot_%I64u.png", GetTickCount64());
		StrA fullPath = 取运行目录() + "\\" + filename;
		Bytes png = dom.保存截图(空, true);
		if (png && 写到文件(fullPath, png)) {
			信息框("已成功保存网页完整截图！\n文件名：" + fullPath, "提示", MB_OK | MB_ICONINFORMATION);
			return true;
		} else {
			信息框("保存网页截图失败！", "错误", MB_OK | MB_ICONERROR);
			return false;
		}
	}

	/**若要同时获取控制键的状态可调 控制键_是否被下()
	 * @param key
	 */
	bool 通用事件_按下某键(BYTE key)
	{
		if (key == 'P' && 控制键_是否按下(VK_CONTROL) && 控制键_是否按下(VK_SHIFT)) {
			保存截图到文件();
		}
		return true;
	}

#pragma region 组件成员
	SciterUI st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;
