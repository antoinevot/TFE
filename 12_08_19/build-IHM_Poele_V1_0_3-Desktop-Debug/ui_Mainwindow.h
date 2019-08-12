/********************************************************************************
** Form generated from reading UI file 'Mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QComboBox *PORT_SETTING_comboBox;
    QLabel *CONFIRM_SERIAL_CONNEXION_label;
    QLabel *PORT_label;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *temperatureLabel;
    QLCDNumber *potardLcdNumber;
    QLabel *DEG_temp_label;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout_2;
    QSpinBox *timeMotorOff_spinBox;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *timeMotorOn_spinBox;
    QLabel *SEC_time_off_label;
    QPushButton *send_data_pushButton;
    QLabel *label_3;
    QPushButton *stop_process_pushButton;
    QLabel *SEC_time_on_label;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(455, 411);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        PORT_SETTING_comboBox = new QComboBox(centralWidget);
        PORT_SETTING_comboBox->setObjectName(QStringLiteral("PORT_SETTING_comboBox"));
        PORT_SETTING_comboBox->setGeometry(QRect(190, 0, 69, 22));
        CONFIRM_SERIAL_CONNEXION_label = new QLabel(centralWidget);
        CONFIRM_SERIAL_CONNEXION_label->setObjectName(QStringLiteral("CONFIRM_SERIAL_CONNEXION_label"));
        CONFIRM_SERIAL_CONNEXION_label->setGeometry(QRect(260, 0, 101, 20));
        PORT_label = new QLabel(centralWidget);
        PORT_label->setObjectName(QStringLiteral("PORT_label"));
        PORT_label->setGeometry(QRect(56, 0, 131, 20));
        QFont font;
        font.setFamily(QStringLiteral("Franklin Gothic Book"));
        font.setPointSize(12);
        PORT_label->setFont(font);
        horizontalLayoutWidget = new QWidget(centralWidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(50, 30, 354, 121));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        temperatureLabel = new QLabel(horizontalLayoutWidget);
        temperatureLabel->setObjectName(QStringLiteral("temperatureLabel"));
        QFont font1;
        font1.setFamily(QStringLiteral("Franklin Gothic Book"));
        font1.setPointSize(20);
        temperatureLabel->setFont(font1);
        temperatureLabel->setLayoutDirection(Qt::LeftToRight);

        horizontalLayout->addWidget(temperatureLabel, 0, Qt::AlignHCenter);

        potardLcdNumber = new QLCDNumber(horizontalLayoutWidget);
        potardLcdNumber->setObjectName(QStringLiteral("potardLcdNumber"));

        horizontalLayout->addWidget(potardLcdNumber);

        DEG_temp_label = new QLabel(horizontalLayoutWidget);
        DEG_temp_label->setObjectName(QStringLiteral("DEG_temp_label"));
        QFont font2;
        font2.setFamily(QStringLiteral("Franklin Gothic Book"));
        font2.setPointSize(28);
        DEG_temp_label->setFont(font2);

        horizontalLayout->addWidget(DEG_temp_label);

        gridLayoutWidget = new QWidget(centralWidget);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(50, 160, 331, 178));
        gridLayout_2 = new QGridLayout(gridLayoutWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        timeMotorOff_spinBox = new QSpinBox(gridLayoutWidget);
        timeMotorOff_spinBox->setObjectName(QStringLiteral("timeMotorOff_spinBox"));
        timeMotorOff_spinBox->setMinimum(5);
        timeMotorOff_spinBox->setMaximum(255);
        timeMotorOff_spinBox->setValue(5);

        gridLayout_2->addWidget(timeMotorOff_spinBox, 2, 1, 1, 1);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 2, 0, 1, 1);

        timeMotorOn_spinBox = new QSpinBox(gridLayoutWidget);
        timeMotorOn_spinBox->setObjectName(QStringLiteral("timeMotorOn_spinBox"));
        timeMotorOn_spinBox->setMinimum(0);
        timeMotorOn_spinBox->setMaximum(20);
        timeMotorOn_spinBox->setValue(1);

        gridLayout_2->addWidget(timeMotorOn_spinBox, 1, 1, 1, 1);

        SEC_time_off_label = new QLabel(gridLayoutWidget);
        SEC_time_off_label->setObjectName(QStringLiteral("SEC_time_off_label"));

        gridLayout_2->addWidget(SEC_time_off_label, 2, 2, 1, 1);

        send_data_pushButton = new QPushButton(gridLayoutWidget);
        send_data_pushButton->setObjectName(QStringLiteral("send_data_pushButton"));
        send_data_pushButton->setMinimumSize(QSize(0, 50));

        gridLayout_2->addWidget(send_data_pushButton, 3, 0, 1, 3);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        QFont font3;
        font3.setFamily(QStringLiteral("Franklin Gothic Book"));
        font3.setPointSize(16);
        label_3->setFont(font3);
        label_3->setTextFormat(Qt::PlainText);
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_3, 0, 0, 1, 3);

        stop_process_pushButton = new QPushButton(gridLayoutWidget);
        stop_process_pushButton->setObjectName(QStringLiteral("stop_process_pushButton"));

        gridLayout_2->addWidget(stop_process_pushButton, 4, 0, 1, 3);

        SEC_time_on_label = new QLabel(gridLayoutWidget);
        SEC_time_on_label->setObjectName(QStringLiteral("SEC_time_on_label"));

        gridLayout_2->addWidget(SEC_time_on_label, 1, 2, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 455, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        PORT_SETTING_comboBox->clear();
        PORT_SETTING_comboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "ttyUSB0", Q_NULLPTR)
         << QApplication::translate("MainWindow", "ttyACM0", Q_NULLPTR)
        );
        CONFIRM_SERIAL_CONNEXION_label->setText(QApplication::translate("MainWindow", "(connected)", Q_NULLPTR));
        PORT_label->setText(QApplication::translate("MainWindow", "PORT (arduino) :", Q_NULLPTR));
        temperatureLabel->setText(QApplication::translate("MainWindow", "TEMPERATURE :", Q_NULLPTR));
        DEG_temp_label->setText(QApplication::translate("MainWindow", "\302\260C", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Duree moteur : ON", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Duree moteur : OFF", Q_NULLPTR));
        SEC_time_off_label->setText(QApplication::translate("MainWindow", "[sec]", Q_NULLPTR));
        send_data_pushButton->setText(QApplication::translate("MainWindow", "envoyer", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "Param\303\250trage d\303\251bit pellet", Q_NULLPTR));
        stop_process_pushButton->setText(QApplication::translate("MainWindow", "STOP", Q_NULLPTR));
        SEC_time_on_label->setText(QApplication::translate("MainWindow", "[sec]", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
