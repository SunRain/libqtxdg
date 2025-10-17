import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

GroupBox {
    title: "Performance Dashboard - Icon Cache Monitoring (Stage 4)"
    Layout.fillWidth: true
    Layout.fillHeight: true

    property var perfReport: ({})

    // Auto-refresh timer
    Timer {
        id: refreshTimer
        interval: 1000  // Refresh every second
        running: true
        repeat: true
        onTriggered: {
            perfReport = FastIconStats.getPerformanceReport()
        }
    }

    Component.onCompleted: {
        perfReport = FastIconStats.getPerformanceReport()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 15

        // Header with overall stats
        Rectangle {
            Layout.fillWidth: true
            height: 80
            color: "#f5f5f5"
            radius: 8
            border.color: "#d0d0d0"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 20

                // Total Cache Size
                Column {
                    spacing: 5
                    Label {
                        text: "Total Cache Size"
                        font.bold: true
                        font.pointSize: 10
                    }
                    Label {
                        text: (perfReport.overall?.totalCacheMB || 0).toFixed(1) + " MB"
                        font.pointSize: 14
                        color: "#2196F3"
                    }
                }

                Rectangle {
                    width: 1
                    Layout.fillHeight: true
                    color: "#d0d0d0"
                }

                // Total Cached Items
                Column {
                    spacing: 5
                    Label {
                        text: "Total Cached Items"
                        font.bold: true
                        font.pointSize: 10
                    }
                    Label {
                        text: (perfReport.overall?.totalCachedItems || 0).toString()
                        font.pointSize: 14
                        color: "#4CAF50"
                    }
                }

                Rectangle {
                    width: 1
                    Layout.fillHeight: true
                    color: "#d0d0d0"
                }

                // L2 Cache Hit Rate
                Column {
                    spacing: 5
                    Label {
                        text: "L2 Cache Hit Rate"
                        font.bold: true
                        font.pointSize: 10
                    }
                    Label {
                        text: (perfReport.l2Cache?.hitRate || 0).toFixed(1) + "%"
                        font.pointSize: 14
                        color: perfReport.l2Cache?.hitRate > 80 ? "#4CAF50" : "#FF9800"
                    }
                }

                Item { Layout.fillWidth: true }

                // Refresh control
                Button {
                    text: refreshTimer.running ? "⏸ Pause" : "▶ Resume"
                    onClicked: refreshTimer.running = !refreshTimer.running
                }
            }
        }

        // Cache layers grid
        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: 10
            columnSpacing: 10

            // L1 GPU Cache
            GroupBox {
                title: "L1 GPU Cache (Textures)"
                Layout.fillWidth: true
                Layout.preferredHeight: 200

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    Label {
                        text: "Texture Count: " + (perfReport.l1GpuCache?.textureCount || 0)
                        font.pointSize: 10
                    }
                    Label {
                        text: "GPU Memory: " + (perfReport.l1GpuCache?.gpuMB || 0).toFixed(1) + " MB"
                        font.pointSize: 10
                    }
                    Label {
                        text: "Reuse Rate: " + (perfReport.l1GpuCache?.reuseRate || 0).toFixed(1) + "%"
                        font.pointSize: 10
                        color: perfReport.l1GpuCache?.reuseRate > 50 ? "#4CAF50" : "#9E9E9E"
                    }

                    Item { Layout.fillHeight: true }

                    Button {
                        Layout.fillWidth: true
                        text: "Clear GPU Cache"
                        onClicked: {
                            FastIconStats.clearGpuCache()
                            perfReport = FastIconStats.getPerformanceReport()
                        }
                    }
                }
            }

            // L2 CPU Cache
            GroupBox {
                title: "L2 CPU Cache (QImage)"
                Layout.fillWidth: true
                Layout.preferredHeight: 200

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    Label {
                        text: "Hit Count: " + (perfReport.l2Cache?.hitCount || 0)
                        font.pointSize: 10
                    }
                    Label {
                        text: "Miss Count: " + (perfReport.l2Cache?.missCount || 0)
                        font.pointSize: 10
                    }
                    Label {
                        text: "Cache Size: " + (perfReport.l2Cache?.cacheMB || 0).toFixed(1) + " MB"
                        font.pointSize: 10
                    }
                    Label {
                        text: "Cached Items: " + (perfReport.l2Cache?.cachedItems || 0)
                        font.pointSize: 10
                    }

                    Item { Layout.fillHeight: true }

                    Button {
                        Layout.fillWidth: true
                        text: "Clear L2 Cache"
                        onClicked: {
                            FastIconStats.clearCache()
                            perfReport = FastIconStats.getPerformanceReport()
                        }
                    }
                }
            }

            // L3 Disk Cache
            GroupBox {
                title: "L3 Disk Cache (Persistent)"
                Layout.fillWidth: true
                Layout.preferredHeight: 200

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    Label {
                        text: "Status: " + (perfReport.l3DiskCache?.enabled ? "✓ Enabled" : "✗ Disabled")
                        font.pointSize: 10
                        color: perfReport.l3DiskCache?.enabled ? "#4CAF50" : "#9E9E9E"
                    }
                    Label {
                        text: "Cached Icons: " + (perfReport.l3DiskCache?.count || 0)
                        font.pointSize: 10
                    }
                    Label {
                        text: "Disk Usage: " + (perfReport.l3DiskCache?.sizeMB || 0).toFixed(1) + " / " +
                              (perfReport.l3DiskCache?.maxSizeMB || 0).toFixed(0) + " MB"
                        font.pointSize: 10
                    }
                    ProgressBar {
                        Layout.fillWidth: true
                        from: 0
                        to: 100
                        value: perfReport.l3DiskCache?.usagePercent || 0
                    }

                    Item { Layout.fillHeight: true }

                    Button {
                        Layout.fillWidth: true
                        text: "Clear Disk Cache"
                        onClicked: {
                            FastIconStats.clearDiskCache()
                            perfReport = FastIconStats.getPerformanceReport()
                        }
                    }
                }
            }
        }

        // Usage Tracking & Auto-preload
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            // Usage Tracking
            GroupBox {
                title: "Usage Tracking"
                Layout.fillWidth: true
                Layout.preferredHeight: 250

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    Label {
                        text: "Status: " + (perfReport.usageTracking?.enabled ? "✓ Enabled" : "✗ Disabled")
                        font.pointSize: 10
                        color: perfReport.usageTracking?.enabled ? "#4CAF50" : "#9E9E9E"
                    }
                    Label {
                        text: "Tracked Icons: " + (perfReport.usageTracking?.trackedIconCount || 0)
                        font.pointSize: 10
                    }
                    Label {
                        text: "Total Accesses: " + (perfReport.usageTracking?.totalAccesses || 0)
                        font.pointSize: 10
                    }

                    Label {
                        text: "Top 5 Most Used Icons:"
                        font.bold: true
                        font.pointSize: 9
                        topPadding: 5
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        model: perfReport.usageTracking?.topIcons || []
                        delegate: Label {
                            text: (index + 1) + ". " + modelData
                            font.pointSize: 9
                            leftPadding: 10
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Button {
                            Layout.fillWidth: true
                            text: "Clear Stats"
                            onClicked: {
                                FastIconStats.clearUsageStats()
                                perfReport = FastIconStats.getPerformanceReport()
                            }
                        }
                        Button {
                            Layout.fillWidth: true
                            text: "Save Stats"
                            onClicked: FastIconStats.saveUsageStats()
                        }
                    }
                }
            }

            // Auto-preload Configuration
            GroupBox {
                title: "Auto-Preload"
                Layout.fillWidth: true
                Layout.preferredHeight: 250

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    Label {
                        text: "Status: " + (perfReport.autoPreload?.enabled ? "✓ Enabled" : "✗ Disabled")
                        font.pointSize: 10
                        color: perfReport.autoPreload?.enabled ? "#4CAF50" : "#9E9E9E"
                    }

                    Label {
                        text: "Preload Count: " + (perfReport.autoPreload?.count || 0) + " icons"
                        font.pointSize: 10
                    }

                    Label {
                        text: "Currently: " + (perfReport.autoPreload?.isPreloading ? "⏳ Preloading..." : "✓ Idle")
                        font.pointSize: 10
                        color: perfReport.autoPreload?.isPreloading ? "#FF9800" : "#4CAF50"
                    }

                    Item { Layout.fillHeight: true }

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "Count:"
                            font.pointSize: 9
                        }
                        SpinBox {
                            id: preloadCountSpinBox
                            Layout.fillWidth: true
                            from: 1
                            to: 100
                            value: perfReport.autoPreload?.count || 30
                            onValueModified: {
                                FastIconStats.setAutoPreloadCount(value)
                            }
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: perfReport.autoPreload?.enabled ? "Disable Auto-Preload" : "Enable Auto-Preload"
                        onClicked: {
                            FastIconStats.setAutoPreloadEnabled(!perfReport.autoPreload?.enabled)
                            perfReport = FastIconStats.getPerformanceReport()
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        text: "Trigger Preload Now"
                        enabled: !perfReport.autoPreload?.isPreloading
                        onClicked: {
                            FastIconStats.triggerAutoPreload()
                            perfReport = FastIconStats.getPerformanceReport()
                        }
                    }
                }
            }
        }

        // Control buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Button {
                Layout.fillWidth: true
                text: "Clear All Caches"
                highlighted: true
                onClicked: {
                    FastIconStats.clearCache()
                    FastIconStats.clearGpuCache()
                    FastIconStats.clearDiskCache()
                    perfReport = FastIconStats.getPerformanceReport()
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Reset All Stats"
                onClicked: {
                    FastIconStats.resetStats()
                    FastIconStats.resetGpuStats()
                    perfReport = FastIconStats.getPerformanceReport()
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Export Report (Console)"
                onClicked: {
                    console.log("=== Performance Report ===")
                    console.log(JSON.stringify(perfReport, null, 2))
                }
            }
        }
    }
}
