# Sciter 5.0.3.15 原生拉伸光标仲裁补丁技术报告

## 1. 改造结果

本次改造解决了白易宿主自定义窗口拉伸热区与 Sciter DOM/原生滚动条光标逻辑互相争用的问题，并同时适配：

- x86：`sciter_5.0.3.15_mod.dll`
- x64：`sciter64_5.0.3.15_mod.dll`

最终效果：

- 鼠标位于宿主指定的拉伸热区时，稳定显示对应的水平、垂直或对角拉伸光标。
- 鼠标离开拉伸热区后，立即把光标控制权交还 Sciter。
- Sciter 内的文本输入 I-beam、链接手形、自定义 URL 光标等功能保持正常。
- 不注入 HTML 元素，不修改页面布局，不创建额外子窗口。
- 不在运行时修改 IAT、函数入口或页面保护，不使用 `VirtualProtect`，不属于运行时 Hook。

## 2. 问题根因

自定义无边框窗口的拉伸热区由宿主在 `WM_NCHITTEST` 中判定。宿主命中边缘后会设置拉伸光标；但 Sciter 随后仍会处理 DOM 鼠标事件、滚动条 hover 和 `SC_SET_CURSOR`，并在 DLL 内再次调用 `USER32!SetCursor`。

因此，同一个鼠标移动过程中存在两个光标写入者：

1. 白易宿主根据原生拉伸命中码设置 `IDC_SIZEWE`、`IDC_SIZENS` 等光标。
2. Sciter 根据 DOM/滚动条状态设置箭头、I-beam、hand 等光标。

当鼠标从页面内部移动到边缘时，两边会交替调用 `SetCursor`，形成肉眼可见的闪烁。从窗口外侧进入时消息顺序不同，所以该方向可能看起来正常。

单纯永久禁用 Sciter 的全部 `SetCursor` 虽然能消除闪烁，但会令拉伸光标无法恢复，并破坏编辑框、链接等正常光标。因此最终方案必须是“按宿主拉伸状态有条件地放行”。

## 3. 最终架构

宿主和修改版 DLL 通过 DLL 可写数据段中的一个字节交换状态：

```text
鼠标移动 / 命中测试
        │
        ▼
白易宿主调用用户的拉伸区域回调
        │
        ├─ 命中拉伸区域 ──► 标志字节 = 1 ──► 宿主提交拉伸光标
        │                                      │
        │                                      ▼
        │                         Sciter SetCursor 包装器跳过本次调用
        │
        └─ 普通 DOM 区域 ──► 标志字节 = 0
                                               │
                                               ▼
                              Sciter SetCursor 包装器调用原始 USER32 API
```

该标志只表示“当前 Sciter 的光标提交是否应被宿主拉伸逻辑压制”，不存放具体光标类型。具体的拉伸光标仍由宿主根据 `HTLEFT`、`HTBOTTOMRIGHT` 等命中码设置。

## 4. DLL 二进制修改

### 4.1 IDA 定位结果

x86 版 `USER32!SetCursor` IAT 地址为 `0x10492538`，真实调用点只有两处：

| 项目 | 虚拟地址 | 文件偏移 |
|---|---:|---:|
| SetCursor 调用 1 | `0x102DF095` | `0x2DE495` |
| SetCursor 调用 2 | `0x103FBB04` | `0x3FAF04` |
| SetCursor IAT | `0x10492538` | — |

x64 版 `USER32!SetCursor` IAT 地址为 `0x180618A80`，同样只有两处真实调用：

| 项目 | 虚拟地址 | 文件偏移 |
|---|---:|---:|
| SetCursor 调用 1 | `0x1803F66C4` | `0x3F5AC4` |
| SetCursor 调用 2 | `0x180577483` | `0x576883` |
| SetCursor IAT | `0x180618A80` | — |

需要特别注意：x86 中 `0x10492534` 实际是相邻的 `SetFocus` IAT，而不是 `SetCursor`。早期按裸地址推断时曾误改这两处 `SetFocus` 调用，IDA 根据导入名称和交叉引用确认后已经全部恢复。以后适配新版本必须以导入表符号和交叉引用为准，不能只按相邻地址或字节外观推测。

### 4.2 x86 条件包装器

x86 包装器放置在 `.text` 尾部预留空间：

