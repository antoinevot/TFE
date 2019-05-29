#include "Mainwindow.h"
#include "ui_Mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->CONFIRM_SERIAL_CONNEXION_label->hide();
    ui->potardLcdNumber->display("0000");
    stringDataSerial = "";
    setWindowTitle("Thermometre (sur base du module MAX31855)");
    BP_RAZ = new QPushButton("&RAZ", this);
    ui->gridLayout_2->addWidget(BP_RAZ,1,3);
    fileSavingTemp.setFileName("savingTemp.txt");
    fileSavingTime.setFileName("savingTime.txt");
    //recup info port arduino
    QSerialPortInfo infoPort(ui->PORT_SETTING_comboBox->currentText());
    //creation d'un objet de type "SerialPort"
    arduino = new QSerialPort(infoPort);
    connect(ui->PORT_SETTING_comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(upDateSerialPortName()));
    //verification si on a bien acces au buffer de communication dudit port(COMx)
    if(arduino->open(QIODevice::ReadWrite)){
        dataToSend = "";
        qDebug() << "Port ouvert : " << infoPort.portName();
        qDebug() << " description : " << infoPort.description();
        ui->CONFIRM_SERIAL_CONNEXION_label->show();
        connect(arduino,SIGNAL(readyRead()),this,SLOT(receiveSerialData()));
        connect(ui->send_data_pushButton,SIGNAL(clicked()),this,SLOT(sendSerialData()));
        connect(BP_RAZ,SIGNAL(clicked()),this,SLOT(razDebitMotor()));
#if 0
        if(fileSavingTemp.open(QFile::ReadWrite | QFile::Append |QFile::Text) ){
        }
        else{
            QMessageBox::critical(this,"error OPENING FILE", "le fichier de suvegarde des températures ou du temps ne s'est pas ouvert\nFichier inexistant");
            qDebug() << "erreur: impossible d'ouvir le fichier de sauvegarde des temperatures ou temps écoulé";
        }
#endif
    }
    else{
        QMessageBox::critical(this,"error OPENING COMMUNICATION PORT","erreur: aucune communication avec port " + ui->PORT_SETTING_comboBox->currentText());
        qDebug() << "erreur: aucune communication avec port " << ui->PORT_SETTING_comboBox->currentText();
    }
}

void MainWindow::receiveSerialData()
{
    dataSerial = arduino->readAll();
    //stringDataSerial += stringDataSerial;
    stringDataSerial += QString::fromStdString(dataSerial.toStdString());
    QStringList listDataArduino = stringDataSerial.split('\n');
    if(!(listDataArduino.length() < 3)){
        QString dataParse = listDataArduino[1];
        dataParse.append('\n');
        qDebug() << dataParse;
        QString valThrm = "";
        QString tempsEcoule = "";
        int i = 0;
        while(dataParse[i]  != ','){
            valThrm.append(dataParse[i]);
            i++;
        }
        i++;
        qDebug() << "valthrm = ";
        qDebug() << valThrm;
        while(dataParse[i] != '\n'){
            tempsEcoule.append(dataParse[i]);
            i++;
        }
        qDebug() << "temps Ecoule";
        qDebug() << tempsEcoule;
        ui->potardLcdNumber->display(valThrm);
        fileSavingTemp.open(QFile::ReadWrite | QFile::Append |QFile::Text);
        QTextStream out(&fileSavingTemp);
        out << valThrm.append('\n');
        fileSavingTemp.close();
        fileSavingTime.open(QFile::ReadWrite | QFile::Append |QFile::Text);
        QTextStream outsec(&fileSavingTime);
        outsec << tempsEcoule.append('\n');
        fileSavingTime.close();
        stringDataSerial ="";
    }
}

