/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2025
 * Authors:
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef QTXDGQML_XDGDESKTOPFILEWRAPPER_H
#define QTXDGQML_XDGDESKTOPFILEWRAPPER_H

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QUrl>
#include <XdgDesktopFile>

/*!
 * \brief QML wrapper for XdgDesktopFile
 *
 * This class provides QML access to Desktop Entry Specification
 * functionality. Each instance wraps an XdgDesktopFile object.
 */
class XdgDesktopFileWrapper : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    // Desktop Entry Type
    Q_PROPERTY(Type type READ type NOTIFY typeChanged)

    // Common properties
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString comment READ comment NOTIFY commentChanged)
    Q_PROPERTY(QUrl iconUrl READ iconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)

    // Application-specific properties
    Q_PROPERTY(QStringList mimeTypes READ mimeTypes NOTIFY mimeTypesChanged)
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(QStringList actions READ actions NOTIFY actionsChanged)

    // Link-specific property
    Q_PROPERTY(QString url READ url NOTIFY urlChanged)

public:
    enum Type {
        UnknownType = 0,
        ApplicationType,
        LinkType,
        DirectoryType
    };
    Q_ENUM(Type)

    explicit XdgDesktopFileWrapper(QObject *parent = nullptr);
    ~XdgDesktopFileWrapper() override;

    // Loading and saving
    Q_INVOKABLE bool load(const QString &fileName);
    Q_INVOKABLE bool save(const QString &fileName);

    // Value access
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    Q_INVOKABLE QVariant localizedValue(const QString &key, const QVariant &defaultValue = QVariant()) const;
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);
    Q_INVOKABLE void setLocalizedValue(const QString &key, const QVariant &value);
    Q_INVOKABLE void removeEntry(const QString &key);
    Q_INVOKABLE bool contains(const QString &key) const;

    // Application execution
    Q_INVOKABLE bool startDetached(const QStringList &urls = QStringList());
    Q_INVOKABLE bool startDetachedUrl(const QString &url = QString());
    Q_INVOKABLE bool actionActivate(const QString &action, const QStringList &urls = QStringList());
    Q_INVOKABLE QStringList expandExecString(const QStringList &urls = QStringList()) const;

    // Action names
    Q_INVOKABLE QString actionName(const QString &action) const;
    Q_INVOKABLE QUrl actionIconUrl(const QString &action) const;
    Q_INVOKABLE QString actionIconName(const QString &action) const;

    // Visibility control
    Q_INVOKABLE bool isShown(const QString &environment = QString()) const;
    Q_INVOKABLE bool isSuitable(bool excludeHidden = true, const QString &environment = QString()) const;
    Q_INVOKABLE bool tryExec() const;

    // Static methods
    Q_INVOKABLE static QString computeId(const QString &fileName, bool checkFileExists = true);

    // Property getters
    Type type() const;
    QString name() const;
    QString comment() const;
    QUrl iconUrl() const;
    QString iconName() const;
    QString fileName() const;
    bool isValid() const;
    QStringList mimeTypes() const;
    QStringList categories() const;
    QStringList actions() const;
    QString url() const;

Q_SIGNALS:
    void typeChanged();
    void nameChanged();
    void commentChanged();
    void iconUrlChanged();
    void iconNameChanged();
    void fileNameChanged();
    void isValidChanged();
    void mimeTypesChanged();
    void categoriesChanged();
    void actionsChanged();
    void urlChanged();

private:
    XdgDesktopFile m_desktopFile;
    void emitAllChanged();
};

#endif // QTXDGQML_XDGDESKTOPFILEWRAPPER_H
