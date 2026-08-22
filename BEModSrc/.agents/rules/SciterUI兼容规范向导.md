# Sciter HTML 兼容性及移植避坑指南

本指南总结了将常规浏览器 HTML5/CSS3 页面移植到 Sciter UI 时遇到的关键兼容性差异，并以 Sciter 5.0.3.15 下已经验证的界面为基准给出推荐写法。

## 一、自绘窗口标准

> 推荐标杆完整UI皮肤案例：`$(BEInstall)\BEModExamples\SciterUI\MahApps.Metro` （含其中的`@可复用组件拆分`）
>
> 若不自绘标题栏和边框外观，可参考 `$(BEInstall)\__demoTemplates\BE-SciterUI(原生外观)\__demo\out\index.html`。

#### 以下仅讨论含标题栏在内的完全自绘方案：

标准实现以 `$(BEInstall)\__demoTemplates\BE-Sciter窗口程序(完全自绘)\__demo\out\index.html` 为准，推荐结构为：

```html
<html window-frame="transparent">
<head>...</head>
<body>
    <div id="titlebar" role="window-caption">
        <span>SciterUI 窗口</span>
        <button class="title-btn" role="window-close">✕</button>
    </div>

    <div id="contentwin">
        <!-- 页面内容 -->
    </div>
</body>
</html>
```

职责如下：

- `html`：始终覆盖完整客户区，保持透明，不设置 padding，始终保持完整窗口尺寸（由白易原生Win32窗口提供），作为窗口（包括阴影）完整截图的根节点和 `paintForeground` 动画画布。
- `body`：直接承担窗口表面、背景、边框、边框阴影 `box-shadow`；页面用单值 `margin` 声明外围阴影最大宽度，封装层读取该值用于原生命中测试及窗口状态切换。
- `#titlebar[role="window-caption"]`：自绘标题栏。`window-caption` 让标题栏空白区域具备窗口拖动行为；按钮通过各自的 `role` 执行原生窗口命令。上例只展示关闭按钮，最小化和最大化可分别使用 `role="window-minimize"`、`role="window-maximize"`。原生 `<button>` 默认已由 `behavior: button` 提供点击能力，但同时会进入表单按钮的 Tab 焦点链；标题栏按钮应覆盖为 `behavior: clickable`，它仍可点击，只是不再携带不需要的表单焦点语义。
- `#contentwin`：标题栏之外的窗口内容根节点。

基础样式建议如下：

```css
html {
    background: transparent;
}

body {
    margin: 10px; /* 重要！四边必须使用相同的单值，作为外围阴影及承接拉伸热区宽度 */
    overflow: hidden;
    background: #fff;
    box-shadow: inset 0 0 0 1px rgba(0,0,0,.08),
                0 1px 8px rgba(0,0,0,.18);
}

.title-btn {
    /*
     * 原生 <button> 即使不写本规则也能点击，因为默认行为是 behavior: button。
     * 此处改为 clickable 不是为了恢复点击，而是保留鼠标点击的同时，
     * 去掉标题栏按钮不需要的表单按钮语义，避免它进入 Tab 焦点链。
     */
    behavior: clickable;
}

html[window-active] body {
    box-shadow: inset 0 0 0 1px rgba(0,0,0,.10),
                0 1px 10px rgba(0,0,0,.35);
}
```





---

## 二、移植检查清单

将常规 Web 浏览器页面迁移到 Sciter 时，建议按以下清单逐项检查：

1. 只有元素需要按比例占用剩余空间时，才使用 Sciter 的 `width: *`、`height: *` 或 `1*` 弹性单位，并非所有根元素和容器都必须设置。
2. HTML表单原生控件（如滑块 `hslider`）是否采用了 Sciter 兼容标签与样式。
3. 是否移除了依赖 `pointer-events: none` 的覆盖层。
4. 输入框与下拉框文字是否使用 `content-vertical-align` 进行垂直居中。
5. 是否避开了 `filter: brightness` 等不支持的 CSS3 滤镜。
6. 非 `button` 标签作为按钮时是否注入了 `behavior: button` 或 `behavior: clickable`。
7. 按钮内部文字若用普通方式无法稳定居中，是否按需改用 `flow: horizontal` 及双向居中指令。
8. `<button>` 内部是否移除了作为相对定位参考系的悬浮弹层。
9. 弹性容器下的进度条是否改用 `%%` 单位或 `linear-gradient` 渐变背景。
10. 根元素尺寸与主内容区滚动权责是否明确拆分。
11. 只有标准 Flex 多列或换行排版出现兼容问题时，是否按需改用 `flow: horizontal`、`horizontal-flow` 与 `border-spacing`。
12. 所有弹性收缩项目是否补齐了 `min-width: 0` 防止宽度塌陷。
13. 浮层菜单与下拉框是否使用 `<popup>` 标签配合 `.popup()` API。
14. 复选/单选框是否使用 `appearance: none` 或纯 DOM 绘制，开关使用 `transform` 动画。
15. 高频滑动条拖动是否结合了 DOM 事件与 Sciter `state.capture`。
16. 自定义滚动条是否使用 Sciter `@set` 规范定义 `vertical-scrollbar`。
17. 动画属性是否局限于 `transform`、`opacity`、`background` 等基础可过渡属性。
18. 环形进度与时间线是否使用了原生 SVG `path` 或常规 DOM 结构。
19. 骨架屏扫光动画是否避免对 `linear-gradient` 进行 CSS 动画，改用 JS 逐帧驱动。
20. `insertBefore` 动态插入节点时，参照节点是否确保为直接一级子节点。
21. 核心布局是否避开了 `calc()` / `clamp()` 等现代 Web CSS 依赖。
22. 需要被鼠标划选和复制的日志或长文本区块，是否添加了原生的 `selectable` 属性。
23.  高频触发、带持续重绘（`requestPaint`）的 UI 动画是否添加了**世代计数器**，以避免叠加造成 CPU 常驻泄漏。
24.  原生 `<popup>` 元素是否移至主排版容器外平级定义，防止坐标漂移与点击穿透。
25.  复合布局容器（如侧边栏菜单项）是否误加了 `behavior: button` 导致子节点文本消失，弹性文本项是否补齐了 `min-width: 0`。
26.  按钮内部文字或按钮组若出现对齐问题，是否采用经过验证的 `flow` 与星号弹簧边距方案修正。
27.  操作按钮与内容自适应徽章是否分别设置了 `min-width` 与 `max-content` 宽度语义。
28.  SVG 环形进度是否避免依赖浏览器式 transform 起点旋转，改用固定弧线 path。
29.  `display: none` 容器内的 `position: absolute` 子元素仍参与 Sciter 命中测试，隐藏容器必须用物理裁剪三合一（visibility:hidden + height:0 + overflow:hidden）。
30.  自定义右键菜单是否使用 `Window.this.cursorPos()` 坐标与选区快照恢复机制。
31.  单选/复选框外观是否优先采用纯 DOM 绘制，全页主题过渡是否避免长时间影响重绘。
32.  自定义下拉框是否区分了 `:hover`、`:active` 与 `.open` 弹层已打开状态。
33.  基于 `Graphics.Image` 的窗口快照动画是否同时处理了真实 DPI、显示后布局稳定、阴影遮罩和关闭时重新截图。

---

## 1. 布局系统 (Layout System)

### 需要实测的浏览器布局写法
- **标准 Flexbox** (`display: flex`, `flex-wrap`, `gap`):
  - Sciter 不会强制页面使用 `flow`，普通布局和能够正常工作的 Flexbox 可以直接保留。只是在 Sciter 5.0.3.15 的部分多行或复杂宽度分配场景中，标准 Flexbox 可能出现折行异常、容器宽度塌陷，甚至把行内文本拆成单字符垂直排版；遇到这些实际问题时再替换。
- **百分比宽度** (`width: 100%`):
  - 在动态宽度（如弹性单元 `*`）的父容器下，子容器使用 `width: 100%` 无法正常触发第一轮的尺寸度量（measure pass），会导致输入框、选择框等控件塌陷为默认的极窄宽度。

### 标准 Flexbox 不兼容时的替代写法
- **Sciter 原生弹性布局** (`flow`):
  - `flow` 是标准 Flexbox 在目标场景中实测不兼容时的可选替代，不是 `body`、根元素或普通容器的必需属性，也不应无条件加入所有页面。
  - **水平单行排版**（不换行）：`flow: horizontal;`
  - **水平换行排版**（类似按钮组自动折行）：`flow: horizontal-flow;`
  - **垂直排版**：`flow: vertical;`
