#include "QTesseract.h"
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QFile>
#include <QString>
#include <iostream>
//#include <fstream>

static const char * const SETTING_ORGANIZATION_NAME = "marshmallow-tesseract-trainer";
static const char * const SETTING_APP_NAME = "MarshmallowTesseractTrainer";
//static const char * const OUTPUT_DIR = "tessdata";
//static const char * const OUTPUT_DIR = ".";

QTesseract::QTesseract(const QString &lang, const QString &font) : 
    m_api(new tesseract::TessBaseAPI()),
    m_lang(lang),
    m_font(font)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
            SETTING_ORGANIZATION_NAME, SETTING_APP_NAME);
    setlocale(LC_NUMERIC, "C");
    //QByteArray byteArray = lang.toAscii();
    //const char *apiLang = byteArray.constData();
    //UNIX only
    //QByteArray byteArray1 = dataPath.toUtf8();
    //const char *datapath = byteArray1.data();
    //setenv("TESSDATA_PREFIX", datapath, 1);
#if 0 //TODO: implemented
    if(m_api->Init(NULL, apiLang)){
        showMsg("Could not initialize tesseract.\n");
    }
#else
    if(m_api->Init(NULL, "eng")){
        showMsg("Could not initialize tesseract.\n");
    } 
#endif
}

QVector<TessChar> QTesseract::getTessBoxes(const QString &img){
    QString box = QString("%1.%2.exp0").arg(m_lang).arg(m_font);
    runProcess("tesseract", QStringList() << img
                                          << box
                                          << "-l" << m_lang
                                          << "batch.nochop" << "makebox"); 
    QVector<TessChar> boxes;
    QFile file(box + QString(".box"));
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        QString line;
        while(true){
            TessChar c;
            line = in.readLine();
            if(!line.isNull()){
                QStringList list = line.split(" ");
                c.type = list[0];
                c.leftX = list[1].toInt();
                c.leftY = list[2].toInt();
                c.rightX = list[3].toInt();
                c.rightY = list[4].toInt();
                std::cout << c.type.toStdString() << std::endl;
            }else{
                break;
            }
        }
    }

    return boxes;
}
/*
QString QTesseract::getBoxes(const QImage &qImage, const int page){
    PIX *pixs;

    if((pixs = qImage2PIX(qImage)) == NULL){
        showMsg("Unsupported image type");
        return "";
    }
    
    m_api->SetVariable("tessedit_create_boxfile", "1");
    STRING text_out;
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(!m_api->ProcessPage(pixs, page, NULL, NULL, 0, &text_out)){
        showMsg("Error during processing.\n");
    }

    QApplication::restoreOverrideCursor();

    pixDestroy(&pixs);
    return QString::fromUtf8(text_out.string());
}
*/

void QTesseract::makeUnicharsetFile(const int exp){
    /*
    QStringList args;
    args << "-D" << OUTPUT_DIR << QString("%1/%2.%3.exp%4.box").arg(OUTPUT_DIR)
                                                               .arg(m_lang)
                                                               .arg(m_font)
                                                               .arg(exp);
    runProcess("unicharset_extractor", args);
    */
}

void QTesseract::makeTrainingFile(){
    /*
    QString name = QString(OUTPUT_DIR) + QString("/%1.%2.exp0").arg(m_lang).arg(m_font);
    QString img = name + QString(".png");

    runProcess("tesseract", QStringList() << img
            << name << "nobatch" << "box.train.stderr");
    */
}

