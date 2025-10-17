# libqtxdg QML API 使用文档

> **版本:** 4.2.0
> **Qt 版本:** 6.6.0+
> **模块名称:** `org.lxqt.qtxdg 6.0`

## 📋 目录

1. [快速开始](#快速开始)
2. [基础组件](#基础组件)
3. [Phase 1 - MIME 和应用管理](#phase-1---mime-和应用管理)
4. [Phase 2 - 数据模型](#phase-2---数据模型)
5. [完整示例](#完整示例)
6. [常见问题](#常见问题)

---

## 🚀 快速开始

### 安装和导入

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    // 您的应用程序代码
}
```

### QML 导入路径设置

**开发环境：**
```bash
QML_IMPORT_PATH=/path/to/build/qml ./your-app
```

**C++ 主程序配置：**
```cpp
#include <QQmlApplicationEngine>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // 添加 QML 导入路径
    engine.addImportPath("/path/to/qml/modules");

    engine.load(QUrl("qrc:/main.qml"));
    return app.exec();
}
```

---

## 📦 基础组件

### XdgDirs - XDG 目录管理

提供对 XDG 标准目录的访问。

#### 属性

所有属性都是只读的：

| 属性 | 类型 | 描述 | 示例值 |
|-----|------|------|--------|
| `dataHome` | string | 用户数据目录 | `~/.local/share` |
| `configHome` | string | 用户配置目录 | `~/.config` |
| `cacheHome` | string | 用户缓存目录 | `~/.cache` |
| `runtimeDir` | string | 运行时目录 | `/run/user/1000` |
| `autostartHome` | string | 自动启动目录 | `~/.config/autostart` |

#### 方法

```qml
// 获取用户特定目录
string userDir(UserDirectory dir)

// 目录类型枚举
XdgDirs.Desktop      // 桌面目录
XdgDirs.Documents    // 文档目录
XdgDirs.Download     // 下载目录
XdgDirs.Music        // 音乐目录
XdgDirs.Pictures     // 图片目录
XdgDirs.Videos       // 视频目录
Xdg Dirs.Templates    // 模板目录
XdgDirs.PublicShare  // 公共共享目录
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Column {
    spacing: 10

    // 显示XDG目录
    Label { text: "Config: " + XdgDirs.configHome() }
    Label { text: "Data: " + XdgDirs.dataHome() }
    Label { text: "Cache: " + XdgDirs.cacheHome() }

    // 获取用户目录
    Label { text: "Documents: " + XdgDirs.userDir(XdgDirs.Documents) }
    Label { text: "Downloads: " + XdgDirs.userDir(XdgDirs.Download) }

    // 保存配置文件示例
    Button {
        text: "Save Config"
        onClicked: {
            var configPath = XdgDirs.configHome() + "/myapp/config.ini"
            // 保存配置到 configPath
        }
    }
}
```

---

### XdgIcon - 图标主题管理

提供图标主题访问和图标加载功能。

#### 属性

| 属性 | 类型 | 描述 |
|-----|------|------|
| `themeName` | string | 当前图标主题名称（只读） |
| `followColorScheme` | bool | 是否跟随颜色方案（可读写） |

#### 方法

```qml
// 从主题加载图标
url fromTheme(string iconName)
url fromTheme(string iconName, string fallback)

// 设置图标主题
void setThemeName(string name)
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Column {
    spacing: 10

    // 显示图标主题信息
    Label { text: "Icon Theme: " + XdgIcon.themeName }

    // 使用图标的按钮
    Row {
        spacing: 10

        Button {
            text: "Open"
            icon.source: XdgIcon.fromTheme("document-open")
            icon.width: 24
            icon.height: 24
        }

        Button {
            text: "Save"
            icon.source: XdgIcon.fromTheme("document-save", "document-save-as")
            icon.width: 24
            icon.height: 24
        }
    }

    // Image组件使用图标
    Image {
        source: XdgIcon.fromTheme("folder")
        sourceSize.width: 48
        sourceSize.height: 48
    }

    // 切换图标主题
    Button {
        text: "Change Theme"
        onClicked: XdgIcon.setThemeName("breeze")
    }
}
```

---

### XdgDesktopFile - 桌面文件操作

读取和操作 .desktop 文件。

#### 属性

| 属性 | 类型 | 描述 |
|-----|------|------|
| `fileName` | string | 桌面文件路径（可读写） |
| `name` | string | 应用程序名称（只读） |
| `comment` | string | 应用程序描述（只读） |
| `iconName` | string | 图标名称（只读） |
| `iconUrl` | url | 图标 URL（只读） |
| `isValid` | bool | 文件是否有效（只读） |
| `type` | int | 条目类型（只读） |

#### 类型枚举

```qml
XdgDesktopFile.UnknownType      // 未知类型
XdgDesktopFile.ApplicationType  // 应用程序
XdgDesktopFile.LinkType         // 链接
XdgDesktopFile.DirectoryType    // 目录
```

#### 方法

```qml
// 加载桌面文件
bool load(string fileName)

// 获取值
variant value(string key, variant defaultValue = undefined)

// 启动应用程序
bool start(stringlist urls = [])
bool startDetached(stringlist urls = [])

// 检查操作
bool isSuitable(string environment, string desktopName = "")
bool isShown(string environment, string desktopName = "")
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Column {
    spacing: 10

    // 定义桌面文件
    XdgDesktopFile {
        id: firefoxDesktop
        fileName: "/usr/share/applications/firefox.desktop"

        Component.onCompleted: {
            if (load()) {
                console.log("Firefox loaded successfully")
            }
        }
    }

    // 显示应用信息
    Row {
        spacing: 10
        visible: firefoxDesktop.isValid

        Image {
            source: firefoxDesktop.iconUrl
            sourceSize.width: 48
            sourceSize.height: 48
        }

        Column {
            Label {
                text: firefoxDesktop.name
                font.bold: true
            }
            Label {
                text: firefoxDesktop.comment
                color: "gray"
                font.pointSize: 9
            }
        }
    }

    // 启动应用程序
    Button {
        text: "Launch Firefox"
        enabled: firefoxDesktop.isValid
        onClicked: {
            if (firefoxDesktop.startDetached()) {
                console.log("Firefox launched")
            } else {
                console.error("Failed to launch Firefox")
            }
        }
    }

    // 带参数启动
    Button {
        text: "Open URL"
        enabled: firefoxDesktop.isValid
        onClicked: {
            firefoxDesktop.startDetached(["https://lxqt-project.org"])
        }
    }
}
```

---

### XdgDefaultApps - 默认应用程序

获取和设置系统默认应用程序。

#### 方法

```qml
// 获取默认应用程序
string webBrowser()
string emailClient()
string fileManager()
string terminal()
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Column {
    spacing: 10

    Label { text: "Web Browser: " + XdgDefaultApps.webBrowser() }
    Label { text: "Email Client: " + XdgDefaultApps.emailClient() }
    Label { text: "File Manager: " + XdgDefaultApps.fileManager() }
    Label { text: "Terminal: " + XdgDefaultApps.terminal() }

    // 启动默认浏览器
    Button {
        text: "Open Browser"
        onClicked: {
            var browserDesktop = XdgDefaultApps.webBrowser()
            var desktop = Qt.createQmlObject(
                'import org.lxqt.qtxdg 6.0; XdgDesktopFile { fileName: "' + browserDesktop + '" }',
                parent
            )
            if (desktop.load()) {
                desktop.startDetached(["https://lxqt-project.org"])
            }
        }
    }
}
```

---

### XdgAutoStart - 自动启动管理

管理自动启动应用程序。

#### 方法

```qml
// 获取自动启动应用列表
stringlist desktopFileList()
stringlist desktopFileList(bool onlyHidden)

// 获取自动启动目录
string autostartHome()
stringlist autostartDirList()

// 判断是否自动启动
bool isAutostart(string desktopFileName)
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Column {
    spacing: 10

    Label {
        text: "Autostart directory: " + XdgAutoStart.autostartHome()
        font.pointSize: 9
    }

    // 显示自动启动应用列表
    ListView {
        width: parent.width
        height: 200

        model: XdgAutoStart.desktopFileList()

        delegate: ItemDelegate {
            width: ListView.view.width
            text: modelData

            XdgDesktopFile {
                id: autoDesktop
                fileName: XdgAutoStart.autostartHome() + "/" + modelData
                Component.onCompleted: load()
            }

            contentItem: Row {
                spacing: 10
                Image {
                    source: autoDesktop.iconUrl
                    sourceSize.width: 24
                    sourceSize.height: 24
                }
                Label { text: autoDesktop.name || modelData }
            }
        }
    }
}
```

---

## 🎯 Phase 1 - MIME 和应用管理

### XdgMimeTypeWrapper - MIME 类型检测

检测文件的 MIME 类型和 MIME 类型关系。

#### 方法

```qml
// MIME 类型检测
string mimeTypeForFile(string fileName)
string mimeTypeForName(string fileName)
string mimeTypeForData(variant data)
string mimeTypeForUrl(url url)

// MIME 类型信息
string genericIconName(string mimeType)
string iconName(string mimeType)
stringlist globPatterns(string mimeType)

// MIME 类型关系
stringlist parentMimeTypes(string mimeType)
stringlist allMimeTypes()
bool inherits(string mimeType, string parentMimeType)
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import org.lxqt.qtxdg 6.0

Column {
    spacing: 10

    XdgMimeTypeWrapper {
        id: mimeWrapper
    }

    TextField {
        id: filePathField
        placeholderText: "Enter file path..."
        width: parent.width
    }

    Button {
        text: "Detect MIME Type"
        onClicked: {
            var mimeType = mimeWrapper.mimeTypeForFile(filePathField.text)
            resultLabel.text = "MIME Type: " + mimeType
            iconLabel.source = mimeWrapper.iconName(mimeType)
        }
    }

    Row {
        spacing: 10
        Image {
            id: iconLabel
            sourceSize.width: 48
            sourceSize.height: 48
        }
        Label {
            id: resultLabel
            text: "No file detected"
        }
    }

    // 文件选择对话框
    Button {
        text: "Choose File"
        onClicked: fileDialog.open()
    }

    FileDialog {
        id: fileDialog
        title: "Choose a file"
        onAccepted: {
            filePathField.text = fileUrl.toString().replace("file://", "")
            var mimeType = mimeWrapper.mimeTypeForFile(filePathField.text)
            resultLabel.text = "MIME Type: " + mimeType
        }
    }

    // 显示MIME类型信息
    GroupBox {
        title: "MIME Type Info"
        visible: resultLabel.text !== "No file detected"

        Column {
            spacing: 5
            Label { text: "Icon: " + mimeWrapper.iconName(mimeWrapper.mimeTypeForFile(filePathField.text)) }
            Label { text: "Generic Icon: " + mimeWrapper.genericIconName(mimeWrapper.mimeTypeForFile(filePathField.text)) }
            Label { text: "Patterns: " + mimeWrapper.globPatterns(mimeWrapper.mimeTypeForFile(filePathField.text)).join(", ") }
        }
    }
}
```

---

### XdgMimeAppsWrapper - 应用程序关联

管理 MIME 类型与应用程序的关联。

#### 属性

| 属性 | 类型 | 描述 |
|-----|------|------|
| `lastError` | string | 最后的错误信息（只读） |

#### 方法

```qml
// 获取应用程序列表
variant allApps()
variant appsForMime(string mimeType)
string defaultApp(string mimeType)
variant fallbackApps(string mimeType)
variant recommendedApps(string mimeType)

// 应用程序信息
variant desktopFileInfo(string desktopId)

// 刷新
void reset()
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Column {
    spacing: 10

    XdgMimeAppsWrapper {
        id: mimeApps
    }

    TextField {
        id: mimeTypeField
        placeholderText: "Enter MIME type (e.g., text/plain)"
        width: parent.width
        text: "text/plain"
    }

    Button {
        text: "Get Apps"
        onClicked: {
            var apps = mimeApps.appsForMime(mimeTypeField.text)
            appsList.model = apps
            defaultLabel.text = "Default: " + mimeApps.defaultApp(mimeTypeField.text)
        }
    }

    Label {
        id: defaultLabel
        text: "No default app"
        font.bold: true
    }

    ListView {
        id: appsList
        width: parent.width
        height: 200
        clip: true

        delegate: ItemDelegate {
            width: ListView.view.width

            // modelData 是 XdgDesktopFile 对象
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
                // 使用此应用打开文件
                modelData.startDetached()
            }
        }
    }

    // 显示所有应用统计
    Button {
        text: "Show All Apps"
        onClicked: {
            var all = mimeApps.allApps()
            console.log("Total apps:", all.length)
            appsList.model = all
        }
    }
}
```

---

### XdgMenuWrapper - 应用程序菜单

加载和解析 XDG 应用程序菜单。

#### 属性

| 属性 | 类型 | 描述 |
|-----|------|------|
| `menuFile` | string | 菜单文件路径（可读写） |
| `environments` | stringlist | 桌面环境列表（可读写） |
| `lastError` | string | 最后的错误信息（只读） |

#### 方法

```qml
// 加载菜单
bool loadMenuFile(string fileName)
bool loadDefaultMenu()

// 获取数据
stringlist categories()
variant applications()
variant applications(string category)

// 刷新
void reload()
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import org.lxqt.qtxdg 6.0

Column {
    spacing: 10

    XdgMenuWrapper {
        id: menuWrapper
        environments: ["LXQt", "X-LXQT"]
    }

    Row {
        spacing: 10

        Button {
            text: "Load Menu"
            onClicked: {
                if (menuWrapper.loadDefaultMenu()) {
                    var cats = menuWrapper.categories()
                    categoryList.model = cats
                    statusLabel.text = "Loaded " + cats.length + " categories"
                } else {
                    statusLabel.text = "Error: " + menuWrapper.lastError
                }
            }
        }

        Label {
            id: statusLabel
            text: "No menu loaded"
        }
    }

    // 分类列表
    ComboBox {
        id: categoryList
        width: parent.width
        textRole: "display"

        onCurrentTextChanged: {
            var apps = menuWrapper.applications(currentText)
            appsList.model = apps
        }
    }

    // 应用程序列表
    ListView {
        id: appsList
        width: parent.width
        height: 300
        clip: true

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
                        elide: Text.ElideRight
                        width: parent.width - 50
                    }
                }
            }

            onClicked: {
                modelData.startDetached()
            }
        }
    }
}
```

---

## 📊 Phase 2 - 数据模型

### XdgApplicationsModel - 应用程序列表模型

提供可搜索和过滤的应用程序列表。

#### 属性

| 属性 | 类型 | 描述 |
|-----|------|------|
| `category` | string | 分类过滤器（可读写） |
| `searchText` | string | 搜索文本（可读写） |
| `count` | int | 应用程序数量（只读） |
| `lastError` | string | 最后的错误信息（只读） |

#### 角色

| 角色名 | 类型 | 描述 |
|--------|------|------|
| `desktopId` | string | 桌面文件 ID |
| `name` | string | 应用程序名称 |
| `iconName` | string | 图标名称 |
| `exec` | string | 执行命令 |
| `comment` | string | 描述 |
| `categories` | stringlist | 分类列表 |
| `mimeTypes` | stringlist | 支持的 MIME 类型 |

#### 方法

```qml
// 重新加载
void reload()

