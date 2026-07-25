#include "thememanager.h"

#include "theme_interface.h"
#include "settingsstore.h"
#include "mauimanutils.h"

#include <QDebug>
#include <QDBusPendingReply>

using namespace MauiMan;

ThemeManager::ThemeManager(QObject *parent) : QObject(parent)
  ,m_settings(new MauiMan::SettingsStore(this))
{
    auto server = MauiManUtils::instance();
    if (server->serverRunning())
        this->setConnections();

    connect(server, &MauiManUtils::serverRunningChanged, this, [this](bool state) {
        if (state)
            this->setConnections();
    });

    loadSettings();
}

bool ThemeManager::sync(const QString &key, const QVariant &value)
{
    if (m_interface && m_interface->isValid())
    {
        auto finishCall = [key](QDBusPendingReply<> reply) {
            reply.waitForFinished();
            if (!reply.isError())
            {
                return true;
            }

            qWarning() << "ThemeManager::sync failed for call" << key << ":" << reply.error().message();
            return false;
        };

        if (key == QStringLiteral("setStyleType"))
        {
            return finishCall(m_interface->setStyleType(value.toInt()));
        } else if (key == QStringLiteral("setAccentColor"))
        {
            return finishCall(m_interface->setAccentColor(value.toString()));
        } else if (key == QStringLiteral("setIconTheme"))
        {
            return finishCall(m_interface->setIconTheme(value.toString()));
        } else if (key == QStringLiteral("setWindowControlsTheme"))
        {
            return finishCall(m_interface->setWindowControlsTheme(value.toString()));
        } else if (key == QStringLiteral("setEnableCSD"))
        {
            return finishCall(m_interface->setEnableCSD(value.toBool()));
        } else if (key == QStringLiteral("setBorderRadius"))
        {
            return finishCall(m_interface->setBorderRadius(value.toUInt()));
        } else if (key == QStringLiteral("setIconSize"))
        {
            return finishCall(m_interface->setIconSize(value.toUInt()));
        } else if (key == QStringLiteral("setEnableEffects"))
        {
            return finishCall(m_interface->setEnableEffects(value.toBool()));
        } else if (key == QStringLiteral("setPaddingSize"))
        {
            return finishCall(m_interface->setPaddingSize(value.toUInt()));
        } else if (key == QStringLiteral("setMarginSize"))
        {
            return finishCall(m_interface->setMarginSize(value.toUInt()));
        } else if (key == QStringLiteral("setSpacingSize"))
        {
            return finishCall(m_interface->setSpacingSize(value.toUInt()));
        } else if (key == QStringLiteral("setDefaultFont"))
        {
            return finishCall(m_interface->setDefaultFont(value.toString()));
        } else if (key == QStringLiteral("setSmallFont"))
        {
            return finishCall(m_interface->setSmallFont(value.toString()));
        } else if (key == QStringLiteral("setMonospacedFont"))
        {
            return finishCall(m_interface->setMonospacedFont(value.toString()));
        } else if (key == QStringLiteral("setCustomColorScheme"))
        {
            return finishCall(m_interface->setCustomColorScheme(value.toString()));
        } else if (key == QStringLiteral("setAllowCustomStyling"))
        {
            return finishCall(m_interface->setAllowCustomStyling(value.toBool()));
        }

        qWarning() << "ThemeManager::sync received unknown method key:" << key;
    }

    return false;
}