void QTesseract::makeFontPropertiesFile(const FontProperties &prop){
    QFile file(QString("font_properties"));
    if(file.open(QIODevice::WriteOnly)){
        QString str = QString("%1 %2 %3 %4 %5 %6").arg(m_font)
                                                  .arg(prop.italic)
                                                  .arg(prop.bold)
                                                  .arg(prop.fixed)
                                                  .arg(prop.serif)
                                                  .arg(prop.fraktur);
        file.write(str.toLocal8Bit());
    }
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

void QTesseract::showMsg(const QString &text){
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void QTesseract::runProcess(const QString &name, const QStringList &args){
    QProcess proc;
    proc.start(name, args);
    std::cout << "run process: " << name.toStdString() << std::endl;
    
    if(!proc.waitForStarted())
        std::cout << "start failed" << std::endl;

    proc.write("Qt rocks!");
    proc.closeWriteChannel();

    if(!proc.waitForFinished())
        std::cout << "finish failed" << std::endl;

    QByteArray ret = proc.readAll();
    std::cout << ret.constData() << std::endl;
}

void QTesseract::training(const QString &img, const FontProperties &prop){
#if 0
    //runProcess("ln", QStringList() << "-s" << img << OUTPUT_DIR);

    runProcess("tesseract", QStringList() << img
                                          << QString("%1.%2.exp0").arg(m_lang).arg(m_font)
                                          << "-l" << m_lang
                                          << "batch.nochop" << "makebox");
    
    runProcess("mv", QStringList() << QString("%1.%2.exp0.box").arg(m_lang).arg(m_font)
                                   << OUTPUT_DIR);
    
    QString name = QString(OUTPUT_DIR) + QString("/%1.%2.exp0").arg(m_lang).arg(m_font);
    
    QString lnImg = QString("%1/%2").arg(OUTPUT_DIR).arg(img);
    runProcess("tesseract", QStringList() << lnImg
                                          << name 
                                          << "nobatch" << "box.train.stderr"); 

    //makeTrainingFile();
    makeUnicharsetFile(0);
    makeFontPropertiesFile(prop);

    QString tr = QString("%1/%2.%3.exp0.tr").arg(OUTPUT_DIR).arg(m_lang).arg(m_font);
    QString font_properties = QString("%1/font_properties").arg(OUTPUT_DIR);
    QString unicharset = QString("%1/unicharset").arg(OUTPUT_DIR);
    
    runProcess("mftraining", QStringList() << "-D" << OUTPUT_DIR
                                           << "-F" << font_properties
                                           << "-U" << unicharset
                                           << tr);
    
    runProcess("mftraining", QStringList() << "-D" << OUTPUT_DIR
                                           << "-F" << font_properties
                                           << "-U" << unicharset
                                           << "-O" << QString("%1/%2.unicharset").arg(OUTPUT_DIR).arg(m_lang)
                                           << QString("%1/unicharset").arg(OUTPUT_DIR) 
                                           << tr);

    runProcess("cntraining", QStringList() << "-D" << OUTPUT_DIR << tr);
    
    runProcess("mv", QStringList() << QString("%1/inttemp").arg(OUTPUT_DIR)
                                   << QString("%1/%2.inttemp").arg(OUTPUT_DIR).arg(m_lang));
    runProcess("mv", QStringList() << QString("%1/shapetable").arg(OUTPUT_DIR)
                                   << QString("%1/%2.shapetable").arg(OUTPUT_DIR).arg(m_lang));
    runProcess("mv", QStringList() << QString("%1/pffmtable").arg(OUTPUT_DIR)
                                   << QString("%1/%2.pffmtable").arg(OUTPUT_DIR).arg(m_lang));
    runProcess("mv", QStringList() << QString("%1/normproto").arg(OUTPUT_DIR)
                                   << QString("%1/%2.normproto").arg(OUTPUT_DIR).arg(m_lang));

    runProcess("combine_tessdata", QStringList() << QString("%1/%2.").arg(OUTPUT_DIR).arg(m_lang));
#else
    runProcess("tesseract", QStringList() << img
                                          << QString("%1.%2.exp0").arg(m_lang).arg(m_font)
                                          << "-l" << m_lang
                                          << "batch.nochop" << "makebox");
    
    QString name = QString("%1.%2.exp0").arg(m_lang).arg(m_font);
    runProcess("tesseract", QStringList() << img
                                          << name 
                                          << "nobatch" << "box.train.stderr"); 
    
    runProcess("unicharset_extractor", QStringList() << QString("%1.%2.exp0.box").arg(m_lang).arg(m_font));
    makeFontPropertiesFile(prop);

    QString tr = QString("%1.%2.exp0.tr").arg(m_lang).arg(m_font);
    QString font_properties = QString("font_properties");
    QString unicharset = QString("unicharset");
    
    runProcess("mftraining", QStringList() << "-F" << font_properties
                                           << "-U" << unicharset
                                           << tr);
    
    runProcess("mftraining", QStringList() << "-F" << font_properties
                                           << "-U" << unicharset
                                           << "-O" << QString("%1.unicharset").arg(m_lang)
                                           << QString("unicharset")
                                           << tr);

    runProcess("cntraining", QStringList() << tr);
    
    runProcess("mv", QStringList() << QString("inttemp")
                                   << QString("%1.inttemp").arg(m_lang));
    runProcess("mv", QStringList() << QString("shapetable")
                                   << QString("%1.shapetable").arg(m_lang));
    runProcess("mv", QStringList() << QString("pffmtable")
                                   << QString("%1.pffmtable").arg(m_lang));
    runProcess("mv", QStringList() << QString("normproto")
                                   << QString("%1.normproto").arg(m_lang));
    
    runProcess("combine_tessdata", QStringList() << QString("%1.").arg(m_lang)); 
#endif 
}

QTesseract::~QTesseract(){
    m_api->End();
    delete m_api;
}

