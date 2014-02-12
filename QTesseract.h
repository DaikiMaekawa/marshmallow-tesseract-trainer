#ifndef QTESSERACT_H
#define QTESSERACT_H

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QString>
#include <QImage>
#include <QVector>

struct FontProperties{
    QString font;
    bool italic;
    bool bold;
    bool fixed;
    bool serif;
    bool fraktur;
    
    FontProperties(const QString &fontName) : 
        font(fontName),
        italic(false),
        bold(false),
        fixed(false),
        serif(false),
        fraktur(false)
    {

    }
};

class QTesseract{
    tesseract::TessBaseAPI *m_api;
    QString m_lang;
    static void showMsg(const QString &text);
    static void runProcess(const QString &name, const QStringList &args);
    void training();

public:
    QTesseract();
    QString getBoxes(const QImage &qImage, const int page);
    void makeUnicharsetFile(const QString &boxFile, const int exp);
    void makeTrainingFile();
    void makeFontPropertiesFile(const FontProperties &prop);
    static PIX* qImage2PIX(const QImage &qImage);
    static QImage PIX2qImage(PIX *pixImage);
    ~QTesseract();
};

#endif //QTESSERACT_H

