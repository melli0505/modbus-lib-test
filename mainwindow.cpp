#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupUI();

    selectedItem = {-1, "no", "no"};

    connect(ui->scrollArea, &ScrollAreaEx::selectedItem, this, &MainWindow::searchItem);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // device list 추가
    for (DeviceItem item : _device.deviceList) {
        ui->scrollArea->addItem(item.name, item.id);
    }
    // detail page 생성
    detailWidget = new ItemDetailWidget(ui->stackedWidget);
    ui->stackedWidget->addWidget(detailWidget);
}


void MainWindow::searchItem(int id)
{
    // 선택된 device 정보 확인
    for (DeviceItem item : _device.deviceList) {
        if (item.id == id) {
            selectedItem = item;
        }
    }
    // detail page 업데이트
    createDetailView(selectedItem);
}

void MainWindow::createDetailView(DeviceItem item)
{
    detailWidget->setDetailInfo(item);
    ui->stackedWidget->setCurrentIndex(1);
}

// serial data 전송
void MainWindow::on_applyBtn_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "확인", "정말로 ID를 바꾸시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
        sendChangeOp();
    else
        return;

}

void MainWindow::on_testBtn_clicked()
{
    int default_id = ui->defaultID->text().toInt();

    qDebug() << "Selected Item : " << selectedItem.name << selectedItem.id;

    bool ret;
    switch (selectedItem.id) {
    case 1:
        ret = controller.requestData(default_id, Protocol::REQUEST::ADDRESS, 1);
        break;
    case 2:
        ret = controller.requestData(default_id, Protocol::REQUEST::ADDRESS, 1);
        break;
    case 3:
        ret = controller.requestData(default_id, Protocol::REQUEST::ADDRESS, 2);
        break;
    case 4:
        ret = controller.requestData(default_id, Protocol::REQUEST::ADDRESS, 1);
        break;
    case 5:
        ret = controller.requestData(default_id, Protocol::REQUEST::ADDRESS, 3);
        break;
    }

    if (!ret){
        qErrnoWarning("Failed to get test data");
    }
}

void MainWindow::on_getIDBtn_clicked()
{
    if (selectedItem.id == -1)
        QMessageBox::information(this, "확인", "디바이스를 선택해주세요.");
    else if (selectedItem.id > 2) {
        QMessageBox::information(this, "확인", "이 디바이스는 Slave ID 조회를 지원하지 않습니다. Broadcast ID를 사용해서 변경해주세요.");
        ui->defaultID->setText("254");
    }
    else {
        if (!controller.requestCustom0x20(0)) {
            qErrnoWarning("Failed to get slave id");
        }
    }
}


void MainWindow::sendChangeOp()
{
    int default_id = ui->defaultID->text().toInt();
    int new_id = ui->newID->text().toUInt();

    qDebug() << "Selected Item : " << selectedItem.name << selectedItem.id;
    bool ret;
    switch(default_id){
    case 1:
        ret = controller.requestCustom0x10(default_id, new_id);
        break;
    case 2:
        ret = controller.requestCustom0x10(default_id, new_id);
        break;
    case 3:
        ret = controller.requestSingleData(default_id, Protocol::RK330_01::ADDRESS, new_id);
        break;
    case 4:
        ret = controller.requestSingleData(default_id, Protocol::RK400_02::ADDRESS, new_id);
        break;
    case 5:
        ret = controller.requestSingleData(default_id, Protocol::RK520_02::ADDRESS, new_id);
        break;
    }

    if (!ret) {
        qErrnoWarning("Failed to change slave ID");
    }
}

// 성공시 알림
void MainWindow::SLT_receiveState(bool state, uint8_t funcCode, uint8_t value)
{
    if (state)
        QMessageBox::information(this, "Done", "명령을 전송했습니다.");
    if (funcCode == 0x20)
                ui->defaultID->setText(QString::number(value));
    ui->receivedLabel->setNum(value);
}
