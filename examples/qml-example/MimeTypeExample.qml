import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

GroupBox {
    title: "MIME Type & Applications (Phase 1 MVP)"
    Layout.fillWidth: true
    
    property string testFilePath: "/usr/share/applications/firefox.desktop"
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 15
        
        // MIME Type Detection Section
        GroupBox {
            title: "MIME Type Detection"
            Layout.fillWidth: true
            
            ColumnLayout {
                width: parent.width
                spacing: 10
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: filePathField
                        Layout.fillWidth: true
                        placeholderText: "Enter file path..."
                        text: "/usr/share/applications/firefox.desktop"
                    }
                    
                    Button {
                        text: "Detect MIME"
                        onClicked: {
                            var mimeType = XdgMimeType.mimeTypeForFile(filePathField.text)
                            if (mimeType) {
                                mimeTypeLabel.text = "MIME Type: " + mimeType
                                commentLabel.text = "Description: " + XdgMimeType.comment(mimeType)
                                iconLabel.text = "Icon: " + XdgMimeType.iconName(mimeType)
                                globLabel.text = "Patterns: " + XdgMimeType.globPatterns(mimeType).join(", ")
                                
                                // Load icon using the new toImageUrl helper
                                mimeIcon.source = XdgIcon.toImageUrl(XdgMimeType.iconName(mimeType))
                            } else {
                                mimeTypeLabel.text = "Error: " + XdgMimeType.lastError
                            }
                        }
                    }
                }
                
                Image {
                    id: mimeIcon
                    sourceSize.width: 32
                    sourceSize.height: 32
                    visible: source != ""
                }
                
                Label {
                    id: mimeTypeLabel
                    text: "MIME Type: (click 'Detect MIME' button)"
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                    font.bold: true
                }
                
                Label {
                    id: commentLabel
                    text: ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                    visible: text !== ""
                }
                
                Label {
                    id: iconLabel
                    text: ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                    visible: text !== ""
                }
                
                Label {
                    id: globLabel
                    text: ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                    visible: text !== ""
                }
            }
        }
        
        // Applications for MIME Type Section
        GroupBox {
            title: "Applications for MIME Type"
            Layout.fillWidth: true
            
            ColumnLayout {
                width: parent.width
                spacing: 10
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    TextField {
                        id: mimeTypeField
                        Layout.fillWidth: true
                        placeholderText: "Enter MIME type (e.g., text/plain)"
                        text: "text/plain"
                    }
                    
                    Button {
                        text: "List Apps"
                        onClicked: {
                            var apps = XdgMimeApps.appsForMimeType(mimeTypeField.text)
                            if (apps && apps.length > 0) {
                                appsListModel.clear()
                                for (var i = 0; i < apps.length; i++) {
                                    appsListModel.append(apps[i])
                                }
                                appsCountLabel.text = "Found " + apps.length + " application(s)"
                            } else {
                                appsCountLabel.text = "No applications found or error: " + XdgMimeApps.lastError
                                appsListModel.clear()
                            }
                            
                            // Get default app
                            var defaultApp = XdgMimeApps.defaultApp(mimeTypeField.text)
                            defaultAppLabel.text = "Default: " + (defaultApp || "(none)")
                        }
                    }
                }
                
                Label {
                    id: defaultAppLabel
                    text: "Default: (click 'List Apps' button)"
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                    font.italic: true
                }
                
                Label {
                    id: appsCountLabel
                    text: ""
                    visible: text !== ""
                    color: "blue"
                }
                
                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    clip: true
                    
                    model: ListModel {
                        id: appsListModel
                    }
                    
                    delegate: ItemDelegate {
                        width: ListView.view.width
                        
                        contentItem: RowLayout {
                            spacing: 10
                            
                            Image {
                                source: XdgIcon.toImageUrl(model.iconName || "")
                                sourceSize.width: 24
                                sourceSize.height: 24
                            }
                            
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                
                                Label {
                                    text: model.name || "(unnamed)"
                                    font.bold: true
                                }
                                
                                Label {
                                    text: model.comment || ""
                                    font.pointSize: 9
                                    color: "gray"
                                    visible: text !== ""
                                    wrapMode: Text.Wrap
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Menu System Test
        GroupBox {
            title: "XDG Menu System"
            Layout.fillWidth: true
            
            ColumnLayout {
                width: parent.width
                spacing: 10
                
                Button {
                    text: "Load Menu"
                    onClicked: {
                        var menuFile = XdgMenu.defaultMenuFile()
                        if (menuFile) {
                            menuFileLabel.text = "Menu file: " + menuFile
                            if (XdgMenu.loadMenu(menuFile)) {
                                menuStatusLabel.text = "✓ Menu loaded successfully!"
                                menuStatusLabel.color = "green"
                                
                                var categories = XdgMenu.allCategories()
                                categoriesLabel.text = "Categories: " + categories.join(", ")
                            } else {
                                menuStatusLabel.text = "✗ Error: " + XdgMenu.lastError
                                menuStatusLabel.color = "red"
                            }
                        } else {
                            menuStatusLabel.text = "✗ No default menu file found"
                            menuStatusLabel.color = "red"
                        }
                    }
                }
                
                Label {
                    id: menuFileLabel
                    text: ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                    visible: text !== ""
                    font.pointSize: 9
                }
                
                Label {
                    id: menuStatusLabel
                    text: "(Click 'Load Menu' to test)"
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }
                
                Label {
                    id: categoriesLabel
                    text: ""
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                    visible: text !== ""
                    font.pointSize: 9
                }
            }
        }
    }
}
