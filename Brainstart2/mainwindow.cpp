#include "stdafx.h"

#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    this->setFixedSize(600,400);

    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);


    QVBoxLayout *vLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *hLayout_1 = new QHBoxLayout();
    QHBoxLayout *hLayout_2 = new QHBoxLayout();

    streamListWidget = new QListWidget(this);

    demoButton = new QPushButton("Начать", this);
    setKalmanManual = new QPushButton(trUtf8("Параметры фильтра"), this);
    findStreams = new QPushButton(trUtf8("Найти lsl-потоки"),this);

    demoButton->setStyleSheet("text-align: left;");
    setKalmanManual->setStyleSheet("text-align: left;");
    findStreams->setStyleSheet("text-align: left;");

    QVBoxLayout *menu_layout = new QVBoxLayout();

    menu_layout->addWidget(demoButton);
    menu_layout->addWidget(findStreams);
    menu_layout->addWidget(setKalmanManual);
    menu_layout->addStretch(1);

    hLayout_1->addLayout(menu_layout);
    hLayout_1->addStretch(1);
    hLayout_1->addWidget(streamListWidget);
   
    vLayout->addLayout(hLayout_1);

    centralWidget->setLayout(vLayout);

    connect(findStreams, &QPushButton::clicked, this, &MainWindow::onfindStreamsClicked);
    connect(demoButton, &QPushButton::clicked, this, &MainWindow::ondemoButtonclicked);
    connect(setKalmanManual, &QPushButton::clicked, this, &MainWindow::onsetKalmanButtonclicked);
    connect(streamListWidget, &QListWidget::itemClicked, this, &MainWindow::handleStreamSelected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ondemoButtonclicked()
{
    signalplotwin = new SignalPlotWin(datareceiver->Nch, datareceiver, dirToSave); // Attention
    signalplotwin->show();

    demoButton->setText("Сеанс запущен!");
}

void MainWindow::onfindStreamsClicked()
{
    datareceiver->resolve_and_print_streams(streamListWidget);
}


void MainWindow::handleStreamSelected()
{
    datareceiver->stream_idx = streamListWidget->currentRow();
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

    for (uint i = 0; i < datareceiver->Nch; i++) {
        //datareceiver->spat_filter[i] = 0.5; // CHANGE!!

        datareceiver->databuffer[i].resize(datareceiver->maxbufsamples);
        datareceiver->databuffer[i].fill(0.0);
    }

}

void MainWindow::SetConfigurationFileName(char* szConfigFileName, char* szSaveFileName)
{
    paramsFileName.assign(szConfigFileName);

    dirToSave.assign(szSaveFileName);

    LoadParameters();
}

void MainWindow::onsetKalmanButtonclicked()
{
    WhiteKF *kf = new WhiteKF();
    kalmanparamchoice = new KalmanParamChoice(kf);
    kalmanparamchoice->show();
}

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
    std::ifstream file(paramsFileName);

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
        dataproc_kf = new WhiteKF();
        datareceiver = new DataReceiver(dataproc_kf);
        datareceiver->Nch =Nch;
        datareceiver->spat_filter.resize(datareceiver->Nch);

        datareceiver->to_prefilter = values[2];

        dataproc_kf->freq =  values[3];

        datareceiver->q0 = values[4];
        datareceiver->q1 = values[5];

        dataproc_kf->q =values[6];
        dataproc_kf->r = values[7];

        for (int k = 9; k<values.size(); k++)
        {
            datareceiver->spat_filter[k-9] = values[k];
        }
    }


    if (filter_type == 1)
    {
        dataproc_cfir = new CFIR();
        datareceiver = new DataReceiver(dataproc_cfir);
        datareceiver->Nch =Nch;
        datareceiver->spat_filter.resize(datareceiver->Nch);

        datareceiver->to_prefilter = values[2];

        dataproc_cfir->freq =  values[3];

        datareceiver->q0 = values[4];
        datareceiver->q1 = values[5];

        dataproc_cfir->Ntaps =values[6];

        for (int k = 8; k<values.size(); k++)
        {
            datareceiver->spat_filter[k-8] = values[k];
        }
    }

    file.close();
}
