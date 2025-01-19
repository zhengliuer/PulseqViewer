#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
     QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication app(argc, argv);
    MainWindow window;
#ifdef RELEASE
    window.showMaximized();
#endif 

    window.show();

    return app.exec();
}
