#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include "portclient.h"
#include "controlwidget.h"
#include "ui_controlwidget.h"

ControlWidget::ControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlWidget)
{
    ui->setupUi(this);

    //初始化窗口
    this->init_widget();

    //初始化数据
    this->init_data();

    //初始化timer
    this->timer_Boot = new QTimer(this);
    this->timer_LED = new QTimer(this);
    this->timer_Buzz = new QTimer(this);
    this->timer_Warning = new QTimer(this);

    warning_state = false;

    //连接热启动信号槽
    connect(timer_Boot, SIGNAL(timeout()), this, SLOT(do_time_count()));

    //连接LED信号槽
    connect(timer_LED, SIGNAL(timeout()), this, SLOT(do_flash_led()));

    //连接蜂鸣器信号槽
    connect(timer_Buzz, SIGNAL(timeout()), this, SLOT(do_flash_Buzz()));

    //连接警报信号槽
    connect(timer_Warning, SIGNAL(timeout()), this, SLOT(do_warning()));


    //获取PortClient的连接
    PortClient *p = PortClient::get_portClient_link();

    //client传来清屏的信号
    connect(p, SIGNAL(clear_screen()), this, SLOT(clear_screen()));

    //client传来读取电压的信号
    connect(p, SIGNAL(read_volt()), this, SLOT(read_volt()));

    //client传来回显的信号
    connect(p, SIGNAL(echo_data(QString)), this, SLOT(echo_data(QString)));

    //client传来未知的信号
    connect(p, SIGNAL(unknown(QString)), this, SLOT(unknown(QString)));

    //client传来热启动开始的信号
    connect(p, SIGNAL(display_warm_boot_start()), this, SLOT(display_warm_boot_start()));

    //client传来热启动完成的信号
    connect(p, SIGNAL(display_warm_boot_finish()), this, SLOT(display_warm_boot_finish()));

    //client传来显示当前电压的信号
    connect(p, SIGNAL(display_current_volt(QString)), this, SLOT(display_current_volt(QString)));

    //client传来显示电压异常的信号
    connect(p, SIGNAL(display_volt_overflow(QString)), this, SLOT(display_volt_overflow(QString)));

    //client传来打开LED成功的信号
    connect(p, SIGNAL(open_led_success()), this, SLOT(open_led_success()));

    //client传来LED已处于打开状态的信号
    connect(p, SIGNAL(led_already_open()), this, SLOT(led_already_open()));

    //client传来关闭LED成功的信号
    connect(p, SIGNAL(close_led_success()), this, SLOT(close_led_success()));

    //client传来LED已处于关闭状态的信号
    connect(p, SIGNAL(led_already_close()), this, SLOT(led_already_close()));

    //client传来打开蜂鸣器成功的信号
    connect(p, SIGNAL(open_buzz_success()), this, SLOT(open_buzz_success()));

    //client传来蜂鸣器已处于打开状态的信号
    connect(p, SIGNAL(buzz_already_open()), this, SLOT(buzz_already_open()));

    //client传来关闭蜂鸣器成功的信号
    connect(p, SIGNAL(close_buzz_success()), this, SLOT(close_buzz_success()));

    //client传来蜂鸣器已处于关闭状态的信号
    connect(p, SIGNAL(buzz_already_close()), this, SLOT(buzz_already_close()));

    //client传来打开警报成功的信号
    connect(p, SIGNAL(open_warning_success()), this, SLOT(open_warning_success()));

    //client传来警报已处于打开状态的信号
    connect(p, SIGNAL(warning_already_open()), this, SLOT(warning_already_open()));

    //client传来关闭警报成功的信号
    connect(p, SIGNAL(close_warning_success()), this, SLOT(close_warning_success()));

    //client传来蜂鸣器已处于关闭状态的信号
    connect(p, SIGNAL(warning_already_close()), this, SLOT(warning_already_close()));

    //client传来设置日期成功的信号
    connect(p, SIGNAL(set_date_success()), this, SLOT(set_date_success()));

    //client传来设置时间成功的信号
    connect(p, SIGNAL(set_time_success()), this, SLOT(set_time_success()));

    //client传来显示当前日期的信号
    connect(p, SIGNAL(display_current_date(QString)), this, SLOT(display_current_date(QString)));

    //client传来显示当前时间的信号
    connect(p, SIGNAL(display_current_time(QString)), this, SLOT(display_current_time(QString)));

    //client打开文件失败的信号
    connect(p, SIGNAL(open_file_failed()), this, SLOT(open_file_failed()));
}

ControlWidget::~ControlWidget()
{
    delete ui;
}



