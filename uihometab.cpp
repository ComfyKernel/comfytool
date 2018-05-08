#include "uihometab.h"
#include "ui_uihometab.h"

UIhometab::UIhometab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UIhometab)
{
    ui->setupUi(this);
}

UIhometab::~UIhometab()
{
    delete ui;
}
