#include <QApplication>
#include "mainwindow.h"
#include <ctime>
#include <cstdlib>

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned>(time(nullptr)));
    QApplication app(argc, argv);
    app.setApplicationName("坦克大战");

    MainWindow window;
    window.show();

    return app.exec();
}
 