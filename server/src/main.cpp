#include "server.h"

int main(int argc, char *argv[])
{
    Server s(argc, argv);
    if (!s.init())
        return -1;

    return s.exec();
}
