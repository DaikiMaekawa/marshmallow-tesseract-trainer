#include "QTesseract.h"
#include <QSettings>
#include <QApplication>
#include <QMessageBox>

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

QString QTesseract::makeBoxes(const QImage &qImage, const int page){
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

void QTesseract::msg(QString text){
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

QTesseract::~QTesseract(){
    m_api->End();
    delete m_api;
}
