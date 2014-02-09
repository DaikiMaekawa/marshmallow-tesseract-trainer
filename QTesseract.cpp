#include "QTesseract.h"
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QTextStream>
#include "rect.h"
#include "boxread.h"
#include <iostream>

static const char * const SETTING_ORGANIZATION_NAME = "marshmallow-tesseract-trainer";
static const char * const SETTING_APP_NAME = "MarshmallowTesseractTrainer";

QTesseract::QTesseract() : 
    m_api(new tesseract::TessBaseAPI())
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
            SETTING_ORGANIZATION_NAME, SETTING_APP_NAME);
    QString lang;
    if(settings.contains("Tesseract/Lang")){
        lang = settings.value("Tesseract/Lang").toString();
    }
    QString dataPath;
    if(settings.contains("Tesseract/DataPath")){
        dataPath = settings.value("Tesseract/DataPath").toString();
    }

    if(lang.isNull()){
        msg("You need to configure tesseract in Settings!");
    }

    setlocale(LC_NUMERIC, "C");
    QByteArray byteArray = lang.toAscii();
    const char *apiLang = byteArray.constData();
    //UNIX only
    QByteArray byteArray1 = dataPath.toUtf8();
    const char *datapath = byteArray1.data();
    //setenv("TESSDATA_PREFIX", datapath, 1);
#if 0 //TODO: implemented
    if(m_api->Init(NULL, apiLang)){
        msg("Could not initialize tesseract.\n");
    }
#else
    if(m_api->Init(NULL, "eng")){
        msg("Could not initialize tesseract.\n");
    } 
#endif
}

QString QTesseract::getBoxes(const QImage &qImage, const int page){
    PIX *pixs;

    if((pixs = qImage2PIX(qImage)) == NULL){
        msg("Unsupported image type");
        return "";
    }
    
    m_api->SetVariable("tessedit_create_boxfile", "1");
    STRING text_out;
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(!m_api->ProcessPage(pixs, page, NULL, NULL, 0, &text_out)){
        msg("Error during processing.\n");
    }

    QApplication::restoreOverrideCursor();

    pixDestroy(&pixs);
    return QString::fromUtf8(text_out.string());
}

QString QTesseract::getUnicharset(const QVector<QString> &boxes){
    int option;
    UNICHARSET unicharset;

    setlocale(LC_ALL, "");
    unicharset.unichar_insert(" ");
    
    for(int i = 0; i < boxes.size(); i++){
        TBOX box;
        STRING unichar_string;
        char buff[kBoxReadBufSize];
        char *buffptr = buff;
        QString boxStr = boxes[i];
        QTextStream data(&boxStr);
        data.setCodec("UTF-8");
        QStringList lineBoxes = data.readAll().split(QRegExp("\n"), QString::SkipEmptyParts);
        for(int j = 0; j < lineBoxes.size(); j++){
            int page = 0;
            QString line = lineBoxes.at(j);
            buffptr = line.toUtf8().data();
            const unsigned char *ubuf = reinterpret_cast<const unsigned char*>(buffptr);
            if(ubuf[0] == 0xef && ubuf[1] == 0xbb && ubuf[2] == 0xbf)
                buffptr++;
            
            while(*buffptr == ' ' || *buffptr == '\t')
                buffptr++;

            if(*buffptr != '\0'){
                if(!ParseBoxFileStr(buffptr, &page, &unichar_string, &box)){
                    continue;
                }
            }

            unicharset.unichar_insert(unichar_string.string());
            std::cout << "unichar = " << unichar_string.string() << std::endl;
            //setProperties(&unicharset, unichar_string.string());
        }
    }

    if(unicharset.save_to_file("hoge")){
        std::cout << "success" << std::endl;
    }else{
        std::cout << "bad" << std::endl;
    }

    return QString(""); //TODO: Implemented
}

PIX* QTesseract::qImage2PIX(const QImage &qImage){
    QImage qImg = qImage.rgbSwapped();
    const int wpl = qImg.bytesPerLine() / 4;

    PIX * pixs = pixCreate(qImg.width(), qImg.height(), qImg.depth());
    pixSetWpl(pixs, wpl);
    pixSetColormap(pixs, NULL);
    l_uint32 *datas = pixs->data;

    for(int y = 0; y < qImg.height(); y++){
        l_uint32 *lines = datas + y * wpl;
        QByteArray a((const char*)qImg.scanLine(y), qImg.bytesPerLine());
        for(int j = 0; j < a.size(); j++){
            *((l_uint8 *)lines + j) = a[j];
        }
    }

    const qreal toDPM = 1.0 / 0.0254;
    int resolutionX = qImg.dotsPerMeterX() / toDPM;
    int resolutionY = qImg.dotsPerMeterY() / toDPM;

    if(resolutionX < 300) resolutionX = 300;
    if(resolutionY < 300) resolutionY = 300;
    pixSetResolution(pixs, resolutionX, resolutionY);

    return pixEndianByteSwapNew(pixs);
}

QImage QTesseract::PIX2qImage(PIX *pixImage){

}

void setProperties(UNICHARSET *unicharset, const char* const str){

}

void QTesseract::msg(QString text){
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

QTesseract::~QTesseract(){
    m_api->End();
    delete m_api;
}