- **弹性间距** (`border-spacing`):
  - 代替标准 CSS 的 `gap`。在 `flow` 布局下，使用 `border-spacing: 10px;` 来设置子元素之间的固定间距。
- **弹性尺寸单元** (`*`):
  - Sciter 使用 `*` 代表比例弹性单位（相当于标准 CSS 的 `flex: 1`）。
  - **父容器拉伸**：设置 `width: *;` 让整行在垂直卡片中横向拉伸至 100% 宽度。
  - **子元素拉伸**：设置 `width: 1*;` 让输入框、文本框等控件自动瓜分并填满行内剩余的全部空间。

---

## 2. 表单与HTML表单原生控件 (Form Controls)

### ❌ 不支持 / 兼容差
- **滑块输入框** (`<input type="range">`): 
  - 在 Sciter 中直接渲染 `type="range"` 可能会导致控件不可见，且不支持 CSS3 的 `accent-color` 属性。
- **伪元素选择器** (`::-webkit-slider-thumb` / `::-webkit-slider-runnable-track`): 
  - 无法匹配，无法用于自定义滑块样式。

### 推荐的 Sciter 兼容写法
- **滑块控件类型** (`<input type="hslider">`):
  
  - 编写水平滑动条时，HTML 标签必须写为 `<input type="hslider">`（垂直滑动条为 `vslider`）。
- **滑块轨道样式 (Track)**:
  
  - 直接在 input 的 CSS 类名（如 `.gui-range`）上设置高度、背景色、边框等：
    ```css
    .gui-range {
        width: 160px;
        height: 6px;
        background: rgba(255, 255, 255, 0.12);
        border-radius: 3px;
        border: none;
    }
    ```
- **滑块滑块样式 (Thumb/Knob)**:
  - Sciter 会自动在 `hslider` 下创建原生的 `button` 子节点作为滑块。请使用子选择器直接指向 `button`，不要带任何特殊的伪元素或类名：
    ```css
    /* 适配暗色与亮色 */
    .gui-range > button {
        width: 16px;
        height: 16px;
        border-radius: 8px;
        background: #8b5cf6;
        border: none;
    }
    ```
- **滑块两端溢出防范 (Thumb Overhang)**:
  - `hslider` 在滑动到 0% 和 100% 端点时，是以滑块圆心与轨道端点对齐的。因此当滑块宽度为 16px 时，两端会分别向外突出 8px（半个滑块宽度）。
  - 当滑块处于带边框的卡片容器内且使用 `width: *` 时，父容器需声明 `flow: vertical`，并为滑块设置左右外边距（例如 `margin: 6px 8px 12px 8px;`），确保滑块即使拖动至 100% 也严格处于卡片内部。

---

## 3. 鼠标穿透 (Pointer Events)

### ❌ 不支持 / 兼容差
- **鼠标事件穿透** (`pointer-events: none`): 
  - Sciter 的 CSS 渲染引擎**不支持**此属性。
  - 任何设置了 `position: absolute; width: 100%; z-index: 999;` 的层（哪怕设置了 `pointer-events: none`），都会在物理上完全遮盖下方区域，导致底下的菜单、控制按钮完全失去 hover 点燃和 click 点击响应。

### 推荐的 Sciter 兼容写法
- **紧凑绝对定位**:
  
  - 不要使用占满 100% 宽度的绝对定位层来承载中央标题等。
  - 应当将绝对定位层的宽度设为 `width: auto`（或者不设宽度使其由内容撑开），并通过偏移量进行绝对居中。仅让文字所在的中央窄带存在于高层级中，释放两侧的交互空间：
    ```css
    body.menubar-integrated .titlebar-title {
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
    }
    ```

---

## 4. 文本垂直居中 (Text Alignment)

### ❌ 不支持 / 兼容差
- **标准 Flex 垂直居中** (`align-items: center` / `line-height` 高度拉伸): 
  - 在 Sciter 中对于输入框 (`input`)、下拉框 (`select`) 和多行包裹框，使用常规的 line-height 或 flex 垂直对齐极易导致文本在不同 DPI 或缩放比例下**被基线裁切 (clipping)**。

### 推荐的 Sciter 兼容写法
- **内容垂直对齐** (`content-vertical-align`):
  - 设置显式的高度，并使用 Sciter 特有的内容对齐属性（注意需开启：flow: horizontal）：
    ```css
    .gui-select, .gui-input {
        height: 32px;
        flow: horizontal;
        content-vertical-align: middle; /* 核心：使输入框内文本完美垂直居中 */
    }
    ```

---

## 5. 滤镜与过渡动画 (Filters & Transitions)

### ❌ 不支持 / 兼容差
- **CSS3 图像滤镜** (`filter: brightness(...)` / `filter: opacity(...)`):
  - Sciter 无法渲染。使用它们来进行鼠标悬停点燃（变亮）或压下变暗等交互效果将完全无效。

### 推荐的 Sciter 兼容写法
- **显式色彩/透明度渐变**:
  - 必须老老实实在 `:hover` 和 `:active` 伪类中配置对应的颜色或透明度色值。
  - Sciter 对 `background`、`border-color`、`color`、`opacity` 和基础 `transform` 的过渡支持较稳定，但不能据此认为所有现代 CSS 动画属性都兼容。应只对已经验证的基础属性使用 `transition`：
    ```css
    .gui-btn {
        background: #8b5cf6;
        transition: background 0.15s ease, border-color 0.15s ease;
    }
    .gui-btn:hover {
        background: #a78bfa;
    }
    .gui-btn:active {
        background: #7c3aed;
    }
    ```

---

## 6. 原生控件行为注入 (Behavior Property)

### ❌ 不支持 / 兼容差
- **非标准标签的交互响应**（如 `span` / `div` 作为按钮）：
  - 在标准网页中，我们可以直接通过 JS 的 `click` 监听或者简单的 `:hover` CSS 使 `span` 扮演按钮。但在 Sciter 引擎中，非 `button` 标签默认**不会触发按压交互状态**（例如 `:active` 样式不会稳定生效，或者空间状态机与键盘焦点不吻合）。

### 推荐的 Sciter 兼容写法
- **行为属性注入** (`behavior: button`):
  - 在 CSS 中为作为按钮使用的 `span` 或 `div` 注入 `behavior: button;`。这会强制 Sciter 在 C++ 层为其绑定标准按钮的交互反馈逻辑与键盘焦点支持：
    ```css
    .gui-page-btn {
        behavior: button; /* 核心：让 span 具备标准按钮的所有交互反馈与键盘状态 */
        display: inline-block;
        cursor: default;
    }
    ```

---

## 7. 原生按钮文本垂直居中与裁剪 (Button Text Metrics)

### ❌ 不支持 / 兼容差
- **原生 `<button>` 标签文本的偏移与裁剪**：
  - Sciter 中的原生 `<button>` 标签是一个内置组件，使用系统底层的文字度量。如果直接对其设置固定高度（如 `height: 28px`），它内部的排版常会把文本（特别是数字或特定字体，如微软雅黑）向上偏移，即使强加 `line-height` 或 `vertical-align` 也极难消除偏移，甚至引发文字底边被截断的问题。

### 推荐的 Sciter 兼容写法
- **标签替换 + 弹性居中布局**：
  - 将标签改写为普通的 `<span>`，利用 CSS 注入 `behavior: button;` 维持按钮交互。
  - 在 CSS 中配合 `flow: horizontal` 排版，并使用弹性内容对齐指令：
    ```css
    .gui-page-btn {
        behavior: button;
        flow: horizontal;                 /* 弹性横向排版 */
        content-vertical-align: middle;   /* 内容垂直居中 */
        content-horizontal-align: center; /* 内容水平居中 */
        min-width: 28px;
        height: 28px;
    }
    ```

---

## 8. 原生控件的绝对定位锚点限制 (Absolute Position on Widgets)

### ❌ 不支持 / 兼容差
- **在 `<button>` 等原生标签内使用 `position: absolute;` 悬浮子元素**：
  - 在 Sciter 中，所有原生输入/按钮控件（由底层 C++ behavior 渲染的节点）**不支持作为 `position: relative` 的定位参考系**。
  - 如果在 `<button>` 标签内部放置一个 `position: absolute; bottom: 100%;` 的气泡提示框（Tooltip），该气泡将**无法**以按钮顶部为基准定位，而是会直接飘到外面更高层的容器上，导致位置严重跑偏。