//当检测到用户关闭窗口的事件
void ControlWidget::closeEvent(QCloseEvent *event)
{
    //弹出对话框询问用户是否真的要退出程序
    int nRet = QMessageBox::question(this, "提示", "您真的要退出程序么？",
                                     QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

    if(nRet == QMessageBox::Yes){        //如果用户选择"是"
        //获取PortClient的连接
        PortClient *p = PortClient::get_portClient_link();

        //关闭端口
        p->close_port();

        event->accept();                 //接收该事件
    }
    if(nRet == QMessageBox::No){         //如果用户选择"否"
        event->ignore();                 //忽略该事件
    }
}


//重写键盘事件
void ControlWidget::keyPressEvent(QKeyEvent *event)
{
    //设置组合件Ctrl+Enter发送信息
    //先Ctrl后Enter触发，先Enter后Ctrl不可触发
    //注意：Key_Enter是小键盘的确认键，Key_Return是大键盘的回车键
    if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_Return))
    {
        qDebug() << "Ctrl+Enter";
        this->on_btnSend_clicked();  //相当于发送按钮被点击
    }
    //当Esc被按下时
    else if(event->key() == Qt::Key_Escape){
        qDebug() << "Esc";
        this->close();
    }
    QWidget::keyPressEvent(event);
}


//初始化界面
void ControlWidget::init_widget()
{
    //设置左上角界面标题
    this->setWindowTitle("S3C2440 Controller");

    //设置接收栏为只读
    ui->outputEdit->setReadOnly(true);

    //设置LCD字体类型为平面类型
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);

    //设置LCD字体颜色为绿色
    ui->lcdNumber->setStyleSheet("QLCDNumber{color:darkGreen}");
}


//初始化界面数据
void ControlWidget::init_data()
{
    QVector<QString> portName;  //储存端口信息

    //获取PortClient的连接
    PortClient *p = PortClient::get_portClient_link();
    portName = p->init_port();

    ui->cmbPortName->clear();
    foreach(QString pn, portName){
        ui->cmbPortName->addItem(pn);
    }

    QStringList baudList;       //波特率
    QStringList parityList;     //校验位
    QStringList dataBitsList;   //数据位
    QStringList stopBitsList;   //停止位
    QStringList flowControlList;//流控制

    //初始化波特率列表
    baudList << "1200" << "2400" << "4800" << "9600"
             << "19200" << "38400" << "57600" << "115200";
    ui->cmbBaudRate->addItems(baudList);
    ui->cmbBaudRate->setCurrentIndex(7);

    //初始化奇偶判断位列表
    parityList << "NULL" << "ODD" << "EVEN";
    ui->cmbParity->addItems(parityList);
    ui->cmbParity->setCurrentIndex(0);

    //初始化数据位列表
    dataBitsList << "5" << "6" << "7" << "8";
    ui->cmbDataBits->addItems(dataBitsList);
    ui->cmbDataBits->setCurrentIndex(3);

    //初始化停止位列表
    stopBitsList << "1" << "1.5" << "2";
    ui->cmbStopBits->addItems(stopBitsList);
    ui->cmbStopBits->setCurrentIndex(0);

    //初始化流控制列表
    flowControlList << "NULL" << "RTS/CTS" << "XON/XOFF";
    ui->cmbFlowControl->addItems(flowControlList);
    ui->cmbFlowControl->setCurrentIndex(0);

    //设置列表和按钮状态
    this->set_selectable();

}


//当打开串口的按钮被点击
void ControlWidget::on_btnOpenPort_clicked()
{
    //获取Client的连接
    PortClient *p = PortClient::get_portClient_link();

    //如果可用串口为0，重新初始化
    if(ui->cmbPortName->count() == 0){
        QVector<QString> portName;  //储存端口信息
        portName = p->init_port();

        foreach(QString pn, portName){
            ui->cmbPortName->addItem(pn);
        }
    }

    //创建QMap储存列表的选择
    QMap <QString, QString> portMessage;

    //读取串口号
    portMessage["portName"] = ui->cmbPortName->currentText();

    //读取波特率列表的当前下标
    portMessage["baudRate"] = QString::number(ui->cmbBaudRate->currentIndex());

    //读取数据位列表的当前下标
    portMessage["dataBits"] = QString::number(ui->cmbDataBits->currentIndex());

    //读取校验位列表的当前下标
    portMessage["parity"] = QString::number(ui->cmbParity->currentIndex());

    //读取流控制列表的当前下标
    portMessage["flowControl"] = QString::number(ui->cmbFlowControl->currentIndex());

    //读取停止位列表的当前下标
    portMessage["stopBits"] = QString::number(ui->cmbFlowControl->currentIndex());

    if(p->open_port(portMessage) == false){
        QMessageBox::warning(0, "提示", "串口打开失败!", QMessageBox::Yes);
    }
    this->set_selectable();
}

