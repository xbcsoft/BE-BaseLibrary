# Sciter 编辑框光标忽大忽小补丁技术报告

## 1. 问题概述

目标引擎版本：

- Sciter x86：`5.0.3.15`
- Sciter x64：`5.0.3.15`

问题表现为：Windows 非 100% DPI 缩放下，同一个编辑框内的文本光标会随字符位置在不同粗细之间变化。测试环境为 125% DPI，字符串 `785829865` 的末尾位置可以稳定复现 2 像素宽光标，而多数位置为 1 像素宽。

该问题不是 HTML、CSS、字体文件或封装层创建系统 Caret 的代码造成的，而是 Sciter 内部把变换后的光标宽度与左右端点分别取整，产生了位置相关的舍入误差。

最终补丁同时覆盖 x86 与 x64，并保留 Sciter 根据 DPI、字号或元素变换计算出的动态目标宽度。

## 2. 复现工程

工程目录：

```text
D:\MyDocument\白易程序\@白易模块\SciterUI\编辑框的弹出菜单自绘
```

测试输入：

```text
785829865
```

该字符串最后一个字符后的光标在 125% DPI 下可以稳定命中原始错误路径。

生成命令：

```powershell
# x86，使用工程当前配置
bebuild

# x64 Debug
& "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" `
  "D:\MyDocument\白易程序\@白易模块\SciterUI\编辑框的弹出菜单自绘\编辑框的弹出菜单自绘.vcxproj" `
  /t:Build /p:Configuration=Debug /p:Platform=x64 `
  "/p:OutDir=D:\MyDocument\白易程序\@白易模块\SciterUI\编辑框的弹出菜单自绘\out\"