### 推荐的 Sciter 兼容写法
- **触发器重构为基础标签**：
  - 同样使用 `<span>` 作为触发器容器，通过 `behavior: button;` 绑定行为，并声明 `position: relative;`。由于 `<span>` 是标准的排版标签，其绝对定位子元素能够 100% 稳定地以它作为定位参考：
    ```html
    <!-- 按钮本身作为定位基准 -->
    <span class="gui-btn gui-btn-outline" style="position: relative;">
        悬停查看
        <span class="gui-tooltip">这是一个工具提示</span>
    </span>
    ```

---

## 9. 弹性父容器下的进度条比例填充 (Progress/Flex Rendering)

### ❌ 不支持 / 兼容差
- **弹性父容器下的百分比宽度子元素** (`width: 65%`):
  - 如果进度条容器 `.gui-progress` 设置了弹性宽度（如 `width: 1*;` 或 `flex: 1`），在 Sciter 下，其子级进度填充条 `.gui-progress-bar` 如果直接设置 `width: 65%`，会因为在第一轮度量中父容器尚未解析出确切像素宽度，导致百分比直接被算为 **`0`**（进度条完全空旷不显示）。

### 推荐的 Sciter 兼容写法
- **方案 A：使用 Sciter 特有的双百分比单位 (`%%`)**：
  - 在 Sciter 中，`%%` 代表“父容器内部已解析宽度的百分比”：
    ```html
    <div class="gui-progress-bar" style="width: 65%%;"></div>
    ```
- **方案 B：使用渐变背景直接渲染进度（最推荐）**：
  - 模仿滑动条轨道的设计，直接丢弃内层子元素，直接在父级 `.gui-progress` 容器上使用 **`linear-gradient`** 划定颜色临界点，既省去 DOM 节点又能完美抵抗任何缩放渲染问题：
    ```css
    .gui-progress {
        width: 1*;
        height: 8px;
        border-radius: 4px;
        background: linear-gradient(to right, #8b5cf6 0%, #8b5cf6 65%, rgba(255, 255, 255, 0.12) 65%, rgba(255, 255, 255, 0.12) 100%);
    }
    ```

---

## 10. 根元素尺寸与滚动归属

### ❌ 不支持 / 兼容差

- **照搬网页根布局**（`100vw`、`100vh`、`body` 自动增长）：
  - Sciter 窗口不是浏览器标签页。根元素如果没有明确参与窗口尺寸分配，容易出现内容高度计算错误、根页面滚动条、横向滚动条或底部溢出。
- **让多个祖先同时滚动**：
  - `html`、`body` 和内容区都使用 `overflow: auto` 时，窗口缩小时容易出现嵌套滚动、滚轮传递异常和多条滚动条。

### 推荐的 Sciter 兼容写法

- `html` 与 `body` 默认已由原生宿主窗口撑满，**无需在 `html` 和 `body` 上额外声明 `width: *; height: *;`**。只需保持 `body { overflow: hidden; }` 避免外层窗口出现全局滚动条，如需滚动仅为特定主内容区配置滚动：
  ```css
  html {
      background: transparent;
  }

  body {
      overflow: hidden;
  }

  .page-content {
      width: *;
      height: *;
      overflow-x: hidden;
      overflow-y: scroll;
  }
  ```
- 固定标题栏、工具栏和状态栏使用明确高度；中间内容区按需使用 `height: *` 填充剩余空间。
- 需要独立滚动的文本视图必须设置明确高度，不能只依赖内容自动撑开：
  ```css
  .text-view {
      width: *;
      height: 230px;
      overflow-x: hidden;
      overflow-y: auto;
  }
  ```

---

## 11. 双栏、卡片和响应式排列

### ❌ 不支持 / 兼容差

- **CSS Grid**（`display: grid`、`grid-template-columns`）：
  - 不应作为 Sciter 5 页面主布局。复杂 Grid 通常需要完全重写。
- **依赖 `flex-basis`、`gap` 和浏览器自动最小内容宽度**：
  - 卡片可能被长文本或控件固有宽度撑大，造成左右栏宽度不一致或整体横向溢出。

### 推荐的 Sciter 兼容写法

- 若标准 Flex/Grid 在该多列场景中实测异常，可让行容器改用 `flow: horizontal`，并让同一行卡片通过 `width: 1*` 平分空间：
  ```css
  .card-row {
      width: *;
      flow: horizontal;
      border-spacing: 14px;
  }

  .card {
      width: 1*;
      min-width: 0;
      flow: vertical;
  }
  ```
- 需要换行的按钮组使用 `flow: horizontal-flow`，不要依赖 `flex-wrap`：
  ```css
  .button-group {
      flow: horizontal-flow;
      border-spacing: 8px;
  }
  ```
- Sciter 不会自动完成所有浏览器式响应式重排。稳定方案是由 HTML 明确分组每一行，必要时通过脚本或窗口尺寸状态切换布局类名。

---

## 12. `min-width`、固有尺寸与文本截断

### ❌ 不支持 / 兼容差

- **只写 `width: 1*` 就认为元素一定可以收缩**：
  - 输入框、卡片、表格单元格和包含长文本的容器仍可能受固有最小宽度影响，把父容器撑出可视范围。
- **让标签文字自动折行**：
  - 窄窗口下标签可能逐字换行，导致一行控件高度异常。

### 推荐的 Sciter 兼容写法

- 所有需要在弹性行中收缩的项目都补上 `min-width: 0`：
  ```css
  .card,
  .input,
  .select,
  .textarea {
      width: 1*;
      min-width: 0;
  }
  ```
- 左侧标签使用固定宽度和 `white-space: nowrap`；右侧内容使用弹性宽度：
  ```css
  .field-label {
      width: 54px;
      min-width: 54px;
      white-space: nowrap;
  }

  .field-control {
      width: 1*;
      min-width: 0;
  }
  ```
- 单行文本省略必须同时具备宽度约束、隐藏溢出和不换行：
  ```css
  .caption {
      width: *;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
  }
  ```

---

## 13. 弹出菜单和自定义下拉框

### ❌ 不支持 / 兼容差

- **用普通绝对定位 `div` 模拟浏览器浮层**：
  - 浮层容易被祖先的 `overflow: hidden` 裁剪；`z-index` 再高也不能保证逃离祖先的裁剪上下文。
  - 手工通过 `left/top` 定位还容易在窗口边缘、滚动后或 DPI 变化时跑偏。
- **直接依赖原生 `<select>` 的浏览器样式能力**：
  - 下拉箭头、选项高度、圆角、主题和弹出层样式受 Sciter 原生 behavior 限制，很难做到与设计稿一致。

### 推荐的 Sciter 兼容写法

- 菜单和下拉选项使用 Sciter 的 `<popup>` 元素，并把它放在触发元素内部：
  ```html
  <div class="gui-select" tabindex="0">
      <div class="caption">选项一</div>
      <popup class="gui-select-popup">
          <div class="option-item">选项一</div>
          <div class="option-item">选项二</div>
      </popup>
  </div>
  ```
- 使用 Sciter 的锚点弹出 API，不手工计算屏幕坐标：
  ```js
  selectElement.popup(popupElement, "bottom-left");
  ```
- 下拉宽度需要与触发器一致时，在打开前读取已完成布局的 `offsetWidth`，再设置明确像素宽度：
  ```js
  popupElement.style.set({
      width: selectElement.offsetWidth + "px"
  });
  selectElement.popup(popupElement, "bottom-left");
  ```
- 使用 `popupdismissed` 统一清理菜单高亮、打开状态和临时类名。不要同时维护一套延迟隐藏动画，否则容易造成菜单卡顿或位置闪跳。

---

## 14. 复选框、单选框和开关

### ❌ 不支持 / 兼容差

- **浏览器厂商伪元素和 `accent-color`**：
  - WebKit/Chromium 专用选择器在 Sciter 中不能使用，原生勾选标记也不保证能被完整重绘。
- **把装饰层盖在输入框上再使用 `pointer-events: none`**：
  - Sciter 不支持依靠 `pointer-events` 穿透，装饰层可能直接吃掉点击。

### 推荐的 Sciter 兼容写法

- 复选框和单选框使用 `appearance: none`，选中图形通过 `background-image` 绘制，不依赖伪元素：
  ```css
  input[type="radio"] {
      appearance: none;
      width: 16px;
      height: 16px;
      border-radius: 8px;
  }

  input[type="radio"]:checked {
      border-color: #8b5cf6;
      background-image: url("data:image/svg+xml;base64,...");
      background-position: center;
      background-repeat: no-repeat;
  }
  ```
