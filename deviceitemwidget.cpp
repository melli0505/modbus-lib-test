#include "deviceitemwidget.h"
#include "ui_deviceitemwidget.h"

deviceitemwidget::deviceitemwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::deviceitemwidget)
{
    ui->setupUi(this);
}

deviceitemwidget::~deviceitemwidget()
{
    delete ui;
}
