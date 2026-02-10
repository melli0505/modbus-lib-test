#include "scrollablewidgetex.h"
#include "ui_scrollablewidgetex.h"
#include "deviceitemwidget.h"

#include <QDebug>

scrollablewidgetex::scrollablewidgetex(QWidget *parent) :
    QScrollArea(parent)
{
//    ui->setupUi(this);

    contentWidget = new QWidget();
    layout = new QVBoxLayout();

    setupUI();
}

scrollablewidgetex::~scrollablewidgetex()
{
}

void scrollablewidgetex::setupUI()
{
}
