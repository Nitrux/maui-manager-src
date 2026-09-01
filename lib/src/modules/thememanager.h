#pragma once

#include <QColor>
#include <QObject>
#include <QString>
#include <QFont>

#include "mauiman_export.h"

class OrgMauimanThemeInterface;

/** Contains client-side managers for shared Maui application and shell preferences. */
namespace MauiMan
{

    class SettingsStore;

    /**
     * @brief Provides synchronized access to shared Maui theme preferences.
     *
     * Values are synchronized with the Theme object on org.mauiman.Manager.
     * Local settings provide defaults and an offline fallback. Properties with
     * reset functions can be restored to their compiled defaults; from QML,
     * assigning undefined invokes the corresponding reset function.
     */
    class MAUIMAN_EXPORT ThemeManager : public QObject
    {
        Q_OBJECT
        /**
         * Schema version of the Theme module. Bumped when properties are added
         * or removed. Lets consumers feature-detect at runtime.
         */
        Q_PROPERTY(uint version READ version CONSTANT)

        /**
         * The style type for the color scheme. The possible values are:
         * - 0 Light
         * - 1 Dark
         * - 2 Adaptive
         * - 3 Custom (from the plasma color-scheme file preferences)
         * - 4 True Black
         * - 5 Inverted (True White)
         */
        Q_PROPERTY(int styleType READ styleType WRITE setStyleType NOTIFY styleTypeChanged)

        /** Whether MauiKit should derive its global palette from the active wallpaper image. */
        Q_PROPERTY(bool adaptiveColorSchemeEnabled READ adaptiveColorSchemeEnabled WRITE setAdaptiveColorSchemeEnabled NOTIFY adaptiveColorSchemeEnabledChanged)

        /** Canonical local image path used for the global adaptive palette. */
        Q_PROPERTY(QString adaptiveColorSchemeSource READ adaptiveColorSchemeSource WRITE setAdaptiveColorSchemeSource NOTIFY adaptiveColorSchemeSourceChanged)

        /**
         * The preferred accent color used for the highlighted and checked states.
         */
        Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged RESET resetAccentColor)

        /**
         * The preferred icon theme.
         */
        Q_PROPERTY(QString iconTheme READ iconTheme WRITE setIconTheme NOTIFY iconThemeChanged)

        /**
         * The preferred theme for the CSD window control buttons. The list of available options depends on the installed themes. For more information on this refer to the MauiKit CSDControls documentation.
         */
        Q_PROPERTY(QString windowControlsTheme READ windowControlsTheme WRITE setWindowControlsTheme NOTIFY windowControlsThemeChanged)

        /**
         * Whether to use client side decorations (CSD) for the applications.
         * By default this is set to `true`
         */
        Q_PROPERTY(bool enableCSD READ enableCSD WRITE setEnableCSD NOTIFY enableCSDChanged)

        /**
         * The corner border radius of the UI elements, also affects the radius of the CSD window corners.
         */
        Q_PROPERTY(uint borderRadius READ borderRadius WRITE setBorderRadius NOTIFY borderRadiusChanged RESET resetBorderRadius)

        /**
         * The preferred size of the icons in the buttons, menu entries and other elements.
         */
        Q_PROPERTY(uint iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged RESET resetIconSize)

        /**
         * The preferred padding size for the UI elements, such as buttons, tool bars, menu entries, etc.
         */
        Q_PROPERTY(uint paddingSize READ paddingSize WRITE setPaddingSize NOTIFY paddingSizeChanged RESET resetPaddingSize)

        /**
         * The preferred margin size around views.
         */
        Q_PROPERTY(uint marginSize READ marginSize WRITE setMarginSize NOTIFY marginSizeChanged RESET resetMarginSize)

        /**
         * The preferred spacing size between elements in a row or column, such as lists or browsing elements.
         */
        Q_PROPERTY(uint spacingSize READ spacingSize WRITE setSpacingSize NOTIFY spacingSizeChanged RESET resetSpacingSize)

        /**
         * Whether the user prefers for the system to have visual effects, such as animations, blur, etc.
         */
        Q_PROPERTY(bool enableEffects READ enableEffects WRITE setEnableEffects NOTIFY enableEffectsChanged)

        /**
         * The preferred default font, for most part of the UI.
         */
        Q_PROPERTY(QString defaultFont READ defaultFont WRITE setDefaultFont NOTIFY defaultFontChanged RESET resetDefaultFont)

        /**
         * The preferred small font, for details.
         */
        Q_PROPERTY(QString smallFont READ smallFont WRITE setSmallFont NOTIFY smallFontChanged RESET resetSmallFont)

        /**
         * The preferred mono spaced font, for example the one used in console terminals, or in text editors.
         */
        Q_PROPERTY(QString monospacedFont READ monospacedFont WRITE setMonospacedFont NOTIFY monospacedFontChanged RESET resetMonospacedFont)

