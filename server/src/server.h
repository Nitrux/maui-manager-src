#pragma once

#include <QGuiApplication>
#include <QObject>
#include <QVector>

class Server : public QGuiApplication
{
    Q_OBJECT
public:
    explicit Server(int &argc, char **argv);
    ~Server();
    bool init();

private:
    QVector<QObject*> m_modules;
};
