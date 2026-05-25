#pragma once

#include "BackgroundBase.h"

class Background : public BackgroundBase
{
    Q_OBJECT
    Q_PROPERTY(uint version READ version CONSTANT)

public:
    explicit Background(QObject *parent = nullptr);

    uint version() const { return 1u; }
};