void ThemeManager::setConnections()
{
    if(m_interface)
    {
        m_interface->disconnect();
        m_interface->deleteLater();
        m_interface = nullptr;
    }

    m_interface = new OrgMauimanThemeInterface(QStringLiteral("org.mauiman.Manager"),
                                               QStringLiteral("/Theme"),
                                               QDBusConnection::sessionBus(),
                                               this);

    if (m_interface->isValid())
    {
        connect(m_interface, &OrgMauimanThemeInterface::accentColorChanged, this, &ThemeManager::onAccentColorChanged);
        connect(m_interface, &OrgMauimanThemeInterface::iconThemeChanged, this, &ThemeManager::onIconThemeChanged);
        connect(m_interface, &OrgMauimanThemeInterface::windowControlsThemeChanged, this, &ThemeManager::onWindowControlsThemeChanged);
        connect(m_interface, &OrgMauimanThemeInterface::styleTypeChanged, this, &ThemeManager::onStyleTypeChanged);
        connect(m_interface, &OrgMauimanThemeInterface::enableCSDChanged, this, &ThemeManager::onEnableCSDChanged);
        connect(m_interface, &OrgMauimanThemeInterface::borderRadiusChanged, this, &ThemeManager::onBorderRadiusChanged);
        connect(m_interface, &OrgMauimanThemeInterface::iconSizeChanged, this, &ThemeManager::onIconSizeChanged);
        connect(m_interface, &OrgMauimanThemeInterface::paddingSizeChanged, this, &ThemeManager::onPaddingSizeChanged);
        connect(m_interface, &OrgMauimanThemeInterface::marginSizeChanged, this, &ThemeManager::onMarginSizeChanged);
        connect(m_interface, &OrgMauimanThemeInterface::spacingSizeChanged, this, &ThemeManager::onSpacingSizeChanged);
        connect(m_interface, &OrgMauimanThemeInterface::enableEffectsChanged, this, &ThemeManager::onEnableEffectsChanged);
        connect(m_interface, &OrgMauimanThemeInterface::defaultFontChanged, this, &ThemeManager::onDefaultFontChanged);
        connect(m_interface, &OrgMauimanThemeInterface::smallFontChanged, this, &ThemeManager::onSmallFontChanged);
        connect(m_interface, &OrgMauimanThemeInterface::monospacedFontChanged, this, &ThemeManager::onMonospacedFontChanged);
        connect(m_interface, &OrgMauimanThemeInterface::customColorSchemeChanged, this, &ThemeManager::onCustomColorSchemeChanged);
        connect(m_interface, &OrgMauimanThemeInterface::allowCustomStylingChanged, this, &ThemeManager::onAllowCustomStylingChanged);
    }
}

void ThemeManager::loadSettings()
{
    m_settings->beginModule(QStringLiteral("Theme"));

    if(m_interface && m_interface->isValid())
    {
        // When server is available, DBus properties are the source of truth.
        m_accentColor = QColor(m_interface->property("accentColor").toString());
        m_styleType = m_interface->property("styleType").toInt();
        m_iconTheme = m_interface->property("iconTheme").toString();
        m_windowControlsTheme = m_interface->property("windowControlsTheme").toString();
        m_enableCSD = m_interface->property("enableCSD").toBool();
        m_borderRadius = m_interface->property("borderRadius").toUInt();
        m_iconSize = m_interface->property("iconSize").toUInt();
        m_paddingSize = m_interface->property("paddingSize").toUInt();
        m_marginSize = m_interface->property("marginSize").toUInt();
        m_spacingSize = m_interface->property("spacingSize").toUInt();
        m_enableEffects = m_interface->property("enableEffects").toBool();
        m_defaultFont = m_interface->property("defaultFont").toString();
        m_smallFont = m_interface->property("smallFont").toString();
        m_monospacedFont = m_interface->property("monospacedFont").toString();
        m_customColorScheme = m_interface->property("customColorScheme").toString();

        m_settings->endModule();
        return;
    }

    // Offline fallback: use cached local settings until DBus service appears.
    m_accentColor = QColor(m_settings->load(QStringLiteral("AccentColor"), m_accentColor.name()).toString());
    m_styleType = m_settings->load(QStringLiteral("StyleType"), m_styleType).toInt();
    m_iconTheme = m_settings->load(QStringLiteral("IconTheme"), m_iconTheme).toString();
    m_windowControlsTheme = m_settings->load(QStringLiteral("WindowControlsTheme"), m_windowControlsTheme).toString();
    m_enableCSD = m_settings->load(QStringLiteral("EnableCSD"), m_enableCSD).toBool();
    m_borderRadius = m_settings->load(QStringLiteral("BorderRadius"), m_borderRadius).toUInt();
    m_iconSize = m_settings->load(QStringLiteral("IconSize"), m_iconSize).toUInt();
    m_paddingSize = m_settings->load(QStringLiteral("PaddingSize"), m_paddingSize).toUInt();
    m_marginSize = m_settings->load(QStringLiteral("MarginSize"), m_marginSize).toUInt();
    m_spacingSize = m_settings->load(QStringLiteral("SpacingSize"), m_spacingSize).toUInt();
    m_enableEffects = m_settings->load(QStringLiteral("EnableEffects"), m_enableEffects).toBool();
    m_defaultFont = m_settings->load(QStringLiteral("DefaultFont"), m_defaultFont).toString();
    m_smallFont = m_settings->load(QStringLiteral("SmallFont"), m_smallFont).toString();
    m_monospacedFont = m_settings->load(QStringLiteral("MonospacedFont"), m_monospacedFont).toString();
    m_customColorScheme = m_settings->load(QStringLiteral("CustomColorScheme"), m_customColorScheme).toString();
    m_settings->endModule();
}

