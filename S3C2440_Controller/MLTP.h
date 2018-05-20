#ifndef MLTP_H
#define MLTP_H

/* MLTP = My_Low_Transfer_Protocol */

//联合体中成员公用内存，且按所占内存最大的数据成员分配内存
typedef union DataUnion{
    char dataBuf[32];      //发送和接收的数据格式,设置为32位是为了便于ARM端读取

    struct dataStruct{     //赋值和读取的数据格式
        char head[1];      //开始标志
        char type[2];      //数据类型
        char data[28];     //数据内容
        char end[1];       //结束标志
    }dataStruct;

}DataUnion;


//数据类型
enum DataType{
    //Qt端自身的消息类型
    CLEAR,          //清屏
    READ_VOLT,      //读取当前电压
    ECHO_DATA,      //回显信息
    UNKNOW,         //未知

    //Qt端发送给开发板的消息类型
    OPEN_LED,         //打开LED
    CLOSE_LED,        //关闭LED

    OPEN_BUZZ,        //打开蜂鸣器
    CLOSE_BUZZ,       //关闭蜂鸣器

    OPEN_WARNING,     //打开警报
    CLOSE_WARNING,    //关闭警报

    SET_DATE,         //设置日期
    SET_TIME,         //设置时间

    READ_DATE,        //读取日期
    READ_TIME,        //读取时间


    //开发板端发送给Qt端的消息类型
    SEND_PRINTF_TEXT,        //向Qt发送文本内容

    SEND_WARM_BOOT_START,    //向Qt发送开始热启动的信息

    SEND_WARM_BOOT_FINISH,   //向Qt发送热启动完成的信息

    SEND_CURRENT_VOLT,       //向Qt发送当前电压值

    SEND_VOLT_OVERFLOW,      //向Qt发送电压超出正常值

    OPEN_LED_SUCCESS,        //LED打开成功
    LED_ALREADY_OPEN,        //LED已处于打开状态

    CLOSE_LED_SUCCESS,       //LED关闭成功
    LED_ALREADY_CLOSE,       //LED已处于关闭状态

    OPEN_BUZZ_SUCCESS,       //蜂鸣器打开成功
    BUZZ_ALREADY_OPEN,       //蜂鸣器已处于打开状态

    CLOSE_BUZZ_SUCCESS,      //蜂鸣器关闭成功
    BUZZ_ALREADY_CLOSE,      //蜂鸣器已处于关闭状态

    OPEN_WARNING_SUCCESS,    //警报打开成功
    WARNING_ALREADY_OPEN,    //警报已处于打开状态

    CLOSE_WARNING_SUCCESS,   //警报关闭成功
    WARNING_ALREADY_CLOSE,   //警报已处于关闭状态

    SET_DATE_SUCCESS,        //设置日期成功

    SET_TIME_SUCCESSE,       //设置时间成功

    READ_DATE_SUCCESS,       //读取日期成功

    READ_TIME_SUCCESS,       //读取时间成功

};


#endif // MLTP_H
