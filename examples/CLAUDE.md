# libqtxdg 示例程序

> 📍 **导航：** [项目根目录](../CLAUDE.md) > examples

## 模块概述

examples 目录包含 libqtxdg 的使用示例，展示如何在 C++ 和 QML 应用中集成 XDG 功能。

**模块路径：** `examples/`

## 示例列表

### 1. use-qtxdg - C++ 使用示例
**路径：** `examples/use-qtxdg/`
**类型：** C++ 应用程序
**构建目标：** `use-qtxdg`

**功能演示：**
- 基本的 Qt6Xdg 库使用
- XDG 目录访问
- 桌面文件操作
- 图标加载

**构建：**
```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
make use-qtxdg
./examples/use-qtxdg/use-qtxdg
```

### 2. qml-example - QML 使用示例
**路径：** `examples/qml-example/`
**类型：** QML 应用程序
**构建目标：** `qtxdg-qml-example`

**功能演示：**
- QML 中使用 org.lxqt.qtxdg 模块
- XdgDirs QML 组件
- XdgIcon QML 组件
- XdgDesktopFile QML 组件
- 响应式 UI 集成

**构建：**
```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON -DBUILD_QML_PLUGIN=ON
make qtxdg-qml-example
```

**运行：**
```bash
# 设置 QML 导入路径
QML_IMPORT_PATH=build/qml ./build/examples/qml-example/qtxdg-qml-example
```

## 目录结构

```
examples/
├── CMakeLists.txt          # 示例构建配置
├── use-qtxdg/              # C++ 示例
│   ├── CMakeLists.txt
│   ├── main.cpp
│   └── ...
└── qml-example/            # QML 示例
    ├── CMakeLists.txt
    ├── main.cpp            # QML 引擎启动
    ├── main.qml            # QML 主界面
    ├── README.md           # QML 示例说明
    └── ...
```

## C++ 示例详解

### 基本使用模式

```cpp
#include <QApplication>
#include <XdgDesktopFile>
#include <XdgDirs>
#include <XdgIcon>
#include <XdgMenu>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 1. 获取 XDG 目录
    qDebug() << "Config Home:" << XdgDirs::configHome();
    qDebug() << "Data Home:" << XdgDirs::dataHome();
    qDebug() << "Cache Home:" << XdgDirs::cacheHome();

    // 2. 加载桌面文件
    XdgDesktopFile desktop;
    if (desktop.load("/usr/share/applications/firefox.desktop")) {
        qDebug() << "Name:" << desktop.name();
        qDebug() << "Comment:" << desktop.comment();
        qDebug() << "Icon:" << desktop.iconName();

        // 启动应用
        desktop.startDetached();
    }

    // 3. 图标加载
    QIcon icon = XdgIcon::fromTheme("document-open");
    if (!icon.isNull()) {
        qDebug() << "Icon loaded successfully";
    }

    // 4. 菜单系统
    XdgMenu menu;
    menu.setEnvironments(QStringList() << "X-LXQT" << "LXQt");
    if (menu.read("/etc/xdg/menus/lxqt-applications.menu")) {
        qDebug() << "Menu loaded";
        // 遍历菜单项...
    }

    return app.exec();
}
```

### 编译配置

```cmake
find_package(Qt6Xdg REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app Qt6Xdg)
```

## QML 示例详解

### 基本 QML 应用结构

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    id: root
    width: 640
    height: 480
    visible: true
    title: "QtXdg QML Example"

    // XDG 组件实例
    XdgDirs {
        id: xdgDirs
    }

    XdgDefaultApps {
        id: defaultApps
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        // 显示 XDG 目录
        GroupBox {
            title: "XDG Directories"
            Layout.fillWidth: true

            GridLayout {
                columns: 2
                Label { text: "Config Home:" }
                Label { text: xdgDirs.configHome }

                Label { text: "Data Home:" }
                Label { text: xdgDirs.dataHome }

                Label { text: "Cache Home:" }
                Label { text: xdgDirs.cacheHome }
            }
        }

        // 显示默认应用
        GroupBox {
            title: "Default Applications"
            Layout.fillWidth: true

            GridLayout {
                columns: 2
                Label { text: "Web Browser:" }
                Label { text: defaultApps.webBrowser }

                Label { text: "Terminal:" }
                Label { text: defaultApps.terminal }
            }
        }

        // 图标按钮
        GroupBox {
            title: "Icon Examples"
            Layout.fillWidth: true

            Row {
                spacing: 10

                Button {
                    text: "Open"
                    icon.name: "document-open"
                    icon.source: XdgIcon.fromTheme("document-open")
                }

                Button {
                    text: "Save"
                    icon.name: "document-save"
                    icon.source: XdgIcon.fromTheme("document-save")
                }

                Button {
                    text: "Folder"
                    icon.name: "folder"
                    icon.source: XdgIcon.fromTheme("folder")
                }
            }
        }

        // 桌面文件操作
        GroupBox {
            title: "Desktop File Example"
            Layout.fillWidth: true

            XdgDesktopFile {
                id: firefoxDesktop
                fileName: "/usr/share/applications/firefox.desktop"
                Component.onCompleted: load()
            }

            Column {
                spacing: 10

                Label {
                    text: firefoxDesktop.valid
                        ? "Application: " + firefoxDesktop.name
                        : "Firefox not found"
                }

                Button {
                    text: "Launch Firefox"
                    enabled: firefoxDesktop.valid
                    onClicked: {
                        firefoxDesktop.startDetached()
                        console.log("Firefox launched")
                    }
                }
            }
        }
    }
}
```

### QML 主程序（main.cpp）

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 设置 QML 导入路径（如果需要）
    engine.addImportPath("qrc:/");

    // 加载主 QML 文件
    const QUrl url(u"qrc:/main.qml"_qs);
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

## 运行示例

### C++ 示例
```bash
# 构建
mkdir build && cd build
cmake .. -DBUILD_EXAMPLES=ON
make

