#!/bin/bash
# 这是一个简单的冒烟测试脚本
# 它尝试启动程序，如果程序能运行 5 秒没崩溃，就认为通过

EXE_PATH="./RunGoods" # 这里的名字要和下面 yaml 里的编译输出一致

echo "正在启动应用程序进行冒烟测试..."

# 使用 timeout 命令运行 5 秒
# 退出码 124 表示超时（说明程序正常运行直到被杀掉），视为成功
# 其他退出码视为程序崩溃
timeout 5s $EXE_PATH

EXIT_CODE=$?

if [ $EXIT_CODE -eq 124 ]; then
    echo "✅ 测试通过：程序成功启动并运行了 5 秒。"
    exit 0
else
    echo "❌ 测试失败：程序启动失败或异常退出 (Code: $EXIT_CODE)。"
    exit 1
fi
