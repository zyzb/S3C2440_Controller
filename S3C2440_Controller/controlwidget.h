#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class ControlWidget;
}

class ControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(QWidget *parent = 0);
    ~ControlWidget();

    void init_widget();     //初始化界面

    void init_data();       //初始化界面数据

    void set_selectable();  //设置列表和按钮状态

    QString get_text();     //获取并格式化发送框中的信息

protected:
    virtual void closeEvent(QCloseEvent * event);
    virtual void keyPressEvent(QKeyEvent * event);

private slots:
    void on_btnOpenPort_clicked();      //当打开串口的按钮被点击
    void on_btnClosePort_clicked();     //当关闭串口的按钮被点击
    void on_btnSend_clicked();          //当发送按钮被按下时
    void on_btnClose_clicked();         //当关闭按钮被按下时

    void clear_screen();                //清屏
    void read_volt();                   //读取电压
    void echo_data(QString text);       //回显
    void unknown(QString text);         //未知命令

    void display_warm_boot_start();             //显示热启动开始的信息
    void display_warm_boot_finish();            //显示热启动完成的信息
    void display_current_volt(QString data);    //显示当前电压
    void display_volt_overflow(QString data);   //设置当前状态为报警状态

    void open_led_success();                    //打开LED成功
    void led_already_open();                    //LED已处于打开状态

    void close_led_success();                   //关闭LED成功
    void led_already_close();                   //LED已处于关闭状态

    void open_buzz_success();                   //打开蜂鸣器成功
    void buzz_already_open();                   //蜂鸣器已处于打开状态

    void close_buzz_success();                  //关闭蜂鸣器成功
    void buzz_already_close();                  //蜂鸣器已处于关闭状态

    void open_warning_success();                //打开警报成功
    void warning_already_open();                //警报已处于打开状态

    void close_warning_success();               //关闭警报成功
    void warning_already_close();               //警报已处于关闭状态

    void set_date_success();                    //设置日期成功
    void set_time_success();                    //设置时间成功
    void display_current_date(QString data);    //显示当前日期
    void display_current_time(QString data);    //显示当前时间

    void do_time_count();               //用于热启动时的计时
    void do_flash_led();                //使LED字体闪烁
    void do_flash_Buzz();               //使蜂鸣器字体闪烁
    void do_warning();                  //打开警报

    void open_file_failed();            //打开文件失败

    void on_btnTestLED_clicked();

    void on_btnCloseLED_clicked();

    void on_btnTestBuzz_clicked();

    void on_btnCloseBuzz_clicked();

private:
    Ui::ControlWidget *ui;

    QTimer *timer_Boot;                 //用于启动时的计时
    QTimer *timer_LED;                  //控制LED的定时器
    QTimer *timer_Buzz;                 //控制蜂鸣器的定时器
    QTimer *timer_Warning;              //控制警报的定时器
    bool warning_state;                 //储存警报的状态
};

#endif // CONTROLWIDGET_H


