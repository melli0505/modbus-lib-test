#ifndef DEVICELIST_H
#define DEVICELIST_H

#include <QString>
#include <QList>

#define _device Device::instance()

struct DeviceItem {
    int id;
    QString name;
    QString category;
};

class Device
{
public:
    static Device& instance(){
        static Device _device_inst;
        return _device_inst;
    }
    QList<DeviceItem> deviceList = {
        {1, "RK100-02", "풍속 센서"},
        {2, "RK110-02", "풍향 센서"},
        {3, "RK330-01", "온습도/기압 센서"},
        {4, "RK400-02", "감우 센서"},
        {5, "RK520-02", "토양 온습도 센서"},
    };
};

#endif // DEVICELIST_H
