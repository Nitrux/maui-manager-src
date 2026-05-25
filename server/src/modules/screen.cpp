#include "screen.h"
#include "screenadaptor.h"
#include <QDBusConnection>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QScreen>
#include <QtGlobal>

Q_LOGGING_CATEGORY(LOG_SCREEN_SERVER, "mauiman.server.screen")

Screen::Screen(QObject *parent) : QObject(parent)
{
    qCInfo(LOG_SCREEN_SERVER, "INIT SCREEN MODULE");
    new ScreenAdaptor(this);
    if(!QDBusConnection::sessionBus().registerObject(QStringLiteral("/Screen"), this))
    {
        qCWarning(LOG_SCREEN_SERVER) << "FAILED TO REGISTER SCREEN DBUS OBJECT";
        return;
    }

    connect(qGuiApp, &QGuiApplication::primaryScreenChanged, this, &Screen::setPrimaryScreen);
    setPrimaryScreen(QGuiApplication::primaryScreen());
}

double Screen::scaleFactor() const
{
    return m_scaleFactor;
}

uint Screen::orientation() const
{
    return m_orientation;
}

void Screen::setPrimaryScreen(QScreen *screen)
{
    if (m_primaryScreen == screen)
    {
        refreshFromPrimaryScreen();
        return;
    }

    disconnect(m_orientationChangedConnection);
    disconnect(m_geometryChangedConnection);
    disconnect(m_logicalDpiChangedConnection);
    disconnect(m_physicalDpiChangedConnection);

    m_primaryScreen = screen;
    if (m_primaryScreen)
    {
        m_orientationChangedConnection = connect(m_primaryScreen, &QScreen::orientationChanged, this, [this](Qt::ScreenOrientation)
        {
            refreshFromPrimaryScreen();
        });
        m_geometryChangedConnection = connect(m_primaryScreen, &QScreen::geometryChanged, this, [this](const QRect &)
        {
            refreshFromPrimaryScreen();
        });
        m_logicalDpiChangedConnection = connect(m_primaryScreen, &QScreen::logicalDotsPerInchChanged, this, [this](qreal)
        {
            refreshFromPrimaryScreen();
        });
        m_physicalDpiChangedConnection = connect(m_primaryScreen, &QScreen::physicalDotsPerInchChanged, this, [this](qreal)
        {
            refreshFromPrimaryScreen();
        });
    }

    refreshFromPrimaryScreen();
}

void Screen::refreshFromPrimaryScreen()
{
    if (!m_primaryScreen)
    {
        return;
    }

    const double nextScaleFactor = m_primaryScreen->devicePixelRatio();
    if (!qFuzzyCompare(1.0 + m_scaleFactor, 1.0 + nextScaleFactor))
    {
        m_scaleFactor = nextScaleFactor;
        Q_EMIT scaleFactorChanged(m_scaleFactor);
    }

    const uint nextOrientation = static_cast<uint>(m_primaryScreen->orientation());
    if (m_orientation != nextOrientation)
    {
        m_orientation = nextOrientation;
        Q_EMIT orientationChanged(m_orientation);
    }
}
