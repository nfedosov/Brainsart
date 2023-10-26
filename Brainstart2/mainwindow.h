#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRadioButton>
#include "qpushbutton.h"


#include "kalmanparamchoice.h"
#include "qcustomplot.h"
#include "signalplotwin.h"
#include "datareceiver.h"
#include "idataprocessor.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    double base_q0 = 0;
    double base_q1 = 0;

    IDataProcessor *dataproc;


    DataReceiver *datareceiver;

    KalmanParamChoice *kalmanparamchoice;
    SignalPlotWin *signalplotwin;
    QCustomPlot *signalPlot;

    //WhiteKF kf(?????/);




private slots:
    void ondemoButtonclicked();
    void onsetKalmanButtonclicked();
    //void handleButtonGroupKCClick(int);
    //void handleButtonGroupKCClick(int);
    void onfindStreamsClicked();
    void handleStreamSelected();
    void onLoadParams();



    void onQ0changed();
    void onQ1changed();
    //void onusecfirButtonClicked();
    //void onusekfButtonClicked();



private:
    QPushButton *demoButton;
    QPushButton *setKalmanManual;
    QPushButton *findStreams;
    QLineEdit *lineEdit1;
    QLineEdit *lineEdit2;
    //QCheckBox *envelopeFB; REDO INTO QCHECKOX
    //QCheckBox *phaseFB;
    //QRadioButton *envelopeFBButton;
    //QRadioButton *phaseFBButton;
    //QButtonGroup* buttonGroupEP;

    //QRadioButton *useKFButton;
    //QRadioButton *useCFIRButton;

    //QButtonGroup* buttonGroupKC;
    QPushButton* load_params_button;


    QListWidget* streamListWidget;






    //QPushButton *demoButton;
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
