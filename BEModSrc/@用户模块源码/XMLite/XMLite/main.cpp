#include "stdafx.h"
#include "XMLite.h" //由MakeBEC.exe自动生成
using namespace be;

void main()
{
	XMLite xml;
	// 1. 添加节点
	xml.添加节点("", "Root", "");
	xml.添加节点("Root", "User", "");
	xml.添加节点("Root/User", "Name", "Alice");
	xml.添加节点("Root/User", "Age", "18");

	// 2. 打印看看
	print("初始XML:\n", xml);

	// 3. 取节点文本
	print("User Name is:", xml.取节点文本("Root/User/Name"));

	// 4. 置节点文本
	xml.置节点文本("Root/User/Age", "20");
	print("Modified Age:", xml.取节点文本("Root/User/Age"));

	// 5. 取子节点名称
	Arraybe<StrU8> children;
	xml.取子节点名称("Root/User", children);
	print("Children of Root/User:", children);

	// 6. 删除节点
	xml.删除节点("Root/User/Age");
	print("删除了Age后的XML:\n", xml);

	// 7. 测试导入与导出
	StrU8 b = xml.导出();
	XMLite xml2;
	if (xml2.导入(b)) {
		print("导入成功! 解析出的根节点名:", xml2.取根节点名());
		print("重新打印导入后的XML:\n", xml2);
	}
}