#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QSerialPortInfo>
#include "myserialport.h"
#include "portclient.h"

//创建全局变量
const static QString FILE_PATH = "D:\\cx\\record\\";
const static QString CURRENT_TIME = QDateTime::currentDateTime().toString("_yyyy-MM-dd_hhmmss");

//获取portClient的连接
PortClient *PortClient::get_portClient_link()
{
    static PortClient portClient = PortClient();
    return &portClient;
}


//构造函数
PortClient::PortClient(QObject *parent) :
    QObject(parent)
{
    //初始化client_socket
    serial_port = MySerialPort::get_serialPort_link();


    connect(serial_port, SIGNAL(init_data(QString)),
            this, SLOT(deal_init_data(QString)));

    //让Client类处理MySeroalPort传来的数据
    connect(serial_port, SIGNAL(read_data(DataType, QString)),
            this, SLOT(deal_read_data(DataType, QString)));
}


//初始化端口信息
QVector<QString> PortClient::init_port()
{
    //创建Vector储存端口信息
    QVector<QString> portName;

    //读取串口信息
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug()<<"Name:"<<info.portName();
        qDebug()<<"Description:"<<info.description();
        qDebug()<<"Manufacturer:"<<info.manufacturer();

        portName.push_back(info.portName());
    }
    return portName;
}


//打开并配置串口
bool PortClient::open_port(QMap<QString, QString> portMessage)
{
    //设置串口名
    this->set_port_name(portMessage["portName"]);

    //以读写方式打开串口
    if(serial_port->open(QIODevice::ReadWrite)){
        //设置波特率
        this->set_baud_rate(portMessage["baudRate"].toInt());

        //设置数据位
        this->set_data_bits(portMessage["dataBits"].toInt());

        //设置校验位
        this->set_parity(portMessage["parity"].toInt());

        //设置流控制
        this->set_flow_control(portMessage["flowControl"].toInt());

        //设置停止位
        this->set_stop_bits(portMessage["stopBits"].toInt());

        return true;
    }
    return false;
}


//设置串口名
void PortClient::set_port_name(QString portName)
{
    serial_port->setPortName(portName);
}

//设置波特率
void PortClient::set_baud_rate(int index)
{
    switch(index){
    case 0:
        //设置波特率为1200
        serial_port->setBaudRate(QSerialPort::Baud1200);
        break;
    case 1:
        //设置波特率为2400
        serial_port->setBaudRate(QSerialPort::Baud2400);
        break;
    case 2:
        //设置波特率为4800
        serial_port->setBaudRate(QSerialPort::Baud4800);
        break;
    case 3:
        //设置波特率为9600
        serial_port->setBaudRate(QSerialPort::Baud9600);
        break;
    case 4:
        //设置波特率为19200
        serial_port->setBaudRate(QSerialPort::Baud19200);
        break;
    case 5:
        //设置波特率为38400
        serial_port->setBaudRate(QSerialPort::Baud38400);
        break;
    case 6:
        //设置波特率为57600
        serial_port->setBaudRate(QSerialPort::Baud57600);
        break;
    case 7:
        //设置波特率为115200
        serial_port->setBaudRate(QSerialPort::Baud115200);
        qDebug() << "QSerialPort::Baud115200";
        break;
    }

}

//设置数据位
void PortClient::set_data_bits(int index)
{
    switch(index){
    case 0:
        //设置数据位为5位
        serial_port->setDataBits(QSerialPort::Data5);
        break;
    case 1:
        //设置数据位为6位
        serial_port->setDataBits(QSerialPort::Data6);
        break;
    case 2:
        //设置数据位为7位
        serial_port->setDataBits(QSerialPort::Data7);
        break;
    case 3:
        //设置数据位为8位
        serial_port->setDataBits(QSerialPort::Data8);
        qDebug() << "QSerialPort::Data8";
        break;
    }
}

//设置校验位
void PortClient::set_parity(int index)
{
    switch(index){
    case 0:
        //设置校验位为无
        serial_port->setParity(QSerialPort::NoParity);
        qDebug() << "QSerialPort::NoParity";
        break;
    case 1:
        //设置校验位为奇数
        serial_port->setParity(QSerialPort::OddParity);
        break;
    case 2:
        //设置校验位为1位偶数
        serial_port->setParity(QSerialPort::EvenParity);
        break;
    }
}