```

运行程序时必须把工作目录设为 `out`，否则程序使用相对路径读取 `index.html` 时会显示空白窗口。

## 3. 排除 CreateCaret

最初曾在 Win32 API 层 Hook `CreateCaret`，强制宽度为 1，但问题仍然存在。

x86 DLL 中唯一相关调用位于 `0x102E8B82`，其实际参数为：

```cpp
CreateCaret(hwnd, nullptr, 1, 1);
```

高度同样为 1，说明它更接近系统/IME 定位锚点，而不是 Sciter 最终在页面中绘制的可见光标。可见光标实际由 Sciter 通过 Direct2D 的 `FillRectangle` 绘制。

因此，修改 `CreateCaret` 无法根治此问题。

## 4. Direct2D 动态证据

调试器：

```text
x86: C:\Program Files (x86)\Windows Kits\10\Debuggers\x86\cdb.exe
x64: C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe
```

通过在 `d2d1!D2DDeviceContextBase::FillRectangle` 设置断点，直接读取 Sciter 传给 Direct2D 的 `D2D_RECT_F`。

### x86 原始结果

普通位置：

```text
left=100, right=101, width=1
```

字符串末尾错误位置：

```text
left=118, right=120, width=2
```

对应浮点原始数据：

```text
42EC0000 42B60000 42F00000 42E00000
```

### x64 原始结果

与 x86 一致：

```text
left=118, top=91, right=120, bottom=112
```

这证明宽度变化在调用 Direct2D 之前已经由 Sciter 算出，不是 Direct2D 栅格化、显卡驱动或 Windows Caret API 造成的。

## 5. 根因

Sciter 原始算法可以简化为：

```text
left  = trunc(x + 0.5)
right = trunc(x + 0.5 + transformedWidth)
width = right - left
```

在 125% DPI 下，`transformedWidth` 常为 `1.25`。由于 `x` 会随字形推进产生不同的小数部分，因此：

```text
trunc(x + 0.5 + 1.25) - trunc(x + 0.5)
```

会根据字符位置得到 1 或 2。

错误的本质不是目标宽度为小数，而是左右端点各自取整。端点取整误差被转换成了光标宽度误差。

## 6. 最终算法

最终补丁采用：

```text
left   = trunc(x + 0.5)
width  = max(1, round(transformedWidth))
right  = left + width
```

其中：

- `transformedWidth` 继续使用 Sciter 原本计算的值，不写死为 1；
- `cvtss2si` 按 MXCSR 当前舍入模式把宽度转换为整数；
- `cvttss2si` 用于计算已经加过 `0.5` 的左边界；
- 宽度最少为 1，避免缩小变换下得到零宽矩形；
- 右边界从已经取整的左边界派生，因此不再受字符位置小数部分影响。

这既消除了忽粗忽细，也保留 DPI、字号及元素变换可能带来的动态宽度。

## 7. x86 补丁

### 7.1 关键函数

```text
ImageBase: 0x10000000
函数地址: 0x100A9203
IDA 名称: SnapVerticalHairlineToDeviceRect
```

变换宽度由 `sub_100B165F` 计算，原函数在 `0x100A9225` 将结果恢复到 `xmm1`：

```asm
movss xmm1, [ebp-8]
```

这个值必须保留。早期实验曾把该指令 NOP，强制保持 1px；该实验可以证明问题来源，但不适合作为最终补丁，因为它会丢失动态目标宽度。

### 7.2 原始错误代码

地址：

```text
VA:          0x100A925E
文件偏移:   0x000A865E
```

原始机器码：

```text
0F 28 C2 F3 0F 58 C1 F3 0F 2C D8
```

等价逻辑：

```asm
movaps      xmm0, xmm2
addss       xmm0, xmm1
cvttss2si   ebx, xmm0
```

### 7.3 跳转补丁

替换为：

```text
E8 E9 31 39 00 90 90 90 90 90 90
```

即调用位于 `0x1043C44C` 的辅助代码，其余空间填充 NOP。

代码洞：

```text
VA:          0x1043C44C
文件偏移:   0x0043B84C
长度:        17 bytes
```

辅助代码机器码：

```text
F3 0F 2D D9
83 FB 01
83 D3 00
F3 0F 2C CA
03 D9
C3
```

等价汇编：

```asm
cvtss2si   ebx, xmm1       ; width = round(transformedWidth)
cmp        ebx, 1
adc        ebx, 0          ; width = max(1, width)
cvttss2si  ecx, xmm2       ; left
add        ebx, ecx        ; right = left + width
ret
```

## 8. x64 补丁

### 8.1 关键函数

```text
ImageBase: 0x180000000
函数地址: 0x1800F1788
IDA 名称: SnapVerticalHairlineToDeviceRect
```

x64 调用栈中的关键路径：

```text
d2d1!D2DDeviceContextBase::FillRectangle
sciter64+0xFC6D
sciter64+0x1BD914
sciter64+0x6766C
```

矩形取整函数由 `0x1801BD903` 调用。

### 8.2 原始错误代码

地址：

```text
VA:          0x1800F17ED
文件偏移:   0x000F0BED
```

原始机器码：

```text
0F 28 C1 F3 0F 58 C2 F3 44 0F 2C C0
```

等价逻辑：

```asm
movaps      xmm0, xmm1
addss       xmm0, xmm2
cvttss2si   r8d, xmm0
```

### 8.3 跳转补丁

替换为：

```text
E8 39 79 4C 00 90 90 90 90 90 90 90
```

代码洞：

```text
VA:          0x1805B912B
文件偏移:   0x005B852B
长度:        21 bytes
```

辅助代码机器码：

```text
F3 44 0F 2D C2
41 83 F8 01
41 83 D0 00
F3 0F 2C C9
41 01 C8
C3
```

等价汇编：

```asm
cvtss2si   r8d, xmm2       ; width = round(transformedWidth)
cmp        r8d, 1
adc        r8d, 0          ; width = max(1, width)
cvttss2si  ecx, xmm1       ; left
add        r8d, ecx        ; right = left + width
ret
```

### 8.4 x64 寄存器存活陷阱

最初的 x64 辅助代码曾使用 `EAX` 保存左边界。虽然原函数稍后会重新计算 `EAX`，但调用辅助代码之后紧接着还有：

```asm
test al, al
```

`AL` 保存矩形方向/分支标志，覆盖 `EAX` 会同时破坏 `AL`。其表现是横向宽度已经修正，但光标顶部由 `91` 错误变成 `83`：

```text
42B60000 -> 42A60000
```

最终改用此处已经空闲的 `ECX` 保存左边界，保持 `AL` 不变。修正后横向和纵向坐标均正确。

这是移植二进制补丁时必须检查活跃寄存器，而不能只逐句翻译 x86 汇编的典型案例。

## 9. 验证结果

### x86

```text
起始位置: left=33,  right=34,  width=1
末尾位置: left=118, right=119, width=1
```

### x64

```text
起始位置: left=33,  top=91, right=34,  bottom=112
末尾位置: left=118, top=91, right=119, bottom=112
```

x64 原始末尾结果为：

```text
left=118, top=91, right=120, bottom=112
```

补丁只修正了错误的右边界，没有改变光标高度和垂直位置。

x86 还使用 `60px` 字号进行过首尾位置验证，未再出现由字符位置导致的宽度跳变；测试 CSS 已恢复，最终 HTML 没有残留测试改动。

## 10. 文件与校验值

| 文件 | 大小 | SHA256 |
|---|---:|---|
| 原始 x86 `sciter_5.0.3.15.dll` | 6,558,720 | `2050605B6A580409F2B406229BD0015DE76FE408C214E345B8BB7D9C592A9698` |
| 修补 x86 `sciter_5.0.3.15_fixed.dll` | 6,558,720 | `E61545FE82B77BAD8B6DCA4F4853237471BBDB53F4DA98C6A2366B5D0A706D01` |
| 原始 x64 `sciter64_5.0.3.15.dll` | 8,506,368 | `A82FC7D5BFECBD138766FEC18F3A15CD470AC1533E166356E353BD0FFBBBB394` |
| 修补 x64 `sciter64_5.0.3.15_fixed.dll` | 8,506,368 | `89169B0548F9EB8A852693D8982D75721EEE795AAD77045C8F72ABB269E30987` |

修补文件：

```text
D:\MyDocument\白易程序\@白易模块\SciterUI\编辑框的弹出菜单自绘\out\sciter_5.0.3.15_fixed.dll
D:\MyDocument\白易程序\@白易模块\SciterUI\编辑框的弹出菜单自绘\out\sciter64_5.0.3.15_fixed.dll
```

IDA 数据库：

```text
C:\Users\admin\AppData\Local\Temp\be\sciter_5.0.3.15.dll.i64
C:\Users\admin\AppData\Local\Temp\be\sciter64_5.0.3.15.dll.i64
```

## 11. 封装层测试加载方式

封装源码：

```text
D:\Develop\白菜Win32UI\VS-IDE本体插件\BEModG\SciterUI\SciterUI.cpp
```

当前测试阶段按位数使用绝对路径：

```cpp
#ifdef _WIN64
g_sciterModule = LoadLibraryW(
    LR"(D:\MyDocument\白易程序\@白易模块\SciterUI\编辑框的弹出菜单自绘\out\sciter64_5.0.3.15_fixed.dll)");
