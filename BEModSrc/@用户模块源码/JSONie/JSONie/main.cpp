#include "stdafx.h"
#include "JSONie.h" //由MakeBEC.exe自动生成
//此main.cpp仅用作模块测试不参与头文件的解析生成
//若想排除特定cpp文件的解析可在工程属性页-生成事件
//改写MakeBEC.exe参数-excludesrc=多文件[需加空格]
using namespace be;

void main()
{
	JSONie json;
	json.导入(R"( {info:{"ver.sion": "1.0"},"mo.de":'123'} )");

	//1. 置属性相关操作
	json.置属性("info.count", 666);
	json.置属性("mo.de", "{}", true, '/'); // 使用其他分隔符时，点号作为普通属性名字符
	json.置属性("mo.de/myobj", "{hello:'world'}", true, '/');

	json.加数组成员("user", "alice");

	// 2. 直接返回对象并打印 (借助移动语义实现高性能返回)
	JSONie info = json.取属性对象("info");
	print("info 子对象: ", info);

	// 3. 获取内部对象的引用联动操作 (零拷贝)
	JSONie users = json.取属性对象("user");
	users.加数组成员("", "앨리스"); //【最后打印全文时再看看】

	// 4. 链式获取测试（单参时若对象名含特殊字符必须使用JS对象(指定分隔符)形式访问）
	print("链式读取测试: ", json.取属性对象("info").取属性("ver.sion", '/'));

	// 5. 使用自定义分隔符安全获取包含点号的属性
	print("从属性安全获取", json.取属性("info/ver.sion", '/'));

	// 6. 打印全文  StrU8 sJson = json.到json文本();（不需要这么麻烦）
	print("根对象全文: ", json); //内置已经重载AutoStr可直接打印本对象
}