//当关闭串口的按钮被点击
void ControlWidget::on_btnClosePort_clicked()
{
    //获取Client的连接
    PortClient *p = PortClient::get_portClient_link();

    if(p->is_open())
    {
        p->close_port();
        this->set_selectable();
    }

}

//设置列表和按钮状态
void ControlWidget::set_selectable()
{
    //获得Client的连接
    PortClient *p = PortClient::get_portClient_link();
    bool bOpen = p->is_open();

    ui->cmbPortName->setEnabled(!bOpen);
    ui->cmbBaudRate->setEnabled(!bOpen);
    ui->cmbParity->setEnabled(!bOpen);
    ui->cmbDataBits->setEnabled(!bOpen);
    ui->cmbStopBits->setEnabled(!bOpen);
    ui->cmbFlowControl->setEnabled(!bOpen);
    ui->btnOpenPort->setEnabled(!bOpen);
    ui->btnClosePort->setEnabled(bOpen);

}


//当发送按钮被按下时
void ControlWidget::on_btnSend_clicked()
{
    //获取Client的连接
    PortClient *p = PortClient::get_portClient_link();

    //如果端口未打开
    if(p->is_open() == true){
        QString text = this->get_text();

        //读取QPlainTextEdit中的文本，若文本为空
        if(text == ""){
            QMessageBox::warning(0, "警告", "发送内容不能为空", QMessageBox::Yes);
            return;
        }
        else{
            bool ret = p->deal_send_data(text); //通知client处理数据

            if(ret == false){
                QMessageBox::warning(0, "警告", "连接已断开!", QMessageBox::Yes);
            }

            ui->inputEdit->clear();             //将QTextEdit中的内容清除
            ui->inputEdit->setFocus();          //设置焦点事件
        }
    }
    else{
        QMessageBox::warning(0, "警告", "端口未打开!", QMessageBox::Yes);
    }
}


//当关闭按钮被按下时
void ControlWidget::on_btnClose_clicked()
{
    this->close();  //关闭窗口，之后会触发closeEvent事件
}

//获取并格式化发送框中的信息
QString ControlWidget::get_text()
{
    QString text = ui->inputEdit->toPlainText();    //读取QPlainTextEdit中的文本
    text = text.trimmed();                          //先移除字符串两端的空白符如空格，换行符等

    //再找出回车的位置（顺序不能反）
    for(int i = 0; text.indexOf("\n", i) != -1; i++){
        text = text.insert(text.indexOf("\n", i), "\\");
        text = text.insert(text.indexOf("\n", i) + 1, ">");
        i = text.indexOf("\n", i) + 1;
    }
    return text;
}


//清屏
void ControlWidget::clear_screen()
{
    ui->outputEdit->clear();
    qDebug() << "clear_screen()";
}

//读取电压
void ControlWidget::read_volt()
{
    QString headText(">>>");
    QString volt = QString::number(ui->lcdNumber->value());
    ui->outputEdit->setFont(QFont("Times New Roman", 10));
    ui->outputEdit->appendPlainText(headText + "Volt = " + volt);
    ui->outputEdit->appendPlainText("");
    qDebug() << "read_volt()";
}

//回显
void ControlWidget::echo_data(QString text)
{
    QString headText(">>>");
    ui->outputEdit->setFont(QFont("Times New Roman", 10));
    ui->outputEdit->appendPlainText(headText + text);
    ui->outputEdit->appendPlainText("");
    qDebug() << "echo_data(QString text)";
}

//未知命令
void ControlWidget::unknown(QString text)
{
    QString headText(">>>");
    QString unknownText(text + " : command not found");
    ui->outputEdit->setFont(QFont("Times New Roman", 10));
    ui->outputEdit->appendPlainText(headText + unknownText);
    ui->outputEdit->appendPlainText("");
    qDebug() << "unknown(QString text)";
}

//显示热启动开始的信息
void ControlWidget::display_warm_boot_start()
{
    this->echo_data("Boot start......");
    timer_Boot->start(1000);
}

//显示热启动完成的信息
void ControlWidget::display_warm_boot_finish()
{
    //阻塞
    while(1){
        //等待本次计时器结束（预留10毫秒的时间）
        if(timer_Boot->remainingTime() < 10){

            this->echo_data("Boot Finished......");
            this->echo_data("Welcome to use our Controller.");
            this->echo_data("");
            timer_Boot->stop();
            break;
        }
        else{
            continue;
        }
    }

}

