// FastIconTest.qml - Enhanced Performance and Comparison Test for FastIconProvider
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    id: window
    width: 1000
    height: 700
    visible: true
    title: "FastIconProvider Performance Test v2.0"

    // Test icon names
    readonly property var testIcons: [
        "document-open", "document-save", "document-print", "document-new",
        "edit-copy", "edit-cut", "edit-paste", "edit-delete", "edit-undo", "edit-redo",
        "folder", "folder-open", "folder-new", "user-home", "user-desktop",
        "go-home", "go-previous", "go-next", "go-up", "go-down",
        "list-add", "list-remove", "view-refresh", "view-fullscreen",
        "media-playback-start", "media-playback-pause", "media-playback-stop",
        "application-exit", "system-search", "preferences-system", "help-about"
    ]

    // Test modes
    property bool useOldProvider: false  // Switch between fasticon and theme
    property var testResults: ({})

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // Header
        Label {
            text: "FastIconProvider vs Theme Provider Performance Benchmark"
            font.pointSize: 16
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        // Live cache statistics panel
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: statsGrid.height + 20
            color: "#e8f4f8"
            border.color: "#4a90e2"
            border.width: 2
            radius: 5

            GridLayout {
                id: statsGrid
                anchors.centerIn: parent
                columns: 3
                rowSpacing: 10
                columnSpacing: 20

                Label {
                    text: "Cache Statistics (FastIcon)"
                    font.bold: true
                    Layout.columnSpan: 3
                    Layout.alignment: Qt.AlignHCenter
                }

                Label { text: "Cache Hit Rate:" }
                Label {
                    id: hitRateLabel
                    text: FastIconStats.hitRate.toFixed(1) + "%"
                    font.bold: true
                    color: FastIconStats.hitRate > 80 ? "green" : "orange"
                }
                ProgressBar {
                    from: 0
                    to: 100
                    value: FastIconStats.hitRate
                    Layout.preferredWidth: 150
                }

                Label { text: "Cache Hits:" }
                Label {
                    text: FastIconStats.hitCount
                    color: "green"
                }
                Label { text: "" }

                Label { text: "Cache Misses:" }
                Label {
                    text: FastIconStats.missCount
                    color: "red"
                }
                Label { text: "" }

                Label { text: "Cached Items:" }
                Label { text: FastIconStats.cachedItems }
                Label { text: "" }

                Label { text: "Cache Size:" }
                Label { text: (FastIconStats.cacheBytes / 1024 / 1024).toFixed(2) + " MB" }
                Label { text: "Max: " + FastIconStats.cacheSize + " MB" }

                Label { text: "Thread Pool:" }
                Label { text: FastIconStats.maxThreadCount + " threads" }
                Label { text: "" }
            }
        }

        // GPU Cache Statistics Panel (Stage 3)
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: gpuStatsGrid.height + 20
            color: "#fff4e6"
            border.color: "#ff9800"
            border.width: 2
            radius: 5

            GridLayout {
                id: gpuStatsGrid
                anchors.centerIn: parent
                columns: 3
                rowSpacing: 10
                columnSpacing: 20

                Label {
                    text: "GPU Cache Statistics (Stage 3)"
                    font.bold: true
                    Layout.columnSpan: 3
                    Layout.alignment: Qt.AlignHCenter
                }

                Label { text: "GPU Textures:" }
                Label {
                    text: FastIconStats.gpuTextureCount
                    font.bold: true
                    color: "blue"
                }
                Label { text: "" }

                Label { text: "GPU Memory:" }
                Label {
                    text: (FastIconStats.gpuMemoryBytes / 1024 / 1024).toFixed(2) + " MB"
                    font.bold: true
                    color: FastIconStats.gpuMemoryBytes > 200*1024*1024 ? "red" : "green"
                }
                Label { text: "Max: 256 MB" }

                Label { text: "Texture Reuse Rate:" }
                Label {
                    text: FastIconStats.textureReuseRate.toFixed(1) + "%"
                    font.bold: true
                    color: FastIconStats.textureReuseRate > 80 ? "green" : "orange"
                }
                ProgressBar {
                    from: 0
                    to: 100
                    value: FastIconStats.textureReuseRate
                    Layout.preferredWidth: 150
                }

                Label { text: "Atlas Optimization:" }
                Label {
                    text: "Enabled (< 64x64)"
                    color: "blue"
                }
                Label { text: "" }

                // GPU cache control buttons
                Label { text: "GPU Cache Control:" }
                Row {
                    spacing: 10
                    Layout.columnSpan: 2

                    Button {
                        text: "Clear GPU Cache"
                        onClicked: {
                            FastIconStats.clearGpuCache()
                            testStatusLabel.text = "Status: GPU cache cleared"
                        }
                    }

                    Button {
                        text: "Reset GPU Stats"
                        onClicked: {
                            FastIconStats.resetGpuStats()
                            testStatusLabel.text = "Status: GPU stats reset"
                        }
                    }
                }
            }
        }

        // FPS Counter (Stage 3.4)
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#e8ffe8"
            border.color: "#4caf50"
            border.width: 2
            radius: 5

            Row {
                anchors.centerIn: parent
                spacing: 20

                Label {
                    text: "Rendering Performance:"
                    font.bold: true
                }

                Label {
                    id: fpsLabel
                    text: "FPS: " + fpsCounter.fps
                    font.pointSize: 14
                    font.bold: true
                    color: fpsCounter.fps >= 55 ? "green" : "red"
                }

                Item {
                    id: fpsCounter
                    property int frameCount: 0
                    property real fps: 0

                    Timer {
                        interval: 1000
                        running: true
                        repeat: true
                        onTriggered: {
                            fpsCounter.fps = fpsCounter.frameCount
                            fpsCounter.frameCount = 0
                        }
                    }

                    FrameAnimation {
                        running: true
                        onTriggered: fpsCounter.frameCount++
                    }
                }
            }
        }

        // Preload Test Panel (Stage 3.3)
        GroupBox {
            title: "Icon Preload Test (Stage 3.3)"
            Layout.fillWidth: true

            ColumnLayout {
                spacing: 10
                width: parent.width

                Row {
                    spacing: 10

                    Button {
                        text: "Preload 30 Icons (24px)"
                        enabled: !FastIconStats.isPreloading()
                        onClicked: {
                            FastIconStats.preloadIcons(testIcons.slice(0, 30), 24)
                            preloadStatus.text = "Starting preload..."
                        }
                    }

                    Button {
                        text: "Cancel Preload"
                        enabled: FastIconStats.isPreloading()
                        onClicked: {
                            FastIconStats.cancelPreload()
                        }
                    }
                }

                ProgressBar {
                    id: preloadProgressBar
                    Layout.fillWidth: true
                    from: 0
                    to: 30
                    value: preloadCurrent
                    visible: FastIconStats.isPreloading()

                    property int preloadCurrent: 0
                }

                Label {
                    id: preloadStatus
                    text: "Ready for preload"
                }
            }

            Connections {
                target: FastIconStats

                function onPreloadProgressChanged(current, total) {
                    preloadProgressBar.preloadCurrent = current
                    preloadProgressBar.to = total
                    preloadStatus.text = "Preloading: " + current + "/" + total
                }

                function onPreloadFinished(success, failed) {
                    preloadStatus.text = "Preload complete! Success: " + success + ", Failed: " + failed
                    testStatusLabel.text = "Status: Preload finished - " + success + " icons loaded"
                }
            }
        }

        // Test results panel
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: resultsColumn.height + 20
            color: "#f0f0f0"
            border.color: "#ccc"
            border.width: 1
            radius: 5
            visible: testStatusLabel.text !== "Status: Ready"

            ColumnLayout {
                id: resultsColumn
                anchors.centerIn: parent
                spacing: 5
                width: parent.width - 40

                Label {
                    id: testStatusLabel
                    text: "Status: Ready"
                    font.bold: true
                    font.pointSize: 12
                }

                Label {
                    id: coldLoadResultLabel
                    text: ""
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    id: hotLoadResultLabel
                    text: ""
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    id: comparisonLabel
                    text: ""
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    color: "#006400"
                    font.bold: true
                }
            }
        }

        // Control panel
        GroupBox {
            title: "Test Controls"
            Layout.fillWidth: true

            GridLayout {
                columns: 3
                rowSpacing: 10
                columnSpacing: 10
                anchors.fill: parent

                Button {
                    text: "1. Cold Load Test (FastIcon)"
                    Layout.fillWidth: true
                    onClicked: runColdLoadTest(false)
                    enabled: !testRunning
                }

                Button {
                    text: "2. Hot Load Test (FastIcon)"
                    Layout.fillWidth: true
                    onClicked: runHotLoadTest(false)
                    enabled: !testRunning
                }

                Button {
                    text: "3. Old Provider Test"
                    Layout.fillWidth: true
                    onClicked: runOldProviderTest()
                    enabled: !testRunning
                }

                Button {
                    text: "Run Full Benchmark"
                    Layout.fillWidth: true
                    Layout.columnSpan: 3
                    highlighted: true
                    onClicked: runFullBenchmark()
                    enabled: !testRunning
                }

                Button {
                    text: "Clear Cache"
                    onClicked: {
                        FastIconStats.clearCache()
                        testStatusLabel.text = "Status: Cache cleared"
                    }
                }

                Button {
                    text: "Reset Stats"
                    onClicked: {
                        FastIconStats.resetStats()
                        testResults = {}
                        coldLoadResultLabel.text = ""
                        hotLoadResultLabel.text = ""
                        comparisonLabel.text = ""
                        testStatusLabel.text = "Status: Stats reset"
                    }
                }

                Button {
                    text: "Refresh Stats"
                    onClicked: FastIconStats.refresh()
                }
            }
        }

        // Icon grid for visual testing
        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            GridView {
                id: gridView
                cellWidth: 90
                cellHeight: 90
                model: 0

                delegate: Rectangle {
                    width: gridView.cellWidth
                    height: gridView.cellHeight
                    color: index % 2 === 0 ? "#ffffff" : "#f5f5f5"
                    border.color: "#ddd"
                    border.width: 1

                    Column {
                        anchors.centerIn: parent
                        spacing: 5

                        Item {
                            width: 48
                            height: 48
                            anchors.horizontalCenter: parent.horizontalCenter

                            Image {
                                id: testImage
                                anchors.fill: parent
                                source: buildIconUrl(index)
                                sourceSize: Qt.size(48, 48)
                                asynchronous: true
                                cache: useOldProvider  // Old provider uses Qt cache

                                property real loadStartTime: 0

                                function buildIconUrl(idx) {
                                    var iconName = testIcons[idx % testIcons.length]
                                    if (useOldProvider) {
                                        return "image://theme/" + iconName
                                    } else {
                                        return "image://fasticon/" + iconName + "?size=48x48&state=0"
                                    }
                                }

                                onStatusChanged: {
                                    if (status === Image.Loading) {
                                        loadStartTime = Date.now()
                                    } else if (status === Image.Ready && loadStartTime > 0) {
                                        var loadTime = Date.now() - loadStartTime
                                        recordLoadTime(loadTime)
                                        loadStartTime = 0
                                    }
                                }

                                // Loading indicator
                                Rectangle {
                                    anchors.fill: parent
                                    color: "#80000000"
                                    visible: testImage.status === Image.Loading

                                    BusyIndicator {
                                        anchors.centerIn: parent
                                        width: 24
                                        height: 24
                                        running: parent.visible
                                    }
                                }
                            }
                        }

                        Label {
                            text: testIcons[index % testIcons.length]
                            font.pointSize: 7
                            width: 80
                            elide: Text.ElideRight
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
    }

    // Performance tracking
    property var loadTimes: []
    property int expectedCount: 0
    property bool testRunning: false
    property string currentTest: ""

    Timer {
        id: statsRefreshTimer
        interval: 500
        running: testRunning
        repeat: true
        onTriggered: FastIconStats.refresh()
    }

    function recordLoadTime(time) {
        loadTimes.push(time)

        if (loadTimes.length >= expectedCount && expectedCount > 0) {
            finishCurrentTest()
        }
    }

    function runColdLoadTest(isFull) {
        testRunning = true
        currentTest = "cold"
        loadTimes = []
        useOldProvider = false

        testStatusLabel.text = "Status: Running cold load test (FastIcon)..."
        FastIconStats.clearCache()
        FastIconStats.resetStats()

        expectedCount = 100
        gridView.model = 0
        Qt.callLater(function() {
            gridView.model = 100
        })
    }

    function runHotLoadTest(isFull) {
        testRunning = true
        currentTest = "hot"
        loadTimes = []
        useOldProvider = false

        testStatusLabel.text = "Status: Running hot load test (FastIcon, cached)..."

        expectedCount = 100
        var oldModel = gridView.model
        gridView.model = 0
        Qt.callLater(function() {
            gridView.model = 100
        })
    }

    function runOldProviderTest() {
        testRunning = true
        currentTest = "old"
        loadTimes = []
        useOldProvider = true

        testStatusLabel.text = "Status: Running old provider test (Theme)..."

        expectedCount = 100
        gridView.model = 0
        Qt.callLater(function() {
            gridView.model = 100
        })
    }

    function finishCurrentTest() {
        testRunning = false

        var sum = 0
        var min = 999999
        var max = 0

        for (var i = 0; i < loadTimes.length; i++) {
            var t = loadTimes[i]
            sum += t
            if (t < min) min = t
            if (t > max) max = t
        }

        var avg = sum / loadTimes.length
        var result = {
            avg: avg,
            min: min,
            max: max,
            count: loadTimes.length
        }

        testResults[currentTest] = result

        if (currentTest === "cold") {
            coldLoadResultLabel.text = "Cold Load (FastIcon): " +
                "Avg=" + avg.toFixed(2) + "ms, " +
                "Min=" + min.toFixed(2) + "ms, " +
                "Max=" + max.toFixed(2) + "ms, " +
                "Samples=" + loadTimes.length
            testStatusLabel.text = "Status: Cold load test complete"
        } else if (currentTest === "hot") {
            hotLoadResultLabel.text = "Hot Load (FastIcon): " +
                "Avg=" + avg.toFixed(2) + "ms, " +
                "Min=" + min.toFixed(2) + "ms, " +
                "Max=" + max.toFixed(2) + "ms, " +
                "Samples=" + loadTimes.length
            testStatusLabel.text = "Status: Hot load test complete"
        } else if (currentTest === "old") {
            var improvement = "N/A"
            if (testResults["cold"]) {
                var speedup = testResults["cold"].avg / avg
                improvement = speedup.toFixed(2) + "x"
            }

            comparisonLabel.text = "Old Provider (Theme): Avg=" + avg.toFixed(2) + "ms | " +
                "FastIcon Improvement: " + improvement
            testStatusLabel.text = "Status: Comparison complete"
        }

        loadTimes = []
        expectedCount = 0
    }

    function runFullBenchmark() {
        testStatusLabel.text = "Status: Running full benchmark suite..."
        testResults = {}
        coldLoadResultLabel.text = ""
        hotLoadResultLabel.text = ""
        comparisonLabel.text = ""

        // Sequence: Cold -> Hot -> Old
        runColdLoadTest(true)

        // Wait for cold test to finish, then run hot
        var waitForCold = function() {
            if (!testRunning && testResults["cold"]) {
                Qt.callLater(function() {
                    runHotLoadTest(true)

                    var waitForHot = function() {
                        if (!testRunning && testResults["hot"]) {
                            Qt.callLater(function() {
                                runOldProviderTest()
                            })
                        } else {
                            Qt.callLater(waitForHot)
                        }
                    }
                    Qt.callLater(waitForHot)
                })
            } else {
                Qt.callLater(waitForCold)
            }
        }
        Qt.callLater(waitForCold)
    }

    Component.onCompleted: {
        console.log("FastIconProvider Performance Test v2.0 loaded")
        console.log("Testing with", testIcons.length, "unique icons")
        console.log("FastIconStats available:", FastIconStats ? "YES" : "NO")

        if (FastIconStats) {
            var stats = FastIconStats.getStats()
            console.log("Initial stats:", JSON.stringify(stats))
        }
    }
}
