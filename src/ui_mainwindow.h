/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Jan 17 12:25:46 2014
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Quit;
    QAction *action_About;
    QAction *action_OpenCL_GPUs;
    QAction *action_LuxBall_HDR;
    QAction *action_OpenCL_CPUs_GPUs;
    QAction *action_Pause;
    QAction *action_OpenCL_CPUs;
    QAction *action_Sala;
    QAction *action_OpenCL_Custom;
    QAction *action_Room;
    QAction *action_Hybrid_GPUs;
    QAction *action_Hybrid_Custom;
    QAction *action_Path;
    QAction *action_StressTest_OpenCL_GPUs;
    QAction *action_StressTest_OpenCL_CPUs_GPUs;
    QAction *action_StressTest_OpenCL_CPUs;
    QAction *action_LuxVR;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter_2;
    QSplitter *splitter;
    QGraphicsView *RenderView;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *hardwareDevicesLabel;
    QTreeView *HardwareView;
    QTextEdit *LogView;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menu_Help;
    QMenu *menu_Mode;
    QMenu *menu_Scene;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 768);
        MainWindow->setMinimumSize(QSize(128, 128));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/resources/SLG_luxball_sppm_small.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        action_About = new QAction(MainWindow);
        action_About->setObjectName(QString::fromUtf8("action_About"));
        action_OpenCL_GPUs = new QAction(MainWindow);
        action_OpenCL_GPUs->setObjectName(QString::fromUtf8("action_OpenCL_GPUs"));
        action_OpenCL_GPUs->setCheckable(true);
        action_LuxBall_HDR = new QAction(MainWindow);
        action_LuxBall_HDR->setObjectName(QString::fromUtf8("action_LuxBall_HDR"));
        action_LuxBall_HDR->setCheckable(true);
        action_OpenCL_CPUs_GPUs = new QAction(MainWindow);
        action_OpenCL_CPUs_GPUs->setObjectName(QString::fromUtf8("action_OpenCL_CPUs_GPUs"));
        action_OpenCL_CPUs_GPUs->setCheckable(true);
        action_Pause = new QAction(MainWindow);
        action_Pause->setObjectName(QString::fromUtf8("action_Pause"));
        action_Pause->setCheckable(true);
        action_OpenCL_CPUs = new QAction(MainWindow);
        action_OpenCL_CPUs->setObjectName(QString::fromUtf8("action_OpenCL_CPUs"));
        action_OpenCL_CPUs->setCheckable(true);
        action_Sala = new QAction(MainWindow);
        action_Sala->setObjectName(QString::fromUtf8("action_Sala"));
        action_Sala->setCheckable(true);
        action_OpenCL_Custom = new QAction(MainWindow);
        action_OpenCL_Custom->setObjectName(QString::fromUtf8("action_OpenCL_Custom"));
        action_OpenCL_Custom->setCheckable(true);
        action_Room = new QAction(MainWindow);
        action_Room->setObjectName(QString::fromUtf8("action_Room"));
        action_Room->setCheckable(true);
        action_Hybrid_GPUs = new QAction(MainWindow);
        action_Hybrid_GPUs->setObjectName(QString::fromUtf8("action_Hybrid_GPUs"));
        action_Hybrid_Custom = new QAction(MainWindow);
        action_Hybrid_Custom->setObjectName(QString::fromUtf8("action_Hybrid_Custom"));
        action_Path = new QAction(MainWindow);
        action_Path->setObjectName(QString::fromUtf8("action_Path"));
        action_StressTest_OpenCL_GPUs = new QAction(MainWindow);
        action_StressTest_OpenCL_GPUs->setObjectName(QString::fromUtf8("action_StressTest_OpenCL_GPUs"));
        action_StressTest_OpenCL_CPUs_GPUs = new QAction(MainWindow);
        action_StressTest_OpenCL_CPUs_GPUs->setObjectName(QString::fromUtf8("action_StressTest_OpenCL_CPUs_GPUs"));
        action_StressTest_OpenCL_CPUs = new QAction(MainWindow);
        action_StressTest_OpenCL_CPUs->setObjectName(QString::fromUtf8("action_StressTest_OpenCL_CPUs"));
        action_LuxVR = new QAction(MainWindow);
        action_LuxVR->setObjectName(QString::fromUtf8("action_LuxVR"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setMinimumSize(QSize(160, 120));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        splitter_2 = new QSplitter(centralwidget);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        RenderView = new QGraphicsView(splitter);
        RenderView->setObjectName(QString::fromUtf8("RenderView"));
        splitter->addWidget(RenderView);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        hardwareDevicesLabel = new QLabel(layoutWidget);
        hardwareDevicesLabel->setObjectName(QString::fromUtf8("hardwareDevicesLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(hardwareDevicesLabel->sizePolicy().hasHeightForWidth());
        hardwareDevicesLabel->setSizePolicy(sizePolicy);
        hardwareDevicesLabel->setFrameShape(QFrame::NoFrame);
        hardwareDevicesLabel->setFrameShadow(QFrame::Sunken);
        hardwareDevicesLabel->setAlignment(Qt::AlignCenter);
        hardwareDevicesLabel->setTextInteractionFlags(Qt::NoTextInteraction);

        verticalLayout->addWidget(hardwareDevicesLabel);

        HardwareView = new QTreeView(layoutWidget);
        HardwareView->setObjectName(QString::fromUtf8("HardwareView"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(HardwareView->sizePolicy().hasHeightForWidth());
        HardwareView->setSizePolicy(sizePolicy1);
        HardwareView->setFrameShape(QFrame::Box);
        HardwareView->setAnimated(true);
        HardwareView->header()->setVisible(false);

        verticalLayout->addWidget(HardwareView);

        splitter->addWidget(layoutWidget);
        splitter_2->addWidget(splitter);
        LogView = new QTextEdit(splitter_2);
        LogView->setObjectName(QString::fromUtf8("LogView"));
        LogView->setReadOnly(true);
        splitter_2->addWidget(LogView);

        verticalLayout_2->addWidget(splitter_2);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1024, 25));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Help = new QMenu(menubar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        menu_Mode = new QMenu(menubar);
        menu_Mode->setObjectName(QString::fromUtf8("menu_Mode"));
        menu_Scene = new QMenu(menubar);
        menu_Scene->setObjectName(QString::fromUtf8("menu_Scene"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_Mode->menuAction());
        menubar->addAction(menu_Scene->menuAction());
        menubar->addAction(menu_Help->menuAction());
        menu_File->addAction(action_Quit);
        menu_Help->addAction(action_About);
        menu_Mode->addAction(action_OpenCL_GPUs);
        menu_Mode->addAction(action_OpenCL_CPUs_GPUs);
        menu_Mode->addAction(action_OpenCL_CPUs);
        menu_Mode->addAction(action_OpenCL_Custom);
        menu_Mode->addAction(action_Hybrid_GPUs);
        menu_Mode->addAction(action_Hybrid_Custom);
        menu_Mode->addAction(action_Path);
        menu_Mode->addSeparator();
        menu_Mode->addAction(action_StressTest_OpenCL_GPUs);
        menu_Mode->addAction(action_StressTest_OpenCL_CPUs_GPUs);
        menu_Mode->addAction(action_StressTest_OpenCL_CPUs);
        menu_Mode->addSeparator();
        menu_Mode->addAction(action_LuxVR);
        menu_Mode->addSeparator();
        menu_Mode->addAction(action_Pause);
        menu_Scene->addAction(action_Room);
        menu_Scene->addAction(action_Sala);
        menu_Scene->addAction(action_LuxBall_HDR);

        retranslateUi(MainWindow);
        QObject::connect(action_Quit, SIGNAL(triggered()), MainWindow, SLOT(exitApp()));
        QObject::connect(action_About, SIGNAL(triggered()), MainWindow, SLOT(showAbout()));
        QObject::connect(action_LuxBall_HDR, SIGNAL(triggered()), MainWindow, SLOT(setLuxBallHDRScene()));
        QObject::connect(action_OpenCL_CPUs_GPUs, SIGNAL(triggered()), MainWindow, SLOT(setMode_BENCHMARK_OCL_CPUGPU()));
        QObject::connect(action_OpenCL_GPUs, SIGNAL(triggered()), MainWindow, SLOT(setMode_BENCHMARK_OCL_GPU()));
        QObject::connect(action_Pause, SIGNAL(triggered()), MainWindow, SLOT(setMode_PAUSE()));
        QObject::connect(action_OpenCL_CPUs, SIGNAL(triggered()), MainWindow, SLOT(setMode_BENCHMARK_OCL_CPU()));
        QObject::connect(action_Sala, SIGNAL(triggered()), MainWindow, SLOT(setSalaScene()));
        QObject::connect(action_OpenCL_Custom, SIGNAL(triggered()), MainWindow, SLOT(setMode_BENCHMARK_OCL_CUSTOM()));
        QObject::connect(action_Room, SIGNAL(triggered()), MainWindow, SLOT(setRoomScene()));
        QObject::connect(action_Hybrid_GPUs, SIGNAL(triggered()), MainWindow, SLOT(setMode_BENCHMARK_HYBRID_GPU()));
        QObject::connect(action_Hybrid_Custom, SIGNAL(triggered()), MainWindow, SLOT(setMode_BENCHMARK_HYBRID_CUSTOM()));
        QObject::connect(action_Path, SIGNAL(triggered()), MainWindow, SLOT(setMode_BENCHMARK_NATIVE_PATH()));
        QObject::connect(action_StressTest_OpenCL_GPUs, SIGNAL(triggered()), MainWindow, SLOT(setMode_STRESSTEST_OCL_GPU()));
        QObject::connect(action_StressTest_OpenCL_CPUs_GPUs, SIGNAL(triggered()), MainWindow, SLOT(setMode_STRESSTEST_OCL_CPUGPU()));
        QObject::connect(action_StressTest_OpenCL_CPUs, SIGNAL(triggered()), MainWindow, SLOT(setMode_STRESSTEST_OCL_CPU()));
        QObject::connect(action_LuxVR, SIGNAL(triggered()), MainWindow, SLOT(setMode_DEMO_LUXVR()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "LuxMark", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0, QApplication::UnicodeUTF8));
        action_Quit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        action_About->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        action_OpenCL_GPUs->setText(QApplication::translate("MainWindow", "Benchmark (OpenCL-only GPUs-only)", 0, QApplication::UnicodeUTF8));
        action_LuxBall_HDR->setText(QApplication::translate("MainWindow", "LuxBall &HDR (Simple Benchmark: 262K triangles)", 0, QApplication::UnicodeUTF8));
        action_LuxBall_HDR->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        action_OpenCL_CPUs_GPUs->setText(QApplication::translate("MainWindow", "Benchmark (OpenCL-only CPUs + GPUs)", 0, QApplication::UnicodeUTF8));
        action_Pause->setText(QApplication::translate("MainWindow", "&Pause", 0, QApplication::UnicodeUTF8));
        action_OpenCL_CPUs->setText(QApplication::translate("MainWindow", "Benchmark (OpenCL-only CPUs-only)", 0, QApplication::UnicodeUTF8));
        action_Sala->setText(QApplication::translate("MainWindow", "&Sala (Medium Benchmark: 488K triangles)", 0, QApplication::UnicodeUTF8));
        action_Sala->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0, QApplication::UnicodeUTF8));
        action_OpenCL_Custom->setText(QApplication::translate("MainWindow", "Benchmark (OpenCL-only selected devices only)", 0, QApplication::UnicodeUTF8));
        action_Room->setText(QApplication::translate("MainWindow", "&Room (Complex Benchmark: 2016K triangles)", 0, QApplication::UnicodeUTF8));
        action_Room->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", 0, QApplication::UnicodeUTF8));
        action_Hybrid_GPUs->setText(QApplication::translate("MainWindow", "Benchmark (Hybrid C++/OpenCL GPUs-only)", 0, QApplication::UnicodeUTF8));
        action_Hybrid_Custom->setText(QApplication::translate("MainWindow", "Benchmark (Hybrid C++/OpenCL selected devices only)", 0, QApplication::UnicodeUTF8));
        action_Path->setText(QApplication::translate("MainWindow", "Benchmark (Native C++ only)", 0, QApplication::UnicodeUTF8));
        action_StressTest_OpenCL_GPUs->setText(QApplication::translate("MainWindow", "Stress Test (OpenCL-only GPUs-only)", 0, QApplication::UnicodeUTF8));
        action_StressTest_OpenCL_CPUs_GPUs->setText(QApplication::translate("MainWindow", "Stress Test (OpenCL-only CPUs+GPUs)", 0, QApplication::UnicodeUTF8));
        action_StressTest_OpenCL_CPUs->setText(QApplication::translate("MainWindow", "Stress Test (OpenCL-only CPUs)", 0, QApplication::UnicodeUTF8));
        action_LuxVR->setText(QApplication::translate("MainWindow", "LuxVR (OpenCL-only GPUs-only)", 0, QApplication::UnicodeUTF8));
        hardwareDevicesLabel->setText(QApplication::translate("MainWindow", "Hardware Devices", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
        menu_Mode->setTitle(QApplication::translate("MainWindow", "&Mode", 0, QApplication::UnicodeUTF8));
        menu_Scene->setTitle(QApplication::translate("MainWindow", "&Scene", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
