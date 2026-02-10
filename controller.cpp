#include "controller.h"

#include <QDebug>
#include <QSerialPortInfo>
#include <QCoreApplication>
#include <QTimer>

#define TIMEOUT         1000        // 1000 ms

Controller::Controller()
{

    this->qserial = new QSerialPort();
//    _initTimer();
    checkTimer = new QElapsedTimer();

    setup_port(qserial);

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
        if (serialPortInfo.manufacturer() != "Microsoft") {
//        if (serialPortInfo.portName() == "COM2"){
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

// RTU 읽기 콜백
int32_t Controller::nmbs_read(uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {

    auto self = static_cast<Controller*>(arg);

    if(self->qserial->bytesAvailable() < count)
        self->qserial->waitForReadyRead(1000);

#if 0
    qDebug() << "request count : " << count;
    qDebug() << "data ok : " << len;

    for (int i = 0; i < len; i++)
        qDebug("%x", buf[i]);
#endif

    return self->qserial->read(reinterpret_cast<char*>(buf), count);
}

// RTU 쓰기 콜백
int32_t Controller::nmbs_write(const uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {

    auto self = static_cast<Controller*>(arg);
    self->m_incomingBuffer.clear();
    int64_t written = self->qserial->write(reinterpret_cast<const char*>(buf), count);
    if (!self->qserial->waitForBytesWritten(byte_timeout_ms)){
        return -1;
    }

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

// 0x10, 직접 구현 필요
bool Controller::sendCustom0x10(int slaveID, uint8_t data) {
    qDebug() << "[sendCustom0x10] started.";
    nmbs_error send_err, receive_err;
    uint8_t fc = 0x10;

    nmbs_set_destination_rtu_address(&nmbs, slaveID);
    qDebug() << "[sendCustom0x10] set destination address.";
    send_err = nmbs_send_raw_pdu(&nmbs, fc, &data, 1);
    qDebug() << "[sendCustom0x10] send raw pdu.";
    if (send_err == NMBS_ERROR_NONE) {
        qDebug() << QString("0x10 성공");
    }
    else {
        qDebug() << "0x10 실패: " << send_err ;
    }

    uint8_t receive_buffer[4];
    receive_err = nmbs_receive_raw_pdu_response(&nmbs, receive_buffer, 0);

    if (receive_err == NMBS_ERROR_NONE){
        qDebug() << QString("0x10 응답: ") << receive_buffer[0];
        return true;
    }
    else{
        qDebug() << "0x10 응답 없음: " << receive_err ;
        return false;
    }
}


// 0x20, 직접 구현 필요
bool Controller::sendCustom0x20(int slaveID) {
    nmbs_error send_err, receive_err;
    uint8_t fc = 0x20;

    nmbs_set_destination_rtu_address(&nmbs, slaveID);
    send_err = nmbs_send_raw_pdu(&nmbs, fc, NULL, 0);

    if (send_err == NMBS_ERROR_NONE)
        qDebug() << QString("0x20 성공");
    else{
        qDebug() << "0x20 실패: " << send_err ;
        return false;
    }

    uint8_t receive_buffer[1];
    receive_err = nmbs_receive_raw_pdu_response(&nmbs, receive_buffer, 1);

    if (receive_err == NMBS_ERROR_NONE){
        qDebug() << QString("0x20 응답: ") << receive_buffer[0];
        return true;
    }
    else{
        qDebug() << "0x20 응답 없음: " << receive_err ;
        return false;
    }
}
