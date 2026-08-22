/*
 * 百叶渐显（Shutter Reveal）
 * 用完整窗口快照实现自上而下打开、自下而上关闭的百叶窗动画。
 * Graphics.Image 使用物理像素，路径与绘制目标使用 DIP。
 */

// 样式由全局注入，使用后续只需添加或移除此class，无需再配套声明 CSS。
var shutterRevealStyle = document.createElement("style");
shutterRevealStyle.textContent =
	 "html.shutter-reveal-running body{background:transparent!important;box-shadow:none!important;}" +
	 "html.shutter-reveal-running body>*{opacity:0!important;}";
document.head.appendChild(shutterRevealStyle);

function createStripPath(width, top, bottom) {
	var path = new Graphics.Path();
	path.moveTo(0, top);
	path.lineTo(width, top);
	path.lineTo(width, bottom);
	path.lineTo(0, bottom);
	path.close();
	return path;
}

function drawSnapshot(gfx, image, path, width, height, opacity) {
	gfx.pushLayer(path);
	gfx.draw(image, {x: 0, y: 0, width: width, height: height, opacity: opacity});
	gfx.popLayer();
}

function smoothStep(progress) {
	return progress * progress * (3 - 2 * progress);
}

function createFrame(html, reverse) {
	var rect = html.getBoundingClientRect();
	var width = rect.width;
	var height = rect.height;
	var ratio = Window.this.devicePixelRatio || window.devicePixelRatio || 1;
	var rowCount = Math.max(6, Math.round(1 / 0.06));
	var rowHeight = height / rowCount;
	var delayStep = Math.max(4, Math.round(130 / rowCount));
	var rows = [];

	for (var row = 0; row < rowCount; row++) {
		var top = Math.round(row * rowHeight);
		var bottom = row === rowCount - 1 ? Math.round(height) : Math.round((row + 1) * rowHeight);
		rows.push({
			path: createStripPath(width, top, bottom),
			bottom: bottom,
			delay: (reverse ? rowCount - 1 - row : row) * delayStep
		});
	}

	return {
		width: width,
		height: height,
		rows: rows,
		image: new Graphics.Image(Math.ceil(width * ratio), Math.ceil(height * ratio), html)
	};
}

export const ShutterReveal = {
	start: function (closeButtonSelector, beforeClose) {
		var html = document.documentElement;
		var duration = 65;
		var allowClose = false;

		var closing = false;
		function requestClose() {
			if (closing) return;
			if (beforeClose && !beforeClose()) return;
			closing = true;

			// 关闭时重新读取当前窗口尺寸，避免沿用启动阶段的旧矩形而缩放快照。
			var frame = createFrame(html, true);

			html.classList.add("shutter-reveal-running");
			var startedAt = Date.now();
			html.paintForeground = function (gfx) {
				var elapsed = Date.now() - startedAt;
				var visibleBottom = 0;
				var transitioning = [];
				var done = true;

				for (var i = 0; i < frame.rows.length; i++) {
					var item = frame.rows[i];
					if (elapsed < item.delay) {
						visibleBottom = item.bottom;
						done = false;
						continue;
					}

					var progress = (elapsed - item.delay) / duration;
					if (progress < 1) {
						done = false;
						var opacity = 1 - smoothStep(progress);
						if (opacity > 0) transitioning.push({path: item.path, opacity: opacity});
					}
				}

				if (visibleBottom > 0) {
					drawSnapshot(gfx, frame.image, createStripPath(frame.width, 0, visibleBottom), frame.width, frame.height, 1);
				}
				for (var j = 0; j < transitioning.length; j++) {
					drawSnapshot(gfx, frame.image, transitioning[j].path, frame.width, frame.height, transitioning[j].opacity);
				}

				if (done) {
					html.paintForeground = null;
					allowClose = true;
					Window.this.close();
				} else {
					html.requestPaint();
				}
			};
			html.requestPaint();
		}

		var closeButton = document.querySelector(closeButtonSelector);
		if (closeButton) {
			closeButton.on("click", function () {
				requestClose();
				return true;
			});
		}

		Window.this.on("closerequest", function (event) {
			if (allowClose) return;
			event.preventDefault();
			requestClose();
		});

		function captureAndOpen() {
			// Graphics.Image 可以直接对尚未显示的 DOM 建立离屏快照。
			// 因此先取得包含 body 外阴影的完整页面，再遮住真实 DOM 并显示窗口；
			var frame = createFrame(html, false);
			html.classList.add("shutter-reveal-running");
			Window.this.state = Window.WINDOW_SHOWN;
			Window.this.activate();

			var startedAt = Date.now();
			html.paintForeground = function (gfx) {
				var elapsed = Date.now() - startedAt;
				var revealedBottom = 0;
				var transitioning = [];
				var done = true;

				for (var i = 0; i < frame.rows.length; i++) {
					var item = frame.rows[i];
					if (elapsed < item.delay) {
						done = false;
						break;
					}

					var progress = (elapsed - item.delay) / duration;
					if (progress >= 1) {
						revealedBottom = item.bottom;
					} else {
						done = false;
						var opacity = smoothStep(progress);
						if (opacity > 0) transitioning.push({path: item.path, opacity: opacity});
					}
				}

				if (revealedBottom > 0) {
					drawSnapshot(gfx, frame.image, createStripPath(frame.width, 0, revealedBottom), frame.width, frame.height, 1);
				}
				for (var j = 0; j < transitioning.length; j++) {
					drawSnapshot(gfx, frame.image, transitioning[j].path, frame.width, frame.height, transitioning[j].opacity);
				}

				if (done) {
					html.paintForeground = null;
					html.classList.remove("shutter-reveal-running");
				}
				html.requestPaint();
			};
			html.requestPaint();
		}
		captureAndOpen();
		return {close: requestClose};
	}
};
