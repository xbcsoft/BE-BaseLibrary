#include <BEMod.h>
#include "窗口01_DOM基础.h"

struct _窗口00_JS基础 : 窗口
{
	SciterDom dom;
	void 事件_创建完毕()
	{
		标题_(L"JS基础操作");
		dom = st.取文档模型();
		//对象的引用();
		//数组的引用();
		//实验_传MAP对象跟JS对象哪个性能好();
		//多参的使用();
		//字节集演示();
		方法调用测试();
	}

	void 方法调用测试()
	{
		SciterObj jsObj = dom.执行JS脚本(R"({
			sum: function(a, b) { return a + parseInt(b); },
			greet: function(name) { return "Hello " + name; },
			getNull: function(val) { return val; },
			now: function() { return "2026-08-01"; }
		})");

		SciterObj r1, r2;
		jsObj.方法_("sum", { 10, "20" }, r1);
		jsObj.方法_("greet", "World", r2);
		SciterObj r3 = jsObj.方法("getNull");

		调试输出("【SciterObj.方法 - 单参】 greet('World') = ", r2);
		调试输出("【SciterObj.方法 - 多参】 sum(10,20) = ", r1);
		调试输出("【SciterObj.方法 - 无参】 getNull(nil) = ", r3);

	}

	void 字节集演示()
	{
		dom.执行JS脚本(R"(
		function bytes_test(data){
			document.body.innerText = data;
			var view = new Uint8Array(data);
			document.body.innerText+="\n"+ '{'+ view +'}';
			view[1] = 222; console.log("JS输出",view);
			return view;
		})");
		Bytes bytes = { 1, 2, 3, 4, 5 };
		SciterObj bytesArg(bytes), r;
		dom.调用JS函数_("bytes_test", bytes, r);
		调试输出(bytesArg, r);
	}


	//《实验结果》（按理来说T_MAP传入JS后会进行一次深拷贝，但JS对象的构造特别是多级时开销更大）：
	//【三级 MAP(T_MAP) 构造+投入】耗时 = | 2250 |  ms
	//【三级 JS对象(T_OBJECT) 构造+投入】耗时 = | 14016 |  ms
	void 实验_传MAP对象跟JS对象哪个性能好()
	{
		using be::print;

		调试输出("\n====== 实验：三级对象大数据 (T_MAP vs T_OBJECT) 性能对比 ======");

		// 1. 用 取重复文本 制造较大的字符串
		StrW bigStr = 取重复文本(1000, L"Sciter 3-level nested object benchmark testing string... ");
		print("大字符串字符数: ", bigStr.len());

		// 2. 注入 JS 侧三级对象处理函数
		dom.执行JS脚本(
			"function process3LevelObject(obj) {"
			"   let totalLen = 0;"
			"   for(let k1 in obj) {"
			"       let sub1 = obj[k1];"
			"       for(let k2 in sub1) {"
			"           let sub2 = sub1[k2];"
			"           for(let k3 in sub2) {"
			"               totalLen += sub2[k3].length;"
			"           }"
			"       }"
			"   }"
			"   return totalLen;"
			"}");

		int n1 = 10, n2 = 5, n3 = 5; // 三级结构: 10 * 5 * 5 = 250 个叶子节点

		// ----------------------------------------------------
		// 实验 A：构造 C++ MAP 三级对象 (T_MAP) 并投入 JS 调用
		// ----------------------------------------------------
		int64 t1 = 取启动时间();

		SciterObj mapObj; // C++ 原生 T_MAP 对象
		for (int i = 0; i < n1; i++) {
			for (int j = 0; j < n2; j++) {
				for (int k = 0; k < n3; k++) {
					StrW path = StrW(i) + L"." + StrW(j) + L"." + StrW(k);
					mapObj.置属性(path, SciterObj(bigStr));
				}
			}
		}

		SciterObj res1 = dom.调用JS函数("process3LevelObject", mapObj);

		int64 t2 = 取启动时间();
		int64 costA = t2 - t1;

		print("【三级 MAP (T_MAP) 构造+投入】耗时 = ", costA, " ms");

		// ----------------------------------------------------
		// 实验 B：构造 JS 堆三级对象 (T_OBJECT) 并投入 JS 调用
		// ----------------------------------------------------
		int64 t3 = 取启动时间();

		SciterObj jsObj = dom.执行JS脚本("({})"); // 在 JS 堆建立 T_OBJECT 对象
		for (int i = 0; i < n1; i++) {
			for (int j = 0; j < n2; j++) {
				for (int k = 0; k < n3; k++) {
					StrW path = StrW(i) + L"." + StrW(j) + L"." + StrW(k);
					jsObj.置属性(path, SciterObj(bigStr));
				}
			}
		}

		SciterObj res2 = dom.调用JS函数("process3LevelObject", jsObj);

		int64 t4 = 取启动时间();
		int64 costB = t4 - t3;

		print("【三级 JS对象 (T_OBJECT) 构造+投入】耗时 = ", costB, " ms");
	}

	void 多参的使用()
	{
		SciterObj p3;
		p3.置属性("age", 17);
		p3.置属性("addr.city", "广州");

		//投入引用必须是JS版对象
		SciterObj p4 = dom.执行JS脚本("{age:18,addr:{city:'上海'}}");
		p4.置属性("addr.street", "三环");

		dom.执行JS脚本(
			R"(function test(p1, p2, p3, p4) {
				document.body.innerText = p1 + '\n' + p2 + '\n'
				+ JSON.stringify(p3) + '\n' + JSON.stringify(p4);
				p3.age=20; p3.addr.street='三环';   //纯C++对象这里无法更改（值拷贝到JS）
				p4.age=20;p4.addr.city='北京'; p4.addr.street='零环';
			})");

		//多参这里要求写成C++的数组形式
		dom.调用JS函数("test", { 123, "hello", p3, p4 });
		调试输出(p3, p4);                //p3还是原来的
	}

	void 数组的引用()
	{
		调试输出("\n\n——————测试用例【数组的引用】——————");
		// 1. 原生 C++ 数组 (T_ARRAY) 示例
		SciterObj arr1;
		arr1.加入成员("苹果");
		arr1.加入成员("香蕉");
		arr1.加入成员(100);

		调试输出("【arr1 成员数】=", arr1.取数组成员数());
		调试输出("【arr1 内容】", arr1);
		调试输出("【arr1[1]】", arr1[1]);

		arr1.置成员(1, "鸭梨");
		调试输出("【修改 arr1[1] 后】 = ", arr1);

		// 2. JS 堆Array (T_OBJECT) 示例
		SciterObj arr2 = dom.执行JS脚本("[]");
		arr2.加入成员("红楼梦");
		arr2.加入成员("西游记");
		调试输出("【arr2 成员数】=", arr2.取数组成员数());
		调试输出("【arr2 内容】", arr2);

		arr2.置成员(0, "三国演义");
		调试输出("【修改 arr2[0] 后】 = ", arr2);

		dom.执行JS脚本(
			"function modifyArray(arr) {"
			"   arr[0] = '水浒传';"
			"   arr.push('封神演义');"
			"}");

		dom.调用JS函数("modifyArray", arr1);
		调试输出("【T_ARRAY版】经JS修改后 arr1 = ", arr1);

		dom.调用JS函数("modifyArray", arr2);
		调试输出("【T_OBJECT_Array版】经JS修改后 arr2 = ", arr2);
	}

	void 对象的引用()
	{
		调试输出("\n\n——————测试用例【对象的引用】——————");
		SciterObj o1;
		o1.置属性("age", 17);
		o1.置属性("addr.city", "广州");
		o1.置属性("addr.street", "中山路");
		调试输出(o1);

		SciterObj o2 = o1.取属性("addr");
		调试输出(o2);
		o2.置属性("city", "深圳");
		调试输出(o2, o1); //虽然在C++这里可改写引用
		//但T_MAP是纯的C++对象(无JS环境引用计数)，它不能用于JS交互的引用！
		//当投入JS引擎中的函数时是会进行深拷贝到JS环境中产生新JS版对象(T_OBJECT)

		// 在 JS 引擎中注入一个修改对象的函数
		dom.执行JS脚本(
			"function modifyPerson(p) {"
			"   p.age = 30;"
			"   p.addr.city = '上海';"
			"}");

		dom.调用JS函数("modifyPerson", o1);
		调试输出("【T_MAP版_经JS修改后】 o1 = ", o1);

		//接着测一下它是值拷贝还是引用
		SciterObj o10 = o1;
		o10.置属性("test", "ref");
		调试输出("【T_MAP版_经赋值修改后】 o1 = ", o1);

		//————————————————————————————

		调试输出("\n接下来测T_OBJECT版：", o1);
		SciterObj o3 = dom.执行JS脚本("{}");  //使用dom创建的JS对象就是T_OBJECT版
		//【关于生命周期】：
		//脚本执行完成后，Sciter API 会将该 JS 对象的引用句柄填入 o3.val (类型为 T_OBJECT)，并向 JS 垃圾回收器（GC）注册一次强引用计数（引用计数 +1）；
		//C++ 内部拷贝/赋值：例如 SciterObj o30=o3 或 传入JS调用中，在SciterObj的拷贝构造与赋值运算符中，
		//内部均调用了 g_sapi->ValueCopy(&val, &other.val)：
		//·若对象为T_OBJECT则SciterAPI 会自动对 JS 堆上的对象引用计数再 +1。
		//·若是普通的MAP，实则进行的是值深拷贝

		o3.置属性("age", 17);
		o3.置属性("addr.city", "广州");
		o3.置属性("addr.street", "中山路");
		调试输出(o3);

		SciterObj o4 = o3.取属性("addr");
		调试输出(o4);
		o4.置属性("city", "深圳"); //这个改写是真引用
		调试输出(o3, o4);

		dom.调用JS函数("modifyPerson", o3);
		调试输出("【T_OBJECT版_经JS修改后】 o3 = ", o3);

		//接着测一下它是值拷贝还是引用
		SciterObj o30 = o3;
		o30.置属性("test", "ref");
		调试输出("【T_OBJECT版_经赋值修改后】 o1 = ", o3);
	}

#pragma region 组件成员
	struct _st :SciterUI {
	} st;
#pragma endregion
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} 窗口00_JS基础;
