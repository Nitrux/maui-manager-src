#pragma once

#include <QObject>
#include "mauiman_export.h"

class OrgMauimanAccessibilityInterface;
namespace MauiMan
{
/**
 * @brief The AccessibilityManager class exposes runtime accessibility-related
 * properties through MauiMan's D-Bus facade. Values are not persisted by
 * MauiMan. Writes are best-effort and may be ignored when no writable
 * upstream source is available.
 */
class MAUIMAN_EXPORT AccessibilityManager : public QObject
{
    Q_OBJECT
    /**
     * Whether to open items with a single click.
     * By default this is set to `true` for mobile, and `false` for desktop.
     */
    Q_PROPERTY(bool singleClick READ singleClick WRITE setSingleClick NOTIFY singleClickChanged)
    
    /**
     * The policy for showing the scroll bars. The possible values are:
     * - 0 Always visible
     * - 1 Visible when needed
     * - 2 Auto Hide
     * - 3 Always hidden
     */
    Q_PROPERTY(uint scrollBarPolicy READ scrollBarPolicy WRITE setScrollBarPolicy NOTIFY scrollBarPolicyChanged)
    
    /**
     * Whether the user prefers the application to emit notification or alarm sounds.
     */
    Q_PROPERTY(bool playSounds READ playSounds WRITE setPlaySounds NOTIFY playSoundsChanged)

public:

    struct DefaultValues
    {
        static bool isMobile()
        {
            return QByteArrayList{"1", "true"}.contains(qgetenv("QT_QUICK_CONTROLS_MOBILE"));
        }
        static inline const bool singleClick = DefaultValues::isMobile();
        static inline const bool playSounds = true;
        static inline const uint scrollBarPolicy = DefaultValues::isMobile() ? 3 : 0;
    };

    explicit AccessibilityManager(QObject *parent = nullptr);

    bool singleClick() const;
    void setSingleClick(bool singleClick);

    uint scrollBarPolicy() const;
    void setScrollBarPolicy(uint newScrollBarPolicy);

    bool playSounds() const;
    void setPlaySounds(bool newPlaySounds);

private Q_SLOTS:
    void onSingleClickChanged(bool singleClick);
    void onScrollBarPolicyChanged(uint scrollBarPolicy);
    void onPlaySoundsChanged(bool playSounds);

private:
    OrgMauimanAccessibilityInterface *m_interface = nullptr;

    bool m_singleClick = AccessibilityManager::DefaultValues::singleClick;
    uint m_scrollBarPolicy = AccessibilityManager::DefaultValues::scrollBarPolicy;
    bool m_playSounds = AccessibilityManager::DefaultValues::playSounds;

    void sync(const QString &key, const QVariant &value);
    void setConnections();
    void loadSettings();

Q_SIGNALS:
    void singleClickChanged(bool);
    void scrollBarPolicyChanged(uint);
    void playSoundsChanged(bool);
};

}
