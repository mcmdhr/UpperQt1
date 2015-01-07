#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    ui->setupUi(this);
    countSerialSend = 0;
    bit3 = 0;
    flagLED = false;
    flagRelay = false;
    flagInfrared = false;
    flagSmoke = false;
    flagTemp = false;
    flagLight = false;
    tcpServer=new QTcpServer(this);
    sbuf = "SerialPortOpen";
    if(!tcpServer->listen(QHostAddress::Any,8812))
    {//监听本机6666端口。若出错，则输出错误信息，并关闭
      qDebug()<<tcpServer->errorString();
      close();
    }
    else{
        ui->lbConnStatus->setText("ListenSucc");
    }
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(sendMessage())); //连接信号和相应槽函数

    serialport = new QSerialPort(this);
    connect(ui->btnOpenSerial, SIGNAL(clicked()), this, SLOT(openPort()));
    connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(btn_send_clicked()));

//    connect(ui->btnLED1, SIGNAL(clicked()), this, SLOT(dataCalcAndSend()));
//    connect(ui->btnRelay, SIGNAL(clicked()), this, SLOT(dataCalcAndSend()));
//    connect(ui->btnInfrared, SIGNAL(clicked()), this, SLOT(dataCalcAndSend()));
//    connect(ui->btnSmoke, SIGNAL(clicked()), this, SLOT(dataCalcAndSend()));
//    connect(ui->btnTemp, SIGNAL(clicked()), this, SLOT(dataCalcAndSend()));
//    connect(ui->btnLight, SIGNAL(clicked()), this, SLOT(dataCalcAndSend()));
    connect(ui->btnLED1, SIGNAL(clicked()), this, SLOT(btn_led1_clicked()));
    connect(ui->btnLED2, SIGNAL(clicked()), this, SLOT(btn_led2_clicked()));
    connect(ui->btnLED3, SIGNAL(clicked()), this, SLOT(btn_led3_clicked()));
    connect(ui->btnLED4, SIGNAL(clicked()), this, SLOT(btn_led4_clicked()));
    connect(this,SIGNAL(writeFinish()), this, SLOT(dataCalcAndSend()));
}

void MainWindow::sendMessage(){
    qDebug()<<"Before Sent";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out<<(quint16)0;
    out<<tr("HOST Connect!");
    out.device()->seek(0);
    out<<(quint16)(block.size()-sizeof(quint16));


    QTcpSocket *clientConnect = tcpServer->nextPendingConnection();
    connect(clientConnect, &QTcpSocket::disconnected, clientConnect,&QTcpSocket::deleteLater);

    clientConnect->write(block);
    clientConnect->disconnectFromHost();
    ui->lbConnStatus->setText("Client Connect Success");
}

void MainWindow::InitUI(){//Read the COM from regedit
    //串口
    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
    QSettings *settings = new QSettings( path, QSettings::NativeFormat);
    QStringList key = settings->allKeys();
    for(int i=0; i < (int)key.size(); i++)
    {
        m_listcomboName.append(GetCommName(i, "value"));
    }
    ui->cbSerial->addItems(m_listcomboName);

    //
}

void MainWindow::openPort(){//Open the Port and Ready to read from COM
    portName = ui->cbSerial->currentText();
    serialport->setPort(QSerialPortInfo(portName));
    qDebug()<<portName;
    if (serialport->open(QIODevice::ReadWrite)){
        QMessageBox box;
        box.setText("串口打开成功！");
        box.exec();
        serialport->setBaudRate(QSerialPort::Baud115200);
        serialport->setDataBits(QSerialPort::Data8);
        serialport->setParity(QSerialPort::NoParity);
        serialport->setStopBits(QSerialPort::OneStop);
        serialport->setFlowControl(QSerialPort::NoFlowControl);
        //senddata(sbuf);
        connect(serialport,SIGNAL(readyRead()), this, SLOT(readdata()));
    }else{
        QMessageBox box;
        box.setText("串口打开失败！");
        box.exec();
    }
}