        /**
         * The preferred color scheme to be used when the `styleType` is set to Custom.
         */
        Q_PROPERTY(QString customColorScheme READ customColorScheme WRITE setCustomColorScheme NOTIFY customColorSchemeChanged)

        /**
         * Allow third-party styles for Qt Quick Controls. By default this is set to `false`. MauiKit-Style is the best fitted style to be used with Maui Apps, if you want to use a custom style, this must be enabled deliberately, since existing styles won't look good with the MauiKit custom controls.
         */
        Q_PROPERTY(bool allowCustomStyling READ allowCustomStyling WRITE setAllowCustomStyling NOTIFY allowCustomStylingChanged)

        /**
         * Whether vertical scroll bars should be positioned on the left side.
         * By default this is set to `false`.
         */
        Q_PROPERTY(bool scrollBarOnLeft READ scrollBarOnLeft WRITE setScrollBarOnLeft NOTIFY scrollBarOnLeftChanged)

    public:

        uint version() const { return 3u; }

        /**
         * @brief The Theme module default values
         */
        struct DefaultValues
        {
            /**
             * @private
             */
            static int preferredStyleType()
            {
                return 3; // use the system color scheme by default
            }

            /**
             * @private
             */
            static QString getDefaultFont()
            {
                QFont font {QStringLiteral("Noto Sans"), 10, QFont::Normal};
                font.setStyleHint(QFont::SansSerif);
                font.setStyle(QFont::StyleNormal);
                font.setStyleName(QStringLiteral("Regular"));
                return font.toString();
            }

            /**
             * @private
             */
            static QString getSmallFont()
            {
                QFont font {QStringLiteral("Noto Sans"), 8, QFont::Normal};
                font.setStyleHint(QFont::SansSerif);
                font.setStyle(QFont::StyleNormal);
                font.setStyleName(QStringLiteral("Regular"));

                return font.toString();
            }

            /**
             * @private
             */
            static QString getMonospacedFont()
            {
                QFont font {QStringLiteral("Hack"), 10, QFont::Normal};
                font.setStyleHint(QFont::Monospace);
                font.setStyle(QFont::StyleNormal);
                font.setStyleName(QStringLiteral("Regular"));

                return font.toString();
            }

            static inline const int styleType = ThemeManager::DefaultValues::preferredStyleType();
            static inline const bool adaptiveColorSchemeEnabled = false;
            static inline const QString adaptiveColorSchemeSource = QString();
            static inline const QString accentColor = QStringLiteral("#26c6da");
            static inline const QString iconTheme = QStringLiteral("Luv");
            static inline const QString windowControlsTheme = QStringLiteral("Nitrux");
            static inline const bool enableCSD = true;
            static inline const uint borderRadius = 6;
            static inline const uint iconSize = 16;
            static inline const bool enableEffects = true;
            static inline const uint paddingSize = 6;
            static inline const uint marginSize = 6;
            static inline const uint spacingSize = 6;
            static inline const QString defaultFont = getDefaultFont();
            static inline const QString smallFont = getSmallFont();
            static inline const QString monospacedFont = getMonospacedFont();
            static inline const QString customColorScheme = QStringLiteral("Nitrux");
            static inline const bool allowCustomStyling = false;
            static inline const bool scrollBarOnLeft = false;
        };

        explicit ThemeManager(QObject * parent = nullptr);

        int styleType() const;
        void setStyleType(int newStyleType);

        bool adaptiveColorSchemeEnabled() const;
        void setAdaptiveColorSchemeEnabled(bool enabled);

        const QString &adaptiveColorSchemeSource() const;
        void setAdaptiveColorSchemeSource(const QString &source);

        QColor accentColor() const;
        void setAccentColor(const QColor &newAccentColor);
        void resetAccentColor();

        const QString &iconTheme() const;
        void setIconTheme(const QString &newIconTheme);

        const QString &windowControlsTheme() const;
        void setWindowControlsTheme(const QString &newWindowControlsTheme);

        bool enableCSD() const;
        void setEnableCSD(bool enableCSD);

        uint borderRadius() const;
        void setBorderRadius(uint newBorderRadius);
        void resetBorderRadius();

        uint iconSize() const;
        void setIconSize(uint newIconSize);
        void resetIconSize();

        bool enableEffects() const;
        void setEnableEffects(bool enableEffects);

        uint paddingSize() const;
        void setPaddingSize(uint paddingSize);
        void resetPaddingSize();

        uint marginSize() const;
        void setMarginSize(uint marginSize);
        void resetMarginSize();

        uint spacingSize() const;
        void setSpacingSize(uint spacingSize);
        void resetSpacingSize();
        // TODO(maui-5): remove compatibility alias on next ABI break.
        void resetSPacingSize();

        QString defaultFont() const;
        void setDefaultFont(const QString &defaultFont);
        void resetDefaultFont();

