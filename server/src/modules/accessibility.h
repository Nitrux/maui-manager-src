#pragma once

#include <QObject>
#include "modules/accessibilitymanager.h"

class Accessibility : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool singleClick READ singleClick NOTIFY singleClickChanged)
    Q_PROPERTY(uint scrollBarPolicy READ scrollBarPolicy NOTIFY scrollBarPolicyChanged)
    Q_PROPERTY(bool playSounds READ playSounds NOTIFY playSoundsChanged)

public:
    explicit Accessibility(QObject *parent = nullptr);

    bool singleClick() const;

    /**
     * @brief scrollBarPolicy
     * The policy values are:
     * 0- AlwaysVisible
     * 1- AsNeeded
     * 2- Hidden
     * 3- AutoHide
     */
    uint scrollBarPolicy() const;

    bool playSounds() const;

private:
    bool m_singleClick = MauiMan::AccessibilityManager::DefaultValues::singleClick;
    uint m_scrollBarPolicy = MauiMan::AccessibilityManager::DefaultValues::scrollBarPolicy;
    bool m_playSounds = MauiMan::AccessibilityManager::DefaultValues::playSounds;

Q_SIGNALS:
    void singleClickChanged(bool);
    void scrollBarPolicyChanged(uint);
    void playSoundsChanged(bool);
};