- 开关使用透明原生输入框覆盖整个点击区，轨道作为其相邻兄弟，滑块只对 `transform` 做过渡：
  ```css
  .switch { position: relative; width: 42px; height: 24px; }
  .switch input {
      position: absolute;
      left: 0;
      top: 0;
      width: 42px;
      height: 24px;
      opacity: 0;
      z-index: 2;
  }
  .switch-knob {
      transform: translate(0, 0);
      transition: transform 0.18s ease;
  }
  .switch input:checked + .switch-track .switch-knob {
      transform: translate(18px, 0);
  }
  ```
- 不要通过改变 `left` 来制作开关动画；基础 `transform` 在 Sciter 中更稳定，也不会反复触发布局。

---

## 15. 滑块的连续拖动

### ❌ 不支持 / 兼容差

- `hslider` 适合常规值选择，但如果拖动时还要持续刷新复杂界面、跨语言调用或重绘透明窗口，可能出现拖动不连续。
- 在 `mousemove` 中执行耗时逻辑，会直接降低鼠标采样和绘制流畅度。

### 推荐的 Sciter 兼容写法

- 对连续性要求高时，使用普通 DOM 节点构建轨道、填充条和滑块，在按下后使用 Sciter 鼠标捕获：
  ```js
  slider.addEventListener("mousedown", function(e) {
      dragging = true;
      slider.state.capture(true);
      e.preventDefault();
  });

  slider.addEventListener("mousemove", function(e) {
      if (dragging) updateSlider(e.clientX);
  });

  slider.addEventListener("mouseup", function() {
      dragging = false;
      slider.state.capture(false);
  });
  ```
- 拖动中只更新必要的 `width`、`left` 和文字；耗时操作在松开后提交。
- 填充条和滑块必须位于同一个明确尺寸的相对定位容器内，避免每次移动重新测量整张卡片。

---

## 16. 自定义滚动条

### ❌ 不支持 / 兼容差

- **WebKit 滚动条伪元素**（`::-webkit-scrollbar` 等）：
  - Sciter 不使用 Chromium 的滚动条 DOM，相关样式不会生效。
- **只给父级写 `overflow`，不指定滚动视图尺寸**：
  - 如果父级仍由内容撑高，滚动条不会出现。

### 推荐的 Sciter 兼容写法

- 使用 Sciter 的 `@set` 定义滚动条部件，再通过 `vertical-scrollbar` 应用：
  ```css
  @set app-scrollbar {
      .base {
          width: 8dip;
          background: transparent;
      }
      .slider {
          margin: 2dip;
          border-radius: 4dip;
          background: rgba(0,0,0,0.14);
      }
      .slider:hover {
          background: rgba(0,0,0,0.25);
      }
  }

  .page-content {
      height: *;
      overflow-y: scroll;
      vertical-scrollbar: app-scrollbar;
  }
  ```
- 暗色和亮色主题分别定义 scrollbar set，避免依赖 CSS 滤镜反色。
- 页面只保留一个主滚动视图；局部日志、文本视图需要滚动时必须有明确高度。

---

## 17. 动画的可靠范围

### ❌ 不支持 / 兼容差

- **把浏览器中可动画的任意属性直接照搬到 Sciter**：
  - Sciter 5 并不等同于 Chromium。复杂滤镜、遮罩、渐变参数和部分 SVG/CSS 组合动画可能不运行或触发高开销重绘。
- **在动画中反复改变会参与布局的尺寸和位置**：
  - 频繁改变 `left`、`top`、`width`、`height` 容易造成卡顿，透明窗口下尤其明显。

### 推荐的 Sciter 兼容写法

- 优先动画 `transform`、`opacity`、`background`、`color` 和 `border-color`。
- 点状加载使用真实子元素分别设置 `animation-delay`，不要依赖复杂伪元素链：
  ```css
  .dots { flow: horizontal; border-spacing: 5px; }
  .dots span {
      width: 7px;
      height: 7px;
      border-radius: 4px;
      animation: dot-pulse 1.4s ease-in-out infinite;
  }
  .dots span:nth-child(2) { animation-delay: 0.2s; }
  .dots span:nth-child(3) { animation-delay: 0.4s; }

  @keyframes dot-pulse {
      0%, 80%, 100% { transform: scale(0.6); opacity: 0.4; }
      40% { transform: scale(1); opacity: 1; }
  }
  ```
- 页面状态变化优先切换类名，让 CSS 负责基础过渡；不要在高频鼠标事件中重建大量 DOM。

---

## 18. 环形进度、时间线等装饰图形

### ❌ 不支持 / 兼容差

- **依赖 `conic-gradient`、`mask`、复杂 SVG CSS 或浏览器绘制扩展**：
  - 在 Sciter 5 中容易完全不显示，或只显示背景轨道。
- **在原生控件上挂复杂伪元素**：
  - 原生 behavior 的内部绘制与普通 DOM 不同，伪元素尺寸和定位不可靠。

### 推荐的 Sciter 兼容写法

- 环形进度使用明确尺寸的 SVG `path`，把 `stroke-dasharray` 直接写在 SVG 属性上；文字使用 `flow: stack` 叠放：
  ```html
  <div class="progress-ring">
      <svg width="44" height="44">
          <path d="M 22 4 A 18 18 0 1 1 21.99 4"
                fill="none" stroke-width="3.5"
                stroke-dasharray="79 34" />
      </svg>
      <span>70%</span>
  </div>
  ```
  ```css
  .progress-ring {
      width: 44px;
      height: 44px;
      flow: stack;
      content-horizontal-align: center;
      content-vertical-align: middle;
  }
  .progress-ring span { margin: *; }
  ```
- 时间线使用普通容器的 `border-left` 绘制主线，节点使用普通项目的 `::before`，并给项目设置 `position: relative` 和明确偏移：

---

## 19. 骨架屏扫光与渐变动画 (Skeleton Sweep / Gradient Animation)

### ❌ 不支持 / 兼容差

- **对 `linear-gradient` 进行 `background-position` / `background-size` 的 CSS 动画**：
  - 在 Sciter 5.0.3.15 引擎中，`linear-gradient` 被作为填充背景色（`background-color` 变体）处理，而不是可平铺/可定位的 `background-image`。因此在 `@keyframes` 中试图修改 `background-position` 不会产生移动效果（动画呈完全静态卡死状态）。
  - 无论渐变方向使用 `90deg`、`to right` 还是 Sciter 原生的 `left` 关键词，结果均相同。

- **`::before` 伪元素 + `transform: translate(...)` 扫光**：
  - 虽然 Sciter 支持 `::before` 伪元素和 `transform: translate()` 静态属性，但在 `@keyframes` 动画中对伪元素施加 `transform: translate()` 动画**不会生效**（伪元素保持静止不动，完全无动画效果）。

- **`opacity` 呼吸动画**：
  - `@keyframes` 中对 `opacity` 做呼吸脉冲（0.4 ↔ 1）在 Sciter 中可以生效，但只能产生整体明暗闪烁，无法实现水平方向的扫光/滑过效果。

### 推荐的 Sciter 兼容写法

