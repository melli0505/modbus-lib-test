#ifndef DEVICEITEMWIDGET_H
#define DEVICEITEMWIDGET_H

#include <QWidget>

namespace Ui {
class deviceitemwidget;
}

class deviceitemwidget : public QWidget
{
    Q_OBJECT

public:
    explicit deviceitemwidget(QWidget *parent = nullptr);
    ~deviceitemwidget();

private:
    Ui::deviceitemwidget *ui;
};

#endif // DEVICEITEMWIDGET_H