- 包装器 RVA：`0x491D80`
- 虚拟地址：`0x10491D80`
- 文件偏移：`0x491180`
- 仲裁标志 RVA：`0x5F57F0`
- 仲裁标志虚拟地址：`0x105F57F0`
- 仲裁标志文件偏移：`0x5F3BF0`

`.text` 的 `VirtualSize` 从 `0x490D7C` 扩展为 `0x490E00`，未越过下一节的 RVA。

包装器等价逻辑：

```asm
call next
next:
pop  edx                         ; 取得当前实际装载地址，兼容 ASLR
cmp  byte ptr [edx+163A6Bh], 0  ; 读取宿主写入的仲裁标志
jne  suppressed
jmp  dword ptr [edx+7B3h]       ; 尾调用原始 USER32!SetCursor

suppressed:
xor  eax, eax
ret  4                           ; 模拟 stdcall，并清理 HCURSOR 参数
```

两处原始的 `call [SetCursor IAT]` 被替换为到包装器的相对调用：

```text
0x102DF095: E8 E6 2C 1B 00 90
0x103FBB04: E8 77 62 09 00 90
```

包装器使用 `call/pop` 取得运行时地址，访问标志和 IAT 时不嵌入固定 ImageBase，因此 DLL 被 ASLR 重定位后仍然有效。

### 4.3 x64 条件包装器

x64 包装器放置在 `.text` 尾部预留空间：

- 包装器 RVA：`0x617E60`
- 虚拟地址：`0x180617E60`
- 文件偏移：`0x617260`
- 仲裁标志 RVA：`0x7CF7F8`
- 仲裁标志虚拟地址：`0x1807CF7F8`
- 仲裁标志文件偏移：`0x7CE9F8`

`.text` 的 `VirtualSize` 从 `0x616E58` 扩展为 `0x617000`，未越过下一节的 RVA。

包装器等价逻辑：

```asm
cmp  byte ptr [rip+1B7991h], 0  ; RIP 相对读取仲裁标志
jne  suppressed
jmp  qword ptr [rip+0C11h]      ; RIP 相对尾调用 USER32!SetCursor

suppressed:
xor  eax, eax
ret
```

两处调用点被替换为到包装器的相对调用：

```text
0x1803F66C4: E8 97 17 22 00 90
0x180577483: E8 D8 09 0A 00 90
```

x64 使用 RIP 相对寻址，不需要增加新的 PE 重定位项，同样兼容 ASLR。

### 4.4 PE 完整性处理

补丁写入后重新计算并写回了 PE CheckSum：

| 架构 | PE CheckSum | 修改版 SHA-256 |
|---|---:|---|
| x86 | `0x0064303B` | `2050605B6A580409F2B406229BD0015DE76FE408C214E345B8BB7D9C592A9698` |
| x64 | `0x0082AD92` | `A82FC7D5BFECBD138766FEC18F3A15CD470AC1533E166356E353BD0FFBBBB394` |

原始文件备份：

| 架构 | 备份文件 | 原始 SHA-256 |
|---|---|---|
| x86 | `sciter_5.0.3.15_mod.orig-05840AC7.bak.dll` | `05840AC718180E8A28FAB10D81F27C00B20967252994D440D8D4C5DB17A48065` |
| x64 | `sciter64_5.0.3.15_mod.orig-9B3F6590.bak` | `9B3F659029F288C351B21A230538E2765C8B7A5F5476EC59903ACA5B74928697` |

## 5. C++ 宿主交互

### 5.1 按架构加载修改版 DLL

宿主保存 `LoadLibraryW` 返回的模块基址，并根据编译架构选择 DLL：

```cpp
#ifdef _WIN64
L"...\\sciter64_5.0.3.15_mod.dll"
#else
L"...\\sciter_5.0.3.15_mod.dll"
#endif
```

仲裁字节的 RVA 也按架构固定：

```cpp
#ifdef _WIN64
static constexpr SIZE_T SCITER_CURSOR_SUPPRESS_FLAG_RVA = 0x7CF7F8;
#else
static constexpr SIZE_T SCITER_CURSOR_SUPPRESS_FLAG_RVA = 0x5F57F0;
#endif
```

宿主通过 `模块基址 + RVA` 得到可写字节：

