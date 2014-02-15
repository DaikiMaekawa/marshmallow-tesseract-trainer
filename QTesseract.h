#ifndef QTESSERACT_H
#define QTESSERACT_H

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QString>
#include <QImage>
#include <QVector>

struct FontProperties{
    bool italic;
    bool bold;
    bool fixed;
    bool serif;
    bool fraktur;
    
    FontProperties() : 
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
    QString m_font;
    static void showMsg(const QString &text);
    static void runProcess(const QString &name, const QStringList &args);
    void makeUnicharsetFile(const int exp);
    void makeTrainingFile();
    void makeFontPropertiesFile(const FontProperties &prop); 

public:
    QTesseract(const QString &lang, const QString &font);
    QString getBoxes(const QImage &qImage, const int page);
    void training(const QString &img, const FontProperties &prop);
    static PIX* qImage2PIX(const QImage &qImage);
    static QImage PIX2qImage(PIX *pixImage);
    ~QTesseract();
};

#endif //QTESSERACT_H