//显示当前电压
void ControlWidget::display_current_volt(QString data)
{
    double volt = data.toDouble() / 1024 * 3.3;
    ui->lcdNumber->display(volt);
}

//设置当前状态为报警状态
void ControlWidget::display_volt_overflow(QString data)
{
    this->display_current_volt(data);
    this->open_warning_success();   //打开警报
}

//打开LED成功
void ControlWidget::open_led_success()
{
    this->echo_data("LED : Open.");

    ui->lblLED->setText("LED：开");

    //如果警报未处于打开状态
    if(timer_Warning->isActive() == false){
        //设定超时时间为0.5秒
        timer_LED->start(500);
    }

}

//LED已处于打开状态
void ControlWidget::led_already_open()
{
    this->echo_data("LED has already opened.");
}

//关闭LED成功
void ControlWidget::close_led_success()
{
    this->echo_data("LED : Close.");

    ui->lblLED->setText("LED：关");

    //停止计时
    timer_LED->stop();

    //修改字体颜色为黑色
    QPalette palette = ui->lblLED->palette();
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->lblLED->setPalette(palette);

}

//LED已处于关闭状态
void ControlWidget::led_already_close()
{
    this->echo_data("LED has already closed.");
}

//打开蜂鸣器成功
void ControlWidget::open_buzz_success()
{
    this->echo_data("Buzz : Open.");

    ui->lblBuzz->setText("蜂鸣器：开");

    //如果警报未处于打开状态
    if(timer_Warning->isActive() == false){
        //设定超时时间为0.5秒
        timer_Buzz->start(500);
    }
}

//蜂鸣器已处于打开状态
void ControlWidget::buzz_already_open()
{
    this->echo_data("Buzz has already opened.");
}

//关闭蜂鸣器成功
void ControlWidget::close_buzz_success()
{
    this->echo_data("Buzz : Close.");

    ui->lblBuzz->setText("蜂鸣器：关");

    //停止计时
    timer_Buzz->stop();

    //修改字体颜色为黑色
    QPalette palette = ui->lblBuzz->palette();
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->lblBuzz->setPalette(palette);

}

//蜂鸣器已处于关闭状态
void ControlWidget::buzz_already_close()
{
    this->echo_data("Buzz has already closed.");
}

//打开警报成功
void ControlWidget::open_warning_success()
{
    //设置LCD字体颜色为红色
    ui->lcdNumber->setStyleSheet("QLCDNumber{color:red}");

    if(this->warning_state == false){
        this->echo_data("Warning : Open.");
        QMessageBox::warning(0, "警告", "当前电压超过正常值!", QMessageBox::Yes);
        warning_state = true;
    }

    //关闭LED
    timer_LED->stop();

    ui->lblLED->setText("LED：开");

    //修改字体颜色为黑色
    QPalette ledPalette = ui->lblLED->palette();
    ledPalette.setColor(QPalette::WindowText, Qt::black);
    ui->lblLED->setPalette(ledPalette);

    //关闭蜂鸣器
    timer_Buzz->stop();

    ui->lblBuzz->setText("蜂鸣器：开");

    //修改字体颜色为黑色
    QPalette buzzPalette = ui->lblBuzz->palette();
    buzzPalette.setColor(QPalette::WindowText, Qt::black);
    ui->lblBuzz->setPalette(buzzPalette);

    //如果timer处于运行状态
    if(timer_Warning->isActive()){
        return;
    }

    //设定超时时间为0.5秒
    timer_Warning->start(500);

}

//警报已处于打开状态
void ControlWidget::warning_already_open()
{
    this->echo_data("Warning has already opened.");
}

//关闭警报成功
void ControlWidget::close_warning_success()
{
    this->echo_data("Warning : Close.");

    //停止计时
    timer_Warning->stop();

    //设置LCD字体颜色为绿色
    ui->lcdNumber->setStyleSheet("QLCDNumber{color:green}");

    //修改LED字体颜色为黑色
    QPalette ledPalette = ui->lblLED->palette();
    ledPalette.setColor(QPalette::WindowText, Qt::black);
    ui->lblLED->setPalette(ledPalette);

    //修改蜂鸣器字体颜色为黑色
    QPalette buzzPalette = ui->lblBuzz->palette();
    buzzPalette.setColor(QPalette::WindowText, Qt::black);
    ui->lblBuzz->setPalette(buzzPalette);

    this->warning_state = false;
}

//警报已处于关闭状态
void ControlWidget::warning_already_close()
{
    this->echo_data("Waring has already closed.");
}

