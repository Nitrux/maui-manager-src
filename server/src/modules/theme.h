#pragma once

#include "ThemeBase.h"

class Theme : public ThemeBase
{
    Q_OBJECT
    Q_PROPERTY(uint version READ version CONSTANT)

public:
    explicit Theme(QObject *parent = nullptr);

    uint version() const { return 2u; }
};
