/**
 * MahApps.Metro reusable component behaviors for Sciter UI.
 * Contains only control-level interaction; window and demo business live in index.js.
 */

var activeTextContextTarget = null;
var activeTextSelectionSnapshot = null;
var executingTextMenuCommand = false;
var textSelectionSnapshots = new WeakMap();
var textPlaceholderSnapshots = new WeakMap();
var textEditCommands = {
    "undo": "edit:undo",
    "cut": "edit:cut",
    "copy": "edit:copy",
    "paste": "edit:paste",
    "delete": "edit:delete-next",
    "select-all": "edit:selectall"
};
function getCursorPosition(evt) {
    var cursor = null;
    try {
        if (typeof Window !== "undefined" && Window.this && typeof Window.this.cursorPos === "function") {
            cursor = Window.this.cursorPos();
        }
    } catch (error) {
        cursor = null;
    }

    var x = cursor && cursor.length > 1 ? cursor[0] : evt.windowX;
    var y = cursor && cursor.length > 1 ? cursor[1] : evt.windowY;
    if (typeof x !== "number") x = evt.clientX || 0;
    if (typeof y !== "number") y = evt.clientY || 0;
    return { x: x, y: y };
}

function getTextEditor(target) {
    return target.edit || target.password || target.textarea;
}

function captureTextSelection(target) {
    var editor = getTextEditor(target);
    var snapshot = {
        text: editor.selectionText || "",
        start: editor.selectionStart,
        end: editor.selectionEnd
    };
    textSelectionSnapshots.set(target, snapshot);
    return snapshot;
}

function restoreTextSelection(target, snapshot) {
    target.focus();
    getTextEditor(target).selectRange(snapshot.start, snapshot.end);
}

function clearTextSelection(target) {
    var snapshot = captureTextSelection(target);
    getTextEditor(target).selectRange(snapshot.end, snapshot.end);
}

function setTextMenuOpen(target, open) {
    if (open) {
        target.setAttribute("text-menu-open", "");
        // Sciter popup 接管焦点后，空编辑框会立即绘制 placeholder。
        // 菜单生命周期内暂时移除，关闭时原样恢复。
        if (target.hasAttribute("placeholder") && !textPlaceholderSnapshots.has(target)) {
            textPlaceholderSnapshots.set(target, target.getAttribute("placeholder"));
            target.removeAttribute("placeholder");
        }
    } else {
        target.removeAttribute("text-menu-open");
        if (textPlaceholderSnapshots.has(target)) {
            target.setAttribute("placeholder", textPlaceholderSnapshots.get(target));
            textPlaceholderSnapshots.delete(target);
        }
    }
}

function canRunTextCommand(target, command) {
    var nativeCommand = textEditCommands[command];
    if (!nativeCommand) return false;
    var readonly = target.hasAttribute("readonly") || target.hasAttribute("disabled");
    var hasSelection = activeTextSelectionSnapshot.text.length > 0;
    var isPassword = (target.getAttribute("type") || "").toLowerCase() === "password";

    if (command === "copy") return !isPassword && hasSelection;
    if (command === "select-all") return target.value.length > 0;
    if (command === "cut" || command === "delete") return !readonly && !isPassword && hasSelection;
    if (command === "undo") return !readonly && target.hasAttribute("data-can-undo");

    try {
        if (typeof target.checkCommand === "function") {
            return (target.checkCommand(nativeCommand) & 0x02) === 0;
        }
    } catch (error) {
    }

    if (command === "paste") return !readonly;
    return false;
}

function showTextContextMenu(evt, target) {
    var menu = document.getElementById("textContextMenu");
    target.focus();
    target.state.focus = true;
    activeTextContextTarget = target;
    setTextMenuOpen(target, true);
    var currentSelection = getTextEditor(target).selectionText || "";
    activeTextSelectionSnapshot = currentSelection.length > 0
        ? captureTextSelection(target)
        : (textSelectionSnapshots.get(target) || captureTextSelection(target));
    menu.querySelectorAll(".context-command-item").forEach(function (item) {
        if (canRunTextCommand(target, item.getAttribute("data-command"))) item.removeAttribute("disabled");
        else item.setAttribute("disabled", "");
    });
    var cursor = getCursorPosition(evt);
    document.body.popup(menu, {
        anchorAt: 7,
        popupAt: 7,
        x: cursor.x,
        y: cursor.y
    });
}

