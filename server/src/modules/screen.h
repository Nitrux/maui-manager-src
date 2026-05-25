#pragma once

#include <QMetaObject>
#include <QObject>
#include "modules/screenmanager.h"

class QScreen;

class Screen : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double scaleFactor READ scaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(uint orientation READ orientation NOTIFY orientationChanged)

public:
    explicit Screen(QObject *parent = nullptr);

    double scaleFactor() const;
    uint orientation() const;

Q_SIGNALS:
    void scaleFactorChanged(double scaleFactor);
    void orientationChanged(uint orientation);

private:
    void setPrimaryScreen(QScreen *screen);
    void refreshFromPrimaryScreen();

    QScreen *m_primaryScreen = nullptr;
    QMetaObject::Connection m_orientationChangedConnection;
    QMetaObject::Connection m_geometryChangedConnection;
    QMetaObject::Connection m_logicalDpiChangedConnection;
    QMetaObject::Connection m_physicalDpiChangedConnection;

    double m_scaleFactor = MauiMan::ScreenManager::DefaultValues::scaleFactor;
    uint m_orientation = MauiMan::ScreenManager::DefaultValues::orientation;
};
