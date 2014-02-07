#ifndef TESSERACTTRAINER_H
#define TESSERACTTRAINER_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class TesseractTrainer;
}

class TesseractTrainer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit TesseractTrainer(int argc, char *argv[], QWidget *parent = 0);
    ~TesseractTrainer();
    
private:
    Ui::TesseractTrainer *m_ui;
    QString m_imgDir;
    void connectSignals();
    void saveSettings();
    void loadSettings();

public slots:
    void onLoadImage();
    void onSaveLang();
};

#endif // TESSERACTTRAINER_H