function runTextMenuCommand(item) {
    if (item.hasAttribute("disabled") || !activeTextContextTarget) return;
    executingTextMenuCommand = true;
    var command = item.getAttribute("data-command");
    var nativeCommand = textEditCommands[command];
    var editorApi = getTextEditor(activeTextContextTarget);
    var exec = typeof activeTextContextTarget.execCommand === "function"
        ? activeTextContextTarget.execCommand.bind(activeTextContextTarget)
        : activeTextContextTarget.executeCommand.bind(activeTextContextTarget);
    if (command !== "copy") restoreTextSelection(activeTextContextTarget, activeTextSelectionSnapshot);
    if (command === "cut") exec("edit:cut");
    else if (command === "delete" && activeTextContextTarget.textarea) activeTextContextTarget.textarea.removeText();
    else if (command === "copy") exec("edit:copy");
    else if (command === "select-all") editorApi.selectAll();
    else exec(nativeCommand);
    var menu = document.getElementById("textContextMenu");
    menu.state.popup = false;
    if (command === "copy") activeTextContextTarget.state.focus = true;
    setTextMenuOpen(activeTextContextTarget, false);
    executingTextMenuCommand = false;
}

/* --------------------------------------------------------------------------
   Metro Tabs
   -------------------------------------------------------------------------- */
function initMetroTabs() {
    document.querySelectorAll(".metro-tabs").forEach(function (tabs) {
        var header = tabs.querySelector(".tab-header");
        if (!header) return;
        header.addEventListener("click", function (evt) {
            var item = evt.target.closest(".tab-item");
            if (!item) return;
            var items = header.querySelectorAll(".tab-item");
            var panes = tabs.querySelectorAll(".tab-pane");
            var targetIndex = -1;
            items.forEach(function (it, idx) {
                it.classList.remove("active");
                if (it === item) targetIndex = idx;
            });
            item.classList.add("active");
            panes.forEach(function (pane, idx) {
                if (idx === targetIndex) pane.classList.add("active");
                else pane.classList.remove("active");
            });
        });
    });
}

/* --------------------------------------------------------------------------
   Slider Sync
   -------------------------------------------------------------------------- */
