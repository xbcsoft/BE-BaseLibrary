/**
 * MahApps.Metro demo window and application business.
 * Navigation, theme commands, page overlays and concrete component instances.
 */

function switchNav(targetId, navEl) {
    if (!targetId) return;

    var navItems = document.querySelectorAll(".metro-sidebar .nav-item");
    var sections = document.querySelectorAll(".main-viewport .section-container");

    navItems.forEach(function (nav) { nav.classList.remove("active"); });
    if (navEl) navEl.classList.add("active");

    sections.forEach(function (sec) {
        if (sec.id === targetId) {
            sec.style.visibility = "visible";
            sec.style.height = undefined;
            sec.style.overflow = undefined;
        } else {
            sec.style.visibility = "hidden";
            sec.style.height = "0px";
            sec.style.overflow = "hidden";
        }
    });

    if (targetId === "secProgress") {
        startSkeletonAnimation();
    } else {
        stopSkeletonAnimation();
    }
}

function toggleTheme() {
    var body = document.body;
    var themeIcon = document.getElementById("themePillIcon");
    var themeLabel = document.getElementById("themePillLabel");
    var isDark = !body.classList.contains("theme-light");
    if (isDark) {
        body.classList.add("theme-light");
        if (themeIcon) themeIcon.innerText = "☀️";
        if (themeLabel) themeLabel.innerText = "浅色";
    } else {
        body.classList.remove("theme-light");
        if (themeIcon) themeIcon.innerText = "🌙";
        if (themeLabel) themeLabel.innerText = "深色";
    }
}

function setAccent(accent) {
    if (accent) {
        var body = document.body;
        body.classList.add("accent-changing");
        var forceAccentStartLayout = body.offsetWidth;
        body.setAttribute("data-accent", accent);
        var forceAccentEndLayout = body.offsetWidth;
        body.classList.remove("accent-changing");
    }
}

function openMetroDialog() {
    var dialog = document.getElementById("demoDialogOverlay");
    if (dialog) dialog.classList.add("active");
}

function closeMetroDialog() {
    var dialog = document.getElementById("demoDialogOverlay");
    if (dialog) dialog.classList.remove("active");
}

function toggleMetroFlyout() {
    var flyout = document.getElementById("demoFlyout");
    if (flyout) flyout.classList.toggle("active");
}

function closeMetroFlyout() {
    var flyout = document.getElementById("demoFlyout");
    if (flyout) flyout.classList.remove("active");
}

/* --------------------------------------------------------------------------
   Concrete component instances used by this page
   -------------------------------------------------------------------------- */
function initSelectPopups() {
    setupCustomSelect(document.getElementById("driveSelect"), document.getElementById("drivePopup"));
    setupCustomSelect(document.getElementById("protocolSelect"), document.getElementById("protocolPopup"));
}

function initWindowEvents() {
    var themeButton = document.getElementById("btnToggleTheme");
    if (themeButton) themeButton.addEventListener("click", toggleTheme);

    document.querySelectorAll(".color-dot").forEach(function (dot) {
        dot.addEventListener("click", function () {
            setAccent(dot.getAttribute("data-accent"));
        });
    });

    document.querySelectorAll(".metro-sidebar .nav-item").forEach(function (item) {
        item.addEventListener("click", function () {
            switchNav(item.getAttribute("data-target"), item);
        });
    });

    var openDialogButton = document.getElementById("btnOpenDialog");
    var closeDialogButton = document.getElementById("btnCloseDialog");
    var openFlyoutButton = document.getElementById("btnOpenFlyout");
    var closeFlyoutButton = document.getElementById("btnCloseFlyout");

    if (openDialogButton) openDialogButton.addEventListener("click", openMetroDialog);
    if (closeDialogButton) closeDialogButton.addEventListener("click", closeMetroDialog);
    if (openFlyoutButton) openFlyoutButton.addEventListener("click", toggleMetroFlyout);
    if (closeFlyoutButton) closeFlyoutButton.addEventListener("click", closeMetroFlyout);
}

// Initialize page business after metro.js has registered component helpers.
initWindowEvents();
initSelectPopups();
