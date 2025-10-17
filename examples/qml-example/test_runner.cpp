#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Set QML import path to find our module
    QString qmlModulePath = QStringLiteral(QML_MODULE_BUILD_DIR);
    qputenv("QML_IMPORT_PATH", qmlModulePath.toUtf8());
    
    qDebug() << "QML module path:" << qmlModulePath;

    QQmlApplicationEngine engine;
    
    // Load the test QML file
    const QUrl url(QStringLiteral("qrc:/test_api.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
