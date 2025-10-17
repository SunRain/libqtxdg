#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QDir>
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    // Set Qt Quick Controls style before creating QGuiApplication
    // This avoids Breeze style compatibility issues
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    QGuiApplication app(argc, argv);

    app.setApplicationName(QStringLiteral("QtXdg QML Example"));
    app.setOrganizationName(QStringLiteral("LXQt"));

    QQmlApplicationEngine engine;

    // Add QML import path for development build
    // Calculate the path relative to the executable location
    QString appDir = QCoreApplication::applicationDirPath();
    QString qmlModulePath = QDir(appDir).filePath(QStringLiteral("../../qml"));
    QString cleanPath = QDir::cleanPath(qmlModulePath);
    qDebug() << "Application directory:" << appDir;
    qDebug() << "QML module path:" << cleanPath;
    engine.addImportPath(cleanPath);

    // Load the main QML file
    const QUrl url(QStringLiteral("qrc:/QtXdgQmlExample/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
