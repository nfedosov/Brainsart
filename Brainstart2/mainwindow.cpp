#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include<QDebug>
#include<QtGlobal>
#include <iostream>
#include <fstream> // Include the <fstream> header
#include <sstream>
#include "kalmanparamchoice.h"

#include "signalplotwin.h"
#include "whitekf.h"
#include "cfir.h"
#include "datareceiver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);


    LoadParameters();

    this->setFixedSize(600,400);

    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);


    QVBoxLayout *vLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *hLayout_1 = new QHBoxLayout();
    QHBoxLayout *hLayout_2 = new QHBoxLayout();

    //QVBoxLayout *vLayKF_CFIR = new QVBoxLayout();
    //QVBoxLayout *vLayENV_PHASE = new QVBoxLayout();



    streamListWidget = new QListWidget(this);
    //streamListWidget->resize(300,200);





    demoButton = new QPushButton("Начать", this);
    setKalmanManual = new QPushButton(trUtf8("Параметры фильтра"), this);
    findStreams = new QPushButton(trUtf8("Найти lsl-потоки"),this);

    demoButton->setStyleSheet("text-align: left;");
    setKalmanManual->setStyleSheet("text-align: left;");
    findStreams->setStyleSheet("text-align: left;");


    lineEdit1 = new QLineEdit(this);
    lineEdit2 = new QLineEdit(this);


    connect(lineEdit1, &QLineEdit::textChanged, this, &MainWindow::onQ0changed);
    connect(lineEdit2, &QLineEdit::textChanged, this, &MainWindow::onQ1changed);
    //envelopeFBButton = new QRadioButton("Envelope\nexperiment", this);
    //phaseFBButton = new QRadioButton("Phase\nexperiment", this);


    //useKFButton = new QRadioButton("Use\nKalman Filter", this);
    //useCFIRButton = new QRadioButton("Use\nCFIR", this);


    //buttonGroupEP = new QButtonGroup();
    //buttonGroupEP->addButton(envelopeFBButton);
    //buttonGroupEP->addButton(phaseFBButton);


    // Set default checked button
    //envelopeFBButton->setChecked(true);


    //buttonGroupKC = new QButtonGroup();
    //buttonGroupKC->addButton(useKFButton, 0);
    //buttonGroupKC->addButton(useCFIRButton, 1);

    // Set default checked button
    //useKFButton->setChecked(true);

    // Connect signals and slots for button group
    //connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(handleButtonGroupClick(int)));

    QVBoxLayout *menu_layout = new QVBoxLayout();

    menu_layout->addWidget(demoButton);
    menu_layout->addWidget(findStreams);
    menu_layout->addStretch(1);
    menu_layout->addWidget(setKalmanManual);
    //
    hLayout_1->addLayout(menu_layout);

    hLayout_1->addStretch(1);
    hLayout_1->addWidget(streamListWidget);

    //hLayout_1->setStretch(0,1);
    //hLayout_1->setStretch(1,1);
    //hLayout_1->setStretch(2,1);
    //hLayout_1->setStretch(3,2);


    //vLayENV_PHASE->addWidget(envelopeFBButton);
    //vLayENV_PHASE->addWidget(phaseFBButton);

    //vLayKF_CFIR->addWidget(useKFButton);
    //vLayKF_CFIR->addWidget(useCFIRButton);



    //hLayout_2->addLayout(vLayKF_CFIR);
    //hLayout_2->addLayout(vLayENV_PHASE);

    vLayout->addLayout(hLayout_1);

    load_params_button = new QPushButton("Load parameteres", this);

    vLayout->addStretch(1);
    vLayout->addWidget(load_params_button);
    vLayout->addWidget(lineEdit1);
    vLayout->addWidget(lineEdit2);

    QWidget *emptyWidget = new QWidget();


    //vLayout->addWidget(emptyWidget);
    //vLayout->addLayout(hLayout_2);

    //vLayout->setStretch(0,1);
    //vLayout->setStretch(1,2);
    //vLayout->setStretch(2,1);



    centralWidget->setLayout(vLayout);


    // default processor
    /*if (filter_type == 0)
    {
    dataproc_kf = new WhiteKF();//new CFIR();//new CFIR(100, 8.0, 12.0, 500.0,"hamming");//new WhiteKF();
    }

    if (filter_type == 1)
    {
    dataproc_cfir = new CFIR();//new CFIR();//new CFIR(100, 8.0, 12.0, 500.0,"hamming");//new WhiteKF();
    }*/


    //new WhiteKF();
    //datareceiver = new DataReceiver(dataproc_kf);


    //setKalmanManual->move(200, 100);

    connect(findStreams, &QPushButton::clicked, this, &MainWindow::onfindStreamsClicked);
    connect(demoButton, &QPushButton::clicked, this, &MainWindow::ondemoButtonclicked);
    connect(setKalmanManual, &QPushButton::clicked, this, &MainWindow::onsetKalmanButtonclicked);
    connect(streamListWidget, &QListWidget::itemClicked, this, &MainWindow::handleStreamSelected);


    //connect(load_params_button, &QPushButton::clicked, this, &MainWindow::onLoadParams);
    //connect(useCFIRButton, &QRadioButton::clicked, this, &MainWindow::onusecfirButtonClicked);


    //connect(buttonGroupKC, &QButtonGroup::buttonToggled,
    //    this, &MainWindow::handleButtonGroupKCClick);
    //connect(buttonGroupKC, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MainWindow::handleButtonGroupKCClick);
    //connect(buttonGroupKC, &QButtonGroup::idClicked, this, &MainWindow::handleButtonGroupKCClick);

}




MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::ondemoButtonclicked()
{


    signalplotwin = new SignalPlotWin(datareceiver->Nch, datareceiver, dirToSave); // Attention
    signalplotwin->show();


    //feedbackwin = new FeedBackWin();
    //feedbackwin->show();


    qInfo() << "C++ Style Info Message";
    qInfo( "C Style Info Message" );




    demoButton->setText("Сеанс запущен!");


}
void MainWindow::onfindStreamsClicked()
{
    datareceiver->resolve_and_print_streams(streamListWidget);
}


void MainWindow::handleStreamSelected()
{
    datareceiver->stream_idx = streamListWidget->currentRow();
    //datareceiver->Nch =datareceiver->results[datareceiver->stream_idx].channel_count();
    //datareceiver->spat_filter.resize(datareceiver->Nch);
    datareceiver->databuffer.resize(datareceiver->Nch);
    datareceiver->memStreamInfo();


    if (filter_type == 0)
    {
        dataproc_kf->srate = datareceiver->srate;
        dataproc_kf->init_params();
    }

    if (filter_type == 1)
    {
        dataproc_cfir->fs = datareceiver->srate;
        dataproc_cfir->init_params();
    }




    for (int i = 0; i < datareceiver->Nch; i++) {
        //datareceiver->spat_filter[i] = 0.5; // CHANGE!!

        datareceiver->databuffer[i].resize(datareceiver->maxbufsamples);
        datareceiver->databuffer[i].fill(0.0);
    }

}




void MainWindow::SetConfigurationFileName(char* szConfigFileName, char* szSaveFileName)
{
    paramsFileName.assign(szConfigFileName);

    dirToSave.assign(szSaveFileName);
}





void MainWindow::onsetKalmanButtonclicked()
{

    WhiteKF *kf = new WhiteKF();
    kalmanparamchoice = new KalmanParamChoice(kf);


    //kalmanparamchoice->setModal(true);


    kalmanparamchoice->show();

}

/*
void MainWindow::handleButtonGroupKCClick(int id)
{
    qInfo( "I'm here" );
    delete dataproc;
    //delete datareceiver;
    if (id == 0)//...?
    {
        dataproc = new WhiteKF();
    }
    if (id == 1)
    {
        setKalmanManual->setText("set CFIR params");
        dataproc = new CFIR();//new CFIR(100, 8.0, 12.0, 500.0,"hamming");//new WhiteKF();

    }
    datareceiver->dataprocessor = dataproc;
    //datareceiver = new DataReceiver(dataproc);

//

}
*/

/*
void MainWindow::onLoadParams()
{

    std::vector<double> values;
    std::string filename = "C:/Users/Fedosov/Documents/projects/brainstart2/results/params.txt";


    std::ifstream file(filename);

        // Check if the file is open
        if (!file.is_open()) {
            std::cerr << "Failed to open the file: " << filename << std::endl;

        }

        std::string line;
        values.clear();

        // Read a line from the file
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            double value;
            while (ss >> value) {
                values.push_back(value);
                // Check if the next character is a comma and skip it
                if (ss.peek() == ',') {
                    ss.ignore();
                }
            }

        }
        qDebug()<<"parameters loaded";
        //qDebug()<<values[0];
        //qDebug()<<values[1];
        qDebug()<<values[2];
        qDebug()<<values[3];
        qDebug()<<values[4];
        qDebug()<<values[5];
        qDebug()<<values[6];

        datareceiver->q0 = values[1];
        datareceiver->q1 = values[2];

        this->base_q0 = values[1];
        this->base_q1 = values[2];

        datareceiver->to_prefilter = values[3];
        for (int i =0; i< this->datareceiver->Nch;i++)
        {
             datareceiver->spat_filter[i] = values[i+4];

        }
        //datareceiver->spat_filter[2] = values[5]; //Attention
        //datareceiver->spat_filter[3] = values[6]; // Attention


        // Close the file
        file.close();


}*/



