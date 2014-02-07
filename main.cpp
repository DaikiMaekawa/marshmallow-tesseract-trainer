#include <QtGui/QApplication>
#include "TesseractTrainer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TesseractTrainer w;
    w.show();
    
    return a.exec();
}
