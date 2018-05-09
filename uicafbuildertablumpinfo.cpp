#include "uicafbuildertablumpinfo.h"
#include "ui_uicafbuildertablumpinfo.h"

UIcafbuildertablumpinfo::UIcafbuildertablumpinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UIcafbuildertablumpinfo)
{
    ui->setupUi(this);
}

UIcafbuildertablumpinfo::~UIcafbuildertablumpinfo()
{
    delete ui;
}
