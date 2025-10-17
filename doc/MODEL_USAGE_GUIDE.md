# libqtxdg Model 类使用指南

> **适用版本:** libqtxdg 4.2.0+
> **Qt 版本:** 6.6.0+

## 📋 目录

1. [Model 架构概述](#model-架构概述)
2. [XdgSimpleListModel 基类](#xdgsimplelistmodel-基类)
3. [XdgApplicationsModel 详解](#xdgapplicationsmodel-详解)
4. [XdgAutoStartModel 详解](#xdgautostartmodel-详解)
5. [XdgMenuTreeModel 详解](#xdgmenutreemodel-详解)
6. [高级用法](#高级用法)
7. [性能优化](#性能优化)
8. [最佳实践](#最佳实践)

---

## 🏗️ Model 架构概述

libqtxdg 的 Model 类采用统一的架构设计，所有模型都继承自 `XdgSimpleListModel` 基类。

### 架构图

```
QAbstractListModel (Qt)
        ↓
XdgSimpleListModel (基类)
        ↓
        ├── XdgApplicationsModel (应用程序列表)
        ├── XdgAutoStartModel (自动启动管理)
        └── XdgMenuTreeModel (菜单树结构)
```

### 设计原则

1. **统一接口**: 所有模型提供一致的 API
2. **延迟加载**: 按需加载数据，提升性能
3. **错误处理**: 统一的错误报告机制
4. **信号通知**: 数据变化自动通知 UI

---

## 🔧 XdgSimpleListModel 基类

### 概述

`XdgSimpleListModel` 是所有 Model 类的基类，提供：
- 统一的错误处理
- 角色名称管理
- 数据更新通知

### 公共属性

```qml
// 所有继承的 Model 都有这些属性
property int count                    // 数据项数量（只读）
property string lastError            // 最后的错误信息（只读）
```

### 公共信号

```qml
signal countChanged()               // 数据数量变化时发出
signal errorOccurred(string error)  // 发生错误时发出
```

### 错误处理模式

```qml
XdgApplicationsModel {
    id: appsModel

    onErrorOccurred: function(error) {
        console.error("Model error:", error)
        errorDialog.text = error
        errorDialog.open()
    }
}

Dialog {
    id: errorDialog
    property alias text: errorLabel.text

    Label {
        id: errorLabel
    }
}
```

---

## 📱 XdgApplicationsModel 详解

### 功能说明

`XdgApplicationsModel` 提供已安装应用程序的可过滤列表，支持：
- 按分类过滤
- 文本搜索
- 延迟加载
- 动态刷新

### 完整 API

#### 属性

```qml
XdgApplicationsModel {
    // 可读写属性
    property string category: ""        // 分类过滤器（空字符串表示全部）
    property string searchText: ""      // 搜索文本

    // 只读属性
    readonly property int count         // 应用程序数量
    readonly property string lastError  // 最后的错误信息
}
```

#### 数据角色

每个数据项包含以下角色：

| 角色名 | 类型 | 描述 | 示例 |
|--------|------|------|------|
| `desktopId` | string | 桌面文件 ID | `firefox.desktop` |
| `name` | string | 应用程序名称 | `Firefox Web Browser` |
| `iconName` | string | 图标名称 | `firefox` |
| `exec` | string | 执行命令 | `/usr/bin/firefox %u` |
| `comment` | string | 应用程序描述 | `Browse the World Wide Web` |
| `categories` | stringlist | 分类列表 | `["Network", "WebBrowser"]` |
| `mimeTypes` | stringlist | 支持的 MIME 类型 | `["text/html", "text/xml"]` |

#### 方法

```qml
// 重新加载数据
void reload()

// 获取指定行的完整数据
variantmap get(int row)
```

### 实战示例

#### 示例 1: 基础应用列表

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Page {
    title: "Applications"

    ListView {
        anchors.fill: parent

        model: XdgApplicationsModel {
            id: appsModel
        }

        delegate: ItemDelegate {
            width: ListView.view.width
            text: model.name
            icon.source: XdgIcon.fromTheme(model.iconName)

            onClicked: {
                console.log("Clicked:", model.name)
                console.log("Exec:", model.exec)
                console.log("Categories:", model.categories)
            }
        }

        ScrollBar.vertical: ScrollBar {}
    }
}
```

#### 示例 2: 搜索和过滤

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

Page {
    title: "Filtered Applications"

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        anchors.margins: 10

        // 搜索框
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: "Search by name or description..."

            // 实时搜索
            onTextChanged: appsModel.searchText = text
        }

        // 分类选择器
        ComboBox {
            id: categoryCombo
            Layout.fillWidth: true
            model: [
                {text: "All Applications", value: ""},
                {text: "Development", value: "Development"},
                {text: "Graphics", value: "Graphics"},
                {text: "Internet", value: "Network"},
                {text: "Multimedia", value: "AudioVideo"},
                {text: "Office", value: "Office"},
                {text: "Settings", value: "Settings"},
                {text: "System", value: "System"},
                {text: "Utilities", value: "Utility"}
            ]
            textRole: "text"

            onCurrentIndexChanged: {
                appsModel.category = model[currentIndex].value
            }
        }

        // 结果统计
        Label {
            text: "Found " + appsModel.count + " applications"
            font.italic: true
        }

        // 应用列表
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: XdgApplicationsModel {
                id: appsModel
            }

            delegate: ItemDelegate {
                width: ListView.view.width

                contentItem: RowLayout {
                    spacing: 10

                    Image {
                        source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                        sourceSize.width: 32
                        sourceSize.height: 32
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: model.name
                            font.bold: true
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        Label {
                            text: model.comment
                            font.pointSize: 9
                            color: "gray"
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                    }
                }

                onDoubleClicked: {
                    // 启动应用程序
                    var desktop = Qt.createQmlObject(
                        'import org.lxqt.qtxdg 6.0; XdgDesktopFile {}',
                        parent
                    )
                    desktop.fileName = model.desktopId
                    if (desktop.load()) {
                        desktop.startDetached()
                    }
                    desktop.destroy()
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }

        // 操作按钮
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "Reload"
                icon.source: XdgIcon.fromTheme("view-refresh")
                onClicked: appsModel.reload()
            }

            Button {
                text: "Clear Filters"
                enabled: searchField.text !== "" || appsModel.category !== ""
                onClicked: {
                    searchField.text = ""
                    categoryCombo.currentIndex = 0
                }
            }

            Item { Layout.fillWidth: true }

            Label {
                text: appsModel.lastError
                color: "red"
                visible: appsModel.lastError !== ""
            }
        }
    }
}
```

#### 示例 3: 应用网格视图

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

Page {
    title: "Application Grid"

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "Category:"
            }

            ComboBox {
                id: categoryCombo
                model: ["All", "Development", "Graphics", "Network", "Office"]
                onCurrentTextChanged: {
                    appsModel.category = currentIndex === 0 ? "" : currentText
                }
            }

            Item { Layout.fillWidth: true }

            ToolButton {
                icon.source: XdgIcon.fromTheme("view-list")
                onClicked: stackView.push("ListView.qml")
            }
        }
    }

    GridView {
        anchors.fill: parent
        anchors.margins: 10
        clip: true

        cellWidth: 120
        cellHeight: 120

        model: XdgApplicationsModel {
            id: appsModel
        }

        delegate: Rectangle {
            width: GridView.view.cellWidth - 10
            height: GridView.view.cellHeight - 10
            color: mouseArea.containsMouse ? "#e0e0e0" : "transparent"
            radius: 5

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                Image {
                    source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                    sourceSize.width: 64
                    sourceSize.height: 64
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: model.name
                    font.pointSize: 9
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
                    // 启动应用
                    var desktop = Qt.createQmlObject(
                        'import org.lxqt.qtxdg 6.0; XdgDesktopFile {}',
                        parent
                    )
                    desktop.fileName = model.desktopId
                    if (desktop.load()) {
                        desktop.startDetached()
                    }
                    desktop.destroy()
                }

                onClicked: {
                    // 显示详情
                    console.log("Selected:", model.name)
                }
            }

            ToolTip.text: model.comment
            ToolTip.visible: mouseArea.containsMouse
            ToolTip.delay: 500
        }

        ScrollBar.vertical: ScrollBar {}
    }
}
```

---

## 🚀 XdgAutoStartModel 详解

### 功能说明

管理系统自动启动的应用程序，支持：
- 列举自动启动项
- 启用/禁用自动启动
- 动态刷新

### 完整 API

#### 属性

```qml
XdgAutoStartModel {
    // 只读属性
    readonly property int count         // 自动启动项数量
    readonly property string lastError  // 最后的错误信息
}
```

#### 数据角色

| 角色名 | 类型 | 描述 |
|--------|------|------|
| `desktopId` | string | 桌面文件 ID |
| `name` | string | 应用程序名称 |
| `iconName` | string | 图标名称 |
| `comment` | string | 描述 |
| `enabled` | bool | 是否启用 |

#### 方法

```qml
// 设置自动启动项的启用状态
bool setEnabled(int row, bool enabled)

// 重新加载数据
void reload()

// 获取指定行的完整数据
variantmap get(int row)
```

### 实战示例

#### 示例 1: 自动启动管理器

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

Page {
    title: "Auto-start Manager"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Label {
            text: "Startup Applications"
            font.bold: true
            font.pointSize: 14
        }

        Label {
            text: "These applications will start automatically when you log in"
            font.pointSize: 9
            color: "gray"
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: XdgAutoStartModel {
                id: autostartModel
            }

            delegate: ItemDelegate {
                width: ListView.view.width
                padding: 10

                contentItem: RowLayout {
                    spacing: 10

                    // 图标
                    Image {
                        source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                        sourceSize.width: 48
                        sourceSize.height: 48
                    }

                    // 应用信息
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: model.name
                            font.bold: true
                        }

                        Label {
                            text: model.comment
                            font.pointSize: 9
                            color: "gray"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        Label {
                            text: "Desktop ID: " + model.desktopId
                            font.pointSize: 8
                            font.family: "monospace"
                            color: "darkgray"
                        }
                    }

                    // 启用开关
                    Switch {
                        id: enableSwitch
                        checked: model.enabled

                        onToggled: {
                            if (!autostartModel.setEnabled(index, checked)) {
                                // 操作失败，恢复状态
                                checked = !checked
                                errorDialog.text = autostartModel.lastError
                                errorDialog.open()
                            } else {
                                statusLabel.text = checked
                                    ? "Enabled " + model.name
                                    : "Disabled " + model.name
                            }
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }

        // 状态栏
        RowLayout {
            Layout.fillWidth: true

            Label {
                id: statusLabel
                Layout.fillWidth: true
                text: autostartModel.count + " startup applications"
            }

            Button {
                text: "Reload"
                icon.source: XdgIcon.fromTheme("view-refresh")
                onClicked: autostartModel.reload()
            }
        }
    }

    // 错误对话框
    Dialog {
        id: errorDialog
        title: "Error"
        property alias text: errorLabel.text

        Label {
            id: errorLabel
        }

        standardButtons: Dialog.Ok
    }
}
```

#### 示例 2: 带确认的自动启动管理

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

Page {
    title: "Auto-start with Confirmation"

    XdgAutoStartModel {
        id: autostartModel
    }

    ListView {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5

        model: autostartModel

        delegate: ItemDelegate {
            width: ListView.view.width
            highlighted: ListView.isCurrentItem

            contentItem: RowLayout {
                spacing: 10

                Image {
                    source: XdgIcon.fromTheme(model.iconName)
                    sourceSize.width: 32
                    sourceSize.height: 32
                }

                ColumnLayout {
                    Layout.fillWidth: true

                    Label {
                        text: model.name
                        font.bold: true
                    }

                    Label {
                        text: model.enabled ? "Enabled" : "Disabled"
                        font.pointSize: 9
                        color: model.enabled ? "green" : "gray"
                    }
                }

                Button {
                    text: model.enabled ? "Disable" : "Enable"
                    onClicked: {
                        confirmDialog.currentIndex = index
                        confirmDialog.currentName = model.name
                        confirmDialog.newState = !model.enabled
                        confirmDialog.open()
                    }
                }
            }
        }
    }

    // 确认对话框
    Dialog {
        id: confirmDialog
        title: "Confirm Change"
        modal: true
        anchors.centerIn: parent

        property int currentIndex
        property string currentName
        property bool newState

        Label {
            text: newState
                ? "Enable '" + currentName + "' at startup?"
                : "Disable '" + currentName + "' from startup?"
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            if (autostartModel.setEnabled(currentIndex, newState)) {
                console.log("Changed autostart for:", currentName)
            } else {
                errorDialog.text = autostartModel.lastError
                errorDialog.open()
            }
        }
    }

    Dialog {
        id: errorDialog
        title: "Error"
        property alias text: errorLabel.text

        Label {
            id: errorLabel
        }

        standardButtons: Dialog.Ok
    }
}
```

---

## 🌲 XdgMenuTreeModel 详解

### 功能说明

提供层次化的应用程序菜单结构，支持：
- 加载 XDG 菜单文件
- 分类和应用程序的树形结构
- 自定义桌面环境

### 完整 API

#### 属性

```qml
XdgMenuTreeModel {
    // 可读写属性
    property string menuFile: ""            // 菜单文件路径
    property stringlist environments: []    // 桌面环境列表

    // 只读属性
    readonly property int count              // 根级项目数量
    readonly property string lastError       // 最后的错误信息
}
```

#### 数据角色

| 角色名 | 类型 | 描述 |
|--------|------|------|
| `name` | string | 名称（分类或应用） |
| `iconName` | string | 图标名称 |
| `isCategory` | bool | 是否是分类节点 |
| `desktopFile` | object | 桌面文件对象（仅应用程序） |

#### 方法

```qml
// 加载指定菜单文件
bool loadMenuFile(string fileName)

// 加载默认菜单
bool loadDefaultMenu()

// 重新加载
void reload()

// 获取指定行的完整数据
variantmap get(int row)
```

### 实战示例

#### 示例 1: 双列菜单浏览器

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

Page {
    title: "Menu Browser"

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // 左侧：分类列表
        Pane {
            SplitView.preferredWidth: 250
            SplitView.minimumWidth: 200

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "Categories"
                    font.bold: true
                }

                ListView {
                    id: categoryView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    highlightFollowsCurrentItem: true
                    highlight: Rectangle {
                        color: "lightblue"
                        radius: 5
                    }

                    model: XdgMenuTreeModel {
                        id: menuModel
                        environments: ["LXQt", "X-LXQT"]

                        Component.onCompleted: {
                            if (!loadDefaultMenu()) {
                                console.error("Failed to load menu:", lastError)
                            }
                        }
                    }

                    delegate: ItemDelegate {
                        width: ListView.view.width
                        text: model.name

                        contentItem: Row {
                            spacing: 10

                            Image {
                                source: XdgIcon.fromTheme(model.iconName || "folder")
                                sourceSize.width: 24
                                sourceSize.height: 24
                            }

                            Label {
                                text: model.name
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        onClicked: {
                            ListView.view.currentIndex = index
                            var data = menuModel.get(index)
                            // 这里需要加载该分类下的应用
                            // 实际实现中，可能需要使用 XdgApplicationsModel
                        }
                    }
                }

                Button {
                    Layout.fillWidth: true
                    text: "Reload Menu"
                    icon.source: XdgIcon.fromTheme("view-refresh")
                    onClicked: menuModel.reload()
                }
            }
        }

        // 右侧：应用列表
        Pane {
            SplitView.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "Applications"
                    font.bold: true
                }

                ListView {
                    id: appsView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    // 这里的 model 应该根据选中的分类动态填充
                    // 实际实现中使用 XdgApplicationsModel

                    delegate: ItemDelegate {
                        width: ListView.view.width

                        contentItem: RowLayout {
                            spacing: 10

                            Image {
                                source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                                sourceSize.width: 48
                                sourceSize.height: 48
                            }

                            ColumnLayout {
                                Layout.fillWidth: true

                                Label {
                                    text: model.name
                                    font.bold: true
                                }

                                Label {
                                    text: model.desktopFile ? model.desktopFile.comment : ""
                                    font.pointSize: 9
                                    color: "gray"
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                            }

                            Button {
                                text: "Launch"
                                icon.source: XdgIcon.fromTheme("system-run")
                                onClicked: {
                                    if (model.desktopFile) {
                                        model.desktopFile.startDetached()
                                    }
                                }
                            }
                        }

                        onDoubleClicked: {
                            if (model.desktopFile) {
                                model.desktopFile.startDetached()
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }
    }
}
```

---

## 🚀 高级用法

### 1. 组合多个 Model

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

TabBar {
    id: tabBar

    TabButton { text: "All Apps" }
    TabButton { text: "Development" }
    TabButton { text: "Autostart" }
}

StackLayout {
    currentIndex: tabBar.currentIndex

    // Tab 0: 所有应用
    ListView {
        model: XdgApplicationsModel {}
        delegate: /* ... */
    }

    // Tab 1: 开发分类
    ListView {
        model: XdgApplicationsModel {
            category: "Development"
        }
        delegate: /* ... */
    }

    // Tab 2: 自动启动
    ListView {
        model: XdgAutoStartModel {}
        delegate: /* ... */
    }
}
```

### 2. 动态过滤和排序

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0
import QtQml.Models

XdgApplicationsModel {
    id: sourceModel
}

// 使用 DelegateModel 添加排序
DelegateModel {
    id: visualModel
    model: sourceModel

    items.onChanged: {
        // 自定义排序逻辑
        items.sort(function(a, b) {
            return a.model.name.localeCompare(b.model.name)
        })
    }

    delegate: ItemDelegate {
        text: model.name
    }
}

ListView {
    model: visualModel
}
```

### 3. 自定义缓存策略

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Item {
    // 缓存加载的数据
    property var cachedApps: []
    property bool cacheValid: false

    XdgApplicationsModel {
        id: appsModel

        onCountChanged: {
            if (count > 0 && !cacheValid) {
                // 缓存数据
                cachedApps = []
                for (var i = 0; i < count; i++) {
                    cachedApps.push(get(i))
                }
                cacheValid = true
            }
        }
    }

    function invalidateCache() {
        cacheValid = false
        appsModel.reload()
    }

    // 使用缓存的数据
    Repeater {
        model: cachedApps
        delegate: ItemDelegate {
            text: modelData.name
        }
    }
}
```

---

## ⚡ 性能优化

### 1. 延迟加载

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Page {
    // 延迟加载模型
    property bool modelLoaded: false

    XdgApplicationsModel {
        id: appsModel
    }

    ListView {
        model: modelLoaded ? appsModel : null

        // 视图可见时才加载
        Component.onCompleted: {
            Qt.callLater(function() {
                modelLoaded = true
            })
        }
    }
}
```

### 2. ListView 优化

```qml
ListView {
    // 启用项目重用
    reuseItems: true

    // 增加缓存缓冲区
    cacheBuffer: 500

    // 使用异步加载
    asynchronous: true

    model: XdgApplicationsModel {
        id: appsModel
    }

    delegate: ItemDelegate {
        // 简化 delegate 复杂度
        text: model.name
        icon.source: XdgIcon.fromTheme(model.iconName)
    }
}
```

### 3. 搜索防抖

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

TextField {
    id: searchField
    placeholderText: "Search..."

    // 防抖定时器
    Timer {
        id: searchTimer
        interval: 300
        onTriggered: appsModel.searchText = searchField.text
    }

    onTextChanged: {
        searchTimer.restart()
    }
}

XdgApplicationsModel {
    id: appsModel
}
```

---

## 💡 最佳实践

### 1. 错误处理

```qml
XdgApplicationsModel {
    id: appsModel

    // 全局错误处理
    onErrorOccurred: function(error) {
        console.error("Model error:", error)
        errorBar.show(error)
    }
}

Rectangle {
    id: errorBar
    visible: false
    color: "#ffebee"
    height: 40

    function show(message) {
        errorLabel.text = message
        visible = true
        hideTimer.restart()
    }

    Label {
        id: errorLabel
        anchors.centerIn: parent
        color: "red"
    }

    Timer {
        id: hideTimer
        interval: 3000
        onTriggered: errorBar.visible = false
    }
}
```

### 2. 加载指示器

```qml
StackView {
    id: stackView

    // 加载指示器
    BusyIndicator {
        anchors.centerIn: parent
        running: appsModel.count === 0 && !loadingComplete
        visible: running
    }

    property bool loadingComplete: false

    XdgApplicationsModel {
        id: appsModel

        onCountChanged: {
            if (count > 0) {
                stackView.loadingComplete = true
            }
        }
    }
}
```

### 3. 数据验证

```qml
ListView {
    model: XdgApplicationsModel {
        id: appsModel
    }

    delegate: ItemDelegate {
        // 验证数据有效性
        visible: model.name && model.name !== ""

        text: model.name || "(Unknown)"
        icon.source: model.iconName
            ? XdgIcon.fromTheme(model.iconName)
            : "image://theme/application-x-executable"

        onClicked: {
            // 检查数据完整性
            var data = appsModel.get(index)
            if (data && data.desktopId) {
                // 启动应用
            } else {
                console.warn("Invalid application data at index:", index)
            }
        }
    }
}
```

### 4. 内存管理

```qml
Component {
    id: pageComponent

    Page {
        XdgApplicationsModel {
            id: appsModel
        }

        // 页面销毁时清理
        Component.onDestruction: {
            appsModel.destroy()
        }
    }
}

// 使用
StackView {
    id: stackView

    pushEnter: Transition {}
    pushExit: Transition {}
    popEnter: Transition {}
    popExit: Transition {
        // 退出动画完成后销毁
        PropertyAnimation {
            property: "opacity"
            from: 1
            to: 0
        }
    }
}
```

---

## 📚 参考资源

- [QML API 完整文档](./QML_API_GUIDE.md)
- [Qt ListView 文档](https://doc.qt.io/qt-6/qml-qtquick-listview.html)
- [Qt Model/View 编程](https://doc.qt.io/qt-6/model-view-programming.html)

---

*文档版本: 1.0*
*最后更新: 2025-11-20*
*维护者: LXQt Team*