//设置流控制
void PortClient::set_flow_control(int index)
{
    switch(index){
    case 0:
        //设置流控制为无
        serial_port->setFlowControl(QSerialPort::NoFlowControl);

        //流控制不为硬件时，需要设置 RTS、DTR 为高电平，保证对方能顺利的发送数据过来
        serial_port->setRequestToSend(true);
        serial_port->setDataTerminalReady(true);
        break;
    case 1:
        //设置流控制为硬件
        serial_port->setFlowControl(QSerialPort::HardwareControl);
        break;
    case 2:
        //设置流控制为软件
        serial_port->setFlowControl(QSerialPort::SoftwareControl);

        //流控制不为硬件时，需要设置 RTS、DTR 为高电平，保证对方能顺利的发送数据过来
        serial_port->setRequestToSend(true);
        serial_port->setDataTerminalReady(true);
        break;
    }
}

//设置停止位
void PortClient::set_stop_bits(int index)
{
    switch(index){
    case 0:
        //设置停止位为1位
        serial_port->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        //设置停止位为1.5位
        serial_port->setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case 2:
        //设置停止位为2位
        serial_port->setStopBits(QSerialPort::TwoStop);
        break;
    }
}

//关闭串口
void PortClient::close_port()
{
    serial_port->close();
}

//判断端口是否被打开
bool PortClient::is_open()
{
    bool ret = serial_port->isOpen();
    return ret;
}


//接收到从串口传来的初始化信息
void PortClient::deal_init_data(QString data)
{
    emit echo_data(data);
}

//接收并处理从串口传来的数据
void PortClient::deal_read_data(DataType type, QString data)
{
    int dataType = type;            //读取消息类型
    qDebug() << dataType;

    //判断消息类型
    switch(dataType){
    case SEND_PRINTF_TEXT:          //接收到ARM端发来的文本数据
        emit echo_data(data);
        break;

    case SEND_WARM_BOOT_START:      //接收到开发板开始热启动的信息
        emit display_warm_boot_start();
        break;

    case SEND_WARM_BOOT_FINISH:      //接收到开发板热启动完成的信息
        emit display_warm_boot_finish();
        break;

    case SEND_CURRENT_VOLT:{        //接收到ARM端发来的当前电压值
        emit display_current_volt(data);
        int ret = this->save_volt(data);
        if(ret == false){
            emit open_file_failed();
        }
        break;
    }
    case SEND_VOLT_OVERFLOW:        //接收到ARM端发来的电压报警
        emit display_volt_overflow(data);
        break;

    case OPEN_LED_SUCCESS:          //LED打开成功
        emit open_led_success();
        break;

    case LED_ALREADY_OPEN:          //LED已处于打开状态
        emit led_already_open();
        break;

    case CLOSE_LED_SUCCESS:         //LED关闭成功
        emit close_led_success();
        break;

    case LED_ALREADY_CLOSE:         //LED已处于关闭状态
        emit led_already_close();
        break;

    case OPEN_BUZZ_SUCCESS:         //蜂鸣器打开成功
        emit open_buzz_success();
        break;

    case BUZZ_ALREADY_OPEN:         //蜂鸣器已处于打开状态
        emit buzz_already_open();
        break;

    case CLOSE_BUZZ_SUCCESS:        //蜂鸣器关闭成功
        emit close_buzz_success();
        break;

    case BUZZ_ALREADY_CLOSE:        //蜂鸣器已处于关闭状态
        emit buzz_already_close();
        break;

    case OPEN_WARNING_SUCCESS:      //警报打开成功
        emit open_warning_success();
        break;

    case WARNING_ALREADY_OPEN:      //警报已处于打开状态
        emit warning_already_open();
        break;

    case CLOSE_WARNING_SUCCESS:     //警报关闭成功
        emit close_warning_success();
        break;

    case WARNING_ALREADY_CLOSE:     //警报已处于关闭状态
        emit warning_already_close();
        break;

    case SET_DATE_SUCCESS:          //设置日期成功
        emit set_date_success();
        break;

    case SET_TIME_SUCCESSE:         //设置时间成功
        emit set_time_success();
        break;

    case READ_DATE_SUCCESS:         //读取日期成功
        emit display_current_date(data);
        break;

    case READ_TIME_SUCCESS:         //读取时间成功
        emit display_current_time(data);
        break;
    }
}