// 获取单行数据
variantmap get(int row)
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ColumnLayout {
    spacing: 10

    // 搜索和过滤控件
    RowLayout {
        Layout.fillWidth: true

        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: "Search applications..."
        }

        ComboBox {
            id: categoryCombo
            model: ["All", "Development", "Graphics", "Network",
                    "Office", "Utility", "AudioVideo", "Game"]
        }
    }

    // 应用程序列表
    ListView {
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true

        model: XdgApplicationsModel {
            id: appsModel
            category: categoryCombo.currentIndex === 0 ? "" : categoryCombo.currentText
            searchText: searchField.text
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
                        text: model.name || "(unnamed)"
                        font.bold: true
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    Label {
                        text: model.comment || ""
                        font.pointSize: 9
                        color: "gray"
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }

                    Label {
                        text: "Categories: " + (model.categories ? model.categories.join(", ") : "")
                        font.pointSize: 8
                        color: "darkgray"
                        visible: model.categories && model.categories.length > 0
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                }
            }

            onClicked: {
                // 可以通过 get() 获取完整数据
                var appData = appsModel.get(index)
                console.log("Launching:", appData.name, "Exec:", appData.exec)
                // 创建 XdgDesktopFile 并启动
            }
        }

        ScrollBar.vertical: ScrollBar {}
    }

    // 操作按钮
    RowLayout {
        Layout.fillWidth: true

        Button {
            text: "Reload"
            onClicked: appsModel.reload()
        }

        Button {
            text: "Clear Search"
            enabled: searchField.text !== ""
            onClicked: searchField.text = ""
        }

        Label {
            Layout.fillWidth: true
            text: "Total: " + appsModel.count
            horizontalAlignment: Text.AlignRight
        }
    }
}
```

---

### XdgAutoStartModel - 自动启动模型

管理自动启动应用程序的模型。

#### 属性

| 属性 | 类型 | 描述 |
|-----|------|------|
| `count` | int | 自动启动项数量（只读） |
| `lastError` | string | 最后的错误信息（只读） |

#### 角色

| 角色名 | 类型 | 描述 |
|--------|------|------|
| `desktopId` | string | 桌面文件 ID |
| `name` | string | 应用程序名称 |
| `iconName` | string | 图标名称 |
| `comment` | string | 描述 |
| `enabled` | bool | 是否启用 |

#### 方法

```qml
// 设置启用状态
bool setEnabled(int row, bool enabled)

