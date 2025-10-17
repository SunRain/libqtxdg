# libqtxdg 示例代码集合

> **版本:** 4.2.0
> **最后更新:** 2025-11-20

本文档包含 libqtxdg QML 插件的完整可运行示例代码，涵盖各种常见使用场景。

## 📋 目录

1. [快速上手示例](#快速上手示例)
2. [文件管理器示例](#文件管理器示例)
3. [应用启动器示例](#应用启动器示例)
4. [系统设置示例](#系统设置示例)
5. [实用工具示例](#实用工具示例)

---

## 🚀 快速上手示例

### 示例 1: Hello libqtxdg

**文件:** `HelloXdg.qml`

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    visible: true
    width: 400
    height: 300
    title: "Hello libqtxdg"

    Column {
        anchors.centerIn: parent
        spacing: 20

        Label {
            text: "XDG Directories"
            font.bold: true
            font.pointSize: 14
        }

        Label { text: "Config: " + XdgDirs.configHome() }
        Label { text: "Data: " + XdgDirs.dataHome() }
        Label { text: "Cache: " + XdgDirs.cacheHome() }

        Button {
            text: "Show Desktop Directory"
            onClicked: {
                console.log("Desktop:", XdgDirs.userDir(XdgDirs.Desktop))
            }
        }
    }
}
```

### 示例 2: 图标展示

**文件:** `IconShowcase.qml`

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    visible: true
    width: 500
    height: 400
    title: "Icon Showcase"

    GridLayout {
        anchors.fill: parent
        anchors.margins: 20
        columns: 4
        rowSpacing: 10
        columnSpacing: 10

        // 常用图标列表
        property var icons: [
            "document-open", "document-save", "document-print",
            "edit-copy", "edit-cut", "edit-paste",
            "folder", "folder-open", "folder-new",
            "user-home", "go-home", "system-file-manager",
            "applications-internet", "applications-games", "applications-graphics",
            "system-run", "system-shutdown", "system-reboot"
        ]

        Repeater {
            model: parent.icons

            Rectangle {
                width: 100
                height: 100
                border.color: "gray"
                border.width: 1
                radius: 5

                Column {
                    anchors.centerIn: parent
                    spacing: 5

                    Image {
                        source: XdgIcon.fromTheme(modelData)
                        sourceSize.width: 48
                        sourceSize.height: 48
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Label {
                        text: modelData
                        font.pointSize: 8
                        elide: Text.ElideMiddle
                        width: 90
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
```

---

## 📁 文件管理器示例

### 示例 3: MIME 类型检测器

**文件:** `MimeDetector.qml`

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    visible: true
    width: 600
    height: 500
    title: "MIME Type Detector"

    XdgMimeTypeWrapper {
        id: mimeWrapper
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Label {
            text: "MIME Type Detector"
            font.bold: true
            font.pointSize: 16
        }

        GroupBox {
            title: "File Selection"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true

                    TextField {
                        id: filePathField
                        Layout.fillWidth: true
                        placeholderText: "Enter file path or click Browse..."
                    }

                    Button {
                        text: "Browse"
                        onClicked: fileDialog.open()
                    }
                }

                Button {
                    text: "Detect MIME Type"
                    Layout.fillWidth: true
                    enabled: filePathField.text !== ""
                    onClicked: detectMimeType()
                }
            }
        }

        GroupBox {
            title: "MIME Type Information"
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: resultPane.visible

            ScrollView {
                anchors.fill: parent

                ColumnLayout {
                    id: resultPane
                    width: parent.width
                    spacing: 10
                    visible: false

                    property string currentMime: ""

                    Row {
                        spacing: 15

                        Image {
                            id: mimeIcon
                            sourceSize.width: 64
                            sourceSize.height: 64
                        }

                        ColumnLayout {
                            Label {
                                text: "MIME Type:"
                                font.bold: true
                            }
                            Label {
                                id: mimeTypeLabel
                                font.family: "monospace"
                                font.pointSize: 12
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "lightgray"
                    }

                    GridLayout {
                        columns: 2
                        columnSpacing: 20
                        rowSpacing: 10

                        Label { text: "Icon:"; font.bold: true }
                        Label { id: iconNameLabel }

                        Label { text: "Generic Icon:"; font.bold: true }
                        Label { id: genericIconLabel }

                        Label { text: "Glob Patterns:"; font.bold: true }
                        Label {
                            id: patternsLabel
                            wrapMode: Text.Wrap
                            Layout.fillWidth: true
                        }

                        Label { text: "Parent Types:"; font.bold: true }
                        Label {
                            id: parentsLabel
                            wrapMode: Text.Wrap
                            Layout.fillWidth: true
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: "lightgray"
                    }

                    Label {
                        text: "Associated Applications:"
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150
                        clip: true

                        model: XdgMimeAppsWrapper {
                            id: mimeApps
                        }

                        delegate: ItemDelegate {
                            width: ListView.view.width

                            contentItem: Row {
                                spacing: 10

                                Image {
                                    source: modelData.iconUrl
                                    sourceSize.width: 32
                                    sourceSize.height: 32
                                }

                                Column {
                                    Label {
                                        text: modelData.name
                                        font.bold: true
                                    }
                                    Label {
                                        text: modelData.comment
                                        font.pointSize: 9
                                        color: "gray"
                                    }
                                }
                            }

                            onClicked: {
                                // 打开文件
                                modelData.start([filePathField.text])
                            }
                        }

                        ScrollBar.vertical: ScrollBar {}
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Choose a file to detect its MIME type"
        onAccepted: {
            filePathField.text = selectedFile.toString().replace("file://", "")
            detectMimeType()
        }
    }

    function detectMimeType() {
        var path = filePathField.text
        var mime = mimeWrapper.mimeTypeForFile(path)

        if (mime) {
            resultPane.currentMime = mime
            resultPane.visible = true

            mimeTypeLabel.text = mime
            iconNameLabel.text = mimeWrapper.iconName(mime)
            genericIconLabel.text = mimeWrapper.genericIconName(mime)
            patternsLabel.text = mimeWrapper.globPatterns(mime).join(", ")
            parentsLabel.text = mimeWrapper.parentMimeTypes(mime).join(", ")

            mimeIcon.source = XdgIcon.fromTheme(mimeWrapper.iconName(mime))

            // 加载关联应用
            var apps = mimeApps.appsForMime(mime)
            // 注意：这里需要将 apps 设置到 ListView 的 model
        } else {
            resultPane.visible = false
        }
    }
}
```

### 示例 4: 文件打开方式选择器

**文件:** `OpenWithDialog.qml`

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

Dialog {
    id: root
    title: "Open With"
    modal: true
    standardButtons: Dialog.Open | Dialog.Cancel

    property string filePath: ""
    property string mimeType: ""

    XdgMimeAppsWrapper {
        id: mimeApps
    }

    XdgMimeTypeWrapper {
        id: mimeWrapper
    }

    onAboutToShow: {
        // 检测文件的 MIME 类型
        if (filePath) {
            mimeType = mimeWrapper.mimeTypeForFile(filePath)
            appsList.model = mimeApps.appsForMime(mimeType)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        Label {
            text: "Choose an application to open:"
            font.bold: true
        }

        Label {
            text: filePath
            font.family: "monospace"
            font.pointSize: 9
            elide: Text.ElideMiddle
            Layout.fillWidth: true
        }

        Label {
            text: "Type: " + mimeType
            font.pointSize: 9
            color: "gray"
        }

        ListView {
            id: appsList
            Layout.fillWidth: true
            Layout.preferredHeight: 300
            clip: true
            highlightFollowsCurrentItem: true
            highlight: Rectangle {
                color: "lightblue"
                radius: 5
            }

            delegate: ItemDelegate {
                width: ListView.view.width

                contentItem: RowLayout {
                    spacing: 10

                    Image {
                        source: modelData.iconUrl
                        sourceSize.width: 48
                        sourceSize.height: 48
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: modelData.name
                            font.bold: true
                        }

                        Label {
                            text: modelData.comment
                            font.pointSize: 9
                            color: "gray"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        Label {
                            text: "Command: " + modelData.value("Exec")
                            font.pointSize: 8
                            font.family: "monospace"
                            color: "darkgray"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }

                    RadioButton {
                        checked: index === appsList.currentIndex
                        onClicked: appsList.currentIndex = index
                    }
                }

                onClicked: {
                    appsList.currentIndex = index
                }

                onDoubleClicked: {
                    root.accept()
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }

        CheckBox {
            id: setDefaultCheck
            text: "Always use this application for " + mimeType
        }
    }

    onAccepted: {
        if (appsList.currentIndex >= 0) {
            var app = appsList.model[appsList.currentIndex]
            app.start([filePath])

            if (setDefaultCheck.checked) {
                // 设置为默认应用
                // 注意：libqtxdg 当前可能不直接支持设置默认应用
                console.log("Set as default:", app.name)
            }
        }
    }
}

// 使用示例
// openWithDialog.filePath = "/path/to/file"
// openWithDialog.open()
```

---

## 🚀 应用启动器示例

### 示例 5: 简单应用启动器

**文件:** `SimpleLauncher.qml`

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    visible: true
    width: 700
    height: 600
    title: "Application Launcher"

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "Search applications..."
            }

            ComboBox {
                id: categoryCombo
                model: ["All", "Development", "Graphics", "Internet",
                        "Multimedia", "Office", "Settings", "System", "Utility"]
            }

            ToolButton {
                icon.source: XdgIcon.fromTheme("view-refresh")
                onClicked: appsModel.reload()
                ToolTip.text: "Reload"
                ToolTip.visible: hovered
            }
        }
    }

    GridView {
        anchors.fill: parent
        anchors.margins: 20
        clip: true

        cellWidth: 120
        cellHeight: 140

        model: XdgApplicationsModel {
            id: appsModel
            category: categoryCombo.currentIndex === 0 ? "" : categoryCombo.currentText
            searchText: searchField.text
        }

        delegate: Rectangle {
            width: GridView.view.cellWidth - 10
            height: GridView.view.cellHeight - 10
            color: mouseArea.containsMouse ? "#f0f0f0" : "transparent"
            radius: 8
            border.color: mouseArea.containsMouse ? "#c0c0c0" : "transparent"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 8

                Image {
                    source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                    sourceSize.width: 64
                    sourceSize.height: 64
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: model.name || "(unnamed)"
                    font.pointSize: 10
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 2
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true

                onDoubleClicked: {
                    launchApp(model.desktopId)
                }

                onPressAndHold: {
                    contextMenu.appIndex = index
                    contextMenu.popup()
                }
            }

            ToolTip.text: model.comment || model.name
            ToolTip.visible: mouseArea.containsMouse && mouseArea.pressed === false
            ToolTip.delay: 1000
        }

        ScrollBar.vertical: ScrollBar {}
    }

    Menu {
        id: contextMenu
        property int appIndex: -1

        MenuItem {
            text: "Launch"
            icon.source: XdgIcon.fromTheme("system-run")
            onTriggered: {
                if (contextMenu.appIndex >= 0) {
                    var data = appsModel.get(contextMenu.appIndex)
                    launchApp(data.desktopId)
                }
            }
        }

        MenuItem {
            text: "Properties"
            icon.source: XdgIcon.fromTheme("document-properties")
            onTriggered: {
                if (contextMenu.appIndex >= 0) {
                    var data = appsModel.get(contextMenu.appIndex)
                    propertiesDialog.show(data)
                }
            }
        }
    }

    Dialog {
        id: propertiesDialog
        title: "Application Properties"
        modal: true
        anchors.centerIn: parent
        width: 400

        property var appData: null

        function show(data) {
            appData = data
            visible = true
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Image {
                source: propertiesDialog.appData
                    ? XdgIcon.fromTheme(propertiesDialog.appData.iconName)
                    : ""
                sourceSize.width: 64
                sourceSize.height: 64
                Layout.alignment: Qt.AlignHCenter
            }

            GridLayout {
                columns: 2
                columnSpacing: 15
                rowSpacing: 8

                Label { text: "Name:"; font.bold: true }
                Label {
                    text: propertiesDialog.appData ? propertiesDialog.appData.name : ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                Label { text: "Description:"; font.bold: true; Layout.alignment: Qt.AlignTop }
                Label {
                    text: propertiesDialog.appData ? propertiesDialog.appData.comment : ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                Label { text: "Command:"; font.bold: true; Layout.alignment: Qt.AlignTop }
                TextEdit {
                    text: propertiesDialog.appData ? propertiesDialog.appData.exec : ""
                    font.family: "monospace"
                    font.pointSize: 9
                    readOnly: true
                    selectByMouse: true
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                Label { text: "Categories:"; font.bold: true; Layout.alignment: Qt.AlignTop }
                Label {
                    text: propertiesDialog.appData && propertiesDialog.appData.categories
                        ? propertiesDialog.appData.categories.join(", ")
                        : ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }
            }
        }

        standardButtons: Dialog.Close
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            Label {
                text: appsModel.count + " applications"
                Layout.fillWidth: true
            }

            Label {
                text: appsModel.lastError
                color: "red"
                visible: appsModel.lastError !== ""
            }
        }
    }

    function launchApp(desktopId) {
        var desktop = Qt.createQmlObject(
            'import org.lxqt.qtxdg 6.0; XdgDesktopFile {}',
            root
        )
        desktop.fileName = desktopId
        if (desktop.load()) {
            if (desktop.startDetached()) {
                console.log("Launched:", desktopId)
            } else {
                console.error("Failed to launch:", desktopId)
            }
        } else {
            console.error("Failed to load desktop file:", desktopId)
        }
        desktop.destroy()
    }
}
```

---

## ⚙️ 系统设置示例

### 示例 6: 自动启动管理器

**文件:** `AutostartManager.qml`

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    visible: true
    width: 700
    height: 500
    title: "Autostart Manager"

    XdgAutoStartModel {
        id: autostartModel
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Label {
            text: "Startup Applications"
            font.bold: true
            font.pointSize: 16
        }

        Label {
            text: "These applications will start automatically when you log in"
            font.pointSize: 10
            color: "gray"
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "lightgray"
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 5

            model: autostartModel

            delegate: ItemDelegate {
                width: ListView.view.width
                padding: 15

                background: Rectangle {
                    color: enabled ? "white" : "#f5f5f5"
                    border.color: "#e0e0e0"
                    border.width: 1
                    radius: 8
                }

                contentItem: RowLayout {
                    spacing: 15

                    // 图标
                    Rectangle {
                        width: 56
                        height: 56
                        color: "#f0f0f0"
                        radius: 8

                        Image {
                            anchors.centerIn: parent
                            source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                            sourceSize.width: 48
                            sourceSize.height: 48
                        }
                    }

                    // 应用信息
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Label {
                            text: model.name
                            font.bold: true
                            font.pointSize: 12
                        }

                        Label {
                            text: model.comment
                            font.pointSize: 10
                            color: "gray"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            wrapMode: Text.Wrap
                            maximumLineCount: 2
                        }

                        Label {
                            text: model.enabled ? "✓ Enabled" : "✗ Disabled"
                            font.pointSize: 9
                            color: model.enabled ? "green" : "red"
                        }
                    }

                    // 控制按钮
                    ColumnLayout {
                        spacing: 8

                        Switch {
                            id: enableSwitch
                            checked: model.enabled

                            onToggled: {
                                if (!autostartModel.setEnabled(index, checked)) {
                                    // 操作失败，恢复状态
                                    checked = !checked
                                    errorDialog.errorText = autostartModel.lastError
                                    errorDialog.open()
                                } else {
                                    statusLabel.text = checked
                                        ? "✓ Enabled " + model.name
                                        : "✗ Disabled " + model.name
                                    statusTimer.restart()
                                }
                            }
                        }

                        Button {
                            text: "Info"
                            flat: true
                            onClicked: {
                                infoDialog.show(autostartModel.get(index))
                            }
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {}

            // 空状态
            Label {
                anchors.centerIn: parent
                text: "No autostart applications"
                font.pointSize: 14
                color: "gray"
                visible: autostartModel.count === 0
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "lightgray"
        }

        // 状态栏
        RowLayout {
            Layout.fillWidth: true

            Label {
                id: statusLabel
                text: autostartModel.count + " autostart applications"
                Layout.fillWidth: true
            }

            Timer {
                id: statusTimer
                interval: 3000
                onTriggered: {
                    statusLabel.text = autostartModel.count + " autostart applications"
                }
            }

            Button {
                text: "Reload"
                icon.source: XdgIcon.fromTheme("view-refresh")
                flat: true
                onClicked: {
                    autostartModel.reload()
                    statusLabel.text = "Reloaded"
                    statusTimer.restart()
                }
            }

            Button {
                text: "Add"
                icon.source: XdgIcon.fromTheme("list-add")
                flat: true
                onClicked: {
                    // 打开添加对话框
                    addDialog.open()
                }
            }
        }
    }

    // 信息对话框
    Dialog {
        id: infoDialog
        title: "Application Information"
        modal: true
        anchors.centerIn: parent
        width: 500

        property var appData: null

        function show(data) {
            appData = data
            visible = true
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 15

            Row {
                spacing: 15

                Image {
                    source: infoDialog.appData
                        ? XdgIcon.fromTheme(infoDialog.appData.iconName)
                        : ""
                    sourceSize.width: 64
                    sourceSize.height: 64
                }

                ColumnLayout {
                    Label {
                        text: infoDialog.appData ? infoDialog.appData.name : ""
                        font.bold: true
                        font.pointSize: 14
                    }

                    Label {
                        text: infoDialog.appData
                            ? (infoDialog.appData.enabled ? "Enabled" : "Disabled")
                            : ""
                        color: infoDialog.appData && infoDialog.appData.enabled
                            ? "green"
                            : "red"
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "lightgray"
            }

            GridLayout {
                columns: 2
                columnSpacing: 15
                rowSpacing: 10

                Label { text: "Description:"; font.bold: true }
                Label {
                    text: infoDialog.appData ? infoDialog.appData.comment : ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                Label { text: "Desktop ID:"; font.bold: true }
                Label {
                    text: infoDialog.appData ? infoDialog.appData.desktopId : ""
                    font.family: "monospace"
                    font.pointSize: 9
                }

                Label { text: "Location:"; font.bold: true; Layout.alignment: Qt.AlignTop }
                TextEdit {
                    text: infoDialog.appData
                        ? XdgAutoStart.autostartHome() + "/" + infoDialog.appData.desktopId
                        : ""
                    font.family: "monospace"
                    font.pointSize: 9
                    readOnly: true
                    selectByMouse: true
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }
            }
        }

        standardButtons: Dialog.Close
    }

    // 错误对话框
    Dialog {
        id: errorDialog
        title: "Error"
        modal: true
        anchors.centerIn: parent

        property alias errorText: errorLabel.text

        Label {
            id: errorLabel
            wrapMode: Text.Wrap
            width: 300
        }

        standardButtons: Dialog.Ok
    }

    // 添加对话框
    Dialog {
        id: addDialog
        title: "Add Autostart Application"
        modal: true
        anchors.centerIn: parent
        width: 500

        Label {
            text: "This feature requires manually creating a .desktop file in:\n" +
                  XdgAutoStart.autostartHome()
            wrapMode: Text.Wrap
            width: parent.width
        }

        standardButtons: Dialog.Ok
    }
}
```

---

## 🔧 实用工具示例

### 示例 7: 系统信息查看器

**文件:** `SystemInfo.qml`

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    visible: true
    width: 600
    height: 500
    title: "System Information"

    TabBar {
        id: tabBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        TabButton { text: "Directories" }
        TabButton { text: "Applications" }
        TabButton { text: "Icons" }
        TabButton { text: "Default Apps" }
    }

    StackLayout {
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        currentIndex: tabBar.currentIndex

        // Tab 0: XDG Directories
        ScrollView {
            GridLayout {
                width: parent.width
                columns: 2
                columnSpacing: 20
                rowSpacing: 10

                Label { text: "Config Home:"; font.bold: true }
                Label { text: XdgDirs.configHome(); selectByMouse: true }

                Label { text: "Data Home:"; font.bold: true }
                Label { text: XdgDirs.dataHome(); selectByMouse: true }

                Label { text: "Cache Home:"; font.bold: true }
                Label { text: XdgDirs.cacheHome(); selectByMouse: true }

                Label { text: "Runtime Dir:"; font.bold: true }
                Label { text: XdgDirs.runtimeDir(); selectByMouse: true }

                Label { text: "Autostart:"; font.bold: true }
                Label { text: XdgDirs.autostartHome(); selectByMouse: true }

                Rectangle { Layout.fillWidth: true; height: 1; color: "lightgray"; Layout.columnSpan: 2 }

                Label { text: "Desktop:"; font.bold: true }
                Label { text: XdgDirs.userDir(XdgDirs.Desktop); selectByMouse: true }

                Label { text: "Documents:"; font.bold: true }
                Label { text: XdgDirs.userDir(XdgDirs.Documents); selectByMouse: true }

                Label { text: "Downloads:"; font.bold: true }
                Label { text: XdgDirs.userDir(XdgDirs.Download); selectByMouse: true }

                Label { text: "Music:"; font.bold: true }
                Label { text: XdgDirs.userDir(XdgDirs.Music); selectByMouse: true }

                Label { text: "Pictures:"; font.bold: true }
                Label { text: XdgDirs.userDir(XdgDirs.Pictures); selectByMouse: true }

                Label { text: "Videos:"; font.bold: true }
                Label { text: XdgDirs.userDir(XdgDirs.Videos); selectByMouse: true }
            }
        }

        // Tab 1: Applications Statistics
        ColumnLayout {
            spacing: 15

            XdgApplicationsModel {
                id: appsModel
            }

            Label {
                text: "Application Statistics"
                font.bold: true
                font.pointSize: 14
            }

            GridLayout {
                columns: 2
                columnSpacing: 20
                rowSpacing: 10

                Label { text: "Total Applications:"; font.bold: true }
                Label { text: appsModel.count }

                Label { text: "Development:"; font.bold: true }
                Label {
                    text: {
                        var devModel = Qt.createQmlObject(
                            'import org.lxqt.qtxdg 6.0; XdgApplicationsModel { category: "Development" }',
                            parent
                        )
                        return devModel.count
                    }
                }

                Label { text: "Graphics:"; font.bold: true }
                Label {
                    text: {
                        var gfxModel = Qt.createQmlObject(
                            'import org.lxqt.qtxdg 6.0; XdgApplicationsModel { category: "Graphics" }',
                            parent
                        )
                        return gfxModel.count
                    }
                }

                Label { text: "Network:"; font.bold: true }
                Label {
                    text: {
                        var netModel = Qt.createQmlObject(
                            'import org.lxqt.qtxdg 6.0; XdgApplicationsModel { category: "Network" }',
                            parent
                        )
                        return netModel.count
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }

        // Tab 2: Icon Theme
        ColumnLayout {
            spacing: 15

            Label {
                text: "Icon Theme Information"
                font.bold: true
                font.pointSize: 14
            }

            GridLayout {
                columns: 2
                columnSpacing: 20
                rowSpacing: 10

                Label { text: "Theme Name:"; font.bold: true }
                Label { text: XdgIcon.themeName }

                Label { text: "Follow Color Scheme:"; font.bold: true }
                Label { text: XdgIcon.followColorScheme ? "Yes" : "No" }
            }

            Label {
                text: "Sample Icons:"
                font.bold: true
            }

            GridLayout {
                columns: 4
                columnSpacing: 10
                rowSpacing: 10

                Repeater {
                    model: ["folder", "document-open", "edit-copy", "system-run"]

                    Column {
                        spacing: 5

                        Image {
                            source: XdgIcon.fromTheme(modelData)
                            sourceSize.width: 48
                            sourceSize.height: 48
                        }

                        Label {
                            text: modelData
                            font.pointSize: 8
                        }
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }

        // Tab 3: Default Applications
        ScrollView {
            GridLayout {
                width: parent.width
                columns: 2
                columnSpacing: 20
                rowSpacing: 15

                Label { text: "Web Browser:"; font.bold: true }
                RowLayout {
                    Label {
                        text: XdgDefaultApps.webBrowser()
                    }
                    Button {
                        text: "Launch"
                        flat: true
                        onClicked: launchDefault(XdgDefaultApps.webBrowser())
                    }
                }

                Label { text: "File Manager:"; font.bold: true }
                RowLayout {
                    Label {
                        text: XdgDefaultApps.fileManager()
                    }
                    Button {
                        text: "Launch"
                        flat: true
                        onClicked: launchDefault(XdgDefaultApps.fileManager())
                    }
                }

                Label { text: "Terminal:"; font.bold: true }
                RowLayout {
                    Label {
                        text: XdgDefaultApps.terminal()
                    }
                    Button {
                        text: "Launch"
                        flat: true
                        onClicked: launchDefault(XdgDefaultApps.terminal())
                    }
                }

                Label { text: "Email Client:"; font.bold: true }
                RowLayout {
                    Label {
                        text: XdgDefaultApps.emailClient()
                    }
                    Button {
                        text: "Launch"
                        flat: true
                        onClicked: launchDefault(XdgDefaultApps.emailClient())
                    }
                }
            }
        }
    }

    function launchDefault(desktopFileName) {
        var desktop = Qt.createQmlObject(
            'import org.lxqt.qtxdg 6.0; XdgDesktopFile {}',
            root
        )
        desktop.fileName = "/usr/share/applications/" + desktopFileName
        if (desktop.load()) {
            desktop.startDetached()
        }
        desktop.destroy()
    }
}
```

---

## 📚 使用说明

### 如何运行这些示例

1. **安装 libqtxdg**
   ```bash
   cd libqtxdg/build
   cmake .. -DBUILD_QML_PLUGIN=ON
   make
   sudo make install
   ```

2. **运行示例**
   ```bash
   # 使用 qml 命令
   qml HelloXdg.qml

   # 或者使用自定义的 QML 引擎
   QML_IMPORT_PATH=/path/to/build/qml qml YourExample.qml
   ```

3. **在项目中使用**
   - 将示例代码复制到您的项目
   - 确保 QML 导入路径配置正确
   - 根据需要修改和扩展

### 许可证

这些示例代码使用 LGPL 2.1+ 许可证发布，与 libqtxdg 项目相同。

---

*文档版本: 1.0*
*最后更新: 2025-11-20*
*维护者: LXQt Team*
