#include "tab_cafbuilder.h"
#include "ui_tab_cafbuilder.h"

#include <QStyledItemDelegate>
#include <QXmlStreamReader>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTreeWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include <QDebug>

// LUMP //

Lump::Lump(QObject* parent)
    : QObject(parent) { }
Lump::Lump(const bLump& l, QObject* parent)
    : _name(l.name), _path(l.path), _type(l.type),
      _data(l.data), _revision(l.revision),
      QObject(parent) { }

void Lump::setName(const QString &name) {
    _name = name;
    emit modified();
}
void Lump::setPath(const QString &path) {
    _path = path;
    emit modified();
}
void Lump::setType(const QString &type) {
    _type = type;
    emit modified();
}
void Lump::setData(const QString &data) {
    _data = data;
    emit modified();
}
void Lump::setRevision(unsigned revision) {
    _revision = revision;
    emit modified();
}

void Lump::setName(const QVariant &var) {
    _name = var.toString();
    emit modified();
}
void Lump::setPath(const QVariant &var) {
    _path = var.toString();
    emit modified();
}
void Lump::setType(const QVariant &var) {
    _type = var.toString();
    emit modified();
}
void Lump::setData(const QVariant &var) {
    _data = var.toString();
    emit modified();
}
void Lump::setRevision(const QVariant &var) {
    _revision = var.toUInt();
    emit modified();
}

QString Lump::name() const {
    return _name;
}
QString Lump::path() const {
    return _path;
}
QString Lump::type() const {
    return _type;
}
QString Lump::data() const {
    return _data;
}
unsigned Lump::revision() const {
    return _revision;
}

bLump Lump::tobLump() const {
    return {.name = _name, .path = _path, .type = _type, .data = _data, .revision = _revision};
}

// !LUMP //

CT_WI_Helper::CT_WI_Helper(QObject *parent)
    : QObject(parent) { }

CTreeWidgetItem::CTreeWidgetItem(QTreeWidget* parent)
    : QTreeWidgetItem(parent) { }
CTreeWidgetItem::CTreeWidgetItem(QTreeWidgetItem* parent)
    : QTreeWidgetItem(parent) { }

void CTreeWidgetItem::setData(int column, int role, const QVariant& value) {
    QTreeWidgetItem::setData(column, role, value);
    emit helper.hasChanged(value);
    QString t = text(0);
    if(t == "Name") {
        parent()->setText(0, value.toString());
    }
}

class CItemDelegate : public QStyledItemDelegate {
public:
    QTreeWidget* tree;

    CItemDelegate(QObject* parent = nullptr, QTreeWidget* qtw = nullptr)
        : QStyledItemDelegate(parent), tree(qtw) {
    }

    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        QLineEdit* qle = new QLineEdit(parent);

        CTreeWidgetItem* ti  = (CTreeWidgetItem*)tree->currentItem();

        qInfo()<<ti->text(0)<<" "<<ti->text(1);

