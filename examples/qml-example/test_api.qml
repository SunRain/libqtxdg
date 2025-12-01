import QtQuick 6.0
import org.deckshell.qtxdg 6.0

Item {
    width: 400
    height: 300
    Component.onCompleted: {
        console.log("========================================")
        console.log("Testing Phase 1 MVP QML Bindings")
        console.log("========================================")
        
        // Test 1: XdgMimeType
        console.log("\n=== Test 1: XdgMimeType ===")
        var testFile = "/usr/share/applications/firefox.desktop"
        var mimeType = XdgMimeType.mimeTypeForFile(testFile)
        console.log("File:", testFile)
        console.log("MIME Type:", mimeType)
        
        if (mimeType) {
            console.log("Comment:", XdgMimeType.comment(mimeType))
            console.log("Icon Name:", XdgMimeType.iconName(mimeType))
            console.log("Generic Icon:", XdgMimeType.genericIconName(mimeType))
            console.log("Glob Patterns:", XdgMimeType.globPatterns(mimeType))
        }
        
        // Test 2: XdgMimeType - text/plain
        console.log("\n=== Test 2: XdgMimeType for text/plain ===")
        var textMime = "text/plain"
        console.log("Comment:", XdgMimeType.comment(textMime))
        console.log("Icon:", XdgMimeType.iconName(textMime))
        console.log("Patterns:", XdgMimeType.globPatterns(textMime))
        console.log("Parent Types:", XdgMimeType.parentMimeTypes(textMime))
        console.log("Inherits text/plain:", XdgMimeType.inherits(textMime, "text/plain"))
        console.log("Inherits application/octet-stream:", XdgMimeType.inherits(textMime, "application/octet-stream"))
        
        // Test 3: XdgMimeApps
        console.log("\n=== Test 3: XdgMimeApps ===")
        var apps = XdgMimeApps.appsForMimeType("text/plain")
        console.log("Apps for text/plain:", apps.length)
        if (apps.length > 0) {
            console.log("First app:", apps[0].name, "Icon:", apps[0].iconName)
            console.log("Valid:", apps[0].isValid, "File:", apps[0].fileName)
        }
        
        var defaultApp = XdgMimeApps.defaultApp("text/plain")
        console.log("Default app for text/plain:", defaultApp)
        
        // Test 4: XdgMimeApps - all apps
        console.log("\n=== Test 4: XdgMimeApps.allApps() ===")
        var allApps = XdgMimeApps.allApps()
        console.log("Total applications:", allApps.length)
        
        // Test 5: XdgMimeApps - category
        console.log("\n=== Test 5: XdgMimeApps.categoryApps() ===")
        var devApps = XdgMimeApps.categoryApps("Development")
        console.log("Development apps:", devApps.length)
        if (devApps.length > 0) {
            console.log("Example:", devApps[0].name)
        }
        
        // Test 6: XdgMenu
        console.log("\n=== Test 6: XdgMenu ===")
        var menuFile = XdgMenu.defaultMenuFile()
        console.log("Default menu file:", menuFile)
        
        if (menuFile) {
            var loaded = XdgMenu.loadMenu(menuFile)
            console.log("Menu loaded:", loaded)
            
            if (loaded) {
                console.log("Menu is loaded:", XdgMenu.isLoaded)
                var categories = XdgMenu.allCategories()
                console.log("Categories count:", categories.length)
                console.log("Categories:", categories.join(", "))
                
                // Try to get applications
                var menuApps = XdgMenu.applications()
                console.log("Applications in menu:", menuApps.length)
            } else {
                console.log("Error:", XdgMenu.lastError)
            }
        }
        
        // Test 7: XdgIcon helper
        console.log("\n=== Test 7: XdgIcon.toImageUrl() ===")
        var iconUrl = XdgIcon.toImageUrl("firefox")
        console.log("Icon URL for 'firefox':", iconUrl)
        console.log("Icon theme:", XdgIcon.themeName)
        
        // Test 8: Error handling
        console.log("\n=== Test 8: Error Handling ===")
        var invalidMime = XdgMimeType.mimeTypeForFile("/nonexistent/file.xyz")
        console.log("Invalid file MIME:", invalidMime)
        console.log("Last error:", XdgMimeType.lastError)
        
        console.log("\n========================================")
        console.log("All tests completed!")
        console.log("========================================")
        
        // Exit after tests
        Qt.quit()
    }
}
