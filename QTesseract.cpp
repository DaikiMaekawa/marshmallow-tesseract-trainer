#include "QTesseract.h"
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <iostream>

static const char * const SETTING_ORGANIZATION_NAME = "marshmallow-tesseract-trainer";
static const char * const SETTING_APP_NAME = "MarshmallowTesseractTrainer";
static const char * const UNICHARSET_EXTRACTOR = "unicharset_extractor";

void set_properties(UNICHARSET *unicharset, const char* const c_string) {
#ifdef USING_WCTYPE
    UNICHAR_ID id;
    int wc;
    id = unicharset->unichar_to_id(c_string);
    unicharset->set_other_case(id, id);
    int step = UNICHAR::utf8_step(c_string);
    if (step == 0)
        return; // Invalid utf-8.
    
    UNICHAR ch(c_string, step);
    wc = ch.first_uni();
    if (iswalpha(wc)) {
        unicharset->set_isalpha(id, 1);
        if (iswlower(wc)) {
            unicharset->set_islower(id, 1);
            unicharset->set_other_case(id, wc_to_unichar_id(*unicharset,towupper(wc)));
        }
        
        if(iswupper(wc)) {
            unicharset->set_isupper(id, 1);
            unicharset->set_other_case(id, wc_to_unichar_id(*unicharset,towlower(wc)));
        }
    }
    
    if (iswdigit(wc))
        unicharset->set_isdigit(id, 1);
    if(iswpunct(wc))
        unicharset->set_ispunctuation(id, 1);
#endif
}

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
    //QByteArray byteArray = lang.toAscii();
    //const char *apiLang = byteArray.constData();
    //UNIX only
    //QByteArray byteArray1 = dataPath.toUtf8();
    //const char *datapath = byteArray1.data();
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
#if 1
    QProcess extractor;
    extractor.start(UNICHARSET_EXTRACTOR, QStringList() << "eng.hiragi.exp0.box");
    if(!extractor.waitForStarted())
        std::cout << "debug01" << std::endl;

    extractor.write("Qt rocks!");
    extractor.closeWriteChannel();

    if(!extractor.waitForFinished())
        std::cout << "debug02" << std::endl;

    QByteArray ret = extractor.readAll();

#else
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

#endif
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
/*
void MasterTrainer* loadTrainingData(int argc, const char* const *argv, bool replication, ShapeTable **shapeTable, STRING *filePrefix){

}
*/
bool training(){
    /*
    int argc = 3;
    char **argv;
    ParseArguments(&argc, &argv);

    ShapeTable *shapeTable = NULL;
    STRING filePrefix = "";
    bool shapeAnalysis = false;
    bool replication = false;
    MasterTrainer *trainer = new MasterTrainer(NM_CHAR_ANISOTROPIC,
            shapeAnalysis, replication, FLAGS_debug_level);
    */
    /*
    if(FLAGS_input_trainer.empty()){
        trainer->LoadUnicharset(FLAGS_U.c_str());
        if(!FLAGS_F.empty() && !trainer->LoadFontInfo(FLAGS_F.c_str())){
            delete trainer;
            return NULL;
        }

        IntFeatureSpace fs;
        fs.Init(kBoostXYBuckets, kBoostXYBuckets, kBoostDirBuckets);
        trainer->SetFeatureSpace(fs);
        const char *pageName;
        while((pageName = GetNextFilename(argc, argv)) != NULL){
            std::cout << "page = " << pageName << std::endl;
            FILE *fp = Efopen(pageName, "rb");
            trainer->ReadTrainingSamples(fp, feature_defs, false);
            fclose(fp);
        
            int pageNameLen = strlen(pageName);
            char *fontinfoFileName = new char[pageNameLen + 7];
            strcpy(fontinfoFileName, pageName, pageNemaLen - 2);
            strcpy(fontinfoFileName + pageNameLen - 2, "fontinfo");
            trainer->AddSpacingInfo(fontinfoFileName);
            delete[] fontinfoFileName;

            if(FLAGS_load_images){
                STRING imageName = pageName;
                imageName.truncate_at(imageName.length() - 2);
                imageName += "tif";
                trainer->LoadPageImages(imageName.string());
            }
        }
        trainer->PostLoadCleanup();
    }
    */
    /*
    bool success = false;
    tprintf("Loading master trainer from file:%s\n",
            FLAGS_input_trainer.c_str());
    FILE *fp = fopen(FLAGS_input_trainer.c_str(), "rb");
    if(fp == NULL){
        tprintf("Can't read file %s to initialize master trainer\n", 
                FLAGS_input_trainer.c_str());
    }else{
        success = trainer->DeSerialize(false, fp);
        fclose(fp);
    }
    if(!success){
        tprintf("Deserialize of master trainer failed!\n");
        delete trainer;
        return NULL;
    }
    trainer->PreTrainingSetup();
    if(!FLAGS_O.empty() && !trainer->unicharset().save_to_file(FLAGS_O.c_str())){
        fprintf(stderr, "Failed to save unicharset to file %s\n", FLAGS_O.c_str());
        delete trainer;
        return NULL;
    }

    if(shapeTable != NULL){
        if(*shapeTable == NULL){
            *shapeTable = new ShapeTable;
            trainer->SetupFlatShapeTable(*shapeTable);
            tprintf("Flat shape table summary: %s\n",
                    352  (*shape_table)->SummaryStr().string());
        }
        (*shapeTable)->set_unicharset(trainer->unicharset());
    }
    
    if(trainer == NULL) return false;
    
    IndexMapBiDi configMap;
    SetupConfigMap(shapeTable, &configMap);
    WriteShapeTable(filePrefix, *shapeTable);
    UNICHARSET shapeSet;
    const UNICHARSET *unicharset = &trainer->unicharset();
    if(shapeTable->AnyMultipleUnichars()){
        unicharset = &shapeSet;
        int numShapes = configMap.CompactSize();
        for(int s = 0; s < numShapes; ++s){
            char shapeLabel[kMaxShapeLabelLength + 1];
            snprintf(shapeLabel, kMaxShapeLabelLength, "sh%04d", s);
            shapeSet.unichar_insert(shapeLabel);
        }
    }

    int numConfigs = shapeTable->NumShapes();
    LIST mfClasses = NIL_LIST;
    for(int s = 0; s < numConfigs; ++s){
        int unicharId, fontId;
        if(unicharset == &shapeSet){
            unicharId = configMap.SparseToCompact(s);
        }else{
            shapeTable->GetFirstUnicharAndFont(s, &unicharId, &fontId);
        }
        const char *classLabel = unicharset->id_to_unichar(unicharId);
        mfClasses = ClusterOneConfig(s, classLabel, mfClasses, *shapeTable, trainer);
    }
    STRING inttempFile = filePrefix;
    inttempFile += "inttemp";
    STRING pffmtableFile = filePrefix;
    pffmtableFile += "pffmtable";
    CLASS_STRUCT *floatClasses = SetUpForFloat2Int(*unicharset, mfClasses);
    trainer->WriteInttempAndPFFMTable(trainer->unicharset(), 
            *unicharset, *shapeTable, floatClasses, inttempFile.string(), pffmtableFile.string());
    delete [] floatClasses;
    FreeLabeledClassList(mfClasses);
    delete trainer;
    delete ShapeTable;
    std::cout << "done" << std::endl;
    if(!FLAGS_test_ch.empty()){
        std::cout << "Hit return to exit..." << std::endl;
        while(getchar() != '\n');
    }
    */
    return true;
}

QTesseract::~QTesseract(){
    m_api->End();
    delete m_api;
}