//处理将要发送给串口的数据
bool PortClient::deal_send_data(QString text)
{
    bool ret;   //判断操作是否成功

    //如果读取的命令是清屏
    if(QString::compare(text, "CLEAR", Qt::CaseInsensitive) == 0){
        emit clear_screen();
        ret = true;
    }
    //如果读取的命令是读取当前电压
    else if(QString::compare(text, "READ_VOLT", Qt::CaseInsensitive) == 0){
        emit read_volt();
        ret = true;
    }
    //如果读取的命令是回显数据（获取QString前9个字符进行比较）
    else if(QString::compare(text.left(10), "ECHO_DATA ", Qt::CaseInsensitive) == 0){
        emit echo_data(text.mid(10));
        ret = true;
    }
    else if(QString::compare(text, "OPEN_LED", Qt::CaseInsensitive) == 0){
        ret = this->open_led();
    }
    else if(QString::compare(text, "CLOSE_LED", Qt::CaseInsensitive) == 0){
        ret = this->close_led();
    }
    else if(QString::compare(text, "OPEN_BUZZ", Qt::CaseInsensitive) == 0){
        ret = this->open_buzz();
    }
    else if(QString::compare(text, "CLOSE_BUZZ", Qt::CaseInsensitive) == 0){
        ret = this->close_buzz();
    }
    else if(QString::compare(text, "OPEN_WARNING", Qt::CaseInsensitive) == 0){
        ret = this->open_warning();
    }
    else if(QString::compare(text, "CLOSE_WARNING", Qt::CaseInsensitive) == 0){
        ret = this->close_warning();
    }
    else if(QString::compare(text, "SET_DATE", Qt::CaseInsensitive) == 0){
        ret = this->set_date();
    }
    else if(QString::compare(text, "SET_TIME", Qt::CaseInsensitive) == 0){
        ret = this->set_time();
    }
    else if(QString::compare(text, "READ_DATE", Qt::CaseInsensitive) == 0){
        ret = this->read_date();
    }
    else if(QString::compare(text, "READ_TIME", Qt::CaseInsensitive) == 0){
        ret = this->read_time();
    }
    else{
        emit unknown(text);    //未知命令
        ret = true;
    }

    return ret;
}

//向ARM端发送打开LED的消息
bool PortClient::open_led()
{
    QString data = NULL;
    serial_port->send_data(OPEN_LED, data);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送关闭LED的消息
bool PortClient::close_led()
{
    QString data = NULL;
    serial_port->send_data(CLOSE_LED, data);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送打开蜂鸣器的消息
bool PortClient::open_buzz()
{
    QString data = NULL;
    serial_port->send_data(OPEN_BUZZ, data);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送关闭蜂鸣器的消息
bool PortClient::close_buzz()
{
    QString data = NULL;
    serial_port->send_data(CLOSE_BUZZ, data);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送打开警报的消息
bool PortClient::open_warning()
{
    QString data = NULL;
    serial_port->send_data(OPEN_WARNING, data);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送关闭警报的消息
bool PortClient::close_warning()
{
    QString data = NULL;
    serial_port->send_data(CLOSE_WARNING, data);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送设置日期的消息
bool PortClient::set_date()
{
    //将当前的时间转化到括号中的形式
    QString date = QDateTime::currentDateTime().toString("yy-MM-dd");
    serial_port->send_data(SET_DATE, date);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送设置时间的消息
bool PortClient::set_time()
{
    //将当前的时间转化到括号中的形式
    QString time = QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss");
    serial_port->send_data(SET_TIME, time);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送读取日期的消息
bool PortClient::read_date()
{
    QString data = NULL;
    serial_port->send_data(READ_DATE, data);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//向ARM端发送读取时间的消息
bool PortClient::read_time()
{
    QString data = NULL;
    serial_port->send_data(READ_TIME, data);

    //该方法在接收完数据且发出readyRead()信号之前，是阻塞的。
    //超时之后，返回超时错误。
    //默认的msecs是30000毫秒。
    //当数据准备好了，返回ture；否则返回false
    bool ret;
    ret = serial_port->waitForReadyRead(10000);
    return ret;
}

//将读取的电压存取到对应文件中
bool PortClient::save_volt(QString volt)
{
    QString fileName("Record" + CURRENT_TIME + ".txt");

    //创建或打开记录文件
    QFile record(FILE_PATH + fileName);

    //以只写，文本，追加方式打开文件
    if(record.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qDebug()<<"文件打开成功!";

        //创建 QTextStream流操作对象，使与QFile对象record绑定。
        QTextStream in(&record);

        QString time = QDateTime::currentDateTime().toString("hh:mm:ss");

        //将当前电压值写入用户列表
        double currentVolt = volt.toDouble() / 1024 * 3.3;
        in << time << " volt = " << currentVolt << endl;

        record.close();
        return true;
    }
    else{
        qDebug()<<"文件打开失败!";
        return false;
    }

}


