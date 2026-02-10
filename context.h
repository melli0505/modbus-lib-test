#ifndef CONTEXT_H
#define CONTEXT_H

#endif // CONTEXT_H

#include <QTimer>
#include <QTime>

/* length: 12 byte */
#pragma pack(push, 1)  // 1바이트 단위로 정렬 시작

struct _packet_1xx {
    uint8_t     ID;
    uint8_t     cmd;
    union
    {
        uint8_t     payload;
        uint8_t     newAddress;
    };
    uint16_t    crc;
};

struct _packet_sensor {
    uint8_t     ID;
    uint8_t     cmd;
    union
    {
        uint8_t payload[4];
        struct
        {
            uint16_t address;
            uint16_t newAddress;
        };
    };
    uint16_t    crc;
};

struct _packet_address {
    uint8_t     ID;
    uint8_t     cmd;
    uint16_t    crc;
};

#pragma pack(pop)  // 이전 정렬 상태로 복원


uint16_t toUint16t(uint8_t _msb,
                   uint8_t _lsb);



class Protocol : public QObject
{
    Q_OBJECT
public:
    Protocol();

    enum { READ  = 0x00,
           WRITE = 0x01 };

    static const uint8_t HEADER                 = 0xF0;
    static const uint8_t TAIL                   = 0x0D;

    /*MOTOR*/
    /* cmd ********************************************/
    static const uint8_t MOTOR_SET              = 0x01;
    static const uint8_t MOTOR_RUN              = 0x02;
    static const uint8_t MOTOR_ORIGIN_SET_RUN   = 0x03;
    static const uint8_t MOTOR_FAULT_DATA_REQ   = 0x05;
    static const uint8_t MOTOR_STOP             = 0x08;
    /***************************************************/
    static const uint8_t LENGTH_1xx               = sizeof(struct _packet_1xx);
    static const uint8_t LENGTH_sensor            = sizeof(struct _packet_sensor);
    static const uint8_t LENGTH_address           = sizeof(struct _packet_address);

    class REQUEST {
    public:
        static const uint8_t CMD        = 0x03;
        static const uint16_t ADDRESS   = 0x0000;
    };

    class READ_ADRESS {
    public:
        static const uint8_t ID         = 0x00;
        static const uint8_t CMD        = 0x20;
    };

    class RK100_02
    {
    public:
        static const uint8_t CMD        = 0x10;
        static const uint16_t READ_BYTE = 0x0001;
    };

    class RK110_02
    {
    public:
        static const uint8_t CMD        = 0x10;
        static const uint16_t READ_BYTE = 0x0001;
    };


    class RK330_01
    {
    public:
        static const uint8_t CMD        = 0x06;
        static const uint16_t ADDRESS   = 0x0000;
        static const uint16_t READ_BYTE = 0x0002;
    };

    class RK400_02
    {
    public:
        static const uint8_t CMD        = 0x06;
        static const uint16_t ADDRESS   = 0x0030;
        static const uint16_t READ_BYTE = 0x0001;
    };

    class RK520_02
    {
    public:
        static const uint8_t CMD        = 0x06;
        static const uint16_t ADDRESS   = 0x0200;
        static const uint16_t READ_BYTE = 0x0003;
    };

};
