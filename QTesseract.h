#ifndef QTESSERACT_H
#define QTESSERACT_H

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QString>
#include <QImage>
#include <QVector>

class QTesseract{
    tesseract::TessBaseAPI *m_api;
    QString m_lang;
    static void showMsg(const QString &text);
    static void runProcess(const QString &name, const QStringList &args);
    void training();

public:
    QTesseract();
    QString getBoxes(const QImage &qImage, const int page);
    QString getUnicharset(const QVector<QString> &boxes);
    void makeTrainingFile();
    static PIX* qImage2PIX(const QImage &qImage);
    static QImage PIX2qImage(PIX *pixImage);
    ~QTesseract();
};

#endif //QTESSERACT_H

