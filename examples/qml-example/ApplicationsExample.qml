import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

GroupBox {
    title: "Applications Model (P0 Optimized - Async Loading)"
    Layout.fillWidth: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // Loading indicator and performance metrics
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            visible: appsModel.loading || appsModel.count > 0

            BusyIndicator {
                running: appsModel.loading
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
            }

            Label {
                text: appsModel.loading
                    ? "Loading applications... " + appsModel.loadProgress + "%"
                    : "Loaded " + appsModel.count + " apps"
                font.italic: appsModel.loading
                color: appsModel.loading ? "gray" : "green"
            }

            Item { Layout.fillWidth: true }

            // Performance metrics
            Label {
                text: "Load time: " + appsModel.loadTimeMs + "ms"
                font.pointSize: 9
                color: appsModel.loadTimeMs < 50 ? "green" :
                       appsModel.loadTimeMs < 200 ? "orange" : "red"
                visible: appsModel.loadTimeMs > 0
            }
        }

        // Search and filter controls
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "Search applications..."
                enabled: !appsModel.loading
            }

            ComboBox {
                id: categoryCombo
                model: ["All", "Development", "Graphics", "Network", "Office", "Utility", "AudioVideo", "Game"]
                currentIndex: 0
                enabled: !appsModel.loading
            }

            Label {
                text: "P0 Optimized"
                color: "blue"
                font.bold: true
            }
        }

        // Status label
        Label {
            id: statusLabel
            text: {
                if (appsModel.loading) {
                    return "Loading applications..."
                } else if (categoryCombo.currentIndex === 0) {
                    return "Showing all " + appsModel.count + " applications"
                } else {
                    return "Showing " + appsModel.count + " '" + categoryCombo.currentText + "' applications"
                }
            }
            font.italic: true
            color: "gray"
        }

        // Applications list
        ListView {
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            clip: true
            enabled: !appsModel.loading
            opacity: appsModel.loading ? 0.5 : 1.0

            model: XdgApplicationsModel {
                id: appsModel
                category: categoryCombo.currentIndex === 0 ? "" : categoryCombo.currentText
                searchText: searchField.text

                // Async loading on component creation
                Component.onCompleted: {
                    console.log("[P0] Starting async load...")
                    loadAsync()
                }

                // Monitor loading events
                onLoadingStarted: {
                    console.log("[P0] Loading started")
                }

                onLoadingFinished: {
                    console.log("[P0] Loading finished! Loaded", count, "apps in", loadTimeMs, "ms")
                }

                onErrorOccurred: function(error) {
                    console.error("[P0] Error:", error)
                }
            }

            delegate: ItemDelegate {
                width: ListView.view.width

                contentItem: RowLayout {
                    spacing: 10

                    // Icon placeholder
                    Rectangle {
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 32
                        color: "lightgray"
                        radius: 4

                        Label {
                            anchors.centerIn: parent
                            text: model.iconName ? model.iconName.substring(0, 1).toUpperCase() : "?"
                            font.bold: true
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: model.name || "(unnamed)"
                            font.bold: true
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        Label {
                            text: model.comment || ""
                            font.pointSize: 9
                            color: "gray"
                            visible: text !== ""
                            wrapMode: Text.NoWrap
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        Label {
                            text: "Categories: " + (model.categories ? model.categories.join(", ") : "")
                            font.pointSize: 8
                            color: "darkgray"
                            visible: model.categories && model.categories.length > 0
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }
                }

                onClicked: {
                    detailsLabel.text = "Selected: " + model.name + "\n" +
                                       "Desktop ID: " + model.desktopId + "\n" +
                                       "Icon: " + model.iconName + "\n" +
                                       "Exec: " + model.exec
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }

        // Details area
        GroupBox {
            title: "Selected Application Details"
            Layout.fillWidth: true
            Layout.preferredHeight: 80

            ScrollView {
                anchors.fill: parent

                Label {
                    id: detailsLabel
                    text: "Click an application to see details..."
                    wrapMode: Text.Wrap
                    font.family: "monospace"
                    font.pointSize: 9
                }
            }
        }

        // Action buttons
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: appsModel.loading ? "Loading..." : "Reload (Async)"
                enabled: !appsModel.loading
                onClicked: {
                    console.log("[P0] Manual reload triggered")
                    appsModel.reload()
                }
            }

            Button {
                text: "Clear Cache"
                enabled: !appsModel.loading
                onClicked: {
                    console.log("[P0] Cache invalidated")
                    appsModel.invalidateCache()
                    appsModel.loadAsync()
                }
            }

            Button {
                text: "Clear Search"
                enabled: searchField.text !== "" && !appsModel.loading
                onClicked: searchField.text = ""
            }

            Item { Layout.fillWidth: true }

            // Status indicator
            Label {
                text: appsModel.loading ? "Loading..." : "Ready"
                color: appsModel.loading ? "orange" : "green"
                font.bold: true
            }

            Label {
                text: "Error: " + appsModel.lastError
                color: "red"
                visible: appsModel.lastError !== ""
            }
        }
    }
}
