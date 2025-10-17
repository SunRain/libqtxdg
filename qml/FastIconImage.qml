// FastIconImage.qml - High-performance async icon component
// Part of Qt6XdgQml v2.0 optimization
import QtQuick 2.15
import QtQuick.Controls 2.15

/*!
 * \brief Async icon loading component with caching
 *
 * This component uses the new FastIconProvider backend for
 * zero-blocking async icon loading with L2 memory cache.
 *
 * Usage:
 *   FastIconImage {
 *       iconName: "document-open"
 *       iconSize: Qt.size(24, 24)
 *       fallbackName: "text-x-generic"
 *       iconState: FastIconImage.Normal
 *   }
 */
Item {
    id: root

    // Public API properties
    property string iconName: ""
    property size iconSize: Qt.size(32, 32)
    property string fallbackName: ""
    property int iconState: FastIconImage.Normal

    // Readonly status properties
    readonly property bool loading: image.status === Image.Loading
    readonly property int status: image.status
    readonly property string errorString: image.status === Image.Error ?
        "Failed to load icon: " + iconName : ""
    readonly property size actualSize: image.sourceSize

    // Icon state enum
    enum State {
        Normal = 0,
        Disabled = 1,
        Pressed = 2,
        Hover = 3
    }

    // Auto-size behavior (optional)
    implicitWidth: iconSize.width
    implicitHeight: iconSize.height

    // Internal image using FastIconProvider
    Image {
        id: image
        anchors.fill: parent
        source: buildIconUrl()
        asynchronous: true  // Always async
        cache: false        // Provider manages caching
        sourceSize: root.iconSize
        fillMode: Image.PreserveAspectFit

        // Build URL for FastIconProvider
        // Format: image://fasticon/iconName?size=24x24&fallback=default&state=0
        function buildIconUrl() {
            if (!root.iconName) {
                return ""
            }

            var url = "image://fasticon/" + root.iconName
            var params = []

            // Size parameter (informational, actual size from sourceSize)
            params.push("size=" + root.iconSize.width + "x" + root.iconSize.height)

            // Fallback icon
            if (root.fallbackName) {
                params.push("fallback=" + root.fallbackName)
            }

            // Icon state
            if (root.iconState !== FastIconImage.Normal) {
                params.push("state=" + root.iconState)
            }

            if (params.length > 0) {
                url += "?" + params.join("&")
            }

            return url
        }

        // Reload when properties change
        Connections {
            target: root
            function onIconNameChanged() { image.source = image.buildIconUrl() }
            function onIconSizeChanged() { image.source = image.buildIconUrl() }
            function onFallbackNameChanged() { image.source = image.buildIconUrl() }
            function onIconStateChanged() { image.source = image.buildIconUrl() }
        }
    }

    // Optional: Loading indicator
    BusyIndicator {
        id: loadingIndicator
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.5
        height: width
        running: root.loading
        visible: root.loading
    }

    // Debug output (can be disabled in production)
    Component.onCompleted: {
        if (Qt.application.arguments.indexOf("--debug-icons") !== -1) {
            console.log("FastIconImage created: iconName=" + root.iconName +
                       ", size=" + root.iconSize.width + "x" + root.iconSize.height)
        }
    }

    onStatusChanged: {
        if (Qt.application.arguments.indexOf("--debug-icons") !== -1) {
            if (status === Image.Ready) {
                console.log("FastIconImage loaded: " + root.iconName +
                           " (" + actualSize.width + "x" + actualSize.height + ")")
            } else if (status === Image.Error) {
                console.warn("FastIconImage failed: " + root.errorString)
            }
        }
    }
}
