package main

import (
	"fmt"
	"time"
)

func main() {
	fmt.Println("Start Go 子进程启动...")
	time.Sleep(500 * time.Millisecond)
	fmt.Println("消息 1: 来自 Go 的问候！")
	time.Sleep(500 * time.Millisecond)
	fmt.Println("消息 2: 运行成功。")
	time.Sleep(500 * time.Millisecond)
	fmt.Println("End Go 子进程结束。")
}
