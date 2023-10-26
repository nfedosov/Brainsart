#include "kalmanparamchoice.h"
#include <QLineEdit>
#include <QPushButton.h>
#include <QDebug>
#include "qboxlayout.h"
#include "qlabel.h"
#include "whitekf.h"
#include <QString>

KalmanParamChoice::KalmanParamChoice(WhiteKF* kf, QWidget *parent)
    : QWidget{parent}
{

    this->kf = kf;

    this->setFixedSize(150, 250);

    this->kf->q = this->kf->default_q;
    this->kf->r = this->kf->default_r;


    QVBoxLayout *commonlay = new QVBoxLayout(this);

    QWidget *freq_empty = new QWidget(this);
    QLabel *freq_label = new QLabel("Central frequeny");
    freqChoice = new QLineEdit(QString::number(this->kf->freq), this);
    //freqChoice->move(50,20);

    QWidget *A_empty = new QWidget(this);
    QLabel *A_label = new QLabel("Damping factor");
    AChoice = new QLineEdit(QString::number(this->kf->A), this);
    //AChoice->move(50,70);

    //QLabel *A_label = new QLabel("Damping factor");
    //srateChoice = new QLineEdit(QString::number(this->kf->srate), this);
    //srateChoice->move(50,120);
    QWidget *r_empty = new QWidget(this);
    QLabel *r_label = new QLabel("Measurement noise std");
    rChoice = new QLineEdit(QString::number(this->kf->r), this);
    //rChoice->move(50,170);

    QWidget *q_empty = new QWidget(this);
    QLabel *q_label = new QLabel("Process noise std");
    qChoice = new QLineEdit(QString::number(this->kf->q), this);
    //qChoice->move(50,220);


    okButton = new QPushButton("OK",this);
    //okButton->move(50,300);

    commonlay->addWidget(freq_label);
    commonlay->addWidget(freqChoice);
    commonlay->addWidget(freq_empty);

    commonlay->addWidget(A_label);
    commonlay->addWidget(AChoice);
    commonlay->addWidget(A_empty);

    commonlay->addWidget(r_label);
    commonlay->addWidget(rChoice);
    commonlay->addWidget(r_empty);

    commonlay->addWidget(q_label);
    commonlay->addWidget(qChoice);
    commonlay->addWidget(q_empty);
    commonlay->addWidget(okButton);


    connect(okButton, &QPushButton::clicked, this, &KalmanParamChoice::okButtonClicked);


}




void KalmanParamChoice::okButtonClicked()
{


    QString text_freq = freqChoice->text();
    QString text_A = AChoice->text();
    QString text_srate = srateChoice->text();
    QString text_r = rChoice->text();
    QString text_q = qChoice->text();

    bool ok;
    bool allok = true;

    double freq = text_freq.toDouble(&ok);
    allok = ok&allok;
    double A = text_A.toDouble(&ok);
    allok = ok&allok;
    double srate = text_srate.toDouble(&ok);
    allok = ok&allok;
    double r = text_r.toDouble(&ok);
    allok = ok&allok;
    double q = text_q.toDouble(&ok);
    allok = ok&allok;

    if (! allok) {
        // The conversion failed, handle the error
        qDebug() << "Invalid value entered";
    } else{
        kf->init_params(freq,srate,A,r,q);
        this->close();
    }

}




