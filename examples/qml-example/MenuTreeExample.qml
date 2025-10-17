import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

GroupBox {
    title: "Menu Tree Model - Complete Implementation (方案1.1)"
    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        // 顶部状态栏
        RowLayout {
            Layout.fillWidth: true
            spacing: 20

            // 加载状态指示器
            RowLayout {
                visible: menuModel.loading
                BusyIndicator {
                    running: menuModel.loading
                    implicitWidth: 24
                    implicitHeight: 24
                }
                Label {
                    text: "Loading menu tree..."
                    color: "blue"
                }
            }

            // 性能指标
            Label {
                visible: menuModel.loaded
                text: `✓ Loaded ${menuModel.totalApplications} apps in ${menuModel.loadTimeMs}ms`
                color: "green"
                font.bold: true
            }

            // 错误信息
            Label {
                visible: menuModel.lastError !== ""
                text: "Error: " + menuModel.lastError
                color: "red"
            }

            Item { Layout.fillWidth: true }

            // 重新加载按钮
            Button {
                text: "Reload"
                enabled: !menuModel.loading
                onClicked: menuModel.reload()
            }
        }

        // 分隔线
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#d0d0d0"
        }

        // 树形视图
        TreeView {
            id: treeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: XdgMenuTreeModel {
                id: menuModel
                menuFile: "/etc/xdg/menus/lxqt-applications.menu"  // 使用LXQt菜单

                Component.onCompleted: {
                    console.log("[MenuTreeExample] Starting async load...")
                    console.log("  - Menu file:", menuFile)
                    loadAsync()
                }

                onLoadingFinished: {
                    console.log("[MenuTreeExample] Loading finished!")
                    console.log("  - Total applications:", totalApplications)
                    console.log("  - Load time:", loadTimeMs, "ms")
                    console.log("  - Categories:", categoryCount())
                }

                onErrorOccurred: function(error) {
                    console.error("[MenuTreeExample] Error:", error)
                }
            }

            delegate: TreeViewDelegate {
                id: treeDelegate

                // Qt 6 required properties for TreeView delegate
                required property int row
                required property var model

                // 鼠标悬停工具提示
                ToolTip.visible: hovered && model !== undefined && model.comment !== ""
                ToolTip.text: model !== undefined ? model.comment : ""
                ToolTip.delay: 500

                // 自定义内容
                Row {
                    spacing: 8
                    anchors.fill: parent
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4

                    // 图标
                    Image {
                        source: {
                            var iconName = model.iconName || ""
                            if (model.type === 0) {  // category
                                return XdgIcon.fromTheme(iconName || "folder", "folder")
                            } else {  // application
                                return XdgIcon.fromTheme(iconName || "application-x-executable")
                            }
                        }
                        sourceSize.width: 24
                        sourceSize.height: 24
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    // 文本信息
                    Column {
                        spacing: 2
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width - 160  // 留出按钮空间

                        // 标题
                        Label {
                            text: model.display || ""
                            font.bold: model.type === 0  // 分类加粗
                            width: parent.width
                            elide: Text.ElideRight
                        }

                        // 注释（仅应用程序）
                        Label {
                            visible: model.type === 1 && model.comment !== ""
                            text: model.comment || ""
                            font.pointSize: 9
                            color: "#808080"
                            width: parent.width
                            elide: Text.ElideRight
                        }
                    }

                    // 应用程序启动按钮
                    Button {
                        visible: model.type === 1  // 仅应用程序显示
                        text: "Launch"
                        implicitHeight: 28
                        anchors.verticalCenter: parent.verticalCenter

                        onClicked: {
                            console.log("[MenuTreeExample] Launching:", model.display)
                            console.log("  - Desktop ID:", model.desktopId)
                            console.log("  - Exec:", model.exec)

                            statusText.text = `Clicked: ${model.display}`
                            statusText.color = "blue"
                        }
                    }
                }
            }
        }

        // 底部状态栏
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#d0d0d0"
        }

        Label {
            id: statusText
            Layout.fillWidth: true
            text: menuModel.loaded
                ? `Menu tree ready: ${menuModel.categoryCount()} categories, ${menuModel.totalApplications} applications`
                : "Waiting for menu to load..."
            font.pointSize: 9
            color: "#606060"
        }
    }
}
