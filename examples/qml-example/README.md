# Qt6Xdg QML Example

This example demonstrates how to use the Qt6Xdg QML module in a QML application.

## Features Demonstrated

- **XDG Directories**: Access to XDG base directories (data, config, cache, runtime, user directories)
- **XDG Icon Theme**: Loading icons from the system theme
- **Desktop Files**: Loading and launching applications from .desktop files
- **Default Applications**: Query system default applications (browser, file manager, terminal, email)
- **Autostart**: List autostart applications

## Building

Make sure you have built libqtxdg with QML support enabled:

```bash
cd /path/to/libqtxdg
mkdir build && cd build
cmake .. -DBUILD_QML_PLUGIN=ON
make -j4
```

Then build the example:

```bash
cd examples/qml-example
mkdir build && cd build
cmake ..
make
```

## Running

To run the example, you need to set the QML import path to find the Qt6Xdg QML module:

```bash
QML2_IMPORT_PATH=/path/to/libqtxdg/build/qml ./qtxdg-qml-example
```

Or from the libqtxdg build directory:

```bash
QML2_IMPORT_PATH=../../build/qml ./examples/qml-example/build/qtxdg-qml-example
```

## Usage

The example window shows several sections:

1. **XDG Directories**: Displays all standard XDG directories
2. **XDG Icon Theme**: Shows the current icon theme and displays sample icons
3. **Desktop File Example**: Loads and displays information about a desktop file, with a button to launch it
4. **Default Applications**: Lists the system default applications for common categories
5. **Autostart Applications**: Lists applications configured to start automatically

## QML API Examples

### Accessing XDG Directories

```qml
import org.lxqt.qtxdg 6.0

Text {
    text: "Config Home: " + XdgDirs.configHome()
}
```

### Loading Icons

```qml
import org.lxqt.qtxdg 6.0

Image {
    source: XdgIcon.fromTheme("document-open")
}
```

### Working with Desktop Files

```qml
import org.lxqt.qtxdg 6.0

XdgDesktopFile {
    id: desktopFile
    Component.onCompleted: {
        load("/usr/share/applications/firefox.desktop")
    }
}

Button {
    text: "Launch: " + desktopFile.name
    enabled: desktopFile.isValid
    onClicked: desktopFile.startDetached()
}
```

### Querying Default Applications

```qml
import org.lxqt.qtxdg 6.0

Text {
    text: "Default Browser: " + XdgDefaultApps.webBrowser()
}
```
