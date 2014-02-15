#ifndef TESSERACTTRAINER_H
#define TESSERACTTRAINER_H

#include <QMainWindow>
#include <QString>
#include "QTesseract.h"

namespace Ui {
class TesseractTrainer;
}

class TesseractTrainer : public QMainWindow
{
    Q_OBJECT
    
public:
    TesseractTrainer(int argc, char *argv[], QWidget *parent = 0);
    ~TesseractTrainer();
    
private:
    Ui::TesseractTrainer *m_ui;
    QString m_imgDir;
    QTesseract m_tess;
    void connectSignals();
    void saveSettings();
    void loadSettings();

public slots:
    void onLoadImage();
    void onSaveLang();
    void onPushTraining();
};

#endif // TESSERACTTRAINER_H
