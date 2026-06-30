#pragma once
/**@ModuleTitle:
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    msvc141
*  @author:
*  @datetime:
*  @description:
*/
#include "stdafx.h"
#include "native\zlib.h"

//#beExcludeNamespaceSymbol(Bytes)

Bytes GZIP压缩(const Bytes& 待压缩数据);

// Gzip 解压函数
Bytes GZIP解压(const Bytes& 待解压数据);
