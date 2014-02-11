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
    static void msg(QString text);
    void setProperties(UNICHARSET *unicharset, const char* const str);
    //void readNextBox(QString boxData, STRING *str, TBOX *boundingBox);
    void training();

public:
    QTesseract();
    QString getBoxes(const QImage &qImage, const int page);
    QString getUnicharset(const QVector<QString> &boxes);
    static PIX* qImage2PIX(const QImage &qImage);
    static QImage PIX2qImage(PIX *pixImage);
    ~QTesseract();
};

#endif //QTESSERACT_H