#else
g_sciterModule = LoadLibraryW(
    LR"(D:\MyDocument\白易程序\@白易模块\SciterUI\编辑框的弹出菜单自绘\out\sciter_5.0.3.15_fixed.dll)");
#endif
```

必须这样测试的原因是原封装使用 `Miniz::加载DLL`，每次启动都会从资源重新释放 DLL。直接修改临时目录中的 DLL 会在下次启动时被资源版本覆盖。

完成产品化后，应使用已修补 DLL 重新生成 `DLL::sciter` 与 `DLL::sciter64` 资源，再恢复 `Miniz::加载DLL`。在资源更新完成前，不应删除绝对路径加载代码，否则测试程序会重新使用未修补的内嵌 DLL。

## 12. 风险与回归建议

补丁所在函数负责把竖向细矩形吸附到设备坐标，它可能不只服务于编辑光标。最终补丁没有写死宽度，也没有修改颜色、位置、矩形高度或 Direct2D 状态，只把“分别取整两端”改成“独立取整宽度”，因此影响范围相对可控。

建议在资源正式替换前回归：

1. Windows DPI：100%、125%、150%、175%、200%；
2. 普通字号与大字号输入框；
3. 单行 `input`、密码框与多行 `textarea`；
4. 页面 CSS `transform: scale(...)` 与应用缩放；
5. 其他 1px 竖线、表格边框及 SVG/CSS 细线；
6. x86、x64 的 Debug 和 Release 构建；
7. 光标首部、中部、末尾及不同中英文字符位置。

## 13. 结论

Sciter 5.0.3.15 的编辑光标忽粗忽细由内部设备坐标吸附算法造成。原算法分别舍入左、右端点，使小数 DPI 缩放误差随字符位置进入最终宽度。

最终补丁保留 Sciter 的动态变换宽度，将宽度独立舍入后再与左边界相加。x86 与 x64 均已通过 Direct2D 入参级验证，错误位置从 2 像素恢复为稳定的目标宽度，同时保持垂直坐标和字号/DPI适配逻辑不变。