//设置日期成功
void ControlWidget::set_date_success()
{
    this->echo_data("Set date success.");
}

//设置时间成功
void ControlWidget::set_time_success()
{
    this->echo_data("Set time success.");
}

//显示当前日期
void ControlWidget::display_current_date(QString data)
{
    QString head("Current date: ");
    this->echo_data(head + data);
}

//显示当前时间
void ControlWidget::display_current_time(QString data)
{
    QString head("Current time: ");
    this->echo_data(head + data);
}

//用于热启动时的计时
void ControlWidget::do_time_count()
{
    this->echo_data("Loading......");
}

//使LED字体闪烁
void ControlWidget::do_flash_led()
{
    static int count = 0;

    if(count == 0){
        //修改字体颜色为黄色
        QPalette palette = ui->lblLED->palette();
        palette.setColor(QPalette::WindowText, Qt::yellow);
        ui->lblLED->setPalette(palette);
        count = 1;
    }
    else if(count == 1){
        //修改字体颜色为黑色
        QPalette palette = ui->lblLED->palette();
        palette.setColor(QPalette::WindowText, Qt::black);
        ui->lblLED->setPalette(palette);
        count = 0;
    }

}

//使蜂鸣器字体闪烁
void ControlWidget::do_flash_Buzz()
{
    static int count = 0;


        if(count == 0){
            //修改字体颜色为红色
            QPalette palette = ui->lblBuzz->palette();
            palette.setColor(QPalette::WindowText, Qt::red);
            ui->lblBuzz->setPalette(palette);
            count = 1;
        }
        else if(count == 1){
            //修改字体颜色为黑色
            QPalette palette = ui->lblBuzz->palette();
            palette.setColor(QPalette::WindowText, Qt::black);
            ui->lblBuzz->setPalette(palette);
            count = 0;
        }

}

//打开警报
void ControlWidget::do_warning()
{

    static int count = 0;

    if(count == 0){
        //修改led字体颜色为黄色
        QPalette ledPalette = ui->lblLED->palette();
        ledPalette.setColor(QPalette::WindowText, Qt::yellow);
        ui->lblLED->setPalette(ledPalette);

        //修改buzz字体颜色为红色
        QPalette buzzPalette = ui->lblBuzz->palette();
        buzzPalette.setColor(QPalette::WindowText, Qt::red);
        ui->lblBuzz->setPalette(buzzPalette);

        count = 1;
    }
    else if(count == 1){
        //修改led字体颜色为黑色
        QPalette ledPalette = ui->lblLED->palette();
        ledPalette.setColor(QPalette::WindowText, Qt::black);
        ui->lblLED->setPalette(ledPalette);

        //修改buzz字体颜色为黑色
        QPalette buzzPalette = ui->lblBuzz->palette();
        buzzPalette.setColor(QPalette::WindowText, Qt::black);
        ui->lblBuzz->setPalette(buzzPalette);

        count = 0;
    }
}

//打开文件失败
void ControlWidget::open_file_failed()
{
    static int count = 0;
    if(count < 5){
        QMessageBox::warning(0, "警告", "记录写入失败!",QMessageBox::Yes);
        count++;
    }

}

//当LED的测试按钮被点击时
void ControlWidget::on_btnTestLED_clicked()
{
    //获取Client的连接
    PortClient *p = PortClient::get_portClient_link();
    bool ret = p->open_led();

    if(ret == false){
        QMessageBox::warning(0, "警告", "连接已断开!", QMessageBox::Yes);
    }
}

//当LED的关闭按钮被点击时
void ControlWidget::on_btnCloseLED_clicked()
{
    //获取Client的连接
    PortClient *p = PortClient::get_portClient_link();
    bool ret = p->close_led();

    if(ret == false){
        QMessageBox::warning(0, "警告", "连接已断开!", QMessageBox::Yes);
    }
}

//当蜂鸣器的测试按钮被点击时
void ControlWidget::on_btnTestBuzz_clicked()
{
    //获取Client的连接
    PortClient *p = PortClient::get_portClient_link();
    bool ret = p->open_buzz();

    if(ret == false){
        QMessageBox::warning(0, "警告", "连接已断开!", QMessageBox::Yes);
    }
}

//当蜂鸣器的关闭按钮被点击时
void ControlWidget::on_btnCloseBuzz_clicked()
{
    //获取Client的连接
    PortClient *p = PortClient::get_portClient_link();
    bool ret = p->close_buzz();

    if(ret == false){
        QMessageBox::warning(0, "警告", "连接已断开!", QMessageBox::Yes);
    }
}
