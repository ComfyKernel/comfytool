#include "uicafbuilderrootinfo.h"
#include "ui_uicafbuilderrootinfo.h"

UIcafbuilderrootinfo::UIcafbuilderrootinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UIcafbuilderrootinfo)
{
    ui->setupUi(this);
}

UIcafbuilderrootinfo::~UIcafbuilderrootinfo()
{
    delete ui;
}
