#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <errno.h>
#include <sstream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    openFlag = false;
    timerFlag = false;
    autosendFlag = false;
    recvNum = 0;
    sendNum = 0;

    seriport = new SerialPort;
    thread = new Thread;
    timer = new QTimer;
    label = new QLabel;
    systemLabel = new QLabel;
    systemTimer = new QTimer;
    sendLable = new QLabel;
    recvLable = new QLabel;

    systemTimer->setInterval(1000);

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeOutEvent()));
    QObject::connect(systemTimer, SIGNAL(timeout()), this, SLOT(systemTimeUpdate()));
    QObject::connect(thread, SIGNAL(sendDateToRecvText(uint8_t * , int)), this, SLOT(showInRecvText(uint8_t * , int)));
    QObject::connect(ui->recvTextEdit, SIGNAL(textChanged()), this, SLOT(autoScroll()));
    QObject::connect(ui->rateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(reLoadSerialPort()));
    QObject::connect(ui->checkBitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(reLoadSerialPort()));
    QObject::connect(ui->dataBitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(reLoadSerialPort()));
    QObject::connect(ui->stopBitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(reLoadSerialPort()));

    QObject::connect(ui->sendLineEdit3, SIGNAL(returnPressed()), this, SLOT(sendLineEdit3()));
    QObject::connect(ui->sendPushButton3, SIGNAL(clicked(bool)), this, SLOT(sendLineEdit3()));
    QObject::connect(ui->sendLineEdit4, SIGNAL(returnPressed()), this, SLOT(sendLineEdit4()));
    QObject::connect(ui->sendPushButton4, SIGNAL(clicked(bool)), this, SLOT(sendLineEdit4()));
    QObject::connect(ui->sendLineEdit5, SIGNAL(returnPressed()), this, SLOT(sendLineEdit5()));
    QObject::connect(ui->sendPushButton5, SIGNAL(clicked(bool)), this, SLOT(sendLineEdit5()));
    QObject::connect(ui->sendLineEdit6, SIGNAL(returnPressed()), this, SLOT(sendLineEdit6()));
    QObject::connect(ui->sendPushButton6, SIGNAL(clicked(bool)), this, SLOT(sendLineEdit6()));
    SerialPortInit();

    label->setText(QString::fromUtf8("     串口状态：　关闭"));
    ui->statusBar->addWidget(label);

    recvLable->setText(QString::fromUtf8("\t接收：") + QString::number(recvNum));
    ui->statusBar->addWidget(recvLable);

    sendLable->setText(QString::fromUtf8("\t发送：") + QString::number(sendNum));
    ui->statusBar->addWidget(sendLable);

    systemLabel->setText(QString::fromUtf8("\t当前时间：") + QTime::currentTime().toString());
    ui->statusBar->addWidget(systemLabel);

    systemTimer->start();
}

MainWindow::~MainWindow() {
    seriport->closeSerialPort();
    systemTimer->stop();
    delete ui;
    delete thread;
    delete seriport;
    delete timer;
    delete label;
    delete systemLabel;
    delete systemTimer;
    delete recvLable;
    delete sendLable;
}

void MainWindow::systemTimeUpdate() {
    recvLable->setText(QString::fromUtf8("\t接收：") + QString::number(recvNum));
    sendLable->setText(QString::fromUtf8("\t发送：") + QString::number(sendNum));
    systemLabel->setText(QString::fromUtf8("\t当前时间：") + QTime::currentTime().toString());
}

void MainWindow::reLoadSerialPort() {
    if (openFlag) {
        on_openPushButton_clicked();
    }
}

// 接收区自动滚屏
void MainWindow::autoScroll() {
    QTextCursor cursor = ui->recvTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->recvTextEdit->setTextCursor(cursor);
}

/**
 * 16进制数组转换成字符串
 * @param buf in 16进制数组
 * @param len 数组长度
 * @return 字符串
 */
static string HexArray2Str(uint8_t *buf, int len) {
    string ret;
    for (int i = 0; i < len; i++) {
        char tmp[4];
        bzero(tmp, sizeof(tmp));
        sprintf(tmp, "%02x", buf[i]);
        ret += tmp;
        if (i < (len - 1)) {
            //加空格
            ret += " ";
        }
    }
    return ret;
}