function initSliders() {
    var sliders = document.querySelectorAll(".custom-slider");
    sliders.forEach(function (slider) {
        var displayId = slider.getAttribute("data-display");
        var displayEl = displayId ? document.getElementById(displayId) : null;
        var fill = slider.querySelector(".custom-slider-fill");
        var thumb = slider.querySelector(".custom-slider-thumb");
        var dragging = false;

        function applyValue(value) {
            value = Math.max(0, Math.min(100, Math.round(value)));
            slider.setAttribute("data-value", value);
            if (fill) fill.style.width = value + "%";
            if (thumb) thumb.style.left = value + "%";
            if (displayEl) displayEl.innerText = value;
        }
        function updateFromMouse(evt) {
            var rect = slider.getBoundingClientRect();
            applyValue((evt.clientX - rect.left) * 100 / rect.width);
        }
        function handleWheel(evt) {
            var wheelDelta = evt.wheelDelta !== undefined ? evt.wheelDelta : (evt.deltaY ? -evt.deltaY : 0);
            var delta = wheelDelta > 0 ? 2 : -2;
            var current = parseInt(slider.getAttribute("data-value") || "0", 10);
            applyValue(current + delta);
            evt.preventDefault();
        }

        slider.addEventListener("mousedown", function (evt) {
            dragging = true;
            updateFromMouse(evt);
            if (slider.state && typeof slider.state.capture === "function") slider.state.capture(true);
            evt.preventDefault();
        });
        slider.addEventListener("mousemove", function (evt) { if (dragging) updateFromMouse(evt); });
        slider.addEventListener("mouseup", function (evt) {
            if (!dragging) return;
            dragging = false;
            if (slider.state && typeof slider.state.capture === "function") slider.state.capture(false);
            updateFromMouse(evt);
        });
        slider.addEventListener("mousewheel", handleWheel);
        slider.addEventListener("wheel", handleWheel);

        applyValue(parseInt(slider.getAttribute("data-value") || "0", 10));
    });

    var nativeSliders = document.querySelectorAll('input[type="hslider"].metro-slider');
    nativeSliders.forEach(function (slider) {
        var displayId = slider.getAttribute("data-display");
        var displayEl = displayId ? document.getElementById(displayId) : null;
        function updateNativeDisplay() {
            if (displayEl) displayEl.innerText = slider.value;
        }
        function handleNativeWheel(evt) {
            var wheelDelta = evt.wheelDelta !== undefined ? evt.wheelDelta : (evt.deltaY ? -evt.deltaY : 0);
            var step = parseInt(slider.getAttribute("step") || "1", 10) * 2;
            var val = parseInt(slider.value || "0", 10);
            var delta = wheelDelta > 0 ? step : -step;
            slider.value = Math.max(0, Math.min(100, val + delta));
            updateNativeDisplay();
            evt.preventDefault();
        }
        slider.addEventListener("input", updateNativeDisplay);
        slider.addEventListener("change", updateNativeDisplay);
        slider.addEventListener("mousewheel", handleNativeWheel);
        slider.addEventListener("wheel", handleNativeWheel);
        updateNativeDisplay();
    });
}

function setupCustomSelect(selectEl, popupEl) {
    if (!selectEl || !popupEl) return;
    var captionEl = selectEl.querySelector(".caption");
    var dismissedTime = 0;
    selectEl.addEventListener("mousedown", function (evt) {
        evt.preventDefault();
        if (Date.now() - dismissedTime < 150) return;
        popupEl.style.set({ width: selectEl.offsetWidth + "px" });
        selectEl.classList.add("open");
        selectEl.popup(popupEl, "bottom-left");
    });
    selectEl.addEventListener("popupdismissed", function () {
        dismissedTime = Date.now();
        selectEl.classList.remove("open");
    });
    popupEl.addEventListener("popupdismissed", function () {
        dismissedTime = Date.now();
        selectEl.classList.remove("open");
    });
    popupEl.addEventListener("click", function (evt) {
        var item = evt.target.closest(".option-item");
        if (!item) return;
        var value = item.getAttribute("data-value");
        if (captionEl && value) captionEl.innerText = value;
        popupEl.state.popup = false;
    });

    // Sciter may momentarily clear :hover while a popup scroll layer is
    // recomposited by the mouse wheel. Keep the hot row as explicit state so
    // wheel ticks (including attempts past either end) cannot make it flash.
    popupEl.addEventListener("mousemove", function (evt) {
        var item = evt.target.closest(".option-item");
        if (!item || item.classList.contains("hot")) return;
        popupEl.querySelectorAll(".option-item.hot").forEach(function (oldItem) {
            oldItem.classList.remove("hot");
        });
        item.classList.add("hot");
    });
    popupEl.addEventListener("mouseleave", function () {
        popupEl.querySelectorAll(".option-item.hot").forEach(function (item) {
            item.classList.remove("hot");
        });
    });
    popupEl.addEventListener("popupdismissed", function () {
        popupEl.querySelectorAll(".option-item.hot").forEach(function (item) {
            item.classList.remove("hot");
        });
    });
}

function initDemoButtons() {
    var group = document.querySelector(".metro-btn-group");
    if (!group) return;
    function selectButton(button) {
        if (!button) return;
        group.querySelectorAll(".metro-btn").forEach(function (item) { item.classList.remove("selected"); });
        button.classList.add("selected");
    }
    group.addEventListener("mousedown", function (evt) {
        var button = evt.target.closest(".metro-btn");
        selectButton(button);
    });
    group.addEventListener("click", function (evt) {
        var button = evt.target.closest(".metro-btn");
        selectButton(button);
    });
}

