#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>


class SerialWorker : public QObject
{
    Q_OBJECT
public:
    SerialWorker(QObject *parent =  nullptr); // contructor
    ~SerialWorker();                          // distructor

    void initPort(quint16 _no);

signals:
    void notConnected(bool state);

private:
    QSerialPort *qserial;
    QTimer *checkTimer;

    bool m_lock;

    void _initTimer();
    void _connectSonicPort(QSerialPort* _sPort, const QString&_portName);
    void _disconnectPort();
    bool _initPort(QSerialPort* _sPort, const QString&_portName);
    int _send(QSerialPort* _sPort, const char *data, const uint8_t maxSize);
    QString _getConfigedPort(quint16 _no);

private slots:
    void SLT_on_connection_error();
    void SLT_received();

public:
    void send(const char *data, const uint8_t maxSize);
    int8_t syncSendEx(const char *data, const uint8_t maxSize, int16_t timeout, uint8_t retry);
    int8_t syncSend(const char *data, const uint8_t maxSize, const uint8_t responseLength, int16_t timeout);

    bool isLocked()     { return m_lock; }

protected:
    virtual void received(QByteArray&) = 0;

};

#endif // SERIALWORKER_H
