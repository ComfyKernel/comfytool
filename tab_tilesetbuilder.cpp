#include "tab_tilesetbuilder.h"
#include "ui_tab_tilesetbuilder.h"

#include <QXmlStreamReader>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPainter>
#include <QDebug>

class CTilePreview : public QWidget {
public:
    QPen    pen;
    QBrush  brush;
    QPixmap pmx;
    tab_tilesetbuilder* ttb;

    CTilePreview(QWidget* parent = nullptr, tab_tilesetbuilder* tbuilder = nullptr)
        : QWidget(parent){
        ttb = tbuilder;

        brush.setColor(Qt::GlobalColor::white);
        brush.setStyle(Qt::BrushStyle::SolidPattern);

        pen.setColor(Qt::GlobalColor::red);
        pen.setStyle(Qt::PenStyle::SolidLine);
        pen.setWidth(1);
    }

    void setPixmap(const QString& file) {
        pmx.load(file);
    }

    void paintEvent(QPaintEvent*) {
        qInfo()<<"Painting";

        QPainter p(this);
        p.setBrush(brush);
        p.setPen  (pen);

        p.drawPixmap(0, 0, pmx.width(), pmx.height(), pmx);

        for(int x = 0; x < pmx.width() / ttb->info.width; ++x) {
            for(int y = 0; y < pmx.height() / ttb->info.height; ++y) {
                qInfo()<<"On square";
                p.drawRect(QRect((x * ttb->info.width) + (ttb->info.spacing * x) + ttb->info.margin, (y * ttb->info.height) + (ttb->info.spacing * y) + ttb->info.margin,
                                 ttb->info.width, ttb->info.height));
            }
        }
    }
};

tab_tilesetbuilder::tab_tilesetbuilder(QWidget *parent) :
    CTabScreen(parent),
    ui(new Ui::tab_tilesetbuilder) {
    ui->setupUi(this);

    setLoadable(true);
    setSavable (true);
    setModeChangeAllowed(false);

    connect(findChild<QLineEdit*>("le_path"  ), &QLineEdit::editingFinished, this, &tab_tilesetbuilder::applyVisualSettings);
    connect(findChild<QLineEdit*>("le_holder"), &QLineEdit::editingFinished, this, &tab_tilesetbuilder::applyVisualSettings);

    connect(findChild<QSpinBox*>("sb_margin" ), &QSpinBox::editingFinished, this, &tab_tilesetbuilder::applyVisualSettings);
    connect(findChild<QSpinBox*>("sb_spacing"), &QSpinBox::editingFinished, this, &tab_tilesetbuilder::applyVisualSettings);
    connect(findChild<QSpinBox*>("sb_width"  ), &QSpinBox::editingFinished, this, &tab_tilesetbuilder::applyVisualSettings);
    connect(findChild<QSpinBox*>("sb_height" ), &QSpinBox::editingFinished, this, &tab_tilesetbuilder::applyVisualSettings);

    QVBoxLayout* l = findChild<QVBoxLayout*>("vl_preview");

    CTilePreview* ctp = new CTilePreview(this, this);
    ctp->setObjectName("ctp_main");
    ctp->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    ctp->setMinimumSize(400, 400);
    l->addWidget(ctp);
    l->setAlignment(ctp, Qt::AlignHCenter);
}

const QList<QMenu*> tab_tilesetbuilder::menus(QWidget *parent) const {
    QList<QMenu*> ret;

    QMenu* m_home = new QMenu(parent);
    m_home->setTitle("Tileset");
    /*QAction* a_about = new QAction(m_home);
    a_about->setText("About");
    m_home->addAction(a_about);*/

    ret.append(m_home);
    return ret;
}

QString tab_tilesetbuilder::getName() {
    return "Tileset";
}

CTabScreen* tab_tilesetbuilder::makeNew(QWidget *parent) const {
    return new tab_tilesetbuilder(parent);
}

void tab_tilesetbuilder::updateVisualInfo() {
    findChild<QLineEdit*>("le_path"  )->setText(texinfo.path);
    findChild<QLineEdit*>("le_holder")->setText(texinfo.placeholder);

    findChild<QSpinBox*>("sb_margin" )->setValue(info.margin);
    findChild<QSpinBox*>("sb_spacing")->setValue(info.spacing);
    findChild<QSpinBox*>("sb_width"  )->setValue(info.width);
    findChild<QSpinBox*>("sb_height" )->setValue(info.height);
}

void tab_tilesetbuilder::applyVisualSettings() {
    qInfo()<<"Applying Tileset Settings";

    texinfo.path        = findChild<QLineEdit*>("le_path"  )->text();
    texinfo.placeholder = findChild<QLineEdit*>("le_holder")->text();

    info.margin  = findChild<QSpinBox*>("sb_margin" )->value();
    info.spacing = findChild<QSpinBox*>("sb_spacing")->value();
    info.width   = findChild<QSpinBox*>("sb_width"  )->value();
    info.height  = findChild<QSpinBox*>("sb_height" )->value();

    QString cfile = _current_file;
    cfile = cfile.remove(cfile.lastIndexOf('/') + 1, cfile.length() - cfile.lastIndexOf('/'));
    cfile += texinfo.placeholder;
    qInfo()<<"Setting pixmap : "<<cfile;

    CTilePreview* ctp = findChild<CTilePreview*>("ctp_main");
    ctp->setPixmap(cfile);
    ctp->repaint();

    updateVisualInfo();
}

bool tab_tilesetbuilder::saveFile(const QString &file) const {
    qInfo()<<"Saving unimplemented";
    return false;
}

bool tab_tilesetbuilder::loadFile(const QString &file) {
    qInfo()<<"Loading file "<<file;
    QFile qf(file);
    if(!qf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qInfo()<<"Failed opening file '"<<file<<"' "<<qf.errorString();
        return false;
    }

    QXmlStreamReader xsr(&qf);
    while(!xsr.atEnd() && !xsr.hasError()) {
        QXmlStreamReader::TokenType token = xsr.readNext();
        switch(token) {
        case QXmlStreamReader::StartElement: {
            QXmlStreamAttributes xsa = xsr.attributes();
            if(xsr.name() == "texture") {
                for(const QXmlStreamAttribute& a : xsa) {
                    if(a.name() == "path") {
                        texinfo.path = a.value().toString();
                        continue;
                    } else if(a.name() == "placeholder") {
                        texinfo.placeholder = a.value().toString();
                        continue;
                    }
                }
            } else if(xsr.name() == "parameters") {
                for(const QXmlStreamAttribute& a : xsa) {
                    if(a.name() == "margin") {
                        info.margin = a.value().toInt();
                        continue;
                    } else if(a.name() == "spacing") {
                        info.spacing = a.value().toInt();
                        continue;
                    } else if(a.name() == "width") {
                        info.width = a.value().toUInt();
                        continue;
                    } else if(a.name() == "height") {
                        info.height = a.value().toUInt();
                        continue;
                    }
                }
            }
        }
        default:
            continue;
        }
    }

    updateVisualInfo();

    if(xsr.hasError()) {
        qInfo()<<"XML Parsing error : "<<xsr.errorString();
        qf.close();
    }

    qf.close();
    return true;
}

tab_tilesetbuilder::~tab_tilesetbuilder() {
    delete ui;
}