        switch(ti->vtype) {
        case CTreeWidgetItem::Number: {
            QIntValidator* qiv = new QIntValidator(0, 65535, qle);
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

    findChild<QHBoxLayout*>("hl_bar")->addStretch();

    QTreeWidget* qtw = findChild<QTreeWidget*>("tw_lumps");;

    qtw->setItemDelegateForColumn(0, new NoEditDelegate(qtw));
    qtw->setItemDelegateForColumn(1, new CItemDelegate (qtw, qtw));

    qtw->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(qtw , &QTreeWidget::itemChanged,
            this, &tab_cafbuilder::handleValueChanged);

    connect(findChild<QToolButton*>("tbtn_remove"), &QToolButton::clicked, [=]() {
        removeVisItem(qtw->currentItem());
    });
    connect(findChild<QToolButton*>("tbtn_add"), &QToolButton::clicked, [=]() {
        addLump({.name="New Lump", .path="/", .type="none", .data="", .revision=0});
    });
    connect(findChild<QToolButton*>("tbtn_clone"), &QToolButton::clicked,[=]() {
        if(CTreeWidgetItem* item = dynamic_cast<CTreeWidgetItem*>(qtw->currentItem())) {
            addLump(item->lump->tobLump());
        }
    });
}

tab_cafbuilder::~tab_cafbuilder() {
    delete ui;
}

void tab_cafbuilder::removeVisItem(QTreeWidgetItem *item) {
    if(CTreeWidgetItem* citem = dynamic_cast<CTreeWidgetItem*>(item)) {
        if(citem->notLump)
            return;

        lumps.removeAll(citem->lump);
        delete citem->lump;
        delete citem;
    }
}

void tab_cafbuilder::addLump(const bLump &lump) {
    Lump* l = new Lump(lump);
    l->setObjectName(l->name());
    lumps.append(l);

    QTreeWidget*     qtw  = findChild<QTreeWidget*>("tw_lumps");
    QTreeWidgetItem* qtwi = qtw->itemAt(0, 0);

    CTreeWidgetItem* twi_head = new CTreeWidgetItem(qtwi);
    twi_head->setText(0, lump.name);
    twi_head->lump = l;

#define addWidget(name, str, value, valtype, slot) \
    CTreeWidgetItem* i_##name = new CTreeWidgetItem(twi_head); \
    i_##name->vtype = valtype; \
    i_##name->setText(0, str); \
    i_##name->setText(1, value); \
    i_##name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable); \
    i_##name->lump = l; \
    connect(&i_##name->helper, SIGNAL(hasChanged(const QVariant&)), l, slot); \
    twi_head->addChild(i_##name);

    addWidget(name, "Name", lump.name, CTreeWidgetItem::Text, SLOT(setName(const QVariant&)));
    addWidget(path, "Path", lump.path, CTreeWidgetItem::Text, SLOT(setPath(const QVariant&)));
    addWidget(type, "Type", lump.type, CTreeWidgetItem::Text, SLOT(setType(const QVariant&)));
    addWidget(data, "Data", lump.data, CTreeWidgetItem::Text, SLOT(setData(const QVariant&)));

    addWidget(revision, "Revision", QString::number(lump.revision), CTreeWidgetItem::Number, SLOT(setRevision(const QVariant&)));

#undef addWidget

    qtwi->addChild(twi_head);

    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addStretch();
    hbl->setDirection(QHBoxLayout::LeftToRight);
    hbl->setSpacing(0);
    hbl->setContentsMargins(0, 0, 0, 0);

    QWidget* qw = new QWidget();
    qw->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    QPushButton* qpb = new QPushButton();
    qpb->setIcon(QIcon::fromTheme("edit-delete"));
    qpb->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));

    hbl->addWidget(qw);
    hbl->addWidget(qpb);

    connect(qpb, &QPushButton::clicked,
            [=]() {
                /*lumps.removeAll(l);
                delete l;
                delete twi_head;*/
                removeVisItem(twi_head);
            });

    QWidget* qmain = new QWidget();
    qmain->setLayout(hbl);

    qtw->setItemWidget(twi_head, 1, qmain);
}

void tab_cafbuilder::addRoot() {
    QTreeWidgetItem* qtwi = findChild<QTreeWidget*>("tw_lumps")->itemAt(0, 0);

    CTreeWidgetItem* twi_head = new CTreeWidgetItem(qtwi);
    twi_head->setText(0, "Info");
    twi_head->notLump = true;

#define addWidget(name, str, value, valtype, slot) \
    CTreeWidgetItem* i_##name = new CTreeWidgetItem(twi_head); \
    i_##name->vtype = valtype; \
    i_##name->setText(0, str); \
    i_##name->setText(1, value); \
    i_##name->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable); \
    connect(&i_##name->helper, SIGNAL(hasChanged(const QVariant&)), this, slot); \
    twi_head->addChild(i_##name);

    addWidget(name, "Asset Path", rootinfo.path, CTreeWidgetItem::Text, SLOT(setRootPath(const QVariant&)));
    addWidget(revision, "Revision", QString::number(rootinfo.revision), CTreeWidgetItem::Number, SLOT(setRootRevision(const QVariant&)));

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

void tab_cafbuilder::setRootPath(const QVariant& var) {
    rootinfo.path = var.toString();
}

void tab_cafbuilder::setRootRevision(const QVariant& var) {
    rootinfo.revision = var.toUInt();
}

CTabScreen* tab_cafbuilder::makeNew(QWidget *parent) const {
    return new tab_cafbuilder(parent);
}

bool tab_cafbuilder::saveFile(const QString &file) const {
    QFile qf(file);

    if(!qf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qInfo()<<"Failed opening file '"<<file<<"'";
        return false;
    }

    QXmlStreamWriter xsw(&qf);
    xsw.setAutoFormatting      (true);
    xsw.setAutoFormattingIndent(true);

    xsw.writeStartDocument();

    xsw.writeStartElement("root");
    xsw.writeAttribute   ("type", "cafbuilder");

    xsw.writeStartElement("info");
    xsw.writeAttribute   ("major", "1");
    xsw.writeAttribute   ("minor", "1");
    xsw.writeAttribute   ("revision", QString::number(rootinfo.revision));
    xsw.writeAttribute   ("path"    , rootinfo.path);
    xsw.writeEndElement();

    for(const Lump* l : lumps) {
        xsw.writeStartElement("lumpitem");
        xsw.writeAttribute   ("name", l->name());
        xsw.writeAttribute   ("path", l->path());
        xsw.writeAttribute   ("type", l->type());
        xsw.writeAttribute   ("data", l->data());
        xsw.writeAttribute   ("revision", QString::number(l->revision()));
        xsw.writeEndElement();
    }

    xsw.writeEndElement();

    xsw.writeEndDocument();

    qf.close();
    return true;
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
            bLump l;
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

    findChild<QTreeWidget*>("tw_lumps")->itemAt(0, 0)->setText(0, rootinfo.path);

    qf.close();
    return true;
}
