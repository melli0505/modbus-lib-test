#include "itemdetailwidget.h"
#include "ui_itemdetailwidget.h"

#include "devicelist.h"

ItemDetailWidget::ItemDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemDetailWidget)
{
    ui->setupUi(this);
}

ItemDetailWidget::~ItemDetailWidget()
{
    delete ui;
}

void ItemDetailWidget::setDetailInfo(DeviceItem item)
{
    QPixmap pixmap(":/img/" + item.name.toLower() + ".jpg");
    ui->image->setPixmap(pixmap);
    ui->image->setScaledContents(true);

    ui->nameLabel->setText(item.name);
    ui->categoryLabel->setText(item.category);
}


