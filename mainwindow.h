#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "devicelist.h"
#include "itemdetailwidget.h"
#include "controller.h"
#include "context.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_applyBtn_clicked();
    void on_testBtn_clicked();
    void on_getIDBtn_clicked();


    void SLT_receiveState(bool state, uint8_t funcCode, uint8_t value);
private:
    Ui::MainWindow *ui;

    void setupUI();
    void setupModbus();
    void searchItem(int id);
    void sendChangeOp();
    void createDetailView(DeviceItem item);

    DeviceItem selectedItem;

    QVBoxLayout *layout;
    ItemDetailWidget *detailWidget;

signals:
    bool requestRead(int slaveID, uint16_t address, uint16_t quantity);
};

#endif // MAINWINDOW_H
