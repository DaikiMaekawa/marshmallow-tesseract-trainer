#include "TesseractTrainer.h"
#include "ui_TesseractTrainer.h"

TesseractTrainer::TesseractTrainer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TesseractTrainer)
{
    ui->setupUi(this);
}

TesseractTrainer::~TesseractTrainer()
{
    delete ui;
}