// 重新加载
void reload()

// 获取单行数据
variantmap get(int row)
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ColumnLayout {
    spacing: 10

    Label {
        text: "Auto-start Applications"
        font.bold: true
        font.pointSize: 14
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
                    }

                    Label {
                        text: model.comment
                        font.pointSize: 9
                        color: "gray"
                    }
                }

                Switch {
                    checked: model.enabled
                    onToggled: {
                        if (!autostartModel.setEnabled(index, checked)) {
                            // 操作失败，恢复状态
                            checked = !checked
                            console.error("Failed to change autostart status")
                        }
                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {}
    }

    RowLayout {
        Layout.fillWidth: true

        Button {
            text: "Reload"
            onClicked: autostartModel.reload()
        }

        Label {
            Layout.fillWidth: true
            text: "Total: " + autostartModel.count
            horizontalAlignment: Text.AlignRight
        }
    }
}
```

---

### XdgMenuTreeModel - 菜单树模型

提供层次化的应用程序菜单模型。

#### 属性

| 属性 | 类型 | 描述 |
|-----|------|------|
| `menuFile` | string | 菜单文件路径（可读写） |
| `environments` | stringlist | 桌面环境列表（可读写） |
| `count` | int | 根级项目数量（只读） |
| `lastError` | string | 最后的错误信息（只读） |

#### 角色

| 角色名 | 类型 | 描述 |
|--------|------|------|
| `name` | string | 名称 |
| `iconName` | string | 图标名称 |
| `isCategory` | bool | 是否是分类 |
| `desktopFile` | object | 桌面文件对象（应用程序） |

#### 方法

```qml
// 加载菜单
bool loadMenuFile(string fileName)
bool loadDefaultMenu()

