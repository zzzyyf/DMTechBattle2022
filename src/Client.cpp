/**
 * 1. 读取文件，按大小均分起始位置，各自顺延起始位置到下一行开始，然后记录下所有的起始位置
 * 2. 解析文件，编码请求（u32请求数 +（u32操作数个数 + 操作数列表））
 * 3. epoll 网络通信（支持 unix domain socket 和普通 tcp socket）
 * 4. 解析响应（u32结果数 + 结果列表）
*/
#include "Client.h"

namespace dm {


}   // end of namespace dm;