int ThemeManager::styleType() const
{
    return m_styleType;
}

void ThemeManager::setStyleType(int newStyleType)
{
    if (m_styleType == newStyleType)
        return;

    m_styleType = newStyleType;
    if (!sync(QStringLiteral("setStyleType"), newStyleType))
    {
        // Persist locally only when DBus write fails/unavailable.
        m_settings->save(QStringLiteral("StyleType"), m_styleType);
    }
    Q_EMIT styleTypeChanged(m_styleType);
}

QColor ThemeManager::accentColor() const
{
    return m_accentColor;
}

void ThemeManager::setAccentColor(const QColor &newAccentColor)
{
    if (m_accentColor == newAccentColor)
        return;

    m_accentColor = newAccentColor;
    if (!sync(QStringLiteral("setAccentColor"), m_accentColor.name()))
    {
        m_settings->save(QStringLiteral("AccentColor"), m_accentColor.name());
    }
    Q_EMIT accentColorChanged(m_accentColor);
}

void ThemeManager::resetAccentColor()
{
    this->setAccentColor(QColor(ThemeManager::DefaultValues::accentColor));
}

const QString &ThemeManager::iconTheme() const
{
    return m_iconTheme;
}

void ThemeManager::setIconTheme(const QString &newIconTheme)
{
    if (m_iconTheme == newIconTheme)
        return;

    m_iconTheme = newIconTheme;
    if (!sync(QStringLiteral("setIconTheme"), m_iconTheme))
    {
        m_settings->save(QStringLiteral("IconTheme"), m_iconTheme);
    }
    Q_EMIT iconThemeChanged(m_iconTheme);
}

const QString &ThemeManager::windowControlsTheme() const
{
    return m_windowControlsTheme;
}

void ThemeManager::setWindowControlsTheme(const QString &newWindowControlsTheme)
{
    if (m_windowControlsTheme == newWindowControlsTheme)
        return;

    m_windowControlsTheme = newWindowControlsTheme;
    if (!sync(QStringLiteral("setWindowControlsTheme"), m_windowControlsTheme))
    {
        m_settings->save(QStringLiteral("WindowControlsTheme"), m_windowControlsTheme);
    }
    Q_EMIT windowControlsThemeChanged(m_windowControlsTheme);
}

bool ThemeManager::enableCSD() const
{
    return m_enableCSD;
}

void ThemeManager::setEnableCSD(bool enableCSD)
{
    if (m_enableCSD == enableCSD)
        return;

    m_enableCSD = enableCSD;
    if (!sync(QStringLiteral("setEnableCSD"), m_enableCSD))
    {
        m_settings->save(QStringLiteral("EnableCSD"), m_enableCSD);
    }
    Q_EMIT enableCSDChanged(m_enableCSD);
}

void ThemeManager::onStyleTypeChanged(const int &newStyleType)
{
    if (m_styleType == newStyleType)
        return;

    m_styleType = newStyleType;
    Q_EMIT styleTypeChanged(m_styleType);
}

void ThemeManager::onAccentColorChanged(const QString &newAccentColor)
{
    const QColor incoming(newAccentColor);
    if (m_accentColor == incoming)
        return;

    m_accentColor = incoming;
    Q_EMIT accentColorChanged(m_accentColor);
}

void ThemeManager::onWindowControlsThemeChanged(const QString &newWindowControlsTheme)
{
    if (m_windowControlsTheme == newWindowControlsTheme)
        return;

    m_windowControlsTheme = newWindowControlsTheme;
    Q_EMIT windowControlsThemeChanged(m_windowControlsTheme);
}

void ThemeManager::onIconThemeChanged(const QString &newIconTheme)
{
    if (m_iconTheme == newIconTheme)
        return;

    m_iconTheme = newIconTheme;
    Q_EMIT iconThemeChanged(m_iconTheme);
}

void ThemeManager::onEnableCSDChanged(const bool &enableCSD)
{
    if (m_enableCSD == enableCSD)
        return;

    m_enableCSD = enableCSD;
    Q_EMIT enableCSDChanged(m_enableCSD);
}

void ThemeManager::onAllowCustomStylingChanged(bool allowCustomStyling)
{
    if (m_allowCustomStyling == allowCustomStyling)
        return;

    m_allowCustomStyling = allowCustomStyling;
    Q_EMIT allowCustomStylingChanged(m_allowCustomStyling);
}