// 显示收到的数据的槽
void MainWindow::showInRecvText(uint8_t *recv_buf, int recv_len) {
    string result;
    if (ui->hexShowCheckBox->isChecked()) {
        //按照16进制解析字符串，并显示
        result = HexArray2Str(recv_buf, recv_len);
    } else {
        //原文
        result = (char *) recv_buf;
    }
    recvNum += recv_len;
    ui->recvTextEdit->insertPlainText("recv:");
    ui->recvTextEdit->insertPlainText(QString::fromStdString(result));
    ui->recvTextEdit->insertPlainText("\n");

    thread->canRecv = true;

}

//  初始化槽，用于动态加载当前的串口到combobox
void MainWindow::SerialPortInitSlots() {
    SerialPortInit();
}

//　初始化函数，获取当前的所有串口
void MainWindow::SerialPortInit() {
    QFile file(PORTFILENAME);
    if (file.exists())       // 如果文件存在删除，或清空
    {
        file.remove();
    }

    // 查看当前的串口，并保存到文件中
    int ret = system("ls /dev/ttyUSB* > serialport.dat");
    if (ret < 0) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("串口异常"));
    }

    //　读取文件中的串口并添加到combobox中
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("串口异常"));
        exit(0);
    }

    // 读取当前的串口并添加到combobox中
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        line.remove(0, 5);
        ui->serialPortComboBox->addItem(QString::fromLocal8Bit(line).trimmed());
    }

    file.close();
    file.remove();
}

// 打开串口，且开启线程读取串口
void MainWindow::on_openPushButton_clicked() {
    if (!openFlag) {
        QString serialPortStr = ui->serialPortComboBox->currentText().isEmpty() ? ""
                                                                                : ui->serialPortComboBox->currentText();
        QString rateStr = ui->rateComboBox->currentText().isEmpty() ? "" : ui->rateComboBox->currentText();
        QString checkBitStr = ui->checkBitComboBox->currentText().isEmpty() ? "" : ui->checkBitComboBox->currentText();
        QString dataBirStr = ui->dataBitComboBox->currentText().isEmpty() ? "" : ui->dataBitComboBox->currentText();
        QString stopBbitStr = ui->stopBitComboBox->currentText().isEmpty() ? "" : ui->stopBitComboBox->currentText();

        qDebug() << "port: " << serialPortStr;
        qDebug() << "rate: " << rateStr;
        qDebug() << "check: " << checkBitStr;
        qDebug() << "data: " << dataBirStr;
        qDebug() << "stop: " << stopBbitStr;

        if (serialPortStr == "") {
            QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("未找到串口"));
            return;
        }

        if (!rateStr.isNull() && !checkBitStr.isNull() && !dataBirStr.isNull() && !stopBbitStr.isNull()) {
            bool rateOK = false;
            bool dataOK = false;
            bool stopOK = false;
            serialPortStr = "/dev/" + serialPortStr;
            seriport->initSet(serialPortStr.toStdString(), rateStr.toInt(&rateOK), dataBirStr.toInt(&dataOK),
                              stopBbitStr.toInt(&stopOK), checkBitStr.toStdString()[0]);
            if (!rateOK || !dataOK || !stopOK) {
                QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("串口异常"));
                exit(0);
            }
        } else {
            QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("串口异常"));
            exit(0);
        }
        int ret = seriport->openSerialPort();
        qDebug() << "open ret: " << ret;
        if (-1 == ret) {
            perror("open error");
            qDebug() << "open error errno: " << errno;
            if (errno == 13) {
                QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请使用root权限"));
            } else {
                QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("串口异常"));
            }
            seriport->closeSerialPort();
        } else {
            seriport->SetSpeed();
            seriport->SetParity();
            ui->openPushButton->setText(QString::fromUtf8("关闭串口"));
            openFlag = true;
            label->setText(QString::fromUtf8("     串口状态：　打开"));
            sendNum = 0;
            recvNum = 0;
            thread->setPort((void *) seriport);
            thread->start();
        }
    } else    // 关闭串口
    {
        seriport->closeSerialPort();
        ui->openPushButton->setText(QString::fromUtf8("打开串口"));
        openFlag = false;
        label->setText(QString::fromUtf8("     串口状态：　关闭"));
        thread->stop();
        autosendFlag = false;
        qDebug() << "closed ...";
    }
}

