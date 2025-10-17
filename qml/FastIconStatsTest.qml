import QtQuick 2.15
import QtQuick.Controls 2.15
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    width: 400
    height: 300
    visible: true
    title: "FastIconStats Test"

    Column {
        anchors.centerIn: parent
        spacing: 20

        Label {
            text: "FastIconStats Available: " + (typeof FastIconStats !== 'undefined' ? "YES" : "NO")
            font.bold: true
        }

        Label {
            text: "Hit Count: " + (FastIconStats ? FastIconStats.hitCount : "N/A")
        }

        Label {
            text: "Miss Count: " + (FastIconStats ? FastIconStats.missCount : "N/A")
        }

        Label {
            text: "Hit Rate: " + (FastIconStats ? FastIconStats.hitRate.toFixed(1) + "%" : "N/A")
        }

        Label {
            text: "Cache Size: " + (FastIconStats ? FastIconStats.cacheSize + " MB" : "N/A")
        }

        Label {
            text: "Thread Count: " + (FastIconStats ? FastIconStats.maxThreadCount : "N/A")
        }

        Button {
            text: "Clear Cache"
            onClicked: {
                if (FastIconStats) {
                    FastIconStats.clearCache()
                    console.log("Cache cleared")
                }
            }
        }

        Button {
            text: "Get Stats"
            onClicked: {
                if (FastIconStats) {
                    var stats = FastIconStats.getStats()
                    console.log("Stats:", JSON.stringify(stats))
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("FastIconStats test loaded")
        if (FastIconStats) {
            console.log("FastIconStats is available!")
            var stats = FastIconStats.getStats()
            console.log("Initial stats:", JSON.stringify(stats))
        } else {
            console.error("FastIconStats is NOT available!")
        }
    }
}