QString MainWindow::GetCommName(int index, QString keyorvalue)//This method is search from the Internet
{
    QString commresult;
    if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),0,KEY_READ,&hKey)!=0)
    {
        QString error="Cannot open regedit!";//无法打开注册表时返回error
        return error;
    }
    QString keymessage;//键名
    QString message;
    QString valuemessage;//键值
    indexnum = index;//要读取键值的索引号
    keysize = sizeof(keyname);
    valuesize = sizeof(keyvalue);
    if (::RegEnumValue(hKey,indexnum,keyname,&keysize,0,&type,(BYTE*)keyvalue,&valuesize)==0)//列举键名和值
    {
        for (DWORD i = 0; i < keysize; i++)
        {
            message=keyname[i];
            keymessage.append(message);
        }// for(int i=0;i<=keysize;i++)    读取键名
        for (DWORD j = 0; j < valuesize; j++)
        {
            if(keyvalue[j] != 0x00)
            {
                valuemessage.append(keyvalue[j]);
            }
        }//for(int j=0;j<valuesize;j++) 读取键值
        if(keyorvalue == "key")
        {
            commresult = keymessage;
        }
        if(keyorvalue == "value")
        {
            commresult = valuemessage;
        }
    }
    else
    {
        commresult = "nokey";
    } //if(::RegEnumValue(hKey,indexnum,keyname,&keysize,0,&type,(BYTE*)keyvalue,&valuesize)==0) 列举键名和值
    ::RegCloseKey(hKey);//关闭注册表
    return commresult;
}

void MainWindow::btn_send_clicked(){
    sbuf = ui->teSendData->text();
    senddata(sbuf);
}

void MainWindow::btn_led2_clicked(){
    QMessageBox box;
    box.setText("麻蛋别点了,这个灯不能用！IO口被占了傻吊！");
    box.exec();
    sbuf ="L2";
    //senddata(sbuf);
}

void MainWindow::btn_led3_clicked(){
    QMessageBox box;
    box.setText("麻蛋别点了,这个灯不能用！IO口被占了傻吊！");
    box.exec();
    sbuf ="L3";
   // senddata(sbuf);
}

void MainWindow::btn_led4_clicked(){
    QMessageBox box;
    box.setText("麻蛋别点了,这个灯不能用！IO口被占了傻吊！");
    box.exec();
    sbuf ="L4";
   // senddata(sbuf);
}

void MainWindow::senddata(QString buf){
    qDebug()<<buf;
    serCount = serialport->write(buf.toLatin1());

    qDebug()<<serCount;
//    if (serCount == 8){
//        emit writeFinish();
//    }
}

void MainWindow::readdata(){
    QByteArray str = serialport->readAll();
    qDebug()<<str;
    ui->teRecData->insertPlainText(str);
    ui->teRecData->insertPlainText("\n");
}

void MainWindow::dataCalcAndSend(){

//    flagLED = !flagLED;
//    flagRelay = !flagRelay;
//    flagInfrared = !flagInfrared;
//    flagSmoke = !flagSmoke;
//    flagTemp = !flagTemp;
//    flagLight = !flagLight;
//    if (flagLED){countSerialSend++; bit3+=1; }else{countSerialSend--; bit3-=1;}
//    if (flagRelay){countSerialSend++; bit3+=2; }else{countSerialSend--; bit3-=2;}
//    if (flagInfrared){countSerialSend++; bit3+=4; }else{countSerialSend--;bit3-=4;}
//    if (flagSmoke){countSerialSend++; bit3+=8; }else{countSerialSend--;bit3-=8;}
//    if (flagTemp){countSerialSend++; bit3+=16; }else{countSerialSend--;bit3-=16;}
//    if (flagLight){countSerialSend++; bit3+=32; }else{countSerialSend--;bit3-=32;}
    //if(sbuf == "SerialPortOpen") exit;
    bit3+=32;
    char str = (char)bit3;
    sbuf += str;
    sbuf += "0000#";
    //connect(serialport,SIGNAL())
    threadSend.setMessage(sbuf, serialport);
    threadSend.start();
//    if (threadSend.isRunning()){
//        threadSend.stop();
//        threadSend.wait(2);
//        threadSend.start();
//    }else{
//        threadSend.start();
//    }
    //senddata(sbuf);
    bit3-=32;
    //qDebug()<<sbuf;
    //qDebug()<<countSerialSend;
    qDebug()<<"bit3="<<bit3;

}

void MainWindow::sendToSerial(){

}
MainWindow::~MainWindow()
{
    threadSend.stop();
    threadSend.wait();
//    threadSmoke.stop();
//    threadTemp.stop();
//    threadLight.stop();
//    threadSmoke.wait();
//    threadTemp.wait();
//    threadLight.wait();
    if(serialport->isOpen())
    {
        serialport->close();
    }
    delete ui;
}