- **使用 JavaScript `requestAnimationFrame` 逐帧更新 `element.style.background` 渐变**：
  - CSS 仅保留简单静态背景色；
  - JS 每帧计算高亮中心位置，拼接新的 `linear-gradient(to right, ...)` 字符串，直接赋值给元素的 `style.background`；
  - 这与 Sciter 中 JS 驱动进度条动画（`style.width`）是同一机制，已验证可靠。

  ```css
  /* CSS：仅静态背景 */
  .gui-skeleton {
      width: *;
      height: 10px;
      border-radius: 5px;
      background: rgba(255,255,255,0.10);
  }
  body.theme-light .gui-skeleton { background: rgba(0,0,0,0.08); }
  ```

  ```javascript
  /* JS：requestAnimationFrame 驱动扫光 */
  function updateSkeletonShimmer() {
      if (!globalAnimEnabled) return;
      var t = (Date.now() % 800) / 800;
      var center = t * 140 - 20;
      var half = 40;
      var s1 = Math.max(0, Math.min(100, center - half));
      var s2 = Math.max(0, Math.min(100, center));
      var s3 = Math.max(0, Math.min(100, center + half));

      var isLight = document.body.classList.contains("theme-light");
      var base = isLight ? "rgba(0,0,0,0.06)" : "rgba(255,255,255,0.15)";
      var hi   = isLight ? "rgba(0,0,0,0.18)" : "rgba(255,255,255,0.30)";

      var grad = "linear-gradient(to right, " +
          base + " " + s1 + "%, " + hi + " " + s2 + "%, " + base + " " + s3 + "%)";

      var els = document.getElementsByClassName("gui-skeleton");
      for (var i = 0; i < els.length; i++) {
          els[i].style.background = grad;
      }
      requestAnimationFrame(updateSkeletonShimmer);
  }
  requestAnimationFrame(up        margin: 12px * 0 *; /* 核心：左右 margin 设为 *，弹簧边距使按钮组在卡片内水平居中 */
    }
    ```low` 容器内的文本标签单独添加顶边距补偿（`margin-top: 4px;`），使文本中轴与后方带边框/内边距的高尺寸组件完美居中对齐：
  ​```css
  .demo-row.wrap-row .demo-label {
      margin-top: 4px;
  }
  ```

---

## 20. DOM `insertBefore` 的直接父子节点层级校验

### ❌ 不支持 / 兼容差
- **跨层级子节点作为 `insertBefore` 的参照节点**：
  - 在标准浏览器中，调用 `parent.insertBefore(newChild, target)` 时如果 `target` 不是 `parent` 的直接子节点，浏览器可能会报错或静默处理。
  - 在 Sciter DOM 引擎中，若 `insertBefore` 的第二个参数参照节点不是主容器的**直接一级子节点**（例如传入了 `.titlebar-left` 内部深层的 `.titlebar-title`），Sciter 会自动退化为 `appendChild(newChild)`，将元素直接追加到父容器的最后一个节点位置（例如将菜单栏错误挂到右侧窗口控制按钮区之后，造成按钮位置颠倒）。

### 推荐的 Sciter 兼容写法
- 动态移动/插入 DOM 节点时，务必使用主容器的**直接一级子节点**作为 `insertBefore` 的参照物：
  ```javascript
  var target = titlebar.querySelector('.titlebar-drag-region') || titlebar.querySelector('.titlebar-controls');
  if (target) titlebar.insertBefore(menubar, target);
  ```

---

## 21. 现代 CSS 语法的使用原则

### ❌ 不支持 / 兼容差

- **默认认为浏览器新语法都可用**：
  - CSS 自定义属性、复杂 `calc()`、`clamp()`、`min()`/`max()` 函数、Grid、浏览器私有伪元素、滤镜和遮罩都不应直接作为 Sciter 5 核心布局依赖。
- **用一套高度抽象的主题变量覆盖所有组件**：
  - 某些现代变量和嵌套计算即使能解析，也可能在 Sciter 版本、控件 behavior 或动态状态下表现不一致。

### 推荐的 Sciter 兼容写法

- 当标准 CSS 在目标 Sciter 版本中实测不兼容时，可按具体需求选用下列已验证的 Sciter 属性；不要求每个页面或容器都使用：
  - `flow`（仅在需要替代标准 Flexbox 时）
  - `width: *` / `width: 1*`
  - `height: *`
  - `border-spacing`
  - `content-vertical-align`
  - `content-horizontal-align`
  - `behavior`
  - `@set` 滚动条
  - `<popup>` 与元素 `popup()` API
- 主题颜色使用明确的类选择器覆盖：
  ```css
  .panel { background: #111522; color: #e8ecf1; }
  body.theme-light .panel { background: #fff; color: #111827; }
  ```
- 新属性先做最小独立样例验证，再放进复杂页面。若某视觉效果不是核心功能，应准备由边框、背景色、普通 SVG 或显式 DOM 结构实现的降级方案。

---


## 22. 文本节点可选中 (Selectable Text)

### ❌ 不支持 / 兼容差
- **在普通 `div` 或 `span` 上仅使用 CSS `user-select: text` 尝试使其可选中**：
  - 在现代浏览器中，通过 CSS 即可让任意区块的文本变为可通过鼠标框选或复制的状态。
  - 在 Sciter 中，常规文本默认是不可选中的。单纯使用 CSS 有时不足以实现稳定的文本高亮选取与复制，甚至在非表单元素上使用特定 behavior 也很难完美响应用户的选中需求。

### 推荐的 Sciter 兼容写法
- **直接使用 `selectable` 属性**：
  - Sciter 提供了一个特有的原生 HTML 属性 `selectable`。如果你需要一个日志输出框、代码块或普通长文本允许用户用鼠标选中并使用 `Ctrl+C` 复制，最简单且 100% 可靠的方式是直接在标签上增加该属性。
    ```html
    <!-- 推荐：带有 selectable 属性，内部文本自动支持划选和右键/快捷键复制 -->
    <div class="ipc-log" id="ipcLog" selectable>
        【系统】这是一条可以被用户选中的日志文本...
    </div>
    ```

---

## 23. 频繁触发的动画与 CPU 飙升 (Concurrent Animations & CPU Leaks)

### ❌ 不支持 / 兼容差
- **在 `requestPaint` 动画未执行完毕时直接被打断丢弃（产生悬空重绘死循环）**：
  - 当你在 JS 中使用 `element.animate` 并在回调中调用 `element.requestPaint()` 时，如果用户高频点击（如狂点“切换主题”），会导致旧的动画 Promise 没走完，新动画就叠加了上来。
  - 在 Sciter 的特定渲染层（如 WARP 软渲染模式），如果旧的渲染回调没有被显式释放且 `animate` 没有返回 `false` 中断，系统会在后台同时运行多个死循环动画帧。这会导致界面的持续重绘，从而让 **CPU 占用率飙升且居高不下（即 CPU 泄漏）**。

### 推荐的 Sciter 兼容写法
- **引入“动画世代计数器 (Generation Counter)”并妥善清理（如果是全局一次性启动动画且后续无重复使用的那种则可不必引入）**：
  
  - 对于可被打断的全局持续动画，必须引入一个计数器来标识当前的最新一次动画。
  - 在动画的每一帧中，首先校验计数器是否匹配。如果不匹配，说明此动画已经过时，必须立刻返回 `false` 中断，并停止一切关联的 `requestPaint`。
    ```javascript
    var animGen = 0; 
    async function playInterruptibleAnimation() {
        var myGen = ++animGen; // 每次调用，世代 +1
        var rootEl = document.documentElement;
        
        rootEl.paintForeground = function(graphics) {
            if (myGen !== animGen) return; // 拦截旧的 paint 回调
            // 绘制逻辑...
      };
  
        var stepFn = function (progress) {
            if (myGen !== animGen) return false; // 世代不匹配，返回 false 强制终止旧动画！
            rootEl.requestPaint();
            return true;
      };
  
        try {
            await rootEl.animate(stepFn, { duration: 300 });
        } finally {
            if (myGen === animGen) {
                // 只有最新世代的调用，才有资格清理残局，避免把正在执行的新动画给清空了
                rootEl.paintForeground = null;
            }
        }
    }
    ```

---

## 24. 原生弹出菜单的坐标漂移与点击穿透 (Popup Positioning & Hit-Testing Bugs)

### ❌ 不支持 / 兼容差
- **在 `<button>` 等排版容器内部嵌套定义 `<popup>` 并使用 `.popup()`**：
  - 虽然 Sciter 支持通过 JS 的 `element.popup(popupEl)` 来将任意 DOM 元素提升为原生弹出窗口，但如果你将 `<popup>` 元素作为子节点**嵌套**在 `flex` 或 `flow` 容器（如菜单栏的 `<button>`）内部，当频繁、极速地在不同菜单之间滑动切换时，引擎的层回收动画和父级排版流的计算极易发生冲突。此时，原本用于锚定位置的按钮坐标在引擎内部可能被误判为 `0,0`，导致弹窗诡异地飞到窗口的最左上角（或根容器起点）。
- **使用 `setTimeout(fn, 1)` 在 `popupdismissed` 后立刻弹新菜单**：
  - Sciter 中销毁一个原生弹出窗口（Layered Window）是一个需要跨越渲染线程周期的底层操作。如果旧菜单刚触发关闭，JS 立刻通过 1 毫秒的 `setTimeout` 强制弹出一个新菜单，系统可能来不及刷新布局树并重置弹窗上下文矩阵。
- **排版间隙与透明背景引发的点击穿透 (Hit-Test Leak)**：
  - 如果在 `flow` 布局中使用了 `border-spacing` 产生间隙，或者你的菜单项 `.ctx-item` 没有任何背景色，鼠标的点击会直接穿透这些透明区域（Ghost Click），点到菜单下方的底层 `document` 上。底层的全局点击事件响应了关闭菜单的逻辑，使得菜单瞬间消失，引发了“菜单点击直接失效”的假象。

### 推荐的 Sciter 兼容写法
必须采用以下 **“三管齐下”** 的组合防御手段，才能实现如丝般顺滑且坐标绝对准确的菜单栏：

1. **DOM 彻底解绑（移出排版容器）**：
   - 所有的 `<popup>` 菜单元素都必须写在 HTML 结构的末尾（比如 `<nav>` 外侧或闭合标签前），与它们所要依附的 `<button>` 成为平级兄弟，而绝对不能嵌套在按钮“肚子”里。这样它们完全游离于排版流之外，坐标计算才不受干扰。
   ```html
   <nav class="menubar">
       <button data-menu="file">文件</button>
       <!-- 将 popup 从 button 内移出 -->
       <popup id="fileMenu">...</popup>
   </nav>
   ```

2. **使用渲染帧锁步切换 (`requestAnimationFrame`)**：
   - 在处理前一个菜单的 `popupdismissed`（销毁通知）并准备挂载下一个菜单时，废弃任何人为的 `setTimeout`，严格等待下一个渲染帧。这确保引擎在底层完成了上一个弹窗的物理销毁，再执行下一次锚定计算：
   ```javascript
   function onMenuBarPopupDismissed() {
       if (pendingMenuBarOpen) {
           var next = pendingMenuBarOpen;
           pendingMenuBarOpen = null;
           requestAnimationFrame(function () {
               showMenuBarDropdown(next.key, next.anchor);
           });
       }
   }
   ```

3. **弹出前强制刷新布局矩阵 (Force Layout Flush)**：
   - 在调用原生的 `.popup()` 前，使用 `getBoundingClientRect()` 主动刺探一下触发元素。这行代码虽然看似废话，但在 Sciter 底层会立刻强制触发布局矩阵的更新，确保下一秒引擎提取到的绝对坐标绝对精准：
   ```javascript
   anchorItem.getBoundingClientRect(); // 核心：强制刷新目标锚点元素的物理坐标
   anchorItem.popup(menu, "bottom-left");
   ```

4. **显式拦截命中测试并赋予按钮生命周期**：
   - 保持使用标准的 `click` 事件处理命令。在 CSS 中，为菜单项强制附加 `background: transparent;`（让透明区域也能阻挡并捕获鼠标点击，防止穿透）。
   - 同时附加 `behavior: clickable;`，让普通 `div` 拥有完整的按钮点击生命周期与事件触发保证：
   ```css
   .ctx-item {
       behavior: clickable;      /* 强制赋予点击生命周期 */
       background: transparent;  /* 填补 hit-test 空洞，防止点击穿透到底层 */
   }
   ```

---

## 25. 复合容器中的 `behavior: button` 与 `width: 1*` 文本截断陷阱

### ❌ 不支持 / 兼容差

- **在包含多子节点（如文本 + 徽章）的布局容器上挂载 `behavior: button`**：
  - 在 Sciter 中，给包含多个 `span` 的排版容器（如侧边栏菜单项 `div.nav-item`）挂载 `behavior: button` 时，Sciter 底层的 C++ 按钮行为引擎会强制接管并覆盖其子节点的文本渲染逻辑，导致子节点文本（如菜单标题）不显示。
- **`flow: horizontal` 弹性布局下 `width: 1*` + `white-space: nowrap` 导致文本宽度算为 0px**：
  - 在横向排版（`flow: horizontal`）中，若给子级文本节点设置了 `width: 1*;`（占满剩余宽度）并配合 `white-space: nowrap;` 阻止换行，如果不显式指定 `min-width: 0;`，Sciter 的第一轮尺寸度量（measure pass）会将 `width: 1*` 的宽度误计算为 **`0px`**，表现为菜单标题文字彻底消失，仅剩右侧固定宽度的图标或 Badge 标签。

### 推荐的 Sciter 兼容写法

- **避开容器级的 `behavior: button`**：
  - 侧边栏菜单或复合列表项保持普通的 DOM 容器属性，通过 CSS 的 `cursor: pointer` 或 `behavior: clickable;` 来提供点击反馈，不要直接在多子节点的父级 `div` 上使用 `behavior: button`：
    ```css
    .nav-item {
        width: *;
        height: 38px;
        flow: horizontal;
        border-spacing: 6px;
        content-vertical-align: middle;
        cursor: pointer; /* 推荐：替代父级的 behavior: button */
    }
    ```
- **弹性文本节点补齐 `min-width: 0` 和颜色继承**：
  - 为所有在 `flow: horizontal` 下需要分配剩余宽度的文本 `span` 统一加上 `min-width: 0;` 和 `color: inherit;`，防止宽度被解析为 `0`：
    ```css
    .nav-title {
        width: 1*;
        min-width: 0;     /* 核心：防宽度计算为 0px */
        color: inherit;   /* 确保在 :hover 或 .active 选中高亮时正确继承父级文本色彩 */
    }
    ```

---

## 26. 按钮内部文字双向居中与按钮组容器水平居中

### ❌ 不支持 / 兼容差

- **按钮内部文字偏下/偏左（只依赖 `line-height` 或 `text-align: center`）**：
  - 在 Sciter 中，给 `<button>` 或 `.btn` 元素单纯设置 `height` 与 `line-height` 试图垂直居中文本时，字体基线容易发生向上或向下偏移，甚至边缘文字被裁切。
- **按钮组在父卡片中靠左倾斜（依赖 Web 的 `justify-content: center` 或 `text-align: center`）**：
  - 在 `flow: vertical` 的父容器卡片中，包裹多个按钮的 `.btn-group` 使用 Web 浏览器的 `justify-content: center` 无法触发水平居中，导致按钮整体偏左。

### 推荐的 Sciter 兼容写法

- **按钮内部文字“水平+垂直”双向居中**：
  - 当普通按钮对齐方式在目标版本中表现异常而选择 `flow` 方案时，同时声明 `flow: horizontal;` 并配套使用 Sciter 的内容对齐指令：
    ```css
    button, .btn {
        behavior: button;
        flow: horizontal;                 /* 核心：声明横向弹性流 */
        content-vertical-align: middle;   /* 核心：文字垂直居中 */
        content-horizontal-align: center; /* 核心：文字水平居中 */
        height: 32px;
        padding: 0 14px;
    }
    ```
- **按钮组容器在父级卡片中的水平居中**：
  - 在包裹按钮组的 `.btn-group` 上声明 `flow: horizontal;`，并利用 Sciter 独特的**星号 `*` 弹簧边距**机制（`margin-left: *; margin-right: *;`）实现容器绝对水平居中：
    ```css
    .btn-group {
        flow: horizontal;
        border-spacing: 8px;
        margin: 12px * 0 *; /* 核心：左右 margin 设为 *，弹簧边距使按钮组在卡片内水平居中 */
    }
    ```

---

## 27. 弹性按钮与内容自适应徽章要分别处理宽度

### ❌ 不支持 / 兼容差

- 在 `flow: horizontal-flow`、`width: 1*` 或空间紧张的卡片中，让操作按钮完全依赖文字固有宽度。
- 反过来，给徽章、状态标签统一设置较大的固定 `min-width`，会让短文本也被拉成整条色块，失去标签随内容收缩的语义。

### 推荐的 Sciter 兼容写法

- 操作按钮设置可控的最小宽度与双向内容居中；同组按钮应当是彼此独立的一级兄弟节点，并确保组宽等于各项宽度之和，不超过卡片内容区：
  ```css
  .action-btn {
      behavior: button;
      min-width: 76px;
      white-space: nowrap;
      flow: horizontal;
      content-horizontal-align: center;
      content-vertical-align: middle;
  }
  .button-group { width: 240px; flow: horizontal; border-spacing: 0; }
  .button-group > .action-btn { width: 80px; min-width: 80px; }
  ```
- 徽章和状态标签使用内容宽度，并取消固定最小宽度；`white-space: nowrap` 防止中文被拆行：
  ```css
  .badge {
      display: block;
      width: max-content;
      min-width: 0;
      white-space: nowrap;
      padding: 2px 8px;
  }
  ```

---

## 28. SVG 环形进度不要依赖浏览器式 transform 起点旋转

### ❌ 不支持 / 兼容差

- 在 `<circle>` 上使用 `transform="rotate(-90 24 24)"` 配合 `stroke-dasharray` 调整进度起点。
- 部分 Sciter 版本不会按浏览器 SVG 规则应用该旋转，结果是进度缺口和起点方向错位。

### 推荐的 Sciter 兼容写法

- 对固定比例的环形进度直接使用从 12 点方向开始的 SVG `path`；动态比例则由 JS 计算弧线路径，不依赖 SVG transform：
  ```html
  <path d="M24 6 A18 18 0 1 1 6 24" fill="none" stroke="#0078d7" stroke-width="4" />
  ```

---

## 29. `display: none` 容器内的 `position: absolute` 子元素仍参与 Sciter 命中测试 (Hit-Test)

### ❌ 不支持 / 陷阱写法

- **侧滑抽屉场景**：为追求 CSS `transition: right` 侧滑过渡动画，将未激活的抽屉设为 `display: block; visibility: hidden; right: -322px;` 常驻渲染在窗口外部。
- **多页面切换场景**：使用 `display: none` 隐藏非活跃 Tab / Section 容器，但容器内部含有 `position: absolute`（如自定义滑块滑块圆点 `.custom-slider-thumb`）或 `position: relative` + `overflow: hidden` 的子元素。
- **Sciter 命中测试陷阱**：在 Sciter UI 中，即使父容器设为 `display: none`，其内部的 `position: absolute` 子节点**在某些渲染路径中依然保留了物理包围盒参与命中测试（Hit-Test / 鼠标悬停仲裁树）**。同样，`visibility: hidden` 的绝对定位节点也会保留命中测试包围盒。这会导致主界面上出现隐形阻挡图层，吃掉下方控件的 Hover、Click 等交互事件。

### 推荐的 Sciter 兼容写法

- **侧滑抽屉 / Flyout 平滑双向动画方案**：基准位置设为 `left: 100%`（完全置于窗口外），结合 `box-sizing: border-box` 与 `transform: translate(-100%, 0)`，实现与窗口右边缘 100% 精确对齐的双向平滑侧滑：
  ```css
  .flyout {
      box-sizing: border-box;
      display: block;
      position: absolute;
      left: 100%;                  /* 基础基准 100% 置于窗口屏幕右侧之外 */
      top: 34px;
      bottom: 0;
      width: 320px;
      height: *;
      visibility: hidden;
      transform: translate(0, 0);
      transition: transform 0.24s ease, visibility 0.24s ease;
  }
  .flyout.active {
      visibility: visible;
      transform: translate(-100%, 0);  /* 展开：按自身 100% 宽度向左滑入，与窗口右边缘完美对齐 */
  }
  ```
- **多页面 / Tab 切换场景（核心方案）**：`display: none` 和 `pointer-events: none` 均**无法**阻止 Sciter 对内部 `position: absolute` 子元素的命中测试。**Sciter 不支持 `pointer-events` CSS 属性。** 正确方案是使用 **`visibility: hidden` + `height: 0` + `overflow: hidden`** 物理裁剪三合一，将隐藏容器的裁剪矩形压缩为零，彻底剪切掉绝对定位子元素的 Hit-Test 包围盒：
  ```js
  // 隐藏 — 物理裁剪三合一
  sec.style.visibility = "hidden";
  sec.style.height = "0px";
  sec.style.overflow = "hidden";
  // 显示 — 恢复
  sec.style.visibility = "visible";
  sec.style.height = undefined;   // 清除内联样式，恢复 CSS 默认值
  sec.style.overflow = undefined;
  ```
  HTML 初始化时，除了默认激活显示的第一个 View 容器外，所有默认隐藏的 Section 容器必须在 HTML 上显式标注物理裁剪三合一（对应 `index.html` 的实测规范）：
  ```html
  <!-- 默认激活显示的容器 -->
  <div class="section-container" id="secOverview">
  <!-- 所有默认隐藏的容器 -->
  <div class="section-container" id="secButtons" style="visibility:hidden;height:0;overflow:hidden;">
  ```
- **全屏 Modal 遮罩对话框场景（补充规则）**：对于无滑动动画需求、常驻置于根节点的全屏遮罩 Modal 对话框（如 `.metro-dialog-overlay`），可以直接使用 `display: none`（隐藏）与 `display: block`（激活显示）控制。遮罩层覆盖全屏时不需要进行 `height: 0` 物理裁剪。
  
  > **为什么不用 `display: none` 做 Tab 切换？** 因为 Sciter 在 `display: none` 的父容器中，其 `position: absolute` 子元素的 Hit-Test 包围盒仍然会残留在鼠标仲裁树中。而 `height: 0` + `overflow: hidden` 会将父容器的物理裁剪矩形压为零，Sciter 引擎无法在这个零高度区域内为任何子元素生成命中区域。

---

## 30. 自定义右键菜单的光标定位、焦点与命令命中

### ❌ 不支持 / 兼容差

- 部分 Sciter 宿主在 `selectable` 文本上触发 `contextmenu` 时，`clientX/clientY` 与 `x/y` 都可能返回 `0`，直接用这些值定位会让菜单飞到窗口左上角。
- 用 `<popup>` 承载输入框编辑菜单时，弹出的独立 mouse-modal 窗口会接管焦点。菜单虽然能显示并收到 `click`，原输入框的编辑命令却可能因焦点或选区已经丢失而没有效果。
- 在 `<popup>` 子项和文档之间重复绑定 `mousedown`、`mouseup`、`click`，还可能被弹层自动关闭时序截断，表现为菜单能点燃但命令处理器不执行。

### 推荐的 Sciter 兼容写法

- `contextmenu` 只负责 `preventDefault()`；右键按下状态可在 `mousedown` 阶段处理。
- 不要用日志框自身作为定位锚点，也不要因为事件坐标异常而退化成固定坐标。调用 `Window.this.cursorPos()` 取得当前光标相对窗口客户区的位置。普通展示型弹层仍可使用 `element.popup(popup, { anchorAt:7, popupAt:7, x, y })`，Sciter 会在接近屏幕边缘时自动调整弹层位置。
- `Window.this.cursorPos()` 不可用时，再依次回退到事件的 `windowX/windowY` 与 `clientX/clientY`。`screenX/screenY` 是桌面坐标，不能未经坐标系转换直接传给 `popup()` 的 `x/y`。
- 需要作用于输入框选区的菜单，优先使用根节点下的页面内绝对定位层，而不是独立 `<popup>`。先显示菜单测量宽高，将窗口坐标减去 `document.body.getBoundingClientRect()` 的原点转换成页面坐标，并把 `left/top` 限制在 `body.offsetWidth/offsetHeight` 内。
- 页面内菜单在父节点统一处理 `mousedown`，立即 `preventDefault()` 并执行命令，可避免菜单本身抢走输入焦点。用 `keyup` 与左键 `mouseup` 保存选区快照，右击后执行命令前通过 `edit.selectRange()` / `textarea.selectRange()` 恢复选区。
- 关闭页面内菜单不要只监听冒泡阶段的 `click`：Sciter 的原生 behavior 或业务控件可能提前停止冒泡。应在 `document` 的捕获阶段监听 `mousedown`，命中菜单外部就立即隐藏；命中菜单内部则保留，让菜单自身先执行命令。

---

## 31. 选择框优先用显式 DOM 绘制；明暗过渡不要污染主题色切换

### ❌ 不支持 / 兼容差

- 直接用 `radial-gradient` 近似单选圆点。部分 Sciter 宿主的渐变边缘与居中结果不稳定，容易重新出现发虚或圆点偏移。
- 在 16px 复选框内使用 `2px` 边框旋转成勾会过于粗壮，`4×8px`、`1px` 又会显得细小。本次实测 `6×10px`、`1.5px` 双边框更均衡；外框需要偏硬朗时使用 `1px` 圆角。
- 指望 CSS 变量自动进入 base64 SVG。数据 URI 内部是独立 SVG 文档，外层的 `var(--accent-color)` 不会自动改变其中的 `fill`。
- 永久给全页面添加颜色 `transition`。这样明暗主题会渐变，但用户点击主题色圆点时也会拖泥带水地过渡。

### 推荐的 Sciter 兼容写法

- 页面结构允许调整时，保留隐藏的原生 `input` 负责状态和表单语义，外观由相邻 DOM 节点绘制。单选框使用内层圆形节点；复选勾使用细边框旋转，不需要背景图、base64 或 SVG，并且可以直接继承主题变量：
  ```html
  <label class="gui-check">
      <input type="checkbox" checked>
      <span class="gui-check-box"><span class="gui-check-mark"></span></span>
      <span>选项</span>
  </label>
  ```
  ```css
  .gui-check input { display:none; }
  .gui-check-mark {
      display:none;
      width:6px;
      height:10px;
      border-right:1.5px solid #fff;
      border-bottom:1.5px solid #fff;
      transform:rotate(45deg);
  }
  input:checked + .gui-check-box { background-color:var(--accent-color); }
  input:checked + .gui-check-box .gui-check-mark { display:block; }
  ```
- 只有目标宿主对细边框旋转的抗锯齿确实不稳定、或 HTML 结构完全不能调整时，再回退到已经验证的 base64 SVG；不要用渐变近似 SVG。
- 不要使用 `body.theme-transitioning *` 再配合 `offsetWidth` 强制全页布局。它会让整棵 DOM 同时建立过渡并重算样式，短动画也会产生明显停顿。只给窗口背景、标题栏、侧边栏、主视图和可见卡片等少数大表面设置约 `55ms` 的常驻颜色过渡，JS 直接切换主题类：
  ```css
  body,
  .metro-titlebar,
  .metro-sidebar,
  .main-viewport {
      transition:background-color .055s linear,
                 color .055s linear,
                 border-color .055s linear;
  }
  ```
  ```js
  document.body.classList.toggle("theme-light");
  ```
- 主题色切换如果必须完全即时，可在修改 `data-accent` 的一次强制布局期间临时使用有范围的 `transition:none`；不要重新对全 DOM 添加长时间过渡。

---

## 32. 自定义下拉框要区分悬停、按下和弹层已打开状态

- 只写 `.select:hover` 时，按下后原生 `<popup>` 脱离锚点，锚点可能立即丢失悬停反馈，看起来像没有点燃态。
- 使用三个短状态：`:hover` 切到 `--accent-hover` 并显示主题光晕，`:active` 切到 `--accent-active`，调用 `popup()` 前给锚点添加 `.open`；在锚点或弹层的 `popupdismissed` 中移除 `.open`。
- 箭头的颜色和旋转跟随 `.open`，过渡控制在 `80–100ms`，既能看见反馈，又不会拖慢弹层出现：
  ```css
  .select { transition:background-color .1s, border-color .1s, box-shadow .1s; }
  .select:hover { border-color:var(--accent-hover); box-shadow:0 0 0 3px var(--accent-glow); }
  .select:active { border-color:var(--accent-active); }
  .select.open .arrow { color:var(--accent-color); transform:rotate(180deg); }
  ```

---

## 33. High DPI 下的整窗快照与百叶窗动画

本节来自 `仿QQ启动特效` 在 Sciter 5.0.3.15、Windows 125% 缩放下的实际排查。动画使用 `Graphics.Image` 截取整窗，再在 `document.documentElement.paintForeground` 中按条带绘制。问题表面上像窗口放大、右侧被截断或出现白色外圈，实际是快照尺寸、DOM 布局时机和阴影遮罩三套状态没有统一。

### 1. DPI 必须从正确的对象读取

- `getBoundingClientRect()`、条带路径以及 `gfx.draw()` 的目标矩形使用 DIP。
- `new Graphics.Image(width, height, element)` 的画布宽高必须使用物理像素，即 DIP 乘当前缩放比。
- 在 Sciter 5.0.3.15 中，`Window.this.devicePixelRatio` 实测为 `undefined`，而 `window.devicePixelRatio` 在 125% 缩放下正确返回 `1.25`。只读取前者并回退到 `1`，不会抛异常，却会静默生成尺寸错误的快照：`540×400` 的窗口等效按约 `432×320` 的布局宽度重排，固定宽度栏占比变大，最终表现为标签竖排、控件放大和右侧被裁掉。

推荐统一封装：

```js
function captureRoot(root) {
    const rect = root.getBoundingClientRect();
    const ratio = window.devicePixelRatio ||
                  Window.this.devicePixelRatio || 1;

    return {
        width: rect.width,       // DIP：路径和绘制目标使用
        height: rect.height,
        image: new Graphics.Image(
            Math.ceil(rect.width * ratio),   // PPX：离屏位图使用
            Math.ceil(rect.height * ratio),
            root
        )
    };
}

// 绘制目标仍是 DIP，不要再乘 ratio。
gfx.draw(frame.image, {
    x: 0,
    y: 0,
    width: frame.width,
    height: frame.height
});
```

不要通过“看起来像放大”就反向把 `Graphics.Image` 改回 DIP 尺寸。那会让离屏布局本身按更窄的逻辑视口重新排版，虽然某些像素比例暂时接近，复杂双栏布局仍会错位。

### 2. 隐藏窗口阶段的布局不能直接拿来截图

- 页面刚进入 `ready`、窗口仍为隐藏状态时，根节点尺寸虽然可能已经有值，但复杂内容未必完成最终窗口状态下的排版。此时截图会把右栏压窄，动画结束后真实 DOM 又恢复正常，于是看起来像窗口内容在动画末尾突然归位。
- `Window.this.state = Window.WINDOW_SHOWN` 后立刻截图仍可能过早；本例中等待一个 `requestAnimationFrame` 也不足以稳定复现。
- 当窗口内容被完全透明遮住时，连续等待多帧还可能因没有有效绘制而停住。实测可靠做法是：先安装遮罩、显示并激活窗口，再用约 `50ms` 的短定时器让布局稳定，随后仅在同步截图期间临时解除遮罩，截图完成后立即恢复遮罩并开始绘制。

```js
root.classList.add("shutter-reveal-running");
Window.this.state = Window.WINDOW_SHOWN;
Window.this.activate();

setTimeout(function() {
    root.classList.remove("shutter-reveal-running");
    const frame = captureRoot(root);
    root.classList.add("shutter-reveal-running");
    playOpen(frame);
}, 50);
```

遮罩样式应在模块加载时就注册，不要等到读取矩形后再插入 `<style>`，否则样式表插入本身可能触发一次重排，使已经保存的几何信息失效。

### 3. 真 DOM、外围阴影和动画快照必须由同一个根坐标系承担

当前标准结构中，`html` 是完整客户区和 `paintForeground` 画布；`body` 使用四边相同的单值 `margin` 承载透明阴影空间，并负责窗口背景与 `box-shadow`。因此：

- 快照必须截取 `document.documentElement`，不能只截 `body`，否则快照缺少透明外圈和阴影。
- 动画时需要隐藏真实页面，同时让 `body` 暂时透明并移除真实阴影；否则快照条带下面还留着一整块白色窗口表面，形成“外边框有白色间距”的假象。
- 不要通过删除节点、改变宽高或 `display:none` 来隐藏真实页面，这些操作会改变布局。这里仅让 `body` 的直接子元素 `opacity:0`，布局尺寸保持不变。

```css
html.shutter-reveal-running body {
    background: transparent !important;
    box-shadow: none !important;
}

html.shutter-reveal-running body > * {
    opacity: 0 !important;
}
```

不要把整个内容树改成 `visibility:hidden` 后再依赖多次绘制回调等待布局；透明窗口在没有可绘制内容时可能不再按预期推进帧。短定时器配合 `opacity:0` 更适合这一场景。

### 4. 关闭动画必须在点击时重新抓取当前画面

- 不要复用启动阶段的快照。启动后窗口可能已经获得焦点、文本发生变化、主题切换，甚至所在显示器 DPI 发生变化；旧快照会造成关闭瞬间跳变。
- 关闭按钮命中后，应先在未加遮罩的状态下重新读取根矩形并截图，然后再添加遮罩、从下到上绘制，完成后调用 `Window.this.close()`。
- 增加一次 `closing` 标记，避免连续点击同时启动多个 `paintForeground` 循环。

```js
if (closing) return;
closing = true;

const frame = captureRoot(document.documentElement);
document.documentElement.classList.add("shutter-reveal-running");
playClose(frame, function() {
    document.documentElement.paintForeground = null;
    Window.this.close();
});
```

### 5. 测试时不要把原生窗口缩放误判为快照缩放

本次用 60 FPS 录像逐帧检查，同时连续读取 Win32 窗口矩形。错误动画期间原生窗口始终保持 `540×400`，变化的只是离屏快照内部排版，因此修复方向应放在 DPI 与截图时机，而不是反复调整原生窗口大小或坐标。

另外，封装层若刚从旧的 root/body padding 方案切换到新的 `body margin` 方案，必须重新编译一次宿主 EXE。旧 EXE 配新 HTML 会保留过时的边距处理，表现成白边或阴影缺失；这不是纯 CSS 动画本身的问题。

