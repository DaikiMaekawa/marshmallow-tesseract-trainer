/*******************************************************************
* Copyright (c) 2014 Daiki Maekawa
*
* @file TesseractTrainer.cpp
* @brief tesseract trainer gui
* @author Daiki Maekawa
* @date 2014-02-07
*******************************************************************/ 

#include "TesseractTrainer.h"
#include "ui_TesseractTrainer.h"
#include <iostream>
#include <cassert>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QIODevice>
#include <QImage>

static const char * const SETTING_FILE_NAME = ".tesseracttrainerrc";
TesseractTrainer::TesseractTrainer(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::TesseractTrainer),
    m_tess("eng", "hiragi")
{
    m_ui->setupUi(this);
    connectSignals();
    loadSettings();
}

void TesseractTrainer::connectSignals(){
    bool ret;
    ret = connect(m_ui->actionLoad, SIGNAL(triggered()), this, SLOT(onLoadImage()));
    assert(ret);
    ret = connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(onSaveLang()));
    assert(ret);
    ret = connect(m_ui->pushTraining, SIGNAL(clicked()), this, SLOT(onPushTraining()));
    assert(ret);
}

void TesseractTrainer::loadSettings(){

    QFile file(QDir::homePath() + "/" + QString(SETTING_FILE_NAME));
    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        m_imgDir = stream.readLine(1024);
    }
}

void TesseractTrainer::saveSettings(){
    QFile file(QDir::homePath() + "/" + QString(SETTING_FILE_NAME));
    if(file.open(QIODevice::WriteOnly)){
        file.write(m_imgDir.toLocal8Bit());
        file.write("\n");
    }
}

void TesseractTrainer::onLoadImage(){
    QString file = QFileDialog::getOpenFileName(this, "Open File", m_imgDir, "Image File(*)");
    m_imgDir = file;
    QImage img(file);
    m_ui->labelImg->setPixmap(QPixmap::fromImage(img));
}

void TesseractTrainer::onPushTraining(){
    QVector<TessChar> boxes = m_tess.getTessBoxes(m_imgDir);
    FontProperties prop;
    prop.italic = m_ui->checkItalic->checkState();
    prop.bold = m_ui->checkBold->checkState();
    prop.fixed = m_ui->checkFixed->checkState();
    prop.serif = m_ui->checkSerif->checkState();
    prop.fraktur = m_ui->checkFraktur->checkState();
    m_tess.training(m_imgDir, prop); 
}

void TesseractTrainer::onSaveLang(){
    std::cout << "save" << std::endl;
    
}

TesseractTrainer::~TesseractTrainer(){
    saveSettings();
    delete m_ui;
}

