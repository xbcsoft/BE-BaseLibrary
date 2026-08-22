#pragma once
/**@ModuleTitle:
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:
*  @datetime:
*  @description:
*/
#include "BEMod/BEMod.h"
// #define USE_EDGEMODE_JSRT // 注释掉，使用传统 jscript9 实现（支持Win7+IE10/11）



#pragma comment(lib, "jsrt.lib") // jscript9.dll对应的导入库


StrU8 json通用转义(const StrU8& 值, bool 不转义单引号 = false, bool 转义双引号 = false);

StrU8 json通用反转义(const StrU8& 值);

//#ifndef JsRuntimeAttributeNone
//#define JsRuntimeAttributeNone 0
//#endif


class JSONie
{
public:
	JSONie();

	// 禁用拷贝
	JSONie(const JSONie&) = delete;
	JSONie& operator=(const JSONie&) = delete;

	// 移动构造
	JSONie(JSONie&& other) noexcept;

	// 移动赋值
	JSONie& operator=(JSONie&& other) noexcept;

	~JSONie();

	// _销毁
	void _销毁();

	// 解析
	bool 导入(const StrU8& 数据文本);

	// 解析W
	bool 导入W(const StrW& 宽字符文本);

	// 清空，可空: 0空对象,1空数组
	void 清空(int 重置为类型 = 0);

	// 到json文本
	StrU8 导出();

	StrW 导出W();

	friend AutoStr __AutoStr__(JSONie& json);

	// 删属性
	void 删属性(const StrU8& 属性名, char 分隔符 = '.');


	/**置属性
	 * @param 属性名 支持多级属性路径
	 * @param 值 可为字符串、C++字面量任意数值、C++字面量bool型、字符串对象(需开启参数4)
	 * @param 值为字面量对象=false 开启后将字符串内容作为字面量对象写入
	 * @param 分隔符 多级属性路径的分隔符，默认为'.'
	 */
	template<typename T>
	void 置属性(const StrU8& 属性名, const T& 值, bool 值为字面量对象 = false, char 分隔符 = '.') {
		StrW path = _取属性路径(属性名, 分隔符);
		StrW code = path + L" = ";
		if constexpr (be::is_boolean<T>) {
			code += (值 ? L"true;" : L"false;");
		} else if constexpr (be::is_numeric<T>) {
			code += 到文本<W>((double)值) + L";";
		} else {
			if (值为字面量对象) {
				code += U8toW(StrU8(值)) + L";";
			} else {
				code += L"'" + U8toW(json通用转义(值)) + L"';";
			}
		}
		_Eval(code);
	}


	// 取属性数值
	double 取属性数值(const StrU8& 属性名, char 分隔符 = '.');

	// 取属性
	StrU8 取属性(const StrU8& 属性名, char 分隔符 = '.', bool 格式化取出对象文本 = false);

	// 取属性对象
	JSONie 取属性对象(const StrU8& 属性名, char 分隔符 = '.');


	// 取数组成员数
	int 取数组成员数(const StrU8& 对象名);

	// 加数组成员
	void 加数组成员(const StrU8& 对象名, const StrU8& 成员值, bool 为对象或整数 = false);

	// 取数组成员对象
	JSONie 取数组成员对象(const StrU8& 对象名, int 索引);

	// 取数组成员
	StrU8 取数组成员(const StrU8& 对象名, int 索引, NilOpt<c_StrU8> 属性名 = nil, bool 格式化取出对象文本 = false);

	// 置数组成员
	void 置数组成员(const StrU8& 对象名, int 索引, const StrU8& 成员值, bool 为对象或整数 = false);

	// 删数组成员
	void 删数组成员(const StrU8& 对象名, int 索引);

	// 取数组成员数值
	double 取数组成员数值(const StrU8& 对象名, int 索引);

	// 对象是否存在
	bool 对象是否存在(const StrU8& 对象名);

	// 取所有属性名
	int 取所有属性名(const StrU8& 对象名, Arraybe<StrU8>& 属性名数组);

	// 取类型 0:空,1:数,2:对象,3:数组,4:文本
	int 取类型(const StrU8& 对象名);

private:
	// JSRT 的 Runtime/Context/Value 句柄在 SDK 中均为 void*。
	// 类接口使用原始兼容类型，避免生成的头文件依赖 <jsrt.h>。
	void* m_runtime = nullptr;
	void* m_context = nullptr;
	bool m_isRef = false;
	StrW m_basePath = L"d";

	// 专供内部用来生成引用对象的私有构造函数
	JSONie(void* rt, void* ctx, const StrW& path);

	StrW _取对象名(const StrU8& objName);

	StrW _取属性路径(const StrU8& 属性名, char 分隔符);

	static StrW _转义json单引号反斜杠(const StrU8& 值);

	void* _Eval(const StrW& code);

	StrW _ToString(void* val);

	double _ToNumber(void* val);

	bool _ToBool(void* val);

	StrW _ExecTextMethod(const StrW& code);
	double _ExecNumMethod(const StrW& code);
	bool _ExecBoolMethod(const StrW& code);
};

AutoStr __AutoStr__(JSONie& json);