void ThemeManager::onBorderRadiusChanged(const uint &radius)
{
    if (m_borderRadius == radius)
        return;
    m_borderRadius = radius;
    Q_EMIT borderRadiusChanged(m_borderRadius);
}

void ThemeManager::onIconSizeChanged(const uint &size)
{
    if (m_iconSize == size)
        return;
    m_iconSize = size;
    Q_EMIT iconSizeChanged(m_iconSize);
}

void ThemeManager::onPaddingSizeChanged(const uint &paddingSize)
{
    if (m_paddingSize == paddingSize)
        return;

    m_paddingSize = paddingSize;
    Q_EMIT paddingSizeChanged(m_paddingSize);
}

void ThemeManager::onMarginSizeChanged(const uint &marginSize)
{
    if (m_marginSize == marginSize)
        return;

    m_marginSize = marginSize;
    Q_EMIT marginSizeChanged(m_marginSize);
}

void ThemeManager::onSpacingSizeChanged(const uint &spacingSize)
{
    if (m_spacingSize == spacingSize)
        return;

    m_spacingSize = spacingSize;
    Q_EMIT spacingSizeChanged(m_spacingSize);
}

void ThemeManager::onEnableEffectsChanged(bool enableEffects)
{
    if (m_enableEffects == enableEffects)
        return;

    m_enableEffects = enableEffects;
    Q_EMIT enableEffectsChanged(m_enableEffects);
}

void ThemeManager::onDefaultFontChanged(const QString &font)
{
    if (m_defaultFont == font)
        return;

    m_defaultFont = font;
    Q_EMIT defaultFontChanged(m_defaultFont);
}

void ThemeManager::onSmallFontChanged(const QString &font)
{
    if (m_smallFont == font)
        return;

    m_smallFont = font;
    Q_EMIT smallFontChanged(m_smallFont);
}

void ThemeManager::onMonospacedFontChanged(const QString &font)
{
    if (m_monospacedFont == font)
        return;

    m_monospacedFont = font;
    Q_EMIT monospacedFontChanged(m_monospacedFont);
}

void ThemeManager::onCustomColorSchemeChanged(const QString &scheme)
{
    if (m_customColorScheme == scheme)
        return;

    m_customColorScheme = scheme;
    Q_EMIT customColorSchemeChanged(m_customColorScheme);
}

uint ThemeManager::borderRadius() const
{
    return m_borderRadius;
}

void ThemeManager::setBorderRadius(uint newBorderRadius)
{
    if (m_borderRadius == newBorderRadius)
        return;
    m_borderRadius = newBorderRadius;
    if (!sync(QStringLiteral("setBorderRadius"), m_borderRadius))
    {
        m_settings->save(QStringLiteral("BorderRadius"), m_borderRadius);
    }
    Q_EMIT borderRadiusChanged(m_borderRadius);
}

void ThemeManager::resetBorderRadius()
{
    this->setBorderRadius(ThemeManager::DefaultValues::borderRadius);
}

uint ThemeManager::iconSize() const
{
    return m_iconSize;
}

void ThemeManager::setIconSize(uint newIconSize)
{
    if (m_iconSize == newIconSize)
        return;
    m_iconSize = newIconSize;
    if (!sync(QStringLiteral("setIconSize"), m_iconSize))
    {
        m_settings->save(QStringLiteral("IconSize"), m_iconSize);
    }
    Q_EMIT iconSizeChanged(m_iconSize);
}

bool ThemeManager::enableEffects() const
{
    return m_enableEffects;
}

void ThemeManager::setEnableEffects(bool enableEffects)
{
    if (m_enableEffects == enableEffects)
        return;

    m_enableEffects = enableEffects;
    if (!sync(QStringLiteral("setEnableEffects"), m_enableEffects))
    {
        m_settings->save(QStringLiteral("EnableEffects"), m_enableEffects);
    }
    Q_EMIT enableEffectsChanged(m_enableEffects);
}

uint ThemeManager::paddingSize() const
{
    return m_paddingSize;
}

uint ThemeManager::marginSize() const
{
    return m_marginSize;
}

void ThemeManager::setPaddingSize(uint paddingSize)
{
    if (m_paddingSize == paddingSize)
        return;

    m_paddingSize = paddingSize;
    if (!sync(QStringLiteral("setPaddingSize"), m_paddingSize))
    {
        m_settings->save(QStringLiteral("PaddingSize"), m_paddingSize);
    }
    Q_EMIT paddingSizeChanged(m_paddingSize);
}