# 运行
./examples/use-qtxdg/use-qtxdg
```

### QML 示例
```bash
# 构建（需要启用 QML 插件）
mkdir build && cd build
cmake .. -DBUILD_EXAMPLES=ON -DBUILD_QML_PLUGIN=ON
make

# 运行（设置 QML 导入路径）
QML_IMPORT_PATH=./qml ./examples/qml-example/qtxdg-qml-example

# 或者使用绝对路径
QML_IMPORT_PATH=/path/to/build/qml ./examples/qml-example/qtxdg-qml-example
```

## 常见问题

### 1. QML 模块找不到
**错误：** `module "org.lxqt.qtxdg" is not installed`

**解决方案：**
```bash
# 方法 1：设置环境变量
export QML_IMPORT_PATH=/path/to/build/qml:$QML_IMPORT_PATH

# 方法 2：安装后使用
sudo make install
# QML 模块会安装到系统路径
```

### 2. 图标不显示
**原因：** 图标主题未安装或路径不正确

**解决方案：**
```bash
# 安装图标主题
sudo apt install breeze-icon-theme  # Debian/Ubuntu
sudo dnf install breeze-icon-theme  # Fedora

# 检查图标路径
ls /usr/share/icons/
```

### 3. 桌面文件加载失败
**原因：** 文件路径不存在或权限问题

**解决方案：**
```cpp
// 检查文件是否存在
QFile file("/usr/share/applications/firefox.desktop");
if (!file.exists()) {
    qDebug() << "Desktop file not found";
}

// 使用绝对路径
desktop.load(QStandardPaths::locate(
    QStandardPaths::ApplicationsLocation,
    "firefox.desktop"
));
```

## 扩展示例

### 创建自定义应用启动器

```cpp
// launcher.cpp
#include <QApplication>
#include <QListWidget>
#include <XdgDesktopFile>
#include <QDir>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QListWidget list;

    // 扫描所有桌面文件
    QDir appsDir("/usr/share/applications");
    for (const QString &file : appsDir.entryList({"*.desktop"})) {
        XdgDesktopFile desktop;
        if (desktop.load(appsDir.filePath(file))) {
            auto *item = new QListWidgetItem(
                desktop.icon(),
                desktop.name()
            );
            item->setData(Qt::UserRole, appsDir.filePath(file));
            list.addItem(item);
        }
    }

    // 双击启动应用
    QObject::connect(&list, &QListWidget::itemDoubleClicked,
        [](QListWidgetItem *item) {
            XdgDesktopFile desktop;
            desktop.load(item->data(Qt::UserRole).toString());
            desktop.startDetached();
        }
    );

    list.show();
    return app.exec();
}
```

## 学习路径

1. **入门：** 运行 `use-qtxdg` 示例，理解基本 API
2. **进阶：** 运行 `qml-example`，学习 QML 集成
3. **实践：** 创建自己的应用，集成 XDG 功能
4. **深入：** 阅读源码，理解实现细节

## 相关模块

- [Qt6Xdg 核心库](../src/qtxdg/CLAUDE.md) - C++ API 文档
- [Qt6XdgQml](../src/qtxdgqml/CLAUDE.md) - QML API 文档

## 参考资源

- **Qt 文档：** https://doc.qt.io/qt-6/
- **QML 教程：** https://doc.qt.io/qt-6/qmlapplications.html
- **XDG 规范：** https://specifications.freedesktop.org/
