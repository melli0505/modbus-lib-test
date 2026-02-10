#include "controller.h"

#include <QDebug>
#include <QSerialPortInfo>
#include <QCoreApplication>
#include <QEventLoop>
#include <QThread>
#include <QTimer>

#define TIMEOUT         1000        // 1000 ms

Controller::Controller()
{
    this->qserial = new QSerialPort();
//    _initTimer();
    checkTimer = new QElapsedTimer();
    if(setup_port(qserial))
        connect(this->qserial, SIGNAL(readyRead()), this, SLOT(SLT_received()));

    nmbs_platform_conf_create(&platform_conf);

    platform_conf.transport = NMBS_TRANSPORT_RTU;
    platform_conf.read = nmbs_read;
    platform_conf.write = nmbs_write;
    platform_conf.arg = this;

    nmbs_client_create(&nmbs, &platform_conf);

    nmbs_set_read_timeout(&nmbs, 1000);
    nmbs_set_byte_timeout(&nmbs, 100);
}

Controller::~Controller()
{
}



bool Controller::setup_port(QSerialPort* _sPort)
{
    QString _portName;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        // microsoft 아닌 COM port 잡기
//        if (serialPortInfo.manufacturer() != "Microsoft") {
        if (serialPortInfo.portName() == "COM2"){
            _portName = serialPortInfo.portName();
        }
        qInfo() << "port info : " << serialPortInfo.manufacturer() << serialPortInfo.portName();

    }

    if (_sPort->isOpen()) {
        _sPort->clear();
        _sPort->close();
    }

    _sPort->setPortName(_portName);
    _sPort->setBaudRate(QSerialPort::Baud9600);
    _sPort->setDataBits(QSerialPort::Data8);
    _sPort->setParity(QSerialPort::NoParity);
    _sPort->setStopBits(QSerialPort::OneStop);
    _sPort->setFlowControl(QSerialPort::NoFlowControl);

    if(!_sPort->open(QIODevice::ReadWrite))
    {
        qCritical() << _portName + " serial port error: " << _sPort->errorString();
        return false;
    }

    qInfo() << _portName + " OK";

    return true;
}

void Controller::SLT_received()
{
    if(qserial->bytesAvailable() > 0) {
        m_incomingBuffer.append(qserial->readAll());
        qDebug() << "Data buffered:" << m_incomingBuffer.toHex() << m_incomingBuffer.size();
        qDebug() << "[Slot] Instance Address:" << this << "Size:" << m_incomingBuffer.size();
    }
    checkTimer->start();
}

