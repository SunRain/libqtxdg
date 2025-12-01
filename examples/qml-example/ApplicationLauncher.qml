/* ApplicationLauncher.qml - 完整的应用启动器示例
 *
 * 功能特性:
 * - 使用XdgMenuTreeModel显示应用菜单
 * - 支持搜索过滤
 * - 应用启动功能
 * - 显示应用详细信息
 * - 支持收藏功能
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deckshell.qtxdg 6.0

Item {
    id: root

    // 菜单模型
    XdgMenuTreeModel {
        id: menuModel
        menuFile: "/etc/xdg/menus/lxqt-applications.menu"

        Component.onCompleted: {
            console.log("[ApplicationLauncher] Loading menu...")
            loadAsync()
        }

        onLoadingFinished: {
            console.log("[ApplicationLauncher] Menu loaded!")
            console.log("  - Categories:", categoryCount())
            console.log("  - Applications:", totalApplications)
            console.log("  - Load time:", loadTimeMs, "ms")
        }

        onErrorOccurred: function(error) {
            errorDialog.text = error
            errorDialog.open()
        }
    }

    // 收藏列表(使用ListModel模拟,实际应用应该持久化)
    ListModel {
        id: favoritesModel
    }

    // 错误对话框
    Dialog {
        id: errorDialog
        title: "Error"
        property alias text: errorText.text
        standardButtons: Dialog.Ok

        Label {
            id: errorText
        }
    }

    // 应用详情对话框
    Dialog {
        id: appInfoDialog
        title: "Application Information"
        width: 500
        standardButtons: Dialog.Close

        property string appName: ""
        property string appComment: ""
        property string appExec: ""
        property string appDesktopFile: ""
        property string appIcon: ""
        property string appGenericName: ""

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            RowLayout {
                Image {
                    source: XdgIcon.fromTheme(appInfoDialog.appIcon || "application-x-executable")
                    sourceSize.width: 48
                    sourceSize.height: 48
                }

                ColumnLayout {
                    Label {
                        text: appInfoDialog.appName
                        font.bold: true
                        font.pointSize: 14
                    }
                    Label {
                        text: appInfoDialog.appGenericName
                        color: "gray"
                    }
                }
            }

            Label {
                text: appInfoDialog.appComment
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            GridLayout {
                columns: 2
                Layout.fillWidth: true

                Label { text: "Command:"; font.bold: true }
                Label {
                    text: appInfoDialog.appExec
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                }

                Label { text: "Desktop File:"; font.bold: true }
                Label {
                    text: appInfoDialog.appDesktopFile
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 工具栏
        ToolBar {
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent

                ToolButton {
                    text: "Refresh"
                    icon.name: "view-refresh"
                    onClicked: menuModel.reload()
                }

                ToolSeparator {}

                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: "Search applications..."

                    onTextChanged: {
                        // 简单的搜索过滤(只过滤可见项,不改变模型)
                        // 实际应用可能需要使用QSortFilterProxyModel
                    }
                }

                Label {
                    text: menuModel.loading ? "Loading..." :
                          `${menuModel.totalApplications} apps`
                }
            }
        }

        // 主内容区域
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // 左侧: 分类和应用树
            Rectangle {
                SplitView.minimumWidth: 250
                SplitView.preferredWidth: 350
                color: palette.base
                border.color: palette.mid
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing: 5

                    Label {
                        text: "All Applications"
                        font.bold: true
                        font.pointSize: 12
                    }

                    TreeView {
                        id: treeView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: menuModel
                        clip: true

                        delegate: TreeViewDelegate {
                            id: treeDelegate

                            required property int row
                            required property var model

                            contentItem: RowLayout {
                                spacing: 5

                                Image {
                                    source: {
                                        if (model.type === "category") {
                                            return XdgIcon.fromTheme(model.iconName || "folder")
                                        } else {
                                            return XdgIcon.fromTheme(model.iconName || "application-x-executable")
                                        }
                                    }
                                    sourceSize.width: 16
                                    sourceSize.height: 16
                                }

                                Label {
                                    text: model.display
                                    font.bold: model.type === "category"
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }

                                // 应用才显示启动按钮
                                ToolButton {
                                    visible: model.type === "application"
                                    text: "▶"
                                    flat: true
                                    onClicked: {
                                        launchApplication(model)
                                    }
                                }
                            }

                            onDoubleClicked: {
                                if (model.type === "application") {
                                    launchApplication(model)
                                }
                            }

                            // 右键菜单
                            TapHandler {
                                acceptedButtons: Qt.RightButton
                                onTapped: {
                                    if (model.type === "application") {
                                        contextMenu.appModel = model
                                        contextMenu.popup()
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 右侧: 应用网格视图(可选视图)
            Rectangle {
                SplitView.fillWidth: true
                color: palette.base
                border.color: palette.mid
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    RowLayout {
                        Label {
                            text: "Favorites"
                            font.bold: true
                            font.pointSize: 12
                        }

                        Item { Layout.fillWidth: true }

                        Label {
                            text: `${favoritesModel.count} favorites`
                            color: "gray"
                        }
                    }

                    GridView {
                        id: favoritesGrid
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        model: favoritesModel
                        cellWidth: 100
                        cellHeight: 100
                        clip: true

                        delegate: Item {
                            width: favoritesGrid.cellWidth
                            height: favoritesGrid.cellHeight

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 5
                                spacing: 5

                                Image {
                                    source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                                    sourceSize.width: 48
                                    sourceSize.height: 48
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Label {
                                    text: model.name
                                    Layout.fillWidth: true
                                    horizontalAlignment: Text.AlignHCenter
                                    elide: Text.ElideRight
                                    wrapMode: Text.WordWrap
                                    maximumLineCount: 2
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                onClicked: function(mouse) {
                                    if (mouse.button === Qt.LeftButton) {
                                        launchFavoriteApp(index)
                                    } else {
                                        favoriteContextMenu.selectedIndex = index
                                        favoriteContextMenu.popup()
                                    }
                                }

                                onDoubleClicked: {
                                    launchFavoriteApp(index)
                                }
                            }
                        }

                        Label {
                            anchors.centerIn: parent
                            visible: favoritesModel.count === 0
                            text: "No favorites yet\n\nRight-click an application\nand select 'Add to Favorites'"
                            horizontalAlignment: Text.AlignHCenter
                            color: "gray"
                        }
                    }
                }
            }
        }

        // 状态栏
        ToolBar {
            Layout.fillWidth: true
            position: ToolBar.Footer

            RowLayout {
                anchors.fill: parent

                Label {
                    text: menuModel.loading ?
                          "⏳ Loading menu..." :
                          "✓ Ready"
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: `Categories: ${menuModel.categoryCount()} | Apps: ${menuModel.totalApplications}`
                }
            }
        }
    }

    // 应用右键菜单
    Menu {
        id: contextMenu
        property var appModel: null

        MenuItem {
            text: "Launch"
            icon.name: "system-run"
            onTriggered: {
                if (contextMenu.appModel) {
                    launchApplication(contextMenu.appModel)
                }
            }
        }

        MenuItem {
            text: "Add to Favorites"
            icon.name: "bookmark-new"
            onTriggered: {
                if (contextMenu.appModel) {
                    addToFavorites(contextMenu.appModel)
                }
            }
        }

        MenuSeparator {}

        MenuItem {
            text: "Properties"
            icon.name: "document-properties"
            onTriggered: {
                if (contextMenu.appModel) {
                    showAppInfo(contextMenu.appModel)
                }
            }
        }
    }

    // 收藏应用右键菜单
    Menu {
        id: favoriteContextMenu
        property int selectedIndex: -1

        MenuItem {
            text: "Launch"
            icon.name: "system-run"
            onTriggered: {
                launchFavoriteApp(favoriteContextMenu.selectedIndex)
            }
        }

        MenuItem {
            text: "Remove from Favorites"
            icon.name: "edit-delete"
            onTriggered: {
                if (favoriteContextMenu.selectedIndex >= 0) {
                    favoritesModel.remove(favoriteContextMenu.selectedIndex)
                }
            }
        }
    }

    // JavaScript 函数
    function launchApplication(appModel) {
        console.log("[Launch] Starting application:", appModel.display)
        console.log("  Desktop ID:", appModel.desktopId)
        console.log("  Exec:", appModel.exec)

        // 使用XdgDesktopFile启动应用
        var desktop = Qt.createQmlObject(
            'import org.deckshell.qtxdg 6.0; XdgDesktopFile {}',
            root,
            'dynamicDesktop'
        )

        desktop.fileName = appModel.desktopFilePath
        if (desktop.load()) {
            desktop.startDetached()
            console.log("[Launch] Application started successfully")
        } else {
            console.error("[Launch] Failed to load desktop file:", appModel.desktopFilePath)
        }
    }

    function launchFavoriteApp(index) {
        if (index < 0 || index >= favoritesModel.count) return

        var app = favoritesModel.get(index)

        var desktop = Qt.createQmlObject(
            'import org.deckshell.qtxdg 6.0; XdgDesktopFile {}',
            root,
            'dynamicDesktop'
        )

        desktop.fileName = app.desktopFilePath
        if (desktop.load()) {
            desktop.startDetached()
        }
    }

    function addToFavorites(appModel) {
        // 检查是否已存在
        for (var i = 0; i < favoritesModel.count; i++) {
            if (favoritesModel.get(i).desktopId === appModel.desktopId) {
                console.log("[Favorites] Application already in favorites")
                return
            }
        }

        favoritesModel.append({
            "name": appModel.display,
            "iconName": appModel.iconName,
            "desktopId": appModel.desktopId,
            "desktopFilePath": appModel.desktopFilePath,
            "exec": appModel.exec
        })

        console.log("[Favorites] Added:", appModel.display)
    }

    function showAppInfo(appModel) {
        appInfoDialog.appName = appModel.display
        appInfoDialog.appComment = appModel.comment || "(No description)"
        appInfoDialog.appExec = appModel.exec
        appInfoDialog.appDesktopFile = appModel.desktopFilePath
        appInfoDialog.appIcon = appModel.iconName
        appInfoDialog.appGenericName = appModel.genericName || ""
        appInfoDialog.open()
    }
}
