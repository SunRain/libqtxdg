import QtQuick
import QtQuick.Window
import org.deckshell.qtxdg 6.0

ApplicationWindow {
    width: 400
    height: 300
    visible: true
    title: qsTr("图标测试")

    Column {
        anchors.centerIn: parent
        spacing: 20

        Image {
            source: XdgIcon.fromTheme("document-open", "text-x-generic")
            sourceSize.width: 64
            sourceSize.height: 64
        }

        Text {
            text: qsTr("document-open 图标")
        }

        Image {
            source: XdgIcon.fromTheme("folder", "folder")
            sourceSize.width: 64
            sourceSize.height: 64
        }

        Text {
            text: qsTr("folder 图标")
        }

        Text {
            text: XdgIcon.toImageUrl("firefox")
        }
    }

    Component.onCompleted: {
        // 初始化图标引擎
        IconEngineInit
        console.log("QML 应用启动完成")
    }
}