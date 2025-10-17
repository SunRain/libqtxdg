/* SearchModesExample.qml - SearchMode功能演示
 *
 * 演示XdgApplicationsModel的三种搜索模式:
 * - PlainText: 简单子串匹配
 * - Regex: 正则表达式匹配
 * - Fuzzy: 模糊匹配(容错输入)
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

Rectangle {
    id: root
    color: palette.window

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // 标题
        Label {
            text: "XdgApplicationsModel - SearchMode 演示"
            font.pointSize: 16
            font.bold: true
            Layout.fillWidth: true
        }

        // 说明文本
        Label {
            text: "演示三种搜索模式:
• PlainText - 简单子串匹配(最快)
• Regex - 正则表达式匹配(强大)
• Fuzzy - 模糊匹配(容错typo)"
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            color: palette.mid
        }

        // 搜索模式选择
        GroupBox {
            title: "搜索模式"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                RadioButton {
                    id: plainTextMode
                    text: "PlainText - 简单匹配"
                    checked: true
                    onCheckedChanged: {
                        if (checked) {
                            appsModel.searchMode = XdgApplicationsModelEnums.PlainText
                            searchHint.text = "提示: 输入任意文本进行子串匹配,如 'fire' 匹配 Firefox"
                        }
                    }
                }

                RadioButton {
                    id: regexMode
                    text: "Regex - 正则表达式"
                    onCheckedChanged: {
                        if (checked) {
                            appsModel.searchMode = XdgApplicationsModelEnums.Regex
                            searchHint.text = "提示: 输入正则表达式,如 '^fire.*' 匹配以fire开头, 'editor|browser' 匹配任一"
                        }
                    }
                }

                RadioButton {
                    id: fuzzyMode
                    text: "Fuzzy - 模糊匹配"
                    onCheckedChanged: {
                        if (checked) {
                            appsModel.searchMode = XdgApplicationsModelEnums.Fuzzy
                            searchHint.text = "提示: 输入可能有拼写错误的文本,如 'firefo' 也能匹配 Firefox (容忍30%编辑距离)"
                        }
                    }
                }
            }
        }

        // 搜索框
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "搜索:"
            }

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: "输入搜索文本..."
                text: ""

                onTextChanged: {
                    appsModel.searchText = text
                }
            }

            Button {
                text: "清除"
                onClicked: {
                    searchField.text = ""
                }
            }
        }

        // 搜索提示
        Label {
            id: searchHint
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: "提示: 输入任意文本进行子串匹配,如 'fire' 匹配 Firefox"
            color: palette.mid
            font.italic: true
        }

        // 快速测试按钮
        GroupBox {
            title: "快速测试"
            Layout.fillWidth: true

            RowLayout {
                spacing: 10

                Button {
                    text: "PlainText: fire"
                    onClicked: {
                        plainTextMode.checked = true
                        searchField.text = "fire"
                    }
                }

                Button {
                    text: "Regex: ^fire.*"
                    onClicked: {
                        regexMode.checked = true
                        searchField.text = "^fire.*"
                    }
                }

                Button {
                    text: "Fuzzy: firefo"
                    onClicked: {
                        fuzzyMode.checked = true
                        searchField.text = "firefo"
                    }
                }

                Button {
                    text: "Regex: edit|brow"
                    onClicked: {
                        regexMode.checked = true
                        searchField.text = "edit|brow"
                    }
                }
            }
        }

        // 结果统计
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: `找到 ${appsModel.count} 个应用`
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            Label {
                text: appsModel.loading ? "⏳ 加载中..." : "✓ 就绪"
            }
        }

        // 应用列表
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: XdgApplicationsModel {
                id: appsModel
                searchMode: XdgApplicationsModelEnums.PlainText
                searchText: ""

                onLoadingChanged: {
                    if (!loading) {
                        console.log("[SearchModesExample] 加载完成, 共", count, "个应用")
                    }
                }

                onErrorOccurred: function(code, message) {
                    errorLabel.text = "错误: " + message
                    errorLabel.visible = true
                }
            }

            delegate: ItemDelegate {
                width: ListView.view.width
                height: 60

                contentItem: RowLayout {
                    spacing: 10

                    Image {
                        source: XdgIcon.fromTheme(model.iconName || "application-x-executable")
                        sourceSize.width: 32
                        sourceSize.height: 32
                        Layout.alignment: Qt.AlignVCenter
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            text: model.name
                            font.bold: true
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        Label {
                            text: model.comment || "无描述"
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            color: palette.mid
                            font.pointSize: font.pointSize * 0.9
                        }

                        Label {
                            text: `Desktop ID: ${model.desktopId}`
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            color: palette.mid
                            font.pointSize: font.pointSize * 0.8
                        }
                    }

                    Button {
                        text: "启动"
                        icon.name: "system-run"
                        onClicked: {
                            console.log("[SearchModesExample] 启动应用:", model.name)
                            // 实际启动逻辑
                            var desktop = Qt.createQmlObject(
                                'import org.lxqt.qtxdg 6.0; XdgDesktopFile {}',
                                root,
                                'dynamicDesktop'
                            )
                            desktop.fileName = model.fileName
                            if (desktop.load()) {
                                desktop.startDetached()
                            }
                        }
                    }
                }

                onDoubleClicked: {
                    console.log("[SearchModesExample] 双击启动:", model.name)
                }
            }

            ScrollBar.vertical: ScrollBar {}

            // 空状态提示
            Label {
                anchors.centerIn: parent
                visible: listView.count === 0 && !appsModel.loading
                text: searchField.text.length > 0 ?
                      "未找到匹配的应用\n\n尝试:\n• 使用更通用的搜索词\n• 切换到模糊匹配模式\n• 检查拼写" :
                      "输入搜索文本以过滤应用"
                horizontalAlignment: Text.AlignHCenter
                color: palette.mid
            }

            // 加载指示器
            BusyIndicator {
                anchors.centerIn: parent
                running: appsModel.loading
                visible: running
            }
        }

        // 错误提示
        Label {
            id: errorLabel
            Layout.fillWidth: true
            visible: false
            color: "red"
            wrapMode: Text.WordWrap
        }

        // 性能和技术信息
        GroupBox {
            title: "性能信息"
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2

                Label {
                    text: "当前模式:"
                    font.bold: true
                }
                Label {
                    text: {
                        if (plainTextMode.checked) return "PlainText (最快)"
                        if (regexMode.checked) return "Regex (中等)"
                        if (fuzzyMode.checked) return "Fuzzy (较慢)"
                        return "未知"
                    }
                }

                Label {
                    text: "应用总数:"
                    font.bold: true
                }
                Label {
                    text: appsModel.count.toString()
                }

                Label {
                    text: "搜索文本:"
                    font.bold: true
                }
                Label {
                    text: searchField.text || "(空)"
                    wrapMode: Text.WrapAnywhere
                    Layout.fillWidth: true
                }

                Label {
                    text: "自动重载:"
                    font.bold: true
                }
                Label {
                    text: appsModel.autoReload ? "✓ 启用" : "✗ 禁用"
                }
            }
        }

        // 使用说明
        GroupBox {
            title: "使用说明"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 5

                Label {
                    text: "• PlainText模式: 不区分大小写的子串匹配,性能最好"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    text: "• Regex模式: 支持完整的正则表达式语法,如 ^...$ 锚点, .* 通配, | 或运算等"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    text: "• Fuzzy模式: 使用Levenshtein编辑距离算法,容忍30%的拼写错误"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    text: "• 所有模式都会搜索应用名称、注释和执行命令"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Label {
                    text: "• 无效的正则表达式会自动降级为PlainText模式"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    color: palette.mid
                    font.italic: true
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("[SearchModesExample] 组件加载完成")
        console.log("[SearchModesExample] SearchMode枚举可用:", typeof XdgApplicationsModelEnums !== 'undefined')
    }
}
