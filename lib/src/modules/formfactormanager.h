#pragma once

#include <QObject>
#include <QRect>
#include <QList>

#include "mauiman_export.h"

class QInputDevice;
class OrgMauimanFormFactorInterface;
namespace MauiMan
{
    class SettingsStore;

    /**
     * @brief Detects runtime capabilities and recommends a UI mode for the current device.
     *
     * This class is read-only and continuously reflects current hardware and
     * screen characteristics such as keyboard, mouse, touch support, screen
     * size, and screen orientation.
     */
    class MAUIMAN_EXPORT FormFactorInfo : public QObject
    {
        Q_OBJECT
        /**
         * The best fitted mode according to the available input devices and the screen size.
         */
        Q_PROPERTY(uint bestMode READ bestMode NOTIFY bestModeChanged FINAL)
        
        /**
         * The default mode selected from the QT_QUICK_CONTROLS_MOBILE environment variable.
         */
        Q_PROPERTY(uint defaultMode READ defaultMode CONSTANT FINAL)

        /**
         * Whether the device has a physical keyboard.
         */
        Q_PROPERTY(bool hasKeyboard READ hasKeyboard NOTIFY hasKeyboardChanged FINAL)
        
        /**
         * Whether the device has a touch screen.
         */
        Q_PROPERTY(bool hasTouchscreen READ hasTouchscreen NOTIFY hasTouchscreenChanged FINAL)
        
        /**
         * Whether the device has a physical mouse.
         */
        Q_PROPERTY(bool hasMouse READ hasMouse  NOTIFY hasMouseChanged FINAL)
        
        /**
         * Whether the device has a trackpad or touchpad 
         */
        Q_PROPERTY(bool hasTouchpad READ hasTouchpad NOTIFY hasTouchpadChanged)

        /**
         * The size of the main screen.
         */
        Q_PROPERTY(QRect screenSize READ screenSize NOTIFY screenSizeChanged)
        
        /**
         * The current orientation of the main screen.
         */
        Q_PROPERTY(Qt::ScreenOrientation screenOrientation READ screenOrientation NOTIFY screenOrientationChanged)

    public:
        /**
         * @brief UI modes selected from the device display and input capabilities.
         */
        enum Mode
        {
            /**
             * Desktop mode for larger screens or devices with pointer and keyboard input.
             */
            Desktop = 0,
            
            /**
             * Tablet mode for touch-oriented medium or large displays.
             */
            Tablet,
            
            /**
             * Phone mode for small touch-oriented displays without peripheral input.
             */
            Phone
        };

        struct DefaultValues
        {
            [[nodiscard]] static uint getDefaultMode()
            {
                return QByteArrayList{"1", "true"}.contains(qgetenv("QT_QUICK_CONTROLS_MOBILE")) ? MauiMan::FormFactorInfo::Mode::Phone : MauiMan::FormFactorInfo::Mode::Desktop;
            }

            [[nodiscard]] static bool getHasTouchScreen()
            {
                return false;
            }

            static inline const uint defaultMode = DefaultValues::getDefaultMode();
            static inline const bool hasTouchscreen = DefaultValues::getHasTouchScreen();
            // Used by FormFactorManager via inherited DefaultValues lookup.
            static inline const bool forceTouchScreen = false;
        } ;

        explicit FormFactorInfo(QObject *parent);

        [[nodiscard]] uint bestMode() const;

        [[nodiscard]] uint defaultMode() const;

        [[nodiscard]] bool hasKeyboard() const;

        [[nodiscard]] bool hasTouchscreen() const;

        [[nodiscard]] bool hasMouse() const;

        [[nodiscard]] bool hasTouchpad() const;

        [[nodiscard]] QRect screenSize();
        [[nodiscard]] Qt::ScreenOrientation screenOrientation();

    private:
        uint m_bestMode = FormFactorInfo::DefaultValues::defaultMode;

        uint m_defaultMode = FormFactorInfo::DefaultValues::defaultMode;

        bool m_hasKeyboard = true;

        bool m_hasTouchscreen = FormFactorInfo::DefaultValues::hasTouchscreen;

        bool m_hasMouse = true;
        bool m_hasTouchpad = true;

        void checkInputs(const QList<const QInputDevice *> &devices);
        void findBestMode();

    Q_SIGNALS:
        void bestModeChanged(uint bestMode);
        void defaultModeChanged(uint defaultMode);

        void hasKeyboardChanged(bool hasKeyboard);
        void hasTouchscreenChanged(bool hasTouchscreen);
        void hasMouseChanged(bool hasMouse);
        void hasTouchpadChanged(bool hasTouchpad);

        void screenSizeChanged(QRect screenSize);
        void screenOrientationChanged(Qt::ScreenOrientation screenOrientation);
    };

    /**
     * @brief Synchronizes persisted user preferences related to form-factor behavior.
     *
     * It extends FormFactorInfo runtime data with preferences such as
     * `preferredMode` and `forceTouchScreen`.
     */
    class MAUIMAN_EXPORT FormFactorManager : public FormFactorInfo
    {
        Q_OBJECT
        /**
         * Schema version of the FormFactor module. Bumped when properties are
         * added or removed. Lets consumers feature-detect at runtime.
         */
        Q_PROPERTY(uint version READ version CONSTANT)

        /**
         * The user-preferred UI mode, using a FormFactorInfo::Mode value.
         */
        Q_PROPERTY(uint preferredMode READ preferredMode WRITE setPreferredMode NOTIFY preferredModeChanged FINAL)

        /**
         * Whether applications should enable touch interactions even when no touchscreen is detected.
         */
        Q_PROPERTY(bool forceTouchScreen READ forceTouchScreen WRITE setForceTouchScreen NOTIFY forceTouchScreenChanged)


    public:
        explicit FormFactorManager(QObject *parent = nullptr);

        uint version() const { return 1u; }

        [[nodiscard]] uint preferredMode() const;
        void setPreferredMode(uint preferredMode);        

        [[nodiscard]] bool forceTouchScreen() const;
        void setForceTouchScreen(bool newForceTouchScreen);

    private Q_SLOTS:
        void onPreferredModeChanged(uint preferredMode);
        void onForceTouchScreenChanged(bool value);

    private:
        OrgMauimanFormFactorInterface *m_interface = nullptr;
        MauiMan::SettingsStore *m_settings;

        uint m_preferredMode;
        bool m_forceTouchScreen = false;

        bool syncPreferredMode(uint preferredMode);
        bool syncForceTouchScreen(bool forceTouchScreen);
        void setConnections();
        void loadSettings();

    Q_SIGNALS:
        void preferredModeChanged(uint preferredMode);
        void forceTouchScreenChanged(bool forceTouchScreen);
    };
}
