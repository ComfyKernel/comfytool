#include "tab_cafbuilder.h"
#include "ui_tab_cafbuilder.h"

#include <QStyledItemDelegate>
#include <QXmlStreamReader>
#include <QHeaderView>
#include <QTreeWidget>
#include <QLineEdit>
#include <QDebug>

class CTreeWidgetItem : public QTreeWidgetItem {
public:
    CTreeWidgetItem(QTreeWidget* parent = nullptr)
        : QTreeWidgetItem(parent) { }
    CTreeWidgetItem(QTreeWidgetItem* parent = nullptr)
        : QTreeWidgetItem(parent) { }

    enum ValueType {
        Number,
        Text
    };

    ValueType vtype = Text;
};

class CItemDelegate : public QStyledItemDelegate {
public:
    QTreeWidget* tree;

    CItemDelegate(QObject* parent = nullptr, QTreeWidget* qtw = nullptr)
        : QStyledItemDelegate(parent), tree(qtw) { }

    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        QLineEdit* qle = new QLineEdit(parent);

        CTreeWidgetItem* pti = (CTreeWidgetItem*)tree->itemAt(0, 0)->child(index.parent().row());
        CTreeWidgetItem* ti  = (CTreeWidgetItem*)pti->child(index.row());

        qInfo()<<"Parent at '"<<index.parent().row()<<"'";
        qInfo()<<"Is CTreeWidget at '"<<index.column()<<" "<<index.row()<<"'";
        qInfo()<<ti->text(0)<<" "<<ti->text(1);

        switch(ti->vtype) {
        case CTreeWidgetItem::Number: {
            QIntValidator* qiv = new QIntValidator(0, 999999, qle);
            qle->setValidator(qiv);
            break;
        }
        case CTreeWidgetItem::Text:
            break;
        default:
            qInfo()<<"CType Unknown";
            break;
        }

        return qle;
    }
};

class NoEditDelegate : public QStyledItemDelegate {
public:
    NoEditDelegate(QObject* parent = nullptr)
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
    qtw->setItemDelegateForColumn(1, new CItemDelegate (qtw, qtw));

    qtw->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(qtw , &QTreeWidget::itemChanged,
            this, &tab_cafbuilder::handleValueChanged);
}

tab_cafbuilder::~tab_cafbuilder() {
    delete ui;
}

void tab_cafbuilder::addLump(const Lump &lump) {
    QTreeWidgetItem* qtwi = findChild<QTreeWidget*>("tw_lumps")->itemAt(0, 0);

    CTreeWidgetItem* twi_head = new CTreeWidgetItem(qtwi);
    twi_head->setText(0, lump.name);

#define addWidget(name, str, value, valtype) \
    CTreeWidgetItem* i_##name = new CTreeWidgetItem(twi_head); \
    i_##name->vtype = valtype; \
    i_##name->setText(0, str); \
    i_##name->setText(1, value); \
    i_##name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable); \
    twi_head->addChild(i_##name);

    addWidget(name, "Name", lump.name, CTreeWidgetItem::Text);
    addWidget(path, "Path", lump.path, CTreeWidgetItem::Text);
    addWidget(type, "Type", lump.type, CTreeWidgetItem::Text);
    addWidget(data, "Data", lump.data, CTreeWidgetItem::Text);

    addWidget(revision, "Revision", QString::number(lump.revision), CTreeWidgetItem::Number);

#undef addWidget

    qtwi->addChild(twi_head);
}

void tab_cafbuilder::addRoot() {
    QTreeWidgetItem* qtwi = findChild<QTreeWidget*>("tw_lumps")->itemAt(0, 0);

    CTreeWidgetItem* twi_head = new CTreeWidgetItem(qtwi);
    twi_head->setText(0, "Info");

#define addWidget(name, str, value, valtype) \
    CTreeWidgetItem* i_##name = new CTreeWidgetItem(twi_head); \
    i_##name->vtype = valtype; \
    i_##name->setText(0, str); \
    i_##name->setText(1, value); \
    i_##name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable); \
    twi_head->addChild(i_##name);

    addWidget(name, "Asset Path", rootinfo.path, CTreeWidgetItem::Text);

    addWidget(revision, "Revision", QString::number(rootinfo.revision), CTreeWidgetItem::Number);

#undef addWidget

    qtwi->insertChild(0, twi_head);
}

void tab_cafbuilder::handleValueChanged(QTreeWidgetItem *item, int column) {
    /*qInfo()<<"Item"<<item->text(0)<<"changed (value)"<<item->text(1)
          <<"On column ("<<column<<")";
    if(item->parent() != nullptr) {
        qInfo()<<"Parent "<<item->parent()->text(0);
    }*/
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
            bool isRootItem = false;

            for(const QXmlStreamAttribute& a : attrs) {
                if(xsr.name() == "info") {
                    isRootItem = true;
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
                break;
            }

            if(isRootItem) {
                addRoot();
                break;
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
