#include "formfactormanager.h"

#include "settingsstore.h"
#include "mauimanutils.h"

#include <QDebug>
#include <QScreen>
#include <QGuiApplication>

#include <QDBusInterface>

#include <QInputDevice>

using namespace MauiMan;


void FormFactorManager::sync(const QString &key, const QVariant &value)
{
    if (m_interface && m_interface->isValid())
    {
        m_interface->call(key, value);
    }
}

void FormFactorManager::setConnections()
{
    if(m_interface)
    {
        m_interface->disconnect();
        m_interface->deleteLater();
        m_interface = nullptr;
    }

    m_interface = new QDBusInterface(QStringLiteral("org.mauiman.Manager"),
                                     QStringLiteral("/FormFactor"),
                                     QStringLiteral("org.mauiman.FormFactor"),
                                     QDBusConnection::sessionBus(), this);

    if (m_interface->isValid())
    {
        connect(m_interface, SIGNAL(preferredModeChanged(uint)), this, SLOT(onPreferredModeChanged(uint)));
        connect(m_interface, SIGNAL(forceTouchScreenChanged(bool)), this, SLOT(onForceTouchScreenChanged(bool)));
    }
}

void FormFactorManager::loadSettings()
{
    m_settings->beginModule(QStringLiteral("FormFactor"));

    if(m_interface && m_interface->isValid())
    {
        m_preferredMode = m_interface->property("preferredMode").toUInt();
        m_forceTouchScreen = m_interface->property("forceTouchScreen").toBool();
        return;
    }

    m_preferredMode = m_settings->load(QStringLiteral("PreferredMode"), m_preferredMode).toUInt();
    m_forceTouchScreen = m_settings->load(QStringLiteral("ForceTouchScreen"), m_forceTouchScreen).toBool();
}

FormFactorManager::FormFactorManager(QObject *parent) : MauiMan::FormFactorInfo(parent)
  ,m_settings(new MauiMan::SettingsStore(this))
{
    qDebug( " INIT FORMFACTOR MANAGER");

    auto server = new MauiManUtils(this);
    if(server->serverRunning())
    {
        this->setConnections();
    }

    connect(server, &MauiManUtils::serverRunningChanged, [this](bool state)
    {
        if(state)
        {
            this->setConnections();
        }
    });
    m_preferredMode = defaultMode();

    loadSettings();
}

uint FormFactorManager::preferredMode() const
{
    return m_preferredMode;
}

uint FormFactorInfo::bestMode() const
{
    return m_bestMode;
}

uint FormFactorInfo::defaultMode() const
{
    return m_defaultMode;
}

bool FormFactorInfo::hasKeyboard() const
{
    return m_hasKeyboard;
}

bool FormFactorInfo::hasTouchscreen() const
{
    return m_hasTouchscreen;
}

bool FormFactorInfo::hasMouse() const
{
    return m_hasMouse;
}

bool FormFactorInfo::hasTouchpad() const
{
    return m_hasTouchpad;
}

void FormFactorManager::setPreferredMode(uint preferredMode)
{
    if (m_preferredMode == preferredMode)
        return;

    m_preferredMode = preferredMode;

    sync(QStringLiteral("setPreferredMode"), m_preferredMode);
    m_settings->save(QStringLiteral("PreferredMode"), m_preferredMode);

    Q_EMIT preferredModeChanged(m_preferredMode);
}

bool FormFactorManager::forceTouchScreen() const
{
    return m_forceTouchScreen;
}

void FormFactorManager::setForceTouchScreen(bool newForceTouchScreen)
{
    if (m_forceTouchScreen == newForceTouchScreen)
        return;

    m_forceTouchScreen = newForceTouchScreen;

    sync(QStringLiteral("setForceTouchScreen"), m_forceTouchScreen);
    m_settings->save(QStringLiteral("ForceTouchScreen"), m_forceTouchScreen);

    Q_EMIT forceTouchScreenChanged(m_forceTouchScreen);
}

void FormFactorManager::onPreferredModeChanged(uint preferredMode)
{
    if (m_preferredMode == preferredMode)
        return;

    m_preferredMode = preferredMode;
    Q_EMIT preferredModeChanged(m_preferredMode);
}

void FormFactorManager::onForceTouchScreenChanged(bool value)
{
    if (m_forceTouchScreen == value)
        return;

    m_forceTouchScreen = value;
    Q_EMIT forceTouchScreenChanged(m_forceTouchScreen);
}

void FormFactorInfo::findBestMode()
{
    uint bestMode = m_defaultMode;
    const QRect currentScreenSize = screenSize();
    /*
     * 0- desktop
     * 1- tablet
     * 2- phone
     * */

    if(m_hasTouchscreen)
    {
        if(currentScreenSize.width() > 1500)
        {
            if(m_hasKeyboard || m_hasMouse || m_hasTouchpad)
            {
                bestMode = 0; //A big touch screen and with keyboard/mouse/trackpad
            }else
            {
                bestMode = 1; //A big touch screen alone
            }
        }
        else if(currentScreenSize.width()  > 500)
        {
            bestMode = 1; //A tablet size touch screen
        }
        else
        {
            bestMode = 2; //A mobile size touch screen
        }

    }else
    {

        if(currentScreenSize.width() > 1500)
        {
            bestMode = 0; // A big screen

        }
        else if(currentScreenSize.width()  > 500)
        {
            if(m_hasTouchpad)
            {
                bestMode = 1; // A small screen with a trackpad
            }else
            {
                bestMode = 0;
            }
        }
        else
        {
            bestMode = 1; //A mobile size touch screen
        }
    }

    m_bestMode = bestMode;
    Q_EMIT bestModeChanged(m_bestMode);
}

QRect FormFactorInfo::screenSize()
{
    QScreen *screen = qApp->primaryScreen();
    return screen->geometry();
}

Qt::ScreenOrientation FormFactorInfo::screenOrientation()
{
    QScreen *screen = qApp->primaryScreen();
    return screen->orientation();
}

void FormFactorInfo::checkInputs(const QList<const QInputDevice *> &devices)
{
    for (const auto &dev : devices) {
        qDebug() << "DEVICE:::" << dev->type();
    }

    auto hasType = [devices](QInputDevice::DeviceType type) -> bool
    {
        qDebug() << "CHECXKING IF DEVICE HAS TYPE" << type;
        auto res= std::find_if(devices.constBegin(), devices.constEnd(), [type](const QInputDevice *device)
        {
            return device->type() == type;
        });

        return res != std::end(devices);
    };

    m_hasKeyboard = QInputDevice::primaryKeyboard() ? true :  false;
    m_hasMouse =  hasType(QInputDevice::DeviceType::Mouse);
    m_hasTouchscreen =  hasType(QInputDevice::DeviceType::TouchScreen);
    m_hasTouchpad =  hasType(QInputDevice::DeviceType::TouchPad);

    qDebug() << "CHECXKING IF DEVICE HAS TYPE" << m_hasKeyboard;


    Q_EMIT hasKeyboardChanged(m_hasKeyboard);
    Q_EMIT hasMouseChanged(m_hasMouse);
    Q_EMIT hasTouchscreenChanged(m_hasTouchscreen);
    Q_EMIT hasTouchpadChanged(m_hasTouchpad);
}

FormFactorInfo::FormFactorInfo(QObject *parent) : QObject(parent)
{
    qDebug( "INIT FORMFACTOR INFO");

    checkInputs(QInputDevice::devices());
qDebug() << "HAS KEYBOARD?" << QInputDevice::primaryKeyboard();
    findBestMode();

}