void ThemeManager::resetPaddingSize()
{
    this->setPaddingSize(ThemeManager::DefaultValues::paddingSize);
}

void ThemeManager::setMarginSize(uint marginSize)
{
    if (m_marginSize == marginSize)
        return;

    m_marginSize = marginSize;
    if (!sync(QStringLiteral("setMarginSize"), m_marginSize))
    {
        m_settings->save(QStringLiteral("MarginSize"), m_marginSize);
    }
    Q_EMIT marginSizeChanged(m_marginSize);
}

void ThemeManager::resetMarginSize()
{
    this->setMarginSize(ThemeManager::DefaultValues::marginSize);
}

uint ThemeManager::spacingSize() const
{
    return m_spacingSize;
}

void ThemeManager::setSpacingSize(uint spacingSize)
{
    if (m_spacingSize == spacingSize)
        return;

    m_spacingSize = spacingSize;
    if (!sync(QStringLiteral("setSpacingSize"), m_spacingSize))
    {
        m_settings->save(QStringLiteral("SpacingSize"), m_spacingSize);
    }
    Q_EMIT spacingSizeChanged(m_spacingSize);
}

void ThemeManager::resetSpacingSize()
{
    this->setSpacingSize(ThemeManager::DefaultValues::spacingSize);
}

void ThemeManager::resetSPacingSize()
{
    this->resetSpacingSize();
}

QString ThemeManager::defaultFont() const
{
    return m_defaultFont;
}

QString ThemeManager::smallFont() const
{
    return m_smallFont;
}

QString ThemeManager::monospacedFont() const
{
    return m_monospacedFont;
}

void ThemeManager::setDefaultFont(const QString &defaultFont)
{
    if (m_defaultFont == defaultFont)
        return;

    m_defaultFont = defaultFont;
    if (!sync(QStringLiteral("setDefaultFont"), m_defaultFont))
    {
        m_settings->save(QStringLiteral("DefaultFont"), m_defaultFont);
    }
    Q_EMIT defaultFontChanged(m_defaultFont);
}

void ThemeManager::resetDefaultFont()
{
    setDefaultFont(ThemeManager::DefaultValues::defaultFont);
}

void ThemeManager::setSmallFont(const QString &smallFont)
{
    if (m_smallFont == smallFont)
        return;

    m_smallFont = smallFont;
    if (!sync(QStringLiteral("setSmallFont"), m_smallFont))
    {
        m_settings->save(QStringLiteral("SmallFont"), m_smallFont);
    }
    Q_EMIT smallFontChanged(m_smallFont);
}

void ThemeManager::resetSmallFont()
{
    setSmallFont(ThemeManager::DefaultValues::smallFont);
}

void ThemeManager::setMonospacedFont(const QString &monospacedFont)
{
    if (m_monospacedFont == monospacedFont)
        return;

    m_monospacedFont = monospacedFont;
    if (!sync(QStringLiteral("setMonospacedFont"), m_monospacedFont))
    {
        m_settings->save(QStringLiteral("MonospacedFont"), m_monospacedFont);
    }
    Q_EMIT monospacedFontChanged(m_monospacedFont);
}

void ThemeManager::resetMonospacedFont()
{
    setMonospacedFont(ThemeManager::DefaultValues::monospacedFont);
}

QString ThemeManager::customColorScheme() const
{
    return m_customColorScheme;
}

void ThemeManager::setCustomColorScheme(const QString &customColorScheme)
{
    if (m_customColorScheme == customColorScheme)
        return;

    m_customColorScheme = customColorScheme;
    if (!sync(QStringLiteral("setCustomColorScheme"), m_customColorScheme))
    {
        m_settings->save(QStringLiteral("CustomColorScheme"), m_customColorScheme);
    }
    Q_EMIT customColorSchemeChanged(m_customColorScheme);
}

bool ThemeManager::allowCustomStyling() const
{
    return m_allowCustomStyling;
}

void ThemeManager::setAllowCustomStyling(bool value)
{
    if (m_allowCustomStyling == value)
        return;

    m_allowCustomStyling = value;
    if (!sync(QStringLiteral("setAllowCustomStyling"), m_allowCustomStyling))
    {
        m_settings->save(QStringLiteral("AllowCustomStyling"), m_allowCustomStyling);
    }
    Q_EMIT allowCustomStylingChanged(m_allowCustomStyling);
}

void MauiMan::ThemeManager::resetIconSize()
{
    this->setIconSize(ThemeManager::DefaultValues::iconSize);
}
