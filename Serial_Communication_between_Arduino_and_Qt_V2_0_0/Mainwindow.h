#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();
private slots:
    void receiveSerialData();
    void sendSerialData();
    void razDebitMotor();
    void upDateSerialPortName();
private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    QByteArray dataSerial;
    QString stringDataSerial;
    QFile fileSavingTemp;
    QFile fileSavingTime;
    QByteArray dataToSend;
    QPushButton *BP_RAZ;
};

#endif // MAINWINDOW_H
