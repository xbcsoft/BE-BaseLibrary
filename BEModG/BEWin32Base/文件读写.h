#pragma once
#include "stdafx.h"

class 文件读写
{
public:
	bool 不存在则创建 = true;
	文件读写(bool 必须存在 = true);

	HANDLE hFile = 0;

	/**打开文件
	 * @param 文件名
	 * @param 打开方式=3 1读，2写，3读写。
	 * @param 共享方式=3 1允许别的进程读，2允许别的进程写，3=1+2
	 * @return
	 */
	bool 打开文件(c_StrX 文件名, int 打开方式 = 3, int 共享方式 = 3);

	int64 获取当前位置();
	int64 取文件大小();

	/**移动读写位置
	 * @param 起始位置=0 0=开头，1=当前，2=结尾
	 * @param 移动距离=0
	 * @return
	 */
	bool 移动读写位置(int 起始位置 = 0, int64 移动距离 = 0);
	Bytes 读入字节集(int 读入长度 = 0, bool 是否不偏移 = false);
	bool 写出字节集(const Bytes& 文件数据, bool 是否不偏移 = false);
	bool 关闭文件();
	void 当前位置截断文件();

	/**从当前位置开始删除指定数量的字节
	 * @param 删的字节数
	 * @return
	 */
	bool 删除字节集(int64 删的字节数);
	void 文件扩容(int64 扩容大小);
	~文件读写();
};

Bytes 读入文件(c_StrX 文件路径);

bool 写到文件(c_StrX 文件路径, const Bytes& dat);

/**将相对路径转换到绝对路径（当然自身已经是绝对路径的不影响）
 * @param path 相对路径
 * @return
 */
StrW 取绝对路径(c_StrW path);

bool 判断是否为相对路径(c_StrW path);
