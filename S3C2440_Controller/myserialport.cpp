#include <QDataStream>
#include <QVector>
#include <string>
#include <QDebug>
#include "myserialport.h"



//获得串口的连接
MySerialPort *MySerialPort::get_serialPort_link()
{
    static MySerialPort mySerialPort = MySerialPort();
    return &mySerialPort;
}


//构造函数
MySerialPort::MySerialPort(QObject *parent) :
    QSerialPort(parent)
{

    connect(this, SIGNAL(readyRead()), this, SLOT(receive_data()));

}


//发送数据，消息内容存放在QVector中
void MySerialPort::send_data(DataType type, QString data)
{
    DataUnion dataUnion;                            //创建联合体储存要发送的数据
    memset(&dataUnion, '\0', sizeof(dataUnion));

    strncpy(dataUnion.dataStruct.head, "#", 1);
    strncpy(dataUnion.dataStruct.end, "$", 1);

    int dataType = type;                            //创建一个int类型的变量储存数据类型

    //字符'0'的ASCII码为48，然后是'1' '2' '3' ... '9'
    //将dataType的十位数和个位数存入char数组中
    dataUnion.dataStruct.type[0] = (char)(dataType / 10 + 48);
    dataUnion.dataStruct.type[1] = (char)(dataType % 10 + 48);

    QByteArray ba = data.toLatin1();                //将QString中的数据转换为QByteArray

    //将QByteArray中的值赋给char数组
    strncpy(dataUnion.dataStruct.data, ba.data(), sizeof(dataUnion.dataStruct.data));

    // 发送信息
    this->write(dataUnion.dataBuf, sizeof(dataUnion.dataBuf));
}



//当接收到readyRead()信号时执行，接收数据
void MySerialPort::receive_data()
{
    static int count = 0;

    if(count == 0){
        if(this->bytesAvailable() < 128){
            return;
        }
        else{
            QByteArray ba;
            ba = this->readAll();
            QString str = QString(ba).toLatin1();
            emit init_data(str);
            count ++;
        }
    }
    else if(count == 1){
        if(this->bytesAvailable() < 119){
            return;
        }
        else{
            QByteArray ba;
            ba = this->readAll();
            QString str = QString(ba).toLatin1();
            emit init_data(str);
            count ++;
        }
    }
    else if(count >= 2){
        DataUnion dataUnion;                            //创建联合体储存要接收的数据
        memset(&dataUnion, '\0', sizeof(dataUnion));

        strncpy(dataUnion.dataStruct.head, "#", 1);
        strncpy(dataUnion.dataStruct.end, "$", 1);


        //如果可读字节数小于数据体的大小
        if(this->bytesAvailable() < sizeof(dataUnion)){
            return;
        }
        else{
            char temp[1] = {0};

            //读取数据头部长度的内容
            this->read(temp, sizeof(dataUnion.dataStruct.head));
            qDebug() << "head " <<  dataUnion.dataStruct.head;

            qDebug()<< "temp " << temp[0];

            //如果内容为数据头部"#",说明可能读取到了一条数据
            if(strncmp(temp, dataUnion.dataStruct.head, 1) == 0){
                //读取数据类型长度的内容
                this->read(dataUnion.dataStruct.type, sizeof(dataUnion.dataStruct.type));

                //读取数据内容长度的内容
                this->read(dataUnion.dataStruct.data, sizeof(dataUnion.dataStruct.data));

                //读取数据尾部长度的内容
                this->read(temp, sizeof(dataUnion.dataStruct.end));

                //如果内容为数据尾部"$$"，证明确实读取到了一条数据
                if(strncmp(temp, dataUnion.dataStruct.end, 1) == 0){
                    //将读取的数据类型转换为int格式
                    int dataType = 10 * (int)(dataUnion.dataStruct.type[0] - 48)
                            + (int)(dataUnion.dataStruct.type[1] - 48);

                    //将读取的数据内容转换为QString格式
                    char data[sizeof(dataUnion.dataStruct.data) + 1] = {0};
                    strncpy(data, dataUnion.dataStruct.data, sizeof(dataUnion.dataStruct.data));

                    //读取了一条信息后发送信号通知client处理
                    emit read_data((DataType)dataType, QString(data));
                }
                //如果内容不是数据尾部"$"，说明读取的不是一条完整的数据
                //可能是某条数据内含有与数据头部"#"相同的字符
                else{
                    //将读取的内容清零
                    memset(dataUnion.dataStruct.type, '\0', sizeof(dataUnion.dataStruct.type));
                    memset(dataUnion.dataStruct.data, '\0', sizeof(dataUnion.dataStruct.data));
                    return;
                }
            }
        }
    }

    //递归调用，再次处理剩余的数据
    if (this->bytesAvailable() > 0)
    {
        this->receive_data();  //继续接收信息
    }

}