// 清空接收区
void MainWindow::on_clearRecvPushButton_clicked() {
    ui->recvTextEdit->clear();
}

static bool IsHexNum(string value) {
    bool ret = false;
    for (int i = 0; i < value.size(); i++) {
        char c = value.at(i);
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
            ret = true;
        } else {
            ret = false;
            break;
        }
    }
    return ret;
}

static bool IsDecNum(string value) {
    bool ret = false;
    for (int i = 0; i < value.size(); i++) {
        char c = value.at(i);
        if ((c >= '0' && c <= '9')) {
            ret = true;
        } else {
            ret = false;
            break;
        }
    }
    return ret;
}

static uint8_t Str2Hex(string value) {
    uint8_t ret = 0;
    if (value.size() != 2) {
        return ret;
    }
    char *str;
    ret = strtol(value.c_str(), &str, 16);

    return ret;
}

/**
 * 将16进制字符串转换为16进制数组
 * @param buf out 16进制数组
 * @param len out 数组长度
 * @param str in 16进制字符串
 * @return 0：转换成功 -1：转换失败
 */
static int HexStr2HexArray(uint8_t *buf, int *len, string str) {
    //1.先将原始文本按照空格进行分割
    vector<string> res;
    string tmp;
    stringstream input(str);
    while (input >> tmp) {
        res.push_back(tmp);
    }

    //检查分割后的字符是否全部是16进制的数字的字符
    for (int i = 0; i < res.size(); i++) {
        if (!IsHexNum(res.at(i))) {

            return -1;
        }
    }
    //将16进制字符串转化为数值
    int index = 0;
    for (int i = 0; i < res.size(); i++) {
        buf[i] = Str2Hex(res.at(i));
        index++;
    }
    *len = index;
    return 0;
}


// 发送１，不清空发送区
void MainWindow::on_sendPushButton1_clicked() {
    if (openFlag) {
        //原始文本数据
        string sendStr = ui->sendTextEdit1->toPlainText().toStdString();
        //发送数据
        uint8_t send_buf[1024 * 512];
        int send_len = 0;
        bzero(send_buf, sizeof(send_buf));
        if (ui->hexSendcheckBox->isChecked()) {
            //需要把发送的文本按照16进制进行发送
            if (HexStr2HexArray(send_buf, &send_len, sendStr) != 0) {
                QMessageBox::information(this, QString::fromUtf8("错误"),
                                         QString::fromUtf8("有非法字符"));
                return;
            }

        } else {
            //获取发送的文本
            memcpy(send_buf, sendStr.c_str(), sendStr.size());
            send_len += sendStr.size();
        }
        //更新发送计数，串口发送，将发送的数据显示出来
        sendNum += send_len;
        seriport->writeSerialPort(send_buf, send_len);
        ui->recvTextEdit->insertPlainText("send:");
        for (int i = 0; i < send_len; i++) {
            char hex_str[4];
            sprintf(hex_str, "%02x ", send_buf[i]);
            ui->recvTextEdit->insertPlainText(QString::fromStdString(hex_str));
        }
        ui->recvTextEdit->insertPlainText("\n");

    } else {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请打开串口"));
    }
}

// 发送２，要判断是否自动清空发送区２
void MainWindow::on_sendPushButton2_clicked() {
    if (openFlag) {
        QString sendStr = ui->sendLineEdit2->text();

        uint8_t send_buf[1024 * 512];
        int send_len = 0;
        bzero(send_buf, sizeof(send_buf));

        memcpy(send_buf, sendStr.toStdString().c_str(), sendStr.length());
        send_len = sendStr.length();

        sendNum += send_len;
        seriport->writeSerialPort(send_buf, send_len);
        if (ui->autoClearSendcheckBox->isChecked()) {
            ui->sendLineEdit2->clear();
        }
    } else {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请打开串口"));
    }
}

