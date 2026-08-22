// page.js

// 初始化全局变量挂在 window上 作共享
window.count = window.count || 0;

export class MyPageJS extends Element {
    // 1. 内部自身的状态（每个子页面独立）
    count = 0;
    title = "";

    // 接收外部传入参数（props）
    this(props) {
        this.title = props?.titest || "默认子页面标题";
    }

    // 组件挂载时（elem.patch(<页面实例类>);）：监听全局广播事件
    componentDidMount() {
        document.on("count-changed", this.handleCountChanged = () => {
            this.componentUpdate(); // 收到广播后重新渲染组件
        });
    }

    // 组件卸载时（elem.patch(null);）：建议注销监听，防止内存泄露
    componentWillUnmount() {
        document.off("count-changed", this.handleCountChanged);
    }

	 // 返回渲染内容供patch加载
    render() {
        return (
            <div class="sub-page">
                <h2>{this.title}</h2>
                
                {/* 内部私有计数 (只影响当前组件) ——特别注释*/}
                <p>内部私有计数: <b>{this.count}</b></p>
                <button onClick={() => this.addLocalCount()}>当前组件 +1</button>

                {/* 全局共享计数 (所有组件均同步) ——特别注释*/}
                <p>全局共享计数: <b>{window.count}</b></p>
                <button onClick={() => this.addGlobalCount()}>全局共享 +1 (所有子页面同步)</button>
            </div>
        );
    }

    // 操作 1：只修改当前组件内部的 this.count
    addLocalCount() {
        this.componentUpdate({ count: this.count + 1 });
    }

    // 操作 2：修改全局的 window.count 并广播通知所有组件
    addGlobalCount() {
        window.count++;
        document.post(new Event("count-changed"));
    }
}
