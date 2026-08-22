import asyncio
import time
import sys
from aiohttp import web

# 兼容 Windows 控制台 UTF-8 输出
if sys.platform == 'win32':
    try:
        sys.stdout.reconfigure(encoding='utf-8')
    except Exception:
        pass

async def handle_api_test(request: web.Request):
    """
    Python 官方 aiohttp 异步协程处理函数
    """
    req_id = request.query.get('id', '0')

    recv_time = time.time()
    recv_time_str = time.strftime('%H:%M:%S', time.localtime(recv_time)) + f".{int(recv_time*1000)%1000:03d}"
    print(f"[RECV] [{recv_time_str}] 请求 #{req_id.rjust(2)} 到达 -> 进入 1 秒异步协程休眠 (asyncio.sleep)", flush=True)

    # 核心：Python 原生协程异步延迟 1.0 秒（非阻塞）
    await asyncio.sleep(1.0)

    resp_time = time.time()
    resp_time_str = time.strftime('%H:%M:%S', time.localtime(resp_time)) + f".{int(resp_time*1000)%1000:03d}"
    print(f"[RESP] [{resp_time_str}] 请求 #{req_id.rjust(2)} 唤醒 -> 发送响应！", flush=True)

    # 官方标准 JSON 响应，自动处理 Content-Length、Keep-Alive 持续复用及 CORS
    return web.json_response(
        {
            "code": 200,
            "id": int(req_id) if req_id.isdigit() else req_id,
            "message": f"请求 #{req_id} 成功由 Python 官方 aiohttp 异步协程处理完毕！",
            "server_recv_time": recv_time_str,
            "server_resp_time": resp_time_str
        },
        headers={
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
            "Access-Control-Allow-Headers": "*"
        }
    )

async def handle_options(request: web.Request):
    """处理 OPTIONS 预检请求"""
    return web.Response(
        status=204,
        headers={
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
            "Access-Control-Allow-Headers": "*"
        }
    )

def create_app():
    app = web.Application()
    app.router.add_get('/api/test', handle_api_test)
    app.router.add_options('/api/test', handle_options)
    return app

if __name__ == '__main__':
    print("=" * 60)
    print("[SERVER] Python 官方 aiohttp 异步协程 HTTP 服务器已启动")
    print("[SERVER] 监听地址: http://127.0.0.1:8000/api/test")
    print("[SERVER] 采用 Python 官方推荐 aiohttp.web.Application 工业级异步网络栈")
    print("=" * 60)

    app = create_app()
    web.run_app(app, host='0.0.0.0', port=8000, access_log=None)
