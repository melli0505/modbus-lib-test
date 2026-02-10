#ifndef CONTROLLER_H
#define CONTROLLER_H

/* for nanoModbus Configuration Section */
#define NMBS_SERVER_DISABLED

/****************************************/
#include "nmbs/nanomodbus.h"
#include <QObject>
#include <QSerialPort>
#include <QElapsedTimer>

#define controller Controller::instance()

class Controller : public QObject
{
    Q_OBJECT
public:
    static Controller& instance()
    {
        static Controller _controller;
        return _controller;
    }


    bool setup_port(QSerialPort* _sPort);

    bool requestData(int slaveID, uint16_t address, uint16_t quantity);
    bool requestInputData(int slaveID, uint16_t address, uint16_t quantity);
    bool requestSingleData(int slaveID, uint16_t address, uint16_t data);
    bool requestMultipleData(int slaveID, uint16_t address, uint16_t quantity, uint16_t* data);
    bool requestCustom0x10(int slaveID, uint8_t data);
    bool requestCustom0x20(int slaveID);

private:
    Controller();
    ~Controller();
    nmbs_t nmbs;
    nmbs_platform_conf platform_conf;

    QSerialPort *qserial;
    QByteArray m_incomingBuffer;

    QElapsedTimer* checkTimer;

    static int32_t nmbs_write(const uint8_t* buf, uint16_t count, int32_t timeout_ms, void* arg);
    static int32_t nmbs_read(uint8_t* buf, uint16_t count, int32_t timeout_ms, void* arg);

    void _initTimer();
};

#endif // CONTROLLER_H
