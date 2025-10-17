# libqtxdg QML API Reference v1.0

> **Last Updated**: 2025-11-23
> **Version**: 1.0.0
> **QML Module**: org.lxqt.qtxdg 6.0

---

## Table of Contents

- [QML Module Import](#qml-module-import)
- [Core Wrappers](#core-wrappers)
  - [XdgMimeType](#xdgmimetype)
  - [XdgMimeApps](#xdgmimeapps)
  - [XdgMenu](#xdgmenu)
  - [XdgDirs](#xdgdirs)
  - [XdgIcon](#xdgicon)
- [Data Models](#data-models)
  - [XdgApplicationsModel](#xdgapplicationsmodel)
  - [XdgAutoStartModel](#xdgautostartmodel)
  - [XdgMenuTreeModel](#xdgmenutreemodel)
- [Helper Types](#helper-types)
  - [XdgDesktopFile](#xdgdesktopfile)
- [Complete Examples](#complete-examples)

---

## QML Module Import

```qml
import org.lxqt.qtxdg 6.0
```

**Module URI**: `org.lxqt.qtxdg`
**Version**: `6.0`
**Platform**: Linux (XDG standards)

---

## Core Wrappers

### XdgMimeType

**Type**: Singleton
**Purpose**: MIME type detection and information

#### Methods

##### `string mimeTypeForFile(string fileName)`

Detects MIME type for a given file.

**Parameters**:
- `fileName` (string) - Absolute or relative file path

**Returns**: MIME type string (e.g., "image/png", "text/plain")

**Example**:
```qml
var mimeType = XdgMimeType.mimeTypeForFile("/path/to/image.png")
console.log(mimeType)  // "image/png"
```

##### `string comment(string mimeType)`

Returns human-readable description of MIME type.

**Parameters**:
- `mimeType` (string) - MIME type string

**Returns**: Localized description

**Example**:
```qml
var desc = XdgMimeType.comment("image/png")
console.log(desc)  // "PNG image"
```

##### `string genericIconName(string mimeType)`

Returns generic icon name for MIME type.

**Parameters**:
- `mimeType` (string) - MIME type string

**Returns**: Generic icon name (e.g., "image-x-generic")

##### `string icon(string mimeType)`

Returns specific icon name for MIME type.

**Parameters**:
- `mimeType` (string) - MIME type string

**Returns**: Specific icon name (e.g., "image-png")

---

### XdgMimeApps

**Type**: Singleton
**Purpose**: Application-MIME type association

#### Methods

##### `variant appsForMimeType(string mimeType)`

Lists applications that can handle a MIME type.

**Parameters**:
- `mimeType` (string) - MIME type string

**Returns**: Array of desktop file IDs

**Example**:
```qml
var apps = XdgMimeApps.appsForMimeType("image/png")
for (var i = 0; i < apps.length; i++) {
    console.log(apps[i])  // "gimp.desktop", "eog.desktop", etc.
}
```

##### `string defaultAppForMimeType(string mimeType)`

Returns default application for a MIME type.

**Parameters**:
- `mimeType` (string) - MIME type string

**Returns**: Desktop file ID of default app

**Example**:
```qml
var defaultApp = XdgMimeApps.defaultAppForMimeType("text/html")
console.log(defaultApp)  // "firefox.desktop"
```

---

### XdgMenu

**Type**: Regular QML type
**Purpose**: XDG menu system access

#### Methods

##### `bool read(string fileName)`

Reads and parses a menu file.

**Parameters**:
- `fileName` (string) - Path to menu file (e.g., "/etc/xdg/menus/applications.menu")

**Returns**: `true` if successful

##### `variant xml()`

Returns parsed menu as XML DOM document.

**Returns**: QDomDocument (use with XdgMenuTreeModel)

---

### XdgDirs

**Type**: Singleton
**Purpose**: XDG directory standards

#### Methods

##### `string configHome()`

Returns XDG config home directory.

**Returns**: Path (typically `~/.config`)

##### `string dataHome()`

Returns XDG data home directory.

**Returns**: Path (typically `~/.local/share`)

##### `string cacheHome()`

Returns XDG cache home directory.

**Returns**: Path (typically `~/.cache`)

##### `string runtimeDir()`

Returns XDG runtime directory.

**Returns**: Path (typically `/run/user/1000`)

##### `string autostartHome()`

Returns autostart directory.

**Returns**: Path (typically `~/.config/autostart`)

##### `variant autostartDirs()`

Returns list of all autostart directories.

**Returns**: Array of paths

**Example**:
```qml
Text {
    text: "Config: " + XdgDirs.configHome()
    // Output: "Config: /home/user/.config"
}
```

---

### XdgIcon

**Type**: Singleton
**Purpose**: Icon theme support

#### Methods

##### `string fromTheme(string iconName, string fallback = "")`

Loads icon from current theme.

**Parameters**:
- `iconName` (string) - Icon name (e.g., "document-open")
- `fallback` (string, optional) - Fallback icon name

**Returns**: Icon URL for use in QML

**Example**:
```qml
Button {
    icon.source: XdgIcon.fromTheme("document-open", "text-x-generic")
    text: "Open"
}

Image {
    source: XdgIcon.fromTheme("folder")
}
```

---

## Data Models

### XdgApplicationsModel

**Type**: List Model (for ListView/GridView)
**Purpose**: Filterable application list

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `category` | string | Filter by category (e.g., "Development") |
| `searchText` | string | Filter by search text |
| `count` | int | Number of applications (read-only) |

#### Methods

##### `reload()`

Reloads the application list.

##### `variant get(int row)`

Gets application data at index.

**Returns**: Object with application properties

#### Model Roles

| Role | Type | Description |
|------|------|-------------|
| `name` | string | Application name |
| `iconName` | string | Icon name |
| `comment` | string | Description |
| `fileName` | string | Desktop file path |
| `categories` | string | Comma-separated categories |

#### Example

```qml
ListView {
    model: XdgApplicationsModel {
        id: appsModel
        category: "Development"
        searchText: "editor"

        Component.onCompleted: {
            console.log("Found", count, "applications")
        }
    }

    delegate: ItemDelegate {
        text: model.name
        icon.source: XdgIcon.fromTheme(model.iconName)

        ToolTip.text: model.comment
        ToolTip.visible: hovered

        onClicked: {
            // Launch application
            var app = appsModel.get(index)
            console.log("Launching:", app.fileName)
        }
    }
}
```

---

### XdgAutoStartModel

**Type**: List Model (for ListView/GridView)
**Purpose**: Autostart entry management

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `count` | int | Number of autostart entries (read-only) |
| `autostartDirs` | variant | List of autostart directories (read-only) |

#### Methods

##### `reload()`

Reloads the autostart entries.

##### `variant get(int row)`

Gets autostart entry at index.

##### `setEnabled(int row, bool enabled)`

Enables or disables autostart entry.

**Note**: Current implementation is simplified (memory-only).

#### Model Roles

| Role | Type | Description |
|------|------|-------------|
| `name` | string | Entry name |
| `fileName` | string | Desktop file path |
| `iconName` | string | Icon name |
| `comment` | string | Description |
| `enabled` | bool | Whether entry is enabled |

#### Example

```qml
ListView {
    model: XdgAutoStartModel {
        id: autostartModel
    }

    delegate: CheckDelegate {
        text: model.name
        icon.source: XdgIcon.fromTheme(model.iconName)
        checked: model.enabled

        onToggled: {
            autostartModel.setEnabled(index, checked)
        }
    }
}
```

---

### XdgMenuTreeModel

**Type**: Tree Model (for TreeView) ⭐ NEW in v1.0
**Purpose**: Hierarchical menu tree

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `menuFile` | string | Path to menu file |
| `loaded` | bool | Whether menu is loaded (read-only) |
| `loading` | bool | Whether loading is in progress (read-only) |
| `loadTimeMs` | int | Last load time in milliseconds (read-only) |
| `totalApplications` | int | Total number of applications (read-only) |
| `lastError` | string | Last error message (read-only) |

#### Methods

##### `loadAsync()`

Loads menu asynchronously (recommended).

**Performance**: ~78ms total, 1ms UI thread

##### `reload()`

Reloads the menu.

##### `int categoryCount()`

Returns number of top-level categories.

##### `QModelIndex findCategory(string categoryName)`

Finds category by name.

**Returns**: Model index of category

##### `variant applicationsInCategory(string category)`

Returns applications in a category.

**Returns**: Array of application objects

#### Signals

| Signal | Description |
|--------|-------------|
| `loadingStarted()` | Emitted when loading starts |
| `loadingFinished()` | Emitted when loading completes |
| `errorOccurred(string error)` | Emitted on error |

#### Model Roles

| Role | Type | Description |
|------|------|-------------|
| `display` | string | Display name |
| `type` | string | "category" or "application" |
| `desktopId` | string | Desktop file ID (apps only) |
| `iconName` | string | Icon name |
| `categoryName` | string | Category name (categories only) |
| `comment` | string | Description |
| `exec` | string | Exec command (apps only) |
| `genericName` | string | Generic name (apps only) |
| `desktopFilePath` | string | Desktop file path (apps only) |
| `terminal` | bool | Run in terminal (apps only) |
| `startupNotify` | bool | Startup notification (apps only) |
| `path` | string | Working directory (apps only) |

#### Example

```qml
TreeView {
    id: treeView
    model: XdgMenuTreeModel {
        id: menuModel
        menuFile: "/etc/xdg/menus/applications.menu"

        Component.onCompleted: loadAsync()

        onLoadingStarted: {
            busyIndicator.running = true
        }

        onLoadingFinished: {
            busyIndicator.running = false
            console.log("Loaded", totalApplications, "apps in", loadTimeMs, "ms")
        }

        onErrorOccurred: function(error) {
            console.error("Menu load error:", error)
        }
    }

    delegate: TreeViewDelegate {
        required property string display
        required property string type
        required property string iconName

        contentItem: Row {
            Image {
                source: XdgIcon.fromTheme(iconName || "application-x-executable")
                sourceSize.width: 16
                sourceSize.height: 16
            }

            Label {
                text: display
                font.bold: type === "category"
            }
        }

        onDoubleClicked: {
            if (type === "application") {
                // Launch application
                console.log("Launching:", display)
            }
        }
    }
}
```

---

## Helper Types

### XdgDesktopFile

**Type**: Regular QML type
**Purpose**: Desktop file parsing and application launching

#### Properties

| Property | Type | Description |
|----------|------|-------------|
| `fileName` | string | Desktop file path |
| `valid` | bool | Whether file is valid (read-only) |

#### Methods

##### `bool load()`

Loads and parses the desktop file.

**Returns**: `true` if successful

##### `string name()`

Returns application name.

##### `string iconName()`

Returns icon name.

##### `string comment()`

Returns description.

##### `bool startDetached()`

Launches the application in background.

**Returns**: `true` if successful

#### Example

```qml
Button {
    text: "Launch Firefox"

    onClicked: {
        var desktop = Qt.createQmlObject(
            'import org.lxqt.qtxdg 6.0; XdgDesktopFile {}',
            parent,
            'dynamicDesktop'
        )

        desktop.fileName = "/usr/share/applications/firefox.desktop"

        if (desktop.load()) {
            console.log("Name:", desktop.name())
            console.log("Icon:", desktop.iconName())
            desktop.startDetached()
        }
    }
}
```

---

## Complete Examples

### Example 1: Simple Application Launcher

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.lxqt.qtxdg 6.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: "Simple Launcher"

    ColumnLayout {
        anchors.fill: parent

        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: "Search applications..."
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: XdgApplicationsModel {
                searchText: searchField.text
            }

            delegate: ItemDelegate {
                width: ListView.view.width
                text: model.name
                icon.source: XdgIcon.fromTheme(model.iconName)

                ToolTip.text: model.comment
                ToolTip.visible: hovered

                onClicked: {
                    var desktop = Qt.createQmlObject(
                        'import org.lxqt.qtxdg 6.0; XdgDesktopFile {}',
                        parent,
                        'dynamic'
                    )
                    desktop.fileName = model.fileName
                    desktop.load()
                    desktop.startDetached()
                }
            }
        }
    }
}
```

### Example 2: Menu Tree

See [examples/qml-example/MenuTreeExample.qml](../examples/qml-example/MenuTreeExample.qml) for a complete menu tree implementation.

### Example 3: Full Application Launcher

See [examples/qml-example/ApplicationLauncher.qml](../examples/qml-example/ApplicationLauncher.qml) for a complete application launcher with dual views, favorites, and search.

---

## Performance Notes

- **XdgApplicationsModel**: Fast reload (~5ms for 525 apps)
- **XdgMenuTreeModel**:
  - Async load: 78ms (background thread)
  - Tree build: 1ms (UI thread)
  - Category search: 52 nanoseconds
  - Application search: 6.4 microseconds

---

## Platform Notes

- **Linux Only**: XDG specifications are Linux-specific
- **Desktop Environment**: Works with any XDG-compliant desktop (KDE, GNOME, LXQt, etc.)
- **Icon Themes**: Requires XDG icon theme installation

---

## See Also

- [XDG Desktop Entry Specification](https://specifications.freedesktop.org/desktop-entry-spec/)
- [XDG Menu Specification](https://specifications.freedesktop.org/menu-spec/)
- [XDG Icon Theme Specification](https://specifications.freedesktop.org/icon-theme-spec/)
- [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/)

---

*Last Updated: 2025-11-23* | *Version: 1.0.0*
