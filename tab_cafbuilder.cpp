#include "tab_cafbuilder.h"
#include "ui_tab_cafbuilder.h"

#include <QStyledItemDelegate>
#include <QXmlStreamReader>
#include <QHeaderView>
#include <QTreeWidget>
#include <QDebug>

class NoEditDelegate : public QStyledItemDelegate {
public:
    NoEditDelegate(QObject* parent = 0)
        : QStyledItemDelegate(parent) { }

    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        return nullptr;
    }
};

tab_cafbuilder::tab_cafbuilder(QWidget *parent) :
    CTabScreen(parent),
    ui(new Ui::tab_cafbuilder) {
    ui->setupUi(this);

    QTreeWidget* qtw = findChild<QTreeWidget*>("tw_lumps");
    qtw->setItemDelegateForColumn(0, new NoEditDelegate(qtw));
    qtw->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(qtw , &QTreeWidget::itemChanged,
            this, &tab_cafbuilder::handleValueChanged);
}

tab_cafbuilder::~tab_cafbuilder() {
    delete ui;
}

void tab_cafbuilder::addLump(const Lump &lump) {
    QTreeWidget* qtw = findChild<QTreeWidget*>("tw_lumps");

    QTreeWidgetItem* twi_head = new QTreeWidgetItem(qtw);
    twi_head->setText(0, lump.name);

#define addWidget(name, str, value) \
    QTreeWidgetItem* i_##name = new QTreeWidgetItem(twi_head); \
    i_##name->setText(0, str); \
    i_##name->setText(1, value); \
    i_##name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable); \
    twi_head->addChild(i_##name);

    addWidget(name, "Name", lump.name);
    addWidget(path, "Path", lump.path);
    addWidget(type, "Type", lump.type);
    addWidget(data, "Data", lump.data);

    addWidget(revision, "Revision", QString::number(lump.revision));

#undef addWidget

    qtw->addTopLevelItem(twi_head);
}

void tab_cafbuilder::handleValueChanged(QTreeWidgetItem *item, int column) {
    qInfo()<<"Item"<<item->text(0)<<"changed (value)"<<item->text(1)
          <<"On column ("<<column<<")";
    if(item->parent() != nullptr) {
        qInfo()<<"Parent "<<item->parent()->text(0);
    }
}

const QList<QMenu*> tab_cafbuilder::menus(QWidget *parent) const {
    QList<QMenu*> ret;

    QMenu* m_caf = new QMenu(parent);
    m_caf->setTitle("CAF Builder");
    QAction* a_build = new QAction(m_caf);
    a_build->setText("Build");
    m_caf->addAction(a_build);

    ret.append(m_caf);
    return ret;
}

QString tab_cafbuilder::getName() {
    return "CAF Builder";
}

CTabScreen* tab_cafbuilder::makeNew(QWidget *parent) const {
    return new tab_cafbuilder(parent);
}

bool tab_cafbuilder::saveFile(const QString &file) const {
    qInfo()<<"Saving unimplemented";
    return false;
}

bool tab_cafbuilder::loadFile(const QString &file) {
    QFile qf(file);

    if(!qf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qInfo()<<"Failed opening file '"<<file<<"'";
        return false;
    }

    QXmlStreamReader xsr(&qf);

    while(!xsr.atEnd() && !xsr.hasError()) {
        QXmlStreamReader::TokenType token = xsr.readNext();

        switch(token) {
        case QXmlStreamReader::StartElement: {
            QXmlStreamAttributes attrs = xsr.attributes();
            Lump l;
            bool isLumpItem = false;

            for(const QXmlStreamAttribute& a : attrs) {
                if(xsr.name() == "info") {
                    if(a.name() == "major") {
                        continue;
                    }
                    if(a.name() == "minor") {
                        continue;
                    }
                    if(a.name() == "path") {
                        rootinfo.path = a.value().toString();
                        continue;
                    }
                    if(a.name() == "revision") {
                        rootinfo.revision = a.value().toUInt();
                        continue;
                    }
                } else if(xsr.name() == "lumpitem") {
                    isLumpItem = true;
                    if(a.name() == "name") {
                        l.name = a.value().toString();
                        continue;
                    }
                    if(a.name() == "path") {
                        l.path = a.value().toString();
                        continue;
                    }
                    if(a.name() == "type") {
                        l.type = a.value().toString();
                        continue;
                    }
                    if(a.name() == "data") {
                        l.data = a.value().toString();
                        continue;
                    }
                    if(a.name() == "revision") {
                        l.revision = a.value().toUInt();
                        continue;
                    }
                }
            }

            if(isLumpItem) {
                addLump(l);
            }

            break;
        }
        default:
            continue;
        }
    }

    if(xsr.hasError()) {
        qInfo()<<"Failed parsing CXF file. Error : "<<xsr.errorString();
        return false;
    }

    qf.close();
    return true;
}
