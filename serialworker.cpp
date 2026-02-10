#include "serialworker.h"

#include <QDebug>
#include <QThread>
#include <QTimer>

//#define  __DEBUG__
SerialWorker::SerialWorker(QObject *parent)
    : QObject(parent)
{
    _initTimer();
}

SerialWorker::~SerialWorker()
{
    this->_disconnectPort();
    delete qserial;
    qserial = nullptr;
    checkTimer->stop();
}

QString SerialWorker::_getConfigedPort(quint16 _no)
{
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
#ifdef __DEBUG__
        qDebug() << "manufacturer : " << serialPortInfo.manufacturer();
        qDebug() << "vendorIdentifier : " << serialPortInfo.vendorIdentifier();
        qDebug() << "productIdentifier : " << serialPortInfo.productIdentifier();

        if (serialPortInfo.manufacturer() == "Microsoft" &&
            serialPortInfo.vendorIdentifier() == 1155 &&
            serialPortInfo.productIdentifier() == _no)
        {
                return serialPortInfo.portName();
        }
#endif
        // microsoft 아닌 COM port 잡기
        if (serialPortInfo.manufacturer() != "Microsoft") {
            return serialPortInfo.portName();
        }
    }
    return "";
}

void SerialWorker::initPort(quint16 _no)
{
#ifdef Q_OS_WIN
    QString portName = _getConfigedPort(_no);
#endif
    this->qserial = new QSerialPort(this);

    if (_initPort(this->qserial, portName))
    {

        connect(qserial, SIGNAL(readyRead()), this, SLOT(SLT_received()));
        qDebug() << "serial port success";
    }
    else
        qDebug() << "serial port error";
}

bool SerialWorker::_initPort(QSerialPort* _sPort, const QString& _portName)
{
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
        qCritical() << _portName + " serial port error";
        return false;
    }

    qInfo() << _portName + " OK";

    return true;
}


#define TIMEOUT         1000        // 1000 ms

void SerialWorker::_initTimer()
{
    checkTimer = new QTimer();
    checkTimer->setTimerType(Qt::PreciseTimer);

    connect(checkTimer, SIGNAL(timeout()), this, SLOT(SLT_on_connection_error()));

    checkTimer->start(TIMEOUT);

    _getConfigedPort(1);
}


void SerialWorker::_disconnectPort()
{
    qserial->close();
    qserial  = nullptr;
}

void SerialWorker::SLT_received()
{
    static QByteArray  receivedData;

    if(qserial->bytesAvailable() > 0)
    {
        checkTimer->stop();
        receivedData = qserial->readAll(); // qserial data read all

        qDebug() << "r: " << receivedData.toHex() << receivedData.size();

        /* call virtual function */
        received(receivedData);
    } else {
        qDebug() << "r: " << "nothing received";
    }

    checkTimer->start(TIMEOUT);
}


void SerialWorker::SLT_on_connection_error()
{
    emit notConnected(false);
}


void SerialWorker::send(const char *data, const uint8_t maxSize)
{
    _send(this->qserial, data, maxSize);
}

int SerialWorker::_send(QSerialPort* _sPort, const char *data, const uint8_t maxSize)
{
    if (!_sPort->isOpen())
        return -1;

    qint64 ret =  _sPort->write(data, maxSize);

    _sPort->flush();

    qDebug() << "return bytes: " << ret;

    if(ret != maxSize)
    {
        qCritical("Do not Sending...");
        return -1;
    }

    QThread::msleep(1);

    return ret;
}

/* for sync */
#include <QElapsedTimer>
int qt_subtract_from_timeout(int timeout, int elapsed)
{
    if (timeout == -1)
        return -1;
    timeout = timeout - elapsed;
    return timeout < 0 ? 0 : timeout;
}

int8_t SerialWorker::syncSendEx(const char *data, const uint8_t maxSize, int16_t timeout, uint8_t retry)
{
    while (retry--)
    {
        if (syncSend(data, maxSize, maxSize, timeout) == 0)
            return 0;

        QThread::msleep(2);

        qDebug() << "retry syncSend";
    }

    return -1;
}


int8_t SerialWorker::syncSend(const char *data, const uint8_t maxSize, const uint8_t responseLength, int16_t timeout)
{
    static QByteArray _buffer = "";
    QElapsedTimer stopWatch;
    uint8_t ret = -1;

    _buffer.clear();

    m_lock = true;

    if(_send(this->qserial, data, maxSize) == -1)


    stopWatch.start();

    do {
        while (qserial->bytesAvailable() > 0)
        {
            QByteArray _tmp = qserial->readAll();

            _buffer.append(_tmp);

            if (_buffer.size() >= responseLength)
            {
                qDebug()<< "received sync: " << _buffer.toHex();

                if (_buffer.contains(data))
                    ret = 0;

                goto out;
            }
        }

    } while (timeout == -1 || qt_subtract_from_timeout(timeout, stopWatch.elapsed()) > 0);

#ifdef __DEBUG__
    if (responseLength != _buffer.size())
    {
            qDebug() << "syncWrite time: " << stopWatch.elapsed() << "ms";
            qDebug() << "_buffer data: " << _buffer.toHex() << "length: " << _buffer.length() << " != corret length: ";
    }
#endif

    ret = -1;

out:
    m_lock = false;

    return ret;
}
