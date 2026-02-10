#include "scrollareaex.h"
#include "devicelist.h"

#include <QScrollArea>
#include <QPushButton>
#include <QButtonGroup>

ScrollAreaEx::ScrollAreaEx(QWidget *parent)
    : QScrollArea(parent)
{
    container = new QWidget();
    layout = new QVBoxLayout(container);
    layout->setAlignment(Qt::AlignTop);

    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    this->setWidgetResizable(true);
    this->setWidget(container);

    // connect
    connect(buttonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &ScrollAreaEx::selectedItem);
}

ScrollAreaEx::~ScrollAreaEx()
{
}

void ScrollAreaEx::addItem(QString name, int id)
{
    QPushButton *btn = new QPushButton(name);
    btn->setCheckable(true);

    btn->setStyleSheet(
                "QPushButton { padding: 10px; border: 1px solid #ccc; }"
                "QPushButton:checked { background-color: #382984; color: white; border: none; }"
    );

    buttonGroup->addButton(btn, id);
    layout->addWidget(btn);
}


int ScrollAreaEx::selectedId() const
{
    return buttonGroup->checkedId();
}
