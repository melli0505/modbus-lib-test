#ifndef SCROLLABLEWIDGETEX_H
#define SCROLLABLEWIDGETEX_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>

namespace Ui {
class scrollablewidgetex;
}

struct DeviceItem {
    QString name;
    QString category;
};

class scrollablewidgetex : public QScrollArea
{
    Q_OBJECT

public:
    explicit scrollablewidgetex(QWidget *parent = nullptr);
    ~scrollablewidgetex();

private:
    QList<DeviceItem> m_datalist = {
        {"RK100-02", "풍속 센서"},
        {"RK110-02", "풍향 센서"},
        {"RK330-01B", "온습도/기압 센서"},
        {"RK400-02", "감우 센서"},
        {"RK520-02", "토지 온습도 센서"},
    };

    QWidget *contentWidget;
    QVBoxLayout *layout;

    void setupUI();
};

#endif // SCROLLABLEWIDGETEX_H
