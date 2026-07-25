#include "NikaidoHiro.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    NikaidoHiro window;
    window.show();
    return app.exec();
}
