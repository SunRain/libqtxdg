import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.deckshell.qtxdg 6.0

ApplicationWindow {
    id: root
    visible: true
    width: 900
    height: 700
    title: "Qt6Xdg QML Example - Phase 1 MVP"

    // 设置Basic样式避免Breeze样式问题
    Component.onCompleted: {
        // 尝试设置Basic样式
        if (typeof Qt !== 'undefined' && Qt.application) {
            Qt.application.style = "Basic"
        }
    }

    TabBar {
        id: tabBar
        width: parent.width

        TabButton {
            text: "Original Features"
        }

        TabButton {
            text: "Phase 1 - MIME & Menu"
        }

        TabButton {
            text: "Phase 2 - Models"
        }

        TabButton {
            text: "Menu Tree (P1 Fix)"
        }

        TabButton {
            text: "App Launcher"
        }

        TabButton {
            text: "Search Modes (v1.1)"
        }

        TabButton {
            text: "Performance Dashboard"
        }

        TabButton {
            text: "Cache Stress Test"
        }
    }

    StackLayout {
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        currentIndex: tabBar.currentIndex
        
        // Tab 0: Original Features
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                width: parent.width - 40
                spacing: 20

            // XDG Directories Section
            GroupBox {
                title: "XDG Directories"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    Label {
                        text: "Data Home: " + XdgDirs.dataHome()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Config Home: " + XdgDirs.configHome()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Cache Home: " + XdgDirs.cacheHome()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Runtime Dir: " + XdgDirs.runtimeDir()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Desktop Dir: " + XdgDirs.userDir(XdgDirs.Desktop)
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Documents Dir: " + XdgDirs.userDir(XdgDirs.Documents)
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                }
            }

            // XDG Icon Section
            GroupBox {
                title: "XDG Icon Theme"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    Label {
                        text: "Current Theme: " + XdgIcon.themeName
                    }

                    Label {
                        text: "Follow Color Scheme: " + (XdgIcon.followColorScheme ? "Yes" : "No")
                    }

                    RowLayout {
                        spacing: 10

                        Image {
                            source: XdgIcon.fromTheme("document-open")
                            sourceSize.width: 48
                            sourceSize.height: 48
                        }

                        Label {
                            text: "document-open icon"
                        }
                    }

                    RowLayout {
                        spacing: 10

                        Image {
                            source: XdgIcon.fromTheme("applications-system")
                            sourceSize.width: 48
                            sourceSize.height: 48
                        }

                        Label {
                            text: "applications-system icon"
                        }
                    }
                }
            }

            // Desktop File Section
            GroupBox {
                title: "Desktop File Example"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    XdgDesktopFile {
                        id: desktopFile
                        Component.onCompleted: {
                            // Try to load a common desktop file
                            var testFiles = [
                                "/usr/share/applications/firefox.desktop",
                                "/usr/share/applications/chromium.desktop",
                                "/usr/share/applications/org.kde.dolphin.desktop",
                                "/usr/share/applications/pcmanfm-qt.desktop"
                            ]
                            for (var i = 0; i < testFiles.length; i++) {
                                if (load(testFiles[i])) {
                                    break
                                }
                            }
                        }
                    }

                    Label {
                        text: "File Name: " + desktopFile.fileName
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Application Name: " + desktopFile.name
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                        visible: desktopFile.isValid
                    }

                    Label {
                        text: "Comment: " + desktopFile.comment
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                        visible: desktopFile.isValid
                    }

                    Label {
                        text: "Type: " + (desktopFile.type === XdgDesktopFile.ApplicationType ? "Application" :
                                        desktopFile.type === XdgDesktopFile.LinkType ? "Link" :
                                        desktopFile.type === XdgDesktopFile.DirectoryType ? "Directory" : "Unknown")
                        visible: desktopFile.isValid
                    }

                    RowLayout {
                        visible: desktopFile.isValid
                        spacing: 10

                        Image {
                            source: desktopFile.iconUrl
                            sourceSize.width: 48
                            sourceSize.height: 48
                        }

                        Button {
                            text: "Launch Application"
                            onClicked: {
                                if (desktopFile.startDetached()) {
                                    statusLabel.text = "Application launched successfully!"
                                } else {
                                    statusLabel.text = "Failed to launch application"
                                }
                            }
                        }
                    }

                    Label {
                        id: statusLabel
                        color: "green"
                        visible: text !== ""
                    }
                }
            }

            // Default Apps Section
            GroupBox {
                title: "Default Applications"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    Label {
                        text: "Web Browser: " + XdgDefaultApps.webBrowser()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "File Manager: " + XdgDefaultApps.fileManager()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Terminal: " + XdgDefaultApps.terminal()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Email Client: " + XdgDefaultApps.emailClient()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                }
            }

            // Autostart Section
            GroupBox {
                title: "Autostart Applications"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    Label {
                        text: "Autostart Directory: " + XdgDirs.autostartHome()
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }

                    Label {
                        id: autostartLabel
                        text: "Autostart files: " + XdgAutoStart.desktopFileList().join(", ")
                        wrapMode: Text.Wrap
                        Layout.fillWidth: true
                    }
                }
            }
            }
        }
        
        // Tab 1: Phase 1 MVP Features
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ColumnLayout {
                width: parent.width - 40
                spacing: 20
                
                MimeTypeExample {
                }
            }
        }
        
        // Tab 2: Phase 2 Models Features
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                width: parent.width - 40
                spacing: 20

                ApplicationsExample {
                }
            }
        }

        // Tab 3: Menu Tree Model (方案1.1完成)
        MenuTreeExample {
        }

        // Tab 4: Application Launcher (方案3.2完成)
        ApplicationLauncher {
        }

        // Tab 5: Search Modes Example (v1.1新增)
        SearchModesExample {
        }

        // Tab 6: Performance Dashboard (Stage 4)
        PerformanceDashboard {
        }

        // Tab 7: Cache Stress Test (Stage 4)
        IconCacheStressTest {
        }
    }
}
