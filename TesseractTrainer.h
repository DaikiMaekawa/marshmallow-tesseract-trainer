#ifndef TESSERACTTRAINER_H
#define TESSERACTTRAINER_H

#include <QMainWindow>

namespace Ui {
class TesseractTrainer;
}

class TesseractTrainer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit TesseractTrainer(QWidget *parent = 0);
    ~TesseractTrainer();
    
private:
    Ui::TesseractTrainer *ui;
};

#endif // TESSERACTTRAINER_H