```cpp
auto* flag = reinterpret_cast<volatile BYTE*>(
    reinterpret_cast<BYTE*>(g_sciterModule) +
    SCITER_CURSOR_SUPPRESS_FLAG_RVA);
*flag = enabled ? 1 : 0;
```

这里写入的是 DLL 原本可写 `.data` 节尾部预留字节，不需要修改页面保护。

### 5.2 `WM_NCHITTEST`

每次命中测试开始时先清零仲裁标志。只有用户回调或默认边缘算法最终确认返回拉伸命中码时，才执行以下操作：

1. 把仲裁标志设为 `1`。
2. 根据命中码调用宿主的 `SetCursor` 设置拉伸光标。
3. 返回对应的 `HTLEFT`、`HTTOPRIGHT` 等原生命中结果。

这样从页面内部进入边缘时，Sciter 后续产生的 DOM/滚动条光标更新会被 DLL 包装器抑制。

### 5.3 `WM_SETCURSOR`

`WM_SETCURSOR` 不依赖上一次命中缓存，而是根据当前屏幕坐标重新询问用户定义的拉伸区域：

- 当前仍在拉伸热区：标志设为 `1`，宿主设置拉伸光标并返回 `TRUE`。
- 已离开拉伸热区：标志清零，消息继续交给 Sciter/原窗口过程。

这保证了从边缘回到页面内部时，Sciter 能主动恢复箭头、I-beam 或 hand，而不会残留拉伸光标。

### 5.4 `SC_SET_CURSOR`

Sciter 发出 `SC_SET_CURSOR` 通知时，宿主再次检查当前是否位于自定义拉伸热区：

- 命中拉伸区：宿主提交拉伸光标并保持标志为 `1`；DLL 包装器跳过 Sciter 紧随其后的原生 `SetCursor`。
- 普通 DOM 区域：标志清零并返回 `0`，让 Sciter 执行原始光标逻辑。

普通区域不再由宿主枚举和模拟 Sciter 的光标类型，因此能够完整保留引擎自身的 I-beam、hand、拖放光标及 URL 自定义光标。

## 6. 为什么这不是运行时 Hook

最终实现没有进行以下操作：

- 不在运行时改写 Sciter 的 IAT 指针。
- 不在运行时覆盖 `SetCursor` 或其他函数入口。
- 不安装 trampoline/detour。
- 不调用 `VirtualProtect` 修改代码页权限。
- 不注入 DOM 屏蔽层或额外窗口。

DLL 的两个调用点和包装器是在磁盘文件中预先修改好的。运行时宿主只写一个普通 RW 数据字节，用于告诉包装器本次是否应放行原始 API。

## 7. 已验证项目

- x86 Debug：`bebuild` 编译通过并实际启动。
- x64 Debug：设置 `Platform=x64` 后通过 `bebuild` 编译，输出机器类型为 `0x8664`。
- x86/x64 均确认加载对应修改版 DLL。
- 两个架构均能正确显示原 `index.html` 页面。
- 用户实测：拉伸区域无闪烁，离开后普通光标可恢复，输入框等 Sciter 定制光标正常。

运行测试时必须将 `out` 设为工作目录，因为启动代码使用相对路径加载 `index.html`。

## 8. 版本升级与维护注意事项

该补丁与 Sciter `5.0.3.15` 的具体二进制布局绑定。升级 DLL 后不能沿用旧 RVA，应重新执行：

1. 记录并验证新原版 DLL 的 SHA-256。
2. 在 IDA 导入表中定位真正的 `USER32!SetCursor`。
3. 根据交叉引用确认全部真实调用点，避免误改相邻导入函数。
4. 分别寻找安全的可执行填充区和 `.data` 可写填充区。
5. 重新计算包装器的相对位移、调用点 `rel32` 和段 `VirtualSize`。
6. 确认所有寻址兼容 ASLR。
7. 更新宿主中的仲裁标志 RVA。
8. 重新计算 PE CheckSum，并记录新旧文件哈希。
9. 完成“编译、实际启动、页面显示、拉伸、输入框和链接光标”全流程测试。

另外，当前仲裁字节是进程内同一 Sciter DLL 实例共享的。一般桌面 UI 的鼠标与窗口消息在同一 UI 线程串行执行，因此符合现有使用场景；如果以后同一进程创建多个独立 UI 线程并同时驱动多个 Sciter 窗口，建议把协议扩展为按线程或按窗口保存状态。
