import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deckshell.qtxdg 6.0

GroupBox {
    title: "Icon Cache Stress Test - Long Running Performance Validation"
    Layout.fillWidth: true
    Layout.fillHeight: true

    property int loadedIconCount: 0
    property int totalLoadTime: 0
    property var startTime: new Date()
    property bool isRunning: false
    property int currentRound: 0
    property int totalRounds: 5

    // 常用图标列表（模拟真实应用场景）
    property var iconList: [
        "document-open", "document-save", "document-new", "document-close",
        "folder", "folder-open", "folder-new", "folder-saved-search",
        "edit-copy", "edit-cut", "edit-paste", "edit-delete",
        "edit-undo", "edit-redo", "edit-find", "edit-select-all",
        "view-refresh", "view-fullscreen", "view-restore", "view-close",
        "go-home", "go-next", "go-previous", "go-up", "go-down",
        "application-exit", "applications-system", "applications-games",
        "preferences-desktop", "preferences-system", "system-run",
        "dialog-information", "dialog-warning", "dialog-error", "dialog-ok",
        "user-home", "user-desktop", "user-trash", "user-bookmarks",
        "mail-message-new", "mail-send", "mail-reply", "mail-forward",
        "media-playback-start", "media-playback-stop", "media-playback-pause",
        "network-wireless", "network-wired", "network-offline",
        "battery-full", "battery-low", "battery-charging",
        "audio-volume-high", "audio-volume-medium", "audio-volume-low", "audio-volume-muted",
        "weather-clear", "weather-few-clouds", "weather-overcast", "weather-storm",
        "list-add", "list-remove", "bookmark-new", "bookmark-new-list",
        "tool-animator", "tools-check-spelling", "accessories-calculator",
        "calendar", "clock", "contact-new", "document-properties",
        "help-about", "help-contents", "help-faq", "internet-web-browser",
        "image-loading", "image-missing", "window-close", "window-new",
        "zoom-in", "zoom-out", "zoom-fit-best", "zoom-original"
    ]

    ColumnLayout {
        anchors.fill: parent
        spacing: 15

        // 测试控制面板
        GroupBox {
            title: "Test Control"
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 4
                columnSpacing: 15
                rowSpacing: 10

                Label {
                    text: "Current Round:"
                    font.bold: true
                }
                Label {
                    text: currentRound + " / " + totalRounds
                    color: "#2196F3"
                    font.pointSize: 12
                }

                Label {
                    text: "Loaded Icons:"
                    font.bold: true
                }
                Label {
                    text: loadedIconCount.toString()
                    color: "#4CAF50"
                    font.pointSize: 12
                }

                Label {
                    text: "Running Time:"
                    font.bold: true
                }
                Label {
                    id: runningTimeLabel
                    text: "0s"
                    color: "#FF9800"
                    font.pointSize: 12
                }

                Label {
                    text: "Status:"
                    font.bold: true
                }
                Label {
                    text: isRunning ? "⏳ Running..." : "✓ Idle"
                    color: isRunning ? "#FF9800" : "#4CAF50"
                    font.pointSize: 12
                }

                Button {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    text: isRunning ? "Stop Test" : "Start Stress Test"
                    highlighted: !isRunning
                    onClicked: {
                        if (isRunning) {
                            stopTest()
                        } else {
                            startTest()
                        }
                    }
                }

                Button {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    text: "Export Statistics"
                    onClicked: exportStats()
                }
            }
        }

        // 实时性能统计
        GroupBox {
            title: "Real-time Performance Stats"
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 4
                columnSpacing: 10

                Label { text: "L2 Hit Rate:"; font.bold: true }
                Label {
                    text: FastIconStats.hitRate.toFixed(1) + "%"
                    color: FastIconStats.hitRate > 80 ? "#4CAF50" : "#FF9800"
                }

                Label { text: "L2 Cached:"; font.bold: true }
                Label { text: FastIconStats.cachedItems.toString() }

                Label { text: "GPU Textures:"; font.bold: true }
                Label { text: FastIconStats.gpuTextureCount.toString() }

                Label { text: "GPU Reuse:"; font.bold: true }
                Label {
                    text: FastIconStats.textureReuseRate.toFixed(1) + "%"
                    color: FastIconStats.textureReuseRate > 50 ? "#4CAF50" : "#9E9E9E"
                }

                Label { text: "Disk Cached:"; font.bold: true }
                Label { text: FastIconStats.diskCacheCount.toString() }

                Label { text: "Disk Size:"; font.bold: true }
                Label { text: (FastIconStats.diskCacheBytes / 1024 / 1024).toFixed(1) + " MB" }

                Label { text: "Tracked Icons:"; font.bold: true }
                Label { text: FastIconStats.trackedIconCount.toString() }

                Label { text: "Total Accesses:"; font.bold: true }
                Label { text: FastIconStats.totalIconAccesses.toString() }
            }
        }

        // 图标网格显示区（模拟真实应用）
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            GridView {
                id: iconGrid
                anchors.fill: parent
                cellWidth: 80
                cellHeight: 80
                model: iconList

                delegate: Item {
                    width: iconGrid.cellWidth
                    height: iconGrid.cellHeight

                    Column {
                        anchors.centerIn: parent
                        spacing: 5

                        Image {
                            source: "image://fasticon/" + modelData
                            sourceSize.width: 48
                            sourceSize.height: 48
                            anchors.horizontalCenter: parent.horizontalCenter

                            onStatusChanged: {
                                if (status === Image.Ready) {
                                    loadedIconCount++
                                }
                            }
                        }

                        Label {
                            text: modelData
                            font.pointSize: 7
                            width: 70
                            elide: Text.ElideRight
                            horizontalAlignment: Text.AlignHCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
        }

        // 测试日志
        GroupBox {
            title: "Test Log"
            Layout.fillWidth: true
            Layout.preferredHeight: 120

            ScrollView {
                anchors.fill: parent
                clip: true

                TextArea {
                    id: logArea
                    readOnly: true
                    wrapMode: TextArea.Wrap
                    font.family: "Monospace"
                    font.pointSize: 9
                }
            }
        }
    }

    // 测试定时器
    Timer {
        id: testTimer
        interval: 2000  // 每2秒一轮加载
        running: isRunning
        repeat: true
        onTriggered: {
            if (currentRound < totalRounds) {
                runTestRound()
            } else {
                stopTest()
                log("✓ All test rounds completed!")
                exportStats()
            }
        }
    }

    // 运行时间更新定时器
    Timer {
        interval: 1000
        running: isRunning
        repeat: true
        onTriggered: {
            var elapsed = Math.floor((new Date() - startTime) / 1000)
            runningTimeLabel.text = elapsed + "s"
        }
    }

    // 测试函数
    function startTest() {
        log("========================================")
        log("Starting Cache Stress Test...")
        log("========================================")

        // 重置计数器
        loadedIconCount = 0
        currentRound = 0
        startTime = new Date()
        isRunning = true

        // 清空缓存（可选，用于冷启动测试）
        // FastIconStats.clearCache()
        // FastIconStats.clearGpuCache()

        log("Initial Stats:")
        log("  L2 Cache: " + FastIconStats.cachedItems + " items")
        log("  GPU Cache: " + FastIconStats.gpuTextureCount + " textures")
        log("  Disk Cache: " + FastIconStats.diskCacheCount + " files")
        log("")
    }

    function stopTest() {
        isRunning = false
        var elapsed = Math.floor((new Date() - startTime) / 1000)

        log("")
        log("========================================")
        log("Test Stopped")
        log("========================================")
        log("Total Time: " + elapsed + "s")
        log("Rounds Completed: " + currentRound + " / " + totalRounds)
        log("Icons Loaded: " + loadedIconCount)
        log("")
    }

    function runTestRound() {
        currentRound++

        log("Round " + currentRound + " - Reloading " + iconList.length + " icons...")

        // 触发图标重新加载
        var temp = iconList
        iconGrid.model = []
        iconGrid.model = temp

        // 记录缓存统计
        log("  L2 Hit Rate: " + FastIconStats.hitRate.toFixed(1) + "%")
        log("  L2 Items: " + FastIconStats.cachedItems)
        log("  GPU Textures: " + FastIconStats.gpuTextureCount)
        log("  Disk Cached: " + FastIconStats.diskCacheCount)
    }

    function exportStats() {
        var report = FastIconStats.getPerformanceReport()

        log("")
        log("========================================")
        log("Performance Statistics Export")
        log("========================================")
        log("")
        log("L1 GPU Cache:")
        log("  Textures: " + report.l1GpuCache.textureCount)
        log("  Memory: " + report.l1GpuCache.gpuMB.toFixed(1) + " MB")
        log("  Reuse Rate: " + report.l1GpuCache.reuseRate.toFixed(1) + "%")
        log("")
        log("L2 CPU Cache:")
        log("  Hit Count: " + report.l2Cache.hitCount)
        log("  Miss Count: " + report.l2Cache.missCount)
        log("  Hit Rate: " + report.l2Cache.hitRate.toFixed(1) + "%")
        log("  Cached Items: " + report.l2Cache.cachedItems)
        log("  Memory: " + report.l2Cache.cacheMB.toFixed(1) + " MB")
        log("")
        log("L3 Disk Cache:")
        log("  Enabled: " + report.l3DiskCache.enabled)
        log("  Count: " + report.l3DiskCache.count)
        log("  Size: " + report.l3DiskCache.sizeMB.toFixed(1) + " MB")
        log("  Usage: " + report.l3DiskCache.usagePercent.toFixed(1) + "%")
        log("")
        log("Usage Tracking:")
        log("  Tracked Icons: " + report.usageTracking.trackedIconCount)
        log("  Total Accesses: " + report.usageTracking.totalAccesses)
        log("  Top Icons: " + report.usageTracking.topIcons.join(", "))
        log("")
        log("Overall:")
        log("  Total Cache: " + report.overall.totalCacheMB.toFixed(1) + " MB")
        log("  Total Items: " + report.overall.totalCachedItems)
        log("")

        console.log("=== Full Performance Report (JSON) ===")
        console.log(JSON.stringify(report, null, 2))
    }

    function log(message) {
        var timestamp = Qt.formatTime(new Date(), "hh:mm:ss")
        logArea.text += "[" + timestamp + "] " + message + "\n"
    }

    Component.onCompleted: {
        log("Icon Cache Stress Test Ready")
        log("Click 'Start Stress Test' to begin")
        log("")
    }
}