// 重新加载
void reload()

// 获取单行数据
variantmap get(int row)
```

#### 使用示例

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

SplitView {
    orientation: Qt.Horizontal

    // 左侧分类列表
    ListView {
        id: categoryView
        SplitView.preferredWidth: 200
        SplitView.minimumWidth: 150
        clip: true

        model: XdgMenuTreeModel {
            id: menuModel
            environments: ["LXQt", "X-LXQT"]

            Component.onCompleted: {
                loadDefaultMenu()
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
                Label { text: model.name }
            }

            onClicked: {
                // 加载此分类下的应用程序
                var data = menuModel.get(index)
                // 处理分类数据...
            }
        }
    }

    // 右侧应用程序列表
    ListView {
        id: appsView
        SplitView.fillWidth: true
        clip: true

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

                    Label {
                        text: model.name
                        font.bold: true
                    }

                    Label {
                        text: model.desktopFile ? model.desktopFile.comment : ""
                        font.pointSize: 9
                        color: "gray"
                    }
                }
            }

            onDoubleClicked: {
                if (model.desktopFile) {
                    model.desktopFile.startDetached()
                }
            }
        }
    }
}
```

---

## 🔧 完整示例

### 完整的应用启动器

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 600
    title: "XDG Application Launcher"

    // 工具栏
    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10

            ToolButton {
                text: "⟳"
                font.pointSize: 16
                onClicked: appsModel.reload()
                ToolTip.text: "Reload"
                ToolTip.visible: hovered
            }

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "Search applications..."
            }

            ComboBox {
                id: categoryCombo
                model: ["All", "Development", "Graphics", "Internet",
                        "Multimedia", "Office", "Settings", "System", "Utility"]
                currentIndex: 0
            }
        }
    }

    // 主视图
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // 左侧：快速访问
        Pane {
            SplitView.preferredWidth: 200
            SplitView.minimumWidth: 150

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "Quick Access"
                    font.bold: true
                }

                Button {
                    Layout.fillWidth: true
                    text: "Web Browser"
                    icon.source: XdgIcon.fromTheme("web-browser")
                    onClicked: launchDefault(XdgDefaultApps.webBrowser())
                }

                Button {
                    Layout.fillWidth: true
                    text: "File Manager"
                    icon.source: XdgIcon.fromTheme("system-file-manager")
                    onClicked: launchDefault(XdgDefaultApps.fileManager())
                }

                Button {
                    Layout.fillWidth: true
                    text: "Terminal"
                    icon.source: XdgIcon.fromTheme("utilities-terminal")
                    onClicked: launchDefault(XdgDefaultApps.terminal())
                }

                Button {
                    Layout.fillWidth: true
                    text: "Email"
                    icon.source: XdgIcon.fromTheme("internet-mail")
                    onClicked: launchDefault(XdgDefaultApps.emailClient())
                }

                Item { Layout.fillHeight: true }

                Label {
                    text: "Auto-start (" + autostartModel.count + ")"
                    font.bold: true
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
                        text: model.name
                        font.pointSize: 9

                        contentItem: Row {
                            spacing: 5
                            Image {
                                source: XdgIcon.fromTheme(model.iconName)
                                sourceSize.width: 16
                                sourceSize.height: 16
                            }
                            Label {
                                text: model.name
                                font.pointSize: 9
                                elide: Text.ElideRight
                                width: parent.width - 25
                            }
                        }
                    }
                }
            }
        }

        // 中间：应用程序网格
        Pane {
            SplitView.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: categoryCombo.currentIndex === 0
                        ? "All Applications (" + appsModel.count + ")"
                        : categoryCombo.currentText + " (" + appsModel.count + ")"
                    font.bold: true
                }

                GridView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    cellWidth: 120
                    cellHeight: 120

                    model: XdgApplicationsModel {
                        id: appsModel
                        category: categoryCombo.currentIndex === 0 ? "" : categoryCombo.currentText
                        searchText: searchField.text
                    }

                    delegate: ItemDelegate {
                        width: GridView.view.cellWidth - 10
                        height: GridView.view.cellHeight - 10

                        contentItem: ColumnLayout {
                            spacing: 5

                            Image {
                                source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                                sourceSize.width: 64
                                sourceSize.height: 64
                                Layout.alignment: Qt.AlignHCenter
                            }

                            Label {
                                text: model.name || "(unnamed)"
                                font.pointSize: 9
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                                elide: Text.ElideRight
                                wrapMode: Text.Wrap
                                maximumLineCount: 2
                            }
                        }

                        onClicked: {
                            detailsPanel.show(model.desktopId, model.name,
                                             model.iconName, model.comment,
                                             model.exec, model.categories)
                        }

                        onDoubleClicked: {
                            launchApp(model.desktopId)
                        }

                        ToolTip.text: model.comment || model.name
                        ToolTip.visible: hovered
                        ToolTip.delay: 500
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }

        // 右侧：详情面板
        Pane {
            id: detailsPanel
            SplitView.preferredWidth: 250
            SplitView.minimumWidth: 200
            visible: false

            property string appDesktopId
            property string appName
            property string appIconName
            property string appComment
            property string appExec
            property var appCategories

            function show(desktopId, name, iconName, comment, exec, categories) {
                appDesktopId = desktopId
                appName = name
                appIconName = iconName
                appComment = comment
                appExec = exec
                appCategories = categories
                visible = true
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                ToolButton {
                    text: "✕"
                    onClicked: detailsPanel.visible = false
                }

                Image {
                    source: XdgIcon.fromTheme(detailsPanel.appIconName || "application-x-executable")
                    sourceSize.width: 96
                    sourceSize.height: 96
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: detailsPanel.appName
                    font.bold: true
                    font.pointSize: 12
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    text: detailsPanel.appComment
                    font.pointSize: 9
                    color: "gray"
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "gray"
                }

                Label {
                    text: "Command:"
                    font.bold: true
                }

                TextEdit {
                    text: detailsPanel.appExec
                    font.family: "monospace"
                    font.pointSize: 8
                    readOnly: true
                    selectByMouse: true
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                Label {
                    text: "Categories:"
                    font.bold: true
                }

                Label {
                    text: detailsPanel.appCategories ? detailsPanel.appCategories.join(", ") : "None"
                    font.pointSize: 9
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                }

                Item { Layout.fillHeight: true }

                Button {
                    text: "Launch"
                    icon.source: XdgIcon.fromTheme("system-run")
                    Layout.fillWidth: true
                    onClicked: {
                        launchApp(detailsPanel.appDesktopId)
                    }
                }
            }
        }
    }

    // 状态栏
    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            Label {
                text: appsModel.lastError || "Ready"
                Layout.fillWidth: true
            }

            Label {
                text: appsModel.count + " apps"
            }
        }
    }

    // 辅助函数
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
        }
        desktop.destroy()
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

