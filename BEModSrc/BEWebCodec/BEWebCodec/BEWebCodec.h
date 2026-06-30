#pragma once
/**@ModuleTitle: Web常用编解码
*  @version:     1.0
*  @platform:    win32(x86|x64)
*  @compiler:    source
*  @author:      
*  @datetime:    
*  @description:
*/
#include "stdafx.h"
#pragma comment(lib, "crypt32.lib")
namespace BEdecode {
/**
* URL解码 (公开函数)
* 支持 %XX 和 + 号还原
*/
StrA URL(const StrA& 欲解码文本);

/**
 * Base64 解码 (Win32 API)
 */
Bytes Base64(const StrA& 待解码文本);

Bytes GZIP(const Bytes& 待解压数据);
}

namespace BEencode {
/**
 * URL编码 (公开函数)
 * 将特殊字符转换为 %XX 格式，空格转换为 +
 */
StrA URL(const StrA& 欲编码文本);

/**
 * Base64 编码 (Win32 API)
 */
StrA Base64(const Bytes& 待编码数据);

Bytes GZIP(const Bytes& 待压缩数据);
}


#include "HTTP键值对.hpp"
