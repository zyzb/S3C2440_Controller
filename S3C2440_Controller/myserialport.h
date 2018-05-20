#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H

#include <QSerialPort>
#include "MLTP.h"

class MySerialPort : public QSerialPort
{
    Q_OBJECT
public:
    static MySerialPort *get_serialPort_link();     //获得串口的连接

    void send_data(DataType type, QString data);    //发送数据

signals:
    void init_data(QString data);
    void read_data(DataType type, QString data);

private slots:
    void receive_data(void);                        //当接收到readyRead()信号时执行，接收数据

private:
    //单例设计模式，一次只能连接一个mySerialPort
    explicit MySerialPort(QObject *parent = 0);
    MySerialPort(const MySerialPort &other);
    MySerialPort &operator =(const MySerialPort &other);

};

#endif // MYSERIALPORT_H
