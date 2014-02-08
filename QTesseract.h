#ifndef QTESSERACT_H
#define QTESSERACT_H

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QString>
#include <QImage>

class QTesseract{
    tesseract::TessBaseAPI *m_api;
    QString m_lang;
    static void msg(QString text);

public:
    QTesseract();
    QString makeBoxes(const QImage &qImage, const int page);
    static PIX* qImage2PIX(const QImage &qImage);
    static QImage PIX2qImage(PIX *pixImage);
    ~QTesseract();
};

#endif //QTESSERACT_H