function initTextContextMenus() {
    var menu = document.getElementById("textContextMenu");
    if (!menu) return;
    menu.addEventListener("popupdismissed", function () {
        if (!activeTextContextTarget) return;
        setTextMenuOpen(activeTextContextTarget, false);
        if (!executingTextMenuCommand && !activeTextContextTarget.state.focus) {
            clearTextSelection(activeTextContextTarget);
        }
    });
    menu.addEventListener("mouseup", function (evt) {
        var item = evt.target.closest(".context-command-item");
        if (!item) return;
        evt.preventDefault();
        evt.stopPropagation();
        runTextMenuCommand(item);
    });
    var targets = document.querySelectorAll('input[type="text"], input[type="password"], textarea, .metro-input, .metro-textarea');
    targets.forEach(function (target) {
        if (target.hasAttribute("data-text-menu-bound")) return;
        target.setAttribute("data-text-menu-bound", "true");
        target.addEventListener("focus", function () {
            if (activeTextContextTarget && activeTextContextTarget !== target &&
                activeTextContextTarget.hasAttribute("text-menu-open")) {
                setTextMenuOpen(activeTextContextTarget, false);
                clearTextSelection(activeTextContextTarget);
            }
        });
        target.addEventListener("blur", function () {
            if (!target.hasAttribute("text-menu-open")) clearTextSelection(target);
        });
        target.addEventListener("keyup", function () { captureTextSelection(target); });
        target.addEventListener("input", function () {
            if (!target.hasAttribute("readonly")) target.setAttribute("data-can-undo", "true");
            captureTextSelection(target);
        });
        target.addEventListener("mouseup", function (evt) {
            if (evt.button === 2) return;
            captureTextSelection(target);
        });
        target.addEventListener("mousedown", function (evt) {
            if (evt.button !== 2) return;
            evt.preventDefault();
            evt.stopPropagation();
            showTextContextMenu(evt, target);
        });
        target.addEventListener("contextmenu", function (evt) {
            evt.preventDefault();
            evt.stopPropagation();
        });
    });

}

/* --------------------------------------------------------------------------
   Skeleton Animation
   -------------------------------------------------------------------------- */
var skeletonAnimGen = 0;
var isSkeletonAnimating = false;

function startSkeletonAnimation() {
    var skeletons = document.querySelectorAll(".metro-skeleton");
    if (!skeletons.length || isSkeletonAnimating) return;

    isSkeletonAnimating = true;
    var myGen = ++skeletonAnimGen;

    function renderFrame() {
        if (myGen !== skeletonAnimGen || !isSkeletonAnimating) return;

        var t = (Date.now() % 1200) / 1200;
        var center = t * 140 - 20;
        var half = 35;
        var s1 = Math.max(0, Math.min(100, center - half));
        var s2 = Math.max(0, Math.min(100, center));
        var s3 = Math.max(0, Math.min(100, center + half));

        var isLight = document.body.classList.contains("theme-light");
        var base = isLight ? "rgba(0,0,0,0.06)" : "rgba(255,255,255,0.08)";
        var hi   = isLight ? "rgba(0,0,0,0.18)" : "rgba(255,255,255,0.25)";

        var grad = "linear-gradient(to right, " +
            base + " " + s1 + "%, " + hi + " " + s2 + "%, " + base + " " + s3 + "%)";

        skeletons.forEach(function (sk) {
            sk.style.background = grad;
        });

        if (window.requestAnimationFrame) {
            requestAnimationFrame(renderFrame);
        }
    }

    if (window.requestAnimationFrame) {
        requestAnimationFrame(renderFrame);
    }
}

function stopSkeletonAnimation() {
    isSkeletonAnimating = false;
    skeletonAnimGen++;
    var skeletons = document.querySelectorAll(".metro-skeleton");
    skeletons.forEach(function (sk) {
        sk.style.background = "";
    });
}

// Initialize reusable controls.
initMetroTabs();
initSliders();
initDemoButtons();
initTextContextMenus();
