#pragma once

#include <QObject>
#include <QString>

#include "mauiman_export.h"

class OrgMauimanScreenInterface;
namespace MauiMan
{
/**
 * The ScreenManager class exposes runtime screen properties through MauiMan's
 * D-Bus interface. Values are treated as facade state and are not persisted by
 * MauiMan. Writes are best-effort and may be ignored when no writable upstream
 * source is available.
 */
class MAUIMAN_EXPORT ScreenManager : public QObject
{
    Q_OBJECT
    /**
     * The preferred scale factor for the main screen.
     */
    Q_PROPERTY(double scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)
    
    /**
     * The preferred orientation of the main screen.
     * Possible values are:
     * - 0 Horizontal - Landscape
     * - 1 Vertical - Portrait
     */
    Q_PROPERTY(uint orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

public:
    
    /**
     * @brief The Screen module default values.
     */
    struct DefaultValues
    {
        static inline const double scaleFactor = 1;
        static inline const double orientation = 0;
    } ;

    explicit ScreenManager(QObject * parent = nullptr);

    double scaleFactor() const;
    void setScaleFactor(double scaleFactor);

    uint orientation() const;
    void setOrientation(uint orientation);

private Q_SLOTS:
    void onScaleFactorChanged(double scale);
    void onOrientationChanged(uint orientation);

Q_SIGNALS:
    void scaleFactorChanged(double scaleFactor);
    void orientationChanged(uint orientation);

private:
    OrgMauimanScreenInterface *m_interface = nullptr;

    void sync(const QString &key, const QVariant &value);
    void setConnections();
    void loadSettings();

    double m_scaleFactor = ScreenManager::DefaultValues::scaleFactor;
    uint m_orientation = ScreenManager::DefaultValues::orientation;
};
}
