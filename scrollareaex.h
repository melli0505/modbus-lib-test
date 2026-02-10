#ifndef SCROLLAREAEX_H
#define SCROLLAREAEX_H

#include <QScrollArea>
#include <QButtonGroup>
#include <QVBoxLayout>

#include "devicelist.h"


class ScrollAreaEx : public QScrollArea
{
    Q_OBJECT

public:
    explicit ScrollAreaEx(QWidget *parent = nullptr);
    ~ScrollAreaEx();

    void setupUI();
    void addItem(QString name, int id);
    int selectedId() const;

//    void searchItem();
signals:
    void selectedItem(int id);

private:
    QWidget *container;
    QVBoxLayout *layout;
    QButtonGroup *buttonGroup;

    int m_selectedId;
};

#endif // SCROLLAREAEX_H
