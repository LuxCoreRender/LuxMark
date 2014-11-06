/********************************************************************************
** Form generated from reading UI file 'resultdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESULTDIALOG_H
#define UI_RESULTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ResultDialog
{
public:
    QFormLayout *formLayout;
    QLabel *modeLabelDesc;
    QLabel *modeLabel;
    QLabel *sceneLabelDesc;
    QLabel *sceneLabel;
    QLabel *deviceLabel;
    QListView *deviceListView;
    QLabel *sampleSecLabel;
    QLCDNumber *resultLCD;
    QWidget *widget;
    QGridLayout *gridLayout;
    QPushButton *okButton;
    QPushButton *submitButton;
    QLabel *sceneValidationLabel;
    QLabel *sceneValidation;
    QLabel *imageValidationLabel;
    QLabel *imageValidation;
    QLabel *donationLabel;

    void setupUi(QDialog *ResultDialog)
    {
        if (ResultDialog->objectName().isEmpty())
            ResultDialog->setObjectName(QString::fromUtf8("ResultDialog"));
        ResultDialog->resize(550, 600);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/resources/SLG_luxball_sppm_small.png"), QSize(), QIcon::Normal, QIcon::Off);
        ResultDialog->setWindowIcon(icon);
        ResultDialog->setModal(true);
        formLayout = new QFormLayout(ResultDialog);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
        formLayout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        formLayout->setFormAlignment(Qt::AlignHCenter|Qt::AlignTop);
        formLayout->setHorizontalSpacing(9);
        formLayout->setVerticalSpacing(9);
        modeLabelDesc = new QLabel(ResultDialog);
        modeLabelDesc->setObjectName(QString::fromUtf8("modeLabelDesc"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(modeLabelDesc->sizePolicy().hasHeightForWidth());
        modeLabelDesc->setSizePolicy(sizePolicy);
        modeLabelDesc->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::LabelRole, modeLabelDesc);

        modeLabel = new QLabel(ResultDialog);
        modeLabel->setObjectName(QString::fromUtf8("modeLabel"));

        formLayout->setWidget(0, QFormLayout::FieldRole, modeLabel);

        sceneLabelDesc = new QLabel(ResultDialog);
        sceneLabelDesc->setObjectName(QString::fromUtf8("sceneLabelDesc"));
        sceneLabelDesc->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(1, QFormLayout::LabelRole, sceneLabelDesc);

        sceneLabel = new QLabel(ResultDialog);
        sceneLabel->setObjectName(QString::fromUtf8("sceneLabel"));

        formLayout->setWidget(1, QFormLayout::FieldRole, sceneLabel);

        deviceLabel = new QLabel(ResultDialog);
        deviceLabel->setObjectName(QString::fromUtf8("deviceLabel"));
        deviceLabel->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(5, QFormLayout::SpanningRole, deviceLabel);

        deviceListView = new QListView(ResultDialog);
        deviceListView->setObjectName(QString::fromUtf8("deviceListView"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(2);
        sizePolicy1.setHeightForWidth(deviceListView->sizePolicy().hasHeightForWidth());
        deviceListView->setSizePolicy(sizePolicy1);
        deviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        formLayout->setWidget(6, QFormLayout::SpanningRole, deviceListView);

        sampleSecLabel = new QLabel(ResultDialog);
        sampleSecLabel->setObjectName(QString::fromUtf8("sampleSecLabel"));
        sampleSecLabel->setAlignment(Qt::AlignCenter);

        formLayout->setWidget(7, QFormLayout::SpanningRole, sampleSecLabel);

        resultLCD = new QLCDNumber(ResultDialog);
        resultLCD->setObjectName(QString::fromUtf8("resultLCD"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(resultLCD->sizePolicy().hasHeightForWidth());
        resultLCD->setSizePolicy(sizePolicy2);
        resultLCD->setNumDigits(8);
        resultLCD->setProperty("value", QVariant(123456));

        formLayout->setWidget(8, QFormLayout::SpanningRole, resultLCD);

        widget = new QWidget(ResultDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout = new QGridLayout(widget);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        okButton = new QPushButton(widget);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(okButton->sizePolicy().hasHeightForWidth());
        okButton->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(okButton, 0, 0, 1, 1);

        submitButton = new QPushButton(widget);
        submitButton->setObjectName(QString::fromUtf8("submitButton"));
        sizePolicy3.setHeightForWidth(submitButton->sizePolicy().hasHeightForWidth());
        submitButton->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(submitButton, 0, 1, 1, 1);


        formLayout->setWidget(10, QFormLayout::SpanningRole, widget);

        sceneValidationLabel = new QLabel(ResultDialog);
        sceneValidationLabel->setObjectName(QString::fromUtf8("sceneValidationLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, sceneValidationLabel);

        sceneValidation = new QLabel(ResultDialog);
        sceneValidation->setObjectName(QString::fromUtf8("sceneValidation"));

        formLayout->setWidget(2, QFormLayout::FieldRole, sceneValidation);

        imageValidationLabel = new QLabel(ResultDialog);
        imageValidationLabel->setObjectName(QString::fromUtf8("imageValidationLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, imageValidationLabel);

        imageValidation = new QLabel(ResultDialog);
        imageValidation->setObjectName(QString::fromUtf8("imageValidation"));

        formLayout->setWidget(3, QFormLayout::FieldRole, imageValidation);

        donationLabel = new QLabel(ResultDialog);
        donationLabel->setObjectName(QString::fromUtf8("donationLabel"));
        QFont font;
        font.setPointSize(10);
        donationLabel->setFont(font);
        donationLabel->setLineWidth(1);
        donationLabel->setTextFormat(Qt::RichText);
        donationLabel->setWordWrap(true);
        donationLabel->setOpenExternalLinks(true);

        formLayout->setWidget(9, QFormLayout::SpanningRole, donationLabel);


        retranslateUi(ResultDialog);
        QObject::connect(okButton, SIGNAL(released()), ResultDialog, SLOT(close()));
        QObject::connect(submitButton, SIGNAL(released()), ResultDialog, SLOT(submitResult()));

        QMetaObject::connectSlotsByName(ResultDialog);
    } // setupUi

    void retranslateUi(QDialog *ResultDialog)
    {
        ResultDialog->setWindowTitle(QApplication::translate("ResultDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        modeLabelDesc->setText(QApplication::translate("ResultDialog", "Mode:", 0, QApplication::UnicodeUTF8));
        modeLabel->setText(QString());
        sceneLabelDesc->setText(QApplication::translate("ResultDialog", "Scene name:", 0, QApplication::UnicodeUTF8));
        sceneLabel->setText(QString());
        deviceLabel->setText(QApplication::translate("ResultDialog", "Devices:", 0, QApplication::UnicodeUTF8));
        sampleSecLabel->setText(QApplication::translate("ResultDialog", "Result:", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("ResultDialog", "&Ok", 0, QApplication::UnicodeUTF8));
        submitButton->setText(QApplication::translate("ResultDialog", "&Submit result", 0, QApplication::UnicodeUTF8));
        sceneValidationLabel->setText(QApplication::translate("ResultDialog", "Scene file validation:", 0, QApplication::UnicodeUTF8));
        sceneValidation->setText(QString());
        imageValidationLabel->setText(QApplication::translate("ResultDialog", "Image validation:", 0, QApplication::UnicodeUTF8));
        imageValidation->setText(QString());
        donationLabel->setText(QApplication::translate("ResultDialog", "<html><head/><body><p align=\"justify\">LuxRender is now part of the <a href=\"http://sfconservancy.org\"><span style=\" text-decoration: underline; color:#0000ff;\">Software Freedom Conservancy</span></a>, which allows us to receive donations to foster the development and cover the expenses of the LuxRender project. For each donation you'll do, a small amount will go to the Conservancy so that it can benefit all the member projects, and the rest will be made available to LuxRender.</p><p align=\"justify\">In the United States, you can benefit from tax deductions according to the Conservancy 501(c)(3) not for profit organization status.</p><p align=\"center\">You can donate at <a href=\"http://www.luxrender.net/en_GB/donate\"><span style=\" text-decoration: underline; color:#0000ff;\">here</span></a><a href=\"http://www.luxrender.net/en_GB/donate\"><span style=\" color:#0000ff;\">.</span></a></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ResultDialog: public Ui_ResultDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESULTDIALOG_H