// RTU 읽기 콜백
int32_t Controller::nmbs_read(uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {

    auto self = static_cast<Controller*>(arg);
/*

    if(self->qserial->bytesAvailable() < count)
        self->qserial->waitForReadyRead(100);

    QCoreApplication::processEvents();


    return self->qserial->read(reinterpret_cast<char*>(buf), count);
*/

    while (self->m_incomingBuffer.size() < count) {
        int64_t remaining = 1000 - self->checkTimer->elapsed();
        if (remaining <= 0) return 0;

        QEventLoop loop;
        QTimer::singleShot(10, &loop, &QEventLoop::quit);
        loop.exec();

        if (self->qserial->bytesAvailable() > 0){
            self->SLT_received();
            qDebug() << "[nmbs_read][byte available] " << self->qserial->bytesAvailable();
        }

    }
    qDebug() << "[Read] Instance Address:" << self << "Size:" << self->m_incomingBuffer.size();

    memcpy(buf, self->m_incomingBuffer.constData(), count);
    self->m_incomingBuffer.remove(0, count);

    qDebug() << "[nmbs_read] count" << count;
    return static_cast<int32_t>(count);  
}

// RTU 쓰기 콜백
int32_t Controller::nmbs_write(const uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {
    qDebug() << "Sending Data (Hex):" << QByteArray::fromRawData((const char*)buf, count).toHex(' ') << count;

    auto self = static_cast<Controller*>(arg);
    self->m_incomingBuffer.clear();
    int64_t written = self->qserial->write(reinterpret_cast<const char*>(buf), count);
    if (!self->qserial->waitForBytesWritten(byte_timeout_ms)){
        qDebug() << "[ERROR nmbs_write] " << self->qserial->errorString();
        return -1;
    }

    qInfo() << "[nmbs_write] ends." << buf;

    return static_cast<int32_t>(written);

}
// 0x03
bool Controller::readData(int slaveID, uint16_t address, uint16_t quantity) {
    if (quantity == 0) return false;
    nmbs_error err;
    QVector<uint16_t> read_regs(quantity);
    checkTimer->start();

    nmbs_set_destination_rtu_address(&nmbs, slaveID);
    err = nmbs_read_holding_registers(&nmbs, address, quantity, read_regs.data());
    if (err == NMBS_ERROR_NONE) {
        for(int i = 0; i < quantity; ++i) {
            qDebug() << QString("[%1] : %2").arg(address + i).arg(read_regs[i]);
        }
        return true;
    }
    else {
        return false;
    }
}

// 0x04
bool Controller::readInputData(int slaveID, uint16_t address, uint16_t quantity) {
    if (quantity == 0) return false;
    nmbs_error err;
    QVector<uint16_t> read_regs(quantity);

    if (qserial->isOpen()) {
        qserial->clear(QSerialPort::AllDirections);
    }

    nmbs_set_destination_rtu_address(&nmbs, slaveID); // slave id = 1
    nmbs_set_read_timeout(&nmbs, 1000);

    err = nmbs_read_input_registers(&nmbs, address, quantity, read_regs.data());
    if (err == NMBS_ERROR_NONE) {
        for(int i = 0; i < quantity; ++i) {
            qDebug() << QString("[%1] : %2").arg(address + i).arg(read_regs[i]);
        }
        return true;
    }
    else {
        return false;
    }
}

// 0x06
bool Controller::writeSingleData(int slaveID, uint16_t address, uint16_t data) {
    nmbs_error err;

    if (qserial->isOpen()) {
        qserial->clear(QSerialPort::AllDirections);
    }

    nmbs_set_destination_rtu_address(&nmbs, slaveID);
    nmbs_set_read_timeout(&nmbs, 1000);

    qDebug() << "write data set end";
    err = nmbs_write_single_register(&nmbs, address, data);

    if (err == NMBS_ERROR_NONE) {
        return true;
    }
    else {
        return false;
    }
}

// 0x10
bool Controller::writeMultipleData(int slaveID, uint16_t address, uint16_t quantity, uint16_t* data) {
    if (quantity == 0) return false;
    nmbs_error err;

    if (qserial->isOpen()) {
        qserial->clear(QSerialPort::AllDirections);
    }

    nmbs_set_destination_rtu_address(&nmbs, slaveID);
    nmbs_set_read_timeout(&nmbs, 1000);

    err = nmbs_write_multiple_registers(&nmbs, address, quantity, data);

    if (err == NMBS_ERROR_NONE) {
        return true;
    }
    else {
        return false;
    }
}

// 0x20, 직접 구현 필요
bool Controller::sendCustom0x10(int slaveID, uint8_t data) {
    qDebug() << "[sendCustom0x10] started.";
    nmbs_error err;
    uint8_t fc = 0x10;

    nmbs_set_destination_rtu_address(&nmbs, slaveID);
    qDebug() << "[sendCustom0x10] set destination address.";
    err = nmbs_send_raw_pdu(&nmbs, fc, &data, 1);
    qDebug() << "[sendCustom0x10] send raw pdu.";
    if (err == NMBS_ERROR_NONE) {
        qDebug() << QString("0x10 성공");
        return true;
    }
    else {
        qDebug() << "0x10 실패: " << err ;
        return false;
    }
}


// 0x20, 직접 구현 필요
bool Controller::sendCustom0x20(int slaveID) {
    nmbs_error err;
    uint8_t fc = 0x20;

    nmbs_set_destination_rtu_address(&nmbs, slaveID);
    err = nmbs_send_raw_pdu(&nmbs, fc, NULL, 0);

    if (err == NMBS_ERROR_NONE) {
        qDebug() << QString("0x20 성공");
        return true;
    }
    else {
        qDebug() << "0x20 실패: " << err ;
        return false;
    }
}