void MainWindow::btn_led1_clicked(){
    flagLED = !flagLED;
    //qDebug()<<bit3;
    if (flagLED){countSerialSend++; bit3+=1; }else{countSerialSend--; bit3-=1;}
    sbuf = "*1";
    dataCalcAndSend();
//    if (flagLED){
//        sbuf = "*100000#";
//        qDebug()<<"LED关";
//        senddata(sbuf);
//        ui->btnLED1->setText(tr("关闭"));
//    } else{
//        sbuf ="*110000#";
//        qDebug()<<"LED开";
//        senddata(sbuf);
//        ui->btnLED1->setText(tr("开启"));
//    }

}

void MainWindow::on_btnRelay_clicked(){
    flagRelay = !flagRelay;
    if (flagRelay){countSerialSend++; bit3+=2; }else{countSerialSend--; bit3-=2;}
    sbuf = "*2";
    dataCalcAndSend();
//    if (flagRelay){
//        sbuf = "*200000#";
//        qDebug()<<"继电器关";
//        senddata(sbuf);
//        ui->btnRelay->setText(tr("关闭"));
//    } else{
//        sbuf ="*210000#";
//        qDebug()<<"继电器开";
//        senddata(sbuf);
//        ui->btnRelay->setText(tr("开启"));
//    }

}

void MainWindow::on_btnInfrared_clicked(){
    flagInfrared = !flagInfrared;
    if (flagInfrared){countSerialSend++; bit3+=4; }else{countSerialSend--;bit3-=4;}
    sbuf = "*3";
    dataCalcAndSend();
//    if (flagInfrared){
//        sbuf = "*300000#";
//        qDebug()<<"红外关";
//        senddata(sbuf);
//        ui->btnInfrared->setText(tr("关闭"));
//    } else{
//        sbuf ="*310000#";
//        qDebug()<<"红外开";
//        senddata(sbuf);
//        ui->btnInfrared->setText(tr("开启"));
//    }

}

void MainWindow::on_btnSmoke_clicked(){
    flagSmoke = !flagSmoke;
    if (flagSmoke){countSerialSend++; bit3+=8; }else{countSerialSend--;bit3-=8;}
    sbuf = "*4";
    dataCalcAndSend();
//    threadSmoke.setMessage("*410000#", serialport);
//    if (threadSmoke.isRunning()){
//        ui->btnSmoke->setText(tr("关闭"));
//        threadSmoke.stop();
//        ui->btnSmoke->setText(tr("开启"));
//    } else{
//        ui->btnSmoke->setText(tr("开启"));
//        threadSmoke.start();
//        ui->btnSmoke->setText(tr("关闭"));
//    }
    //sbuf ="#410000#";
    //senddata(sbuf);
}

void MainWindow::on_btnTemp_clicked(){
    flagTemp = !flagTemp;
    if (flagTemp){countSerialSend++; bit3+=16; }else{countSerialSend--;bit3-=16;}
    sbuf = "*5";
    dataCalcAndSend();
//    threadTemp.setMessage("*510000#", serialport);
//    if (threadTemp.isRunning()){
//        ui->btnTemp->setText(tr("关闭"));
//        threadTemp.stop();
//        ui->btnTemp->setText(tr("开启"));
//    } else{
//        ui->btnTemp->setText(tr("开启"));
//        threadTemp.start();
//        ui->btnTemp->setText(tr("关闭"));
//    }
   // sbuf ="#510000#";
   // senddata(sbuf);
}

void MainWindow::on_btnLight_clicked(){
    flagLight = !flagLight;
    if (flagLight){countSerialSend++; bit3+=32; }else{countSerialSend--;bit3-=32;}
    sbuf = "*6";
    dataCalcAndSend();
//    threadLight.setMessage("*610000#", serialport);
//    if (threadLight.isRunning()){
//        ui->btnLight->setText(tr("关闭"));
//        threadLight.stop();
//        ui->btnLight->setText(tr("开启"));
//    } else{
//        ui->btnLight->setText(tr("开启"));
//        threadLight.start();
//        ui->btnLight->setText(tr("关闭"));
//    }
    //sbuf ="#610000#";
    //senddata(sbuf);
}
