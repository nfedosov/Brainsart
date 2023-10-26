#ifndef KALMANPARAMCHOICE_H
#define KALMANPARAMCHOICE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton.h>
#include "whitekf.h"
#include "idataprocessor.h"


class KalmanParamChoice : public QWidget
{
    Q_OBJECT
public:
    explicit KalmanParamChoice(WhiteKF* kf, QWidget *parent = nullptr);
    WhiteKF* kf;


private:

QLineEdit *freqChoice;
QLineEdit *AChoice;
QLineEdit *srateChoice;
QLineEdit *rChoice;
QLineEdit *qChoice;
QPushButton *okButton;


private slots:
    void okButtonClicked();

signals:



};

#endif // KALMANPARAMCHOICE_H
