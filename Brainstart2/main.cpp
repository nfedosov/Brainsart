#include "mainwindow.h"
#include <stdio.h>
#include <QTextCodec>
#include <QApplication>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

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
