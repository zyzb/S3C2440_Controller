#include <QApplication>
#include <QTranslator>
#include "controlwidget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //加载全局翻译文件
    QTranslator translator1;
    translator1.load(":/qm/qm/qt_zh_CN.qm");
    a.installTranslator(&translator1);

    //加载widget类翻译文件
    QTranslator translator2;
    translator2.load(":/qm/qm/widgets.qm");
    a.installTranslator(&translator2);

    ControlWidget w;
    w.show();

    return a.exec();
}
