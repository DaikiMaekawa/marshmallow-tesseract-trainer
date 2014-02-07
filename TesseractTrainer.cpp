/*******************************************************************
* Copyright (c) 2014 Daiki Maekawa
*
* @file TesseractTrainer.cpp
* @brief tesseract trainer gui
* @author Daiki Maekawa
* @date 2012-09-10
*******************************************************************/ 

#include "TesseractTrainer.h"
#include "ui_TesseractTrainer.h"
#include "baseapi.h"
#include "strngs.h"
#include <iostream>
#include <cassert>

TesseractTrainer::TesseractTrainer(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::TesseractTrainer)
{
    m_ui->setupUi(this);
    connectSignals();
}

void TesseractTrainer::connectSignals(){
    bool ret;
    ret = connect(m_ui->actionLoad, SIGNAL(triggered()), this, SLOT(onLoadImage()));
    assert(ret);
    ret = connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(onSaveLang()));
    assert(ret);
}

void TesseractTrainer::onLoadImage(){
    std::cout << "load" << std::endl;
}

void TesseractTrainer::onSaveLang(){
    std::cout << "save" << std::endl;
}

TesseractTrainer::~TesseractTrainer()
{
    delete m_ui;
}