        QString smallFont() const;
        void setSmallFont(const QString &smallFont);
        void resetSmallFont();

        QString monospacedFont() const;
        void setMonospacedFont(const QString &monospacedFont);
        void resetMonospacedFont();

        QString customColorScheme() const;
        void setCustomColorScheme(const QString &customColorScheme);

        bool allowCustomStyling() const;
        void setAllowCustomStyling(bool value);

        bool scrollBarOnLeft() const;
        void setScrollBarOnLeft(bool value);

    private Q_SLOTS:
        void onStyleTypeChanged(const int &newStyleType);
        void onAdaptiveColorSchemeEnabledChanged(bool enabled);
        void onAdaptiveColorSchemeSourceChanged(const QString &source);
        void onAccentColorChanged(const QString &newAccentColor);
        // Slot stays QString because it's connected to the DBus signal (wire type s).
        void onWindowControlsThemeChanged(const QString &newWindowControlsTheme);
        void onIconThemeChanged(const QString &newIconTheme);
        void onEnableCSDChanged(const bool &enableCSD);
        void onBorderRadiusChanged(const uint &radius);
        void onIconSizeChanged(const uint &size);
        void onPaddingSizeChanged(const uint &paddingSize);
        void onMarginSizeChanged(const uint &marginSize);
        void onSpacingSizeChanged(const uint &spacingSize);
        void onEnableEffectsChanged(bool enableEffects);
        void onDefaultFontChanged(const QString &font);
        void onSmallFontChanged(const QString &font);
        void onMonospacedFontChanged(const QString &font);
        void onCustomColorSchemeChanged(const QString &scheme);
        void onAllowCustomStylingChanged(bool allowCustomStyling);
        void onScrollBarOnLeftChanged(bool scrollBarOnLeft);

    Q_SIGNALS:
        void styleTypeChanged(int styleType);
        void adaptiveColorSchemeEnabledChanged(bool enabled);
        void adaptiveColorSchemeSourceChanged(QString source);
        void accentColorChanged(QColor accentColor);
        void iconThemeChanged(QString iconTheme);
        void windowControlsThemeChanged(QString windowControlsTheme);
        void enableCSDChanged(bool enableCSD);
        void borderRadiusChanged(uint radius);
        void iconSizeChanged(uint size);
        void enableEffectsChanged(bool enableEffects);
        void paddingSizeChanged(uint paddingSize);
        void marginSizeChanged(uint marginSize);
        void spacingSizeChanged(uint spacingSize);
        void defaultFontChanged(QString defaultFont);
        void smallFontChanged(QString smallFont);
        void monospacedFontChanged(QString monospacedFont);
        void customColorSchemeChanged(QString customColorScheme);
        void allowCustomStylingChanged(bool customStyling);
        void scrollBarOnLeftChanged(bool scrollBarOnLeft);

    private:
        OrgMauimanThemeInterface *m_interface = nullptr;

        MauiMan::SettingsStore *m_settings;

        int m_styleType = ThemeManager::DefaultValues::styleType;
        bool m_adaptiveColorSchemeEnabled = ThemeManager::DefaultValues::adaptiveColorSchemeEnabled;
        QString m_adaptiveColorSchemeSource = ThemeManager::DefaultValues::adaptiveColorSchemeSource;
        bool m_adaptiveColorSchemeSupported = false;
        QColor m_accentColor = QColor(ThemeManager::DefaultValues::accentColor);
        QString m_iconTheme = ThemeManager::DefaultValues::iconTheme;
        QString m_windowControlsTheme = ThemeManager::DefaultValues::windowControlsTheme;
        bool m_enableCSD = ThemeManager::DefaultValues::enableCSD;
        uint m_borderRadius = ThemeManager::DefaultValues::borderRadius;
        uint m_iconSize = ThemeManager::DefaultValues::iconSize;
        uint m_paddingSize = ThemeManager::DefaultValues::paddingSize;
        uint m_marginSize = ThemeManager::DefaultValues::marginSize;
        uint m_spacingSize = ThemeManager::DefaultValues::spacingSize;
        bool m_enableEffects = ThemeManager::DefaultValues::enableEffects;
        QString m_defaultFont = MauiMan::ThemeManager::DefaultValues::defaultFont;
        QString m_smallFont = MauiMan::ThemeManager::DefaultValues::smallFont;
        QString m_monospacedFont = MauiMan::ThemeManager::DefaultValues::monospacedFont;
        QString m_customColorScheme = MauiMan::ThemeManager::DefaultValues::customColorScheme;
        bool m_allowCustomStyling = MauiMan::ThemeManager::DefaultValues::allowCustomStyling;
        bool m_scrollBarOnLeft = MauiMan::ThemeManager::DefaultValues::scrollBarOnLeft;

        bool sync(const QString &key, const QVariant &value);
        void setConnections();
        void loadSettings();

    };
}
