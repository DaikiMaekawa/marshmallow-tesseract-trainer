#include "TesseractTrainer.h"
#include "ui_TesseractTrainer.h"
#include "baseapi.h"
#include "strngs.h"
#include <iostream>

TesseractTrainer::TesseractTrainer(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TesseractTrainer)
{
    ui->setupUi(this);
}

TesseractTrainer::~TesseractTrainer()
{
    delete ui;
}
