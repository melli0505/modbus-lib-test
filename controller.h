#ifndef CONTROLLER_H
#define CONTROLLER_H

/* for nanoModbus Configuration Section */
#define NMBS_SERVER_DISABLED

/****************************************/
#include "nmbs/nanomodbus.h"
#include <QObject>
#include <QSerialPort>
#include <QElapsedTimer>

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller();
    ~Controller();

    bool setup_port(QSerialPort* _sPort);

    bool readData(int slaveID, uint16_t address, uint16_t quantity);
    bool readInputData(int slaveID, uint16_t address, uint16_t quantity);
    bool writeSingleData(int slaveID, uint16_t address, uint16_t data);
    bool writeMultipleData(int slaveID, uint16_t address, uint16_t quantity, uint16_t* data);
    bool sendCustom0x10(int slaveID, uint8_t data);
    bool sendCustom0x20(int slaveID);

private slots:
    void SLT_received();

private:
    nmbs_t nmbs;
    nmbs_platform_conf platform_conf;

    QSerialPort *qserial;
    QByteArray m_incomingBuffer;

    QElapsedTimer* checkTimer;

    static int32_t nmbs_write(const uint8_t* buf, uint16_t count, int32_t timeout_ms, void* arg);
    static int32_t nmbs_read(uint8_t* buf, uint16_t count, int32_t timeout_ms, void* arg);

    void _initTimer();
signals:
    void dataReceivced(QVector<uint16_t> read_regs);
    void errOccurred(const char* errstr);
};

#endif // CONTROLLER_H
