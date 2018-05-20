#ifndef PORTCLIENT_H
#define PORTCLIENT_H

#include <QObject>
#include <QVector>
#include <QMap>
#include "myserialport.h"

class PortClient : public QObject
{
    Q_OBJECT
public:
    static PortClient *get_portClient_link();           //获取portClient的连接

    QVector<QString> init_port();                       //初始化端口信息

    bool open_port(QMap<QString, QString> portMessage); //打开并配置串口

    void set_port_name(QString portName);               //设置串口名
    void set_baud_rate(int index);                      //设置波特率
    void set_data_bits(int index);                      //设置数据位
    void set_parity(int index);                         //设置校验位
    void set_flow_control(int index);                   //设置流控制
    void set_stop_bits(int index);                      //设置停止位

    void close_port();                                  //关闭串口

    bool is_open();                                     //判断端口是否被打开

    bool deal_send_data(QString text);                  //处理将要发送给串口的数据

    bool open_led();                                    //向ARM端发送打开LED的消息
    bool close_led();                                   //向ARM端发送关闭LED的消息

    bool open_buzz();                                   //向ARM端发送打开蜂鸣器的消息
    bool close_buzz();                                  //向ARM端发送关闭蜂鸣器的消息

    bool open_warning();                                //向ARM端发送打开警报的消息
    bool close_warning();                               //向ARM端发送关闭警报的消息

    bool set_date();                                    //向ARM端发送设置日期的消息
    bool set_time();                                    //向ARM端发送设置时间的消息

    bool read_date();                                   //向ARM端发送读取日期的消息
    bool read_time();                                   //向ARM端发送读取时间的消息

    bool save_volt(QString volt);                       //将读取的电压存取到对应文件中

signals:
    void clear_screen();                                //清屏
    void read_volt();                                   //读取电压
    void echo_data(QString text);                       //回显
    void unknown(QString text);                         //未知命令

    void display_warm_boot_start();                     //显示热启动开始的信息
    void display_warm_boot_finish();                    //显示热启动完成的信息
    void display_current_volt(QString data);            //显示当前电压
    void display_volt_overflow(QString data);           //设置当前状态为报警状态

    void open_led_success();                            //打开LED成功
    void led_already_open();                            //LED已处于打开状态

    void close_led_success();                           //关闭LED成功
    void led_already_close();                           //LED已处于关闭状态

    void open_buzz_success();                           //打开蜂鸣器成功
    void buzz_already_open();                           //蜂鸣器已处于打开状态

    void close_buzz_success();                          //关闭蜂鸣器成功
    void buzz_already_close();                          //蜂鸣器已处于关闭状态

    void open_warning_success();                        //打开警报成功
    void warning_already_open();                        //警报已处于打开状态

    void close_warning_success();                       //关闭警报成功
    void warning_already_close();                       //警报已处于关闭状态

    void set_date_success();                            //设置日期成功
    void set_time_success();                            //设置时间成功
    void display_current_date(QString data);            //显示当前日期
    void display_current_time(QString data);            //显示当前时间

    void open_file_failed();                            //打开文件失败

public slots:
    void deal_init_data(QString data);
    void deal_read_data(DataType type, QString data);

private:
    //单例设计模式，一个程序只能存在一个portClient
    explicit PortClient(QObject *parent = 0);
    PortClient(const PortClient &other);
    PortClient &operator=(const PortClient &other);

    MySerialPort *serial_port;

};

#endif // PORTCLIENT_H