// 发送２，要判断是否自动清空发送区２,按回车建时发送数据，要发送回车
void MainWindow::on_sendLineEdit2_returnPressed() {
    if (openFlag) {
        QString sendStr = ui->sendLineEdit2->text();

        uint8_t send_buf[1024 * 512];
        int send_len = 0;
        bzero(send_buf, sizeof(send_buf));

        memcpy(send_buf, sendStr.toStdString().c_str(), sendStr.length());
        send_len = sendStr.length();
        send_buf[send_len] = '\n';
        send_len += 1;

        sendNum += send_len;
        seriport->writeSerialPort(send_buf, send_len);
        if (ui->autoClearSendcheckBox->isChecked()) {
            ui->sendLineEdit2->clear();
        }
    } else {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请打开串口"));
    }
}

// 发送３
void MainWindow::sendLineEdit3() {
    if (openFlag) {
        QString sendStr = ui->sendLineEdit3->text();

        uint8_t send_buf[1024 * 512];
        int send_len = 0;
        bzero(send_buf, sizeof(send_buf));

        memcpy(send_buf, sendStr.toStdString().c_str(), sendStr.length());
        send_len = sendStr.length();

        sendNum += send_len;
        seriport->writeSerialPort(send_buf, send_len);
        ui->recvTextEdit->insertPlainText("send: " + sendStr + '\n');
    } else {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请打开串口"));
    }
}

// 发送4
void MainWindow::sendLineEdit4() {
    if (openFlag) {
        QString sendStr = ui->sendLineEdit4->text();

        uint8_t send_buf[1024 * 512];
        int send_len = 0;
        bzero(send_buf, sizeof(send_buf));

        memcpy(send_buf, sendStr.toStdString().c_str(), sendStr.length());
        send_len = sendStr.length();

        sendNum += send_len;
        seriport->writeSerialPort(send_buf, send_len);
        ui->recvTextEdit->insertPlainText("send: " + sendStr + '\n');
    } else {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请打开串口"));
    }
}


// 发送5
void MainWindow::sendLineEdit5() {
    if (openFlag) {
        QString sendStr = ui->sendLineEdit5->text();

        uint8_t send_buf[1024 * 512];
        int send_len = 0;
        bzero(send_buf, sizeof(send_buf));

        memcpy(send_buf, sendStr.toStdString().c_str(), sendStr.length());
        send_len = sendStr.length();

        sendNum += send_len;
        seriport->writeSerialPort(send_buf, send_len);
        ui->recvTextEdit->insertPlainText("send: " + sendStr + '\n');
    } else {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请打开串口"));
    }
}


// 发送6
void MainWindow::sendLineEdit6() {
    if (openFlag) {
        QString sendStr = ui->sendLineEdit6->text();

        uint8_t send_buf[1024 * 512];
        int send_len = 0;
        bzero(send_buf, sizeof(send_buf));

        memcpy(send_buf, sendStr.toStdString().c_str(), sendStr.length());
        send_len = sendStr.length();

        sendNum += send_len;
        seriport->writeSerialPort(send_buf, send_len);
        ui->recvTextEdit->insertPlainText("send: " + sendStr + '\n');
    } else {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请打开串口"));
    }
}

void MainWindow::timeOutEvent() {
    qDebug() << "time out ... ";
    on_sendPushButton1_clicked();
}

void MainWindow::on_autoSendCheckBox_clicked() {
    if (!autosendFlag)   // start
    {
        if (openFlag) {
            bool ok = true;
            int num = ui->autoSendPeriodLineEdit->text().toInt(&ok);
            if (ok) {
                timer->setInterval(num);
            } else {
                QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("时间间隔错误"));
                return;
            }
            timer->start();
            openFlag = true;
            autosendFlag = true;
            qDebug() << "timer start ...";
        } else {
            ui->autoSendCheckBox->setCheckable(false);
            QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请打开串口"));
        }
    } else    // stop
    {
        timer->stop();
        qDebug() << "timer stop ...";
        autosendFlag = false;
    }
}