## ❓ 常见问题

### Q1: QML 模块找不到

**问题:**
```
module "org.lxqt.qtxdg" is not installed
```

**解决方案:**
```bash
# 开发环境设置导入路径
export QML_IMPORT_PATH=/path/to/build/qml:$QML_IMPORT_PATH

# 或在代码中设置
engine.addImportPath("/path/to/build/qml");

# 生产环境安装模块
sudo make install
```

### Q2: 图标不显示

**问题:** 图标显示为空白或默认图标

**解决方案:**
```bash
# 1. 检查图标主题是否安装
ls /usr/share/icons/

# 2. 安装图标主题
sudo apt install breeze-icon-theme hicolor-icon-theme

# 3. 在代码中设置后备图标
icon.source: XdgIcon.fromTheme("document-open", "document-new")
```

### Q3: 桌面文件加载失败

**问题:** `XdgDesktopFile.isValid` 返回 false

**解决方案:**
```qml
XdgDesktopFile {
    id: desktop

    Component.onCompleted: {
        // 检查文件是否存在
        var path = "/usr/share/applications/firefox.desktop"
        if (!load(path)) {
            console.error("Failed to load:", path)
            console.error("Try searching in:", XdgDirs.dataHome())
        }
    }
}
```

### Q4: Model 数据不更新

