#include "mainwindow.h"
#include <stdio.h>


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    if (argc > 1)
    {
        MainWindow w;
        w.SetConfigurationFileName(argv[1]);
        w.show();

        return a.exec();
    }

    return -1;
}
