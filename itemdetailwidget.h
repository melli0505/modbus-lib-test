#ifndef ITEMDETAILWIDGET_H
#define ITEMDETAILWIDGET_H

#include <QWidget>

#include "devicelist.h"

namespace Ui {
class ItemDetailWidget;
}

class ItemDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemDetailWidget(QWidget *parent = nullptr);
    ~ItemDetailWidget();

    void setDetailInfo(DeviceItem item);

private:
    Ui::ItemDetailWidget *ui;
};

#endif // ITEMDETAILWIDGET_H