**问题:** 修改过滤条件后列表不更新

**解决方案:**
```qml
XdgApplicationsModel {
    id: appsModel
    category: categoryCombo.currentText
    searchText: searchField.text

    // 确保属性已正确绑定
    // 如果需要手动刷新
    Component.onCompleted: {
        reload()
    }
}

// 或在属性变化时手动刷新
ComboBox {
    onCurrentTextChanged: appsModel.reload()
}
```

### Q5: 性能问题

**问题:** 应用列表加载慢

**解决方案:**
```qml
// 1. 使用分类过滤减少数据量
XdgApplicationsModel {
    category: "Office"  // 只加载 Office 分类
}

// 2. 使用虚拟化ListView
ListView {
    cacheBuffer: 200  // 缓存额外的项目
    reuseItems: true  // 重用委托
}

// 3. 延迟加载
Component.onCompleted: {
    // 延迟500ms后加载
    Qt.callLater(function() {
        appsModel.reload()
    })
}
```

### Q6: 如何调试

**启用调试输出:**
```bash
# 启用 QML 调试
QML_IMPORT_TRACE=1 ./your-app

# 启用 Qt 调试
QT_LOGGING_RULES="qt.qml.binding=true" ./your-app

# 启用所有调试
QT_LOGGING_RULES="*=true" ./your-app
```

**在代码中调试:**
```qml
// 打印模型数据
Component.onCompleted: {
    console.log("Model count:", appsModel.count)
    for (var i = 0; i < appsModel.count; i++) {
        var item = appsModel.get(i)
        console.log(i, item.name, item.desktopId)
    }
}

// 监视属性变化
onCountChanged: {
    console.log("Count changed to:", count)
}
```

---

## 📚 更多资源

- **项目主页:** https://github.com/lxqt/libqtxdg
- **API 文档:** https://docs.lxqt.org/libqtxdg/
- **Qt QML 文档:** https://doc.qt.io/qt-6/qtqml-index.html
- **XDG 规范:** https://specifications.freedesktop.org/

---

*文档版本: 1.0*
*最后更新: 2025-11-20*
*维护者: LXQt Team*