void MainWindow::sendSerialData()
{
    QDataStream stream(&dataToSend, QIODevice::WriteOnly);
    int motorOn = 0;
    int motorOff = 0;

    motorOn = ui->timeMotorOn_spinBox->value();
    motorOff = ui->timeMotorOff_spinBox->value();
    qDebug() << "spinBox on : " << motorOn;
    qDebug() << "spinBox off : " << motorOff;

    dataToSend.append('O');
    if(motorOn < 10){
        dataToSend.append(QByteArray::number(0));
        dataToSend.append(QByteArray::number(0));
    }
    else if(motorOn < 100){
        dataToSend.append(QByteArray::number(0));
    }
    dataToSend.append(QByteArray::number(motorOn));

    dataToSend.append('C');
    if(motorOff < 10){
        dataToSend.append(QByteArray::number(0));
        dataToSend.append(QByteArray::number(0));
    }
    else if(motorOff < 100){
        dataToSend.append(QByteArray::number(0));
    }
    dataToSend.append(QByteArray::number(motorOff));

    qDebug() << "Data à Envoyer : ";
    qDebug() << dataToSend;
    arduino->write(dataToSend);
    dataToSend = "";
}

void MainWindow::razDebitMotor()
{
    QDataStream stream(&dataToSend, QIODevice::WriteOnly);
    int motorOn = 0;
    int motorOff = 0;

    ui->timeMotorOn_spinBox->setValue(motorOn);  // On met à jour la valeur de la spinBox
    motorOff = ui->timeMotorOff_spinBox->value(); // On recup l'info pour envoyer les 2 parametre (le µC lit les val par 2 toujours..)
    qDebug() << "spinBox on : " << motorOn;
    qDebug() << "spinBox off : " << motorOff;

    dataToSend.append('O'); //time ON
    dataToSend.append(QByteArray::number(0)); // Padding
    dataToSend.append(QByteArray::number(0)); // Padding
    dataToSend.append(QByteArray::number(motorOn));

    dataToSend.append('C'); //time OFF
    if(motorOff < 10){
        dataToSend.append(QByteArray::number(0));
        dataToSend.append(QByteArray::number(0));
    }
    else if(motorOff < 100){
        dataToSend.append(QByteArray::number(0));
    }
    dataToSend.append(QByteArray::number(motorOff));
    qDebug() << "Data à Envoyer : ";
    qDebug() << dataToSend;

    arduino->write(dataToSend);
    dataToSend = "";
}

void MainWindow::upDateSerialPortName()
{
    QSerialPortInfo infoPort(ui->PORT_SETTING_comboBox->currentText());
    arduino = new QSerialPort(infoPort);
        if(arduino->open(QIODevice::ReadWrite)){
        dataToSend = "";
        qDebug() << "Port ouvert : " << infoPort.portName();
        qDebug() << " description : " << infoPort.description();
        ui->CONFIRM_SERIAL_CONNEXION_label->show();
        connect(arduino,SIGNAL(readyRead()),this,SLOT(receiveSerialData()));
        connect(ui->send_data_pushButton,SIGNAL(clicked()),this,SLOT(sendSerialData()));
        connect(BP_RAZ,SIGNAL(clicked()),this,SLOT(razDebitMotor()));
#if 0
        if(fileSavingTemp.open(QFile::ReadWrite | QFile::Append |QFile::Text) ){
        }
        else{
            QMessageBox::critical(this,"error OPENING FILE", "le fichier de suvegarde des températures ou du temps ne s'est pas ouvert\nFichier inexistant");
            qDebug() << "erreur: impossible d'ouvir le fichier de sauvegarde des temperatures ou temps écoulé";
        }
#endif
    }
    else{
        QMessageBox::critical(this,"error OPENING COMMUNICATION PORT","erreur: aucune communication avec port " + ui->PORT_SETTING_comboBox->currentText());
        qDebug() << "erreur: aucune communication avec port " << ui->PORT_SETTING_comboBox->currentText();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
    if(arduino->isOpen()){
        arduino->close();
    }
}