void MainWindow::onQ0changed()
{
    QString text_freq = lineEdit1->text();

    bool ok;


    double thr = text_freq.toDouble(&ok);

    if (! ok) {
        // The conversion failed, handle the error
        qDebug() << "Invalid value entered";
    } else{
        datareceiver->q0 = this->base_q0 - thr*0.01*this->base_q0;
    }

}



void MainWindow::onQ1changed()
{
    QString text_freq = lineEdit2->text();

    bool ok;


    double thr = text_freq.toDouble(&ok);

    if (! ok) {
        // The conversion failed, handle the error
        qDebug() << "Invalid value entered";
    } else{
        datareceiver->q1 = this->base_q1 + thr*0.01*this->base_q1;
    }

}







void MainWindow::LoadParameters()
{
    std::vector<double> values;
    std::ifstream file(paramsFileName);//"C:/Users/Fedosov/Documents/projects/brainstart_final/Brainstart2/AlphaTraining_vs2019/AlphaTraining_VS2019/bin/Debug/Data/users/PTRRRT0/05_december_2023/config_05.12.2023_17.36.txt");
    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << std::endl;
        return;
    }

    std::string line;
    values.clear();

    // Read a line from the file
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double value;
        while (ss >> value) {
            values.push_back(value);
            // Check if the next character is a comma and skip it
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }

    }

    for (int i = 0; i < values.size(); i++)
    {
        qDebug() << values[i];
    }


    int Nch = values[0];




    filter_type  = values[1];


    if (filter_type == 0)
    {
    dataproc_kf = new WhiteKF();//new CFIR();//new CFIR(100, 8.0, 12.0, 500.0,"hamming");//new WhiteKF();
    datareceiver = new DataReceiver(dataproc_kf);
    datareceiver->Nch =Nch;
    datareceiver->spat_filter.resize(datareceiver->Nch);


    //dataproc->init_states();

    datareceiver->to_prefilter = values[2];

    dataproc_kf->freq =  values[3];

    datareceiver->q0 = values[4];
    datareceiver->q1 = values[5];

    dataproc_kf->q =values[6];
    dataproc_kf->r = values[7];

    // srate values[8]

    for (int k = 9; k<values.size(); k++)
    {
        datareceiver->spat_filter[k-9] = values[k];
    }



    //dataproc->r << ;
    //dataproc->q << ;

    //this->freq = free;
    //this->srate = srate;

    //double arg = 2.0*M_PI*freq/srate;
    //Psi<< cos(arg), -sin(arg), sin(arg), cos(arg);

    //reset_states();

    }



    //new WhiteKF();



    /*datareceiver->q0 = values[1];
    datareceiver->q1 = values[2];

    this->base_q0 = values[1];
    this->base_q1 = values[2];

    datareceiver->spat_filter[0] = values[3];
    datareceiver->spat_filter[1] = values[4];
    datareceiver->spat_filter[2] = values[5];
    datareceiver->spat_filter[3] = values[6];*/


    if (filter_type == 1)
    {
    dataproc_cfir = new CFIR();//new CFIR();//new CFIR(100, 8.0, 12.0, 500.0,"hamming");//new WhiteKF();
    datareceiver = new DataReceiver(dataproc_cfir);
    datareceiver->Nch =Nch;
    datareceiver->spat_filter.resize(datareceiver->Nch);


    //dataproc->init_states();

    datareceiver->to_prefilter = values[2];

    dataproc_cfir->freq =  values[3];

    datareceiver->q0 = values[4];
    datareceiver->q1 = values[5];

    dataproc_cfir->Ntaps =values[6];

    // srate values[7]

    for (int k = 8; k<values.size(); k++)
    {
        datareceiver->spat_filter[k-8] = values[k];
    }



    //dataproc->r << ;
    //dataproc->q << ;

    //this->freq = free;
    //this->srate = srate;

    //double arg = 2.0*M_PI*freq/srate;
    //Psi<< cos(arg), -sin(arg), sin(arg), cos(arg);

    //reset_states();

    }





    // Close the file
    file.close();
}




