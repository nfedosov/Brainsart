#include "stdafx.h"

#include "signalplotwin.h"
#include "./ui_mainwindow.h"

#include "../QCustomPlot/qcustomplot.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    resize(800, 400);

    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    startButton = new QPushButton("Начать мониторинг", centralWidget);
    recordButton = new QPushButton("Запись", centralWidget);
    showProcessedRaw = new QPushButton("Обработано", centralWidget);

    scale = 1.0;
    zoomInButton = new QToolButton(centralWidget);
    zoomOutButton = new QToolButton(centralWidget);
    zoomLeftButton = new QToolButton(centralWidget);
    zoomRightButton = new QToolButton(centralWidget);

    QCheckBox* check50Hz = new QCheckBox(centralWidget);
    check50Hz->setFixedSize(QSize(50, 50));

    check50Hz->setChecked(true); // set checkbox to 'on' by default

    zoomInButton->setGeometry(this->width() - 100,
        this->height() - 100,
        zoomInButton->width(), zoomInButton->height());

    QIcon upIcon("./Data/icons/zoom_up.png");
    zoomInButton->setIcon(upIcon);
    zoomInButton->setStyleSheet("QToolButton { border: none; }");

    zoomOutButton->setGeometry(this->width() - 100,
        this->height() - 50,
        zoomOutButton->width(), zoomOutButton->height());

    QIcon downIcon("./Data/icons/zoom_down.png");
    zoomOutButton->setIcon(downIcon);
    zoomOutButton->setStyleSheet("QToolButton { border: none; }");

    zoomLeftButton->setGeometry(this->width() - 125,
        this->height() - 75,
        zoomLeftButton->width(), zoomLeftButton->height());

    QIcon leftIcon("./Data/icons/zoom_left.png");
    zoomLeftButton->setIcon(leftIcon);
    zoomLeftButton->setStyleSheet("QToolButton { border: none; }");

    zoomRightButton->setGeometry(this->width() - 75,
        this->height() - 75,
        zoomRightButton->width(), zoomRightButton->height());

    QIcon rightIcon("./Data/icons/zoom_right.png");
    zoomRightButton->setIcon(rightIcon);
    zoomRightButton->setStyleSheet("QToolButton { border: none; }");

    QIcon recIcon(".\\Data\\icons\\record.png");
    recordButton->setIcon(recIcon);

    low_cut_edit = new QLineEdit(QString::number(this->low_cutoff), centralWidget);
    high_cut_edit = new QLineEdit(QString::number(this->high_cutoff), centralWidget);

    // Create a QVBoxLayout to hold the plot widgets
    QVBoxLayout* globalay = new QVBoxLayout(centralWidget);
    QHBoxLayout* buttonlay = new QHBoxLayout(centralWidget);

    QFont font("Arial", 12);

    QLabel* bp_low_label = new QLabel("Нижн");
    QLabel* bp_high_label = new QLabel("Верх");
    QLabel* notch_label = new QLabel("50Hz");
    bp_low_label->setFont(font);
    bp_high_label->setFont(font);
    notch_label->setFont(font);

    bp_low_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    bp_high_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    notch_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    buttonlay->addWidget(startButton);
    buttonlay->addWidget(showProcessedRaw);
    buttonlay->addWidget(recordButton);
    QWidget* emptyWidget1 = new QWidget(centralWidget);

    // add the widget to the layout
    buttonlay->addWidget(emptyWidget1);
    buttonlay->addWidget(bp_low_label);
    buttonlay->addWidget(low_cut_edit);
    buttonlay->addWidget(bp_high_label);
    buttonlay->addWidget(high_cut_edit);

    buttonlay->addWidget(notch_label);
    buttonlay->addWidget(check50Hz);

    buttonlay->setStretch(0, 5);
    buttonlay->setStretch(1, 5);
    buttonlay->setStretch(2, 5);
    buttonlay->setStretch(3, 4);
    buttonlay->setStretch(4, 2);
    buttonlay->setStretch(5, 2);
    buttonlay->setStretch(6, 2);
    buttonlay->setStretch(7, 2);
    buttonlay->setStretch(8, 2);
    buttonlay->setStretch(9, 2);


    connect(low_cut_edit, &QLineEdit::returnPressed, this, &MainWindow::onLowCutEntered);
    connect(high_cut_edit, &QLineEdit::returnPressed, this, &MainWindow::onHighCutEntered);

    connect(recordButton, &QToolButton::clicked, this, &MainWindow::onrecordButtonclicked);

    connect(zoomInButton, &QToolButton::clicked, this, &MainWindow::onzoomInButtonclicked);
    connect(zoomOutButton, &QToolButton::clicked, this, &MainWindow::onzoomOutButtonclicked);

    connect(showProcessedRaw, &QToolButton::clicked, this, &MainWindow::onshowProcessedRaw);
    
    stackedLayout = new QStackedLayout();

    QVBoxLayout* plotProcLayout = new QVBoxLayout();
    plotProcLayoutWidget = new QWidget();
    plotProcLayoutWidget->setLayout(plotProcLayout);

    plot = new QCustomPlot();
    stackedLayout->addWidget(plot);

    stackedLayout->addWidget(plotProcLayoutWidget);

    // set the first layout as the current layout
    stackedLayout->setCurrentIndex(0);

    for (int i = 0; i < 3; i++)
    {
        plots_processed[i] = new QCustomPlot();

        plots_processed[i]->addGraph();

        if ((i == 0) || (i == 1))
        {
            plots_processed[i]->yAxis->setRange(-100, 100);
            plots_processed[i]->yAxis->setLabel("uV");

        }
        else
        {
            plots_processed[i]->yAxis->setRange(-3.15, 3.15);
            plots_processed[i]->yAxis->setLabel("Rad");
        }

        QLabel* label = new QLabel("");
        if (i == 0)
        {
            label->setText("Обнаруженный ритм"); // Set the text of the labe
        }
        if (i == 1)
        {
            label->setText("Обнаруженная огибающая"); // Set the text of the labe
        }
        if (i == 2)
        {
            label->setText("Обнаруженная фаза"); // Set the text of the labe
        }


        label->setFont(QFont("Arial", 8));
        //label->setColor(Qt::red);
        plotProcLayout->addWidget(label);
        plotProcLayout->addWidget(plots_processed[i]);
    }

    plotProcLayout->setStretch(0, 1);
    plotProcLayout->setStretch(1, 4);
    plotProcLayout->setStretch(2, 1);
    plotProcLayout->setStretch(3, 4);
    plotProcLayout->setStretch(4, 1);
    plotProcLayout->setStretch(5, 4);

    ////////////////////////////////


    // Set the layout for the widget containing the plots
    globalay->addLayout(buttonlay);
    globalay->addLayout(stackedLayout);

    centralWidget->setLayout(globalay);

    zoomInButton->raise();
    zoomOutButton->raise();
    zoomLeftButton->raise();
    zoomRightButton->raise();

    connect(startButton, &QPushButton::clicked, this, &MainWindow::onstartButtonclicked);
    connect(&timer, &QTimer::timeout, this, &MainWindow::updGraphs);
}


void MainWindow::Init(ApplicationConfiguration* pConfig)
{
    configuration = pConfig;
    savedata = new SaveData();

    datareceiver = new DataReceiver(pConfig->values, pConfig->lslStreamInfo);

    double srate = datareceiver->srate;
    visdata.resize(configuration->numberOfChannes);


    bandpass_len = (int)(def_bandpass_lensec*(srate));
    
    IIR::ButterworthFilter vis_iir_low;
    vis_iir_low.CreateLowPass(2*M_PI*high_cutoff/srate, 1.0, 2*M_PI*(high_cutoff*2)/srate, -6.0);
    iir_low_bqC = vis_iir_low.biquadsCascade;

    IIR::ButterworthFilter vis_iir_high;
    vis_iir_high.CreateHighPass(2*M_PI*(low_cutoff*2)/srate, 1.0, 2*M_PI*(low_cutoff)/srate, -6.0);
    iir_high_bqC = vis_iir_high.biquadsCascade;


    IIR::ButterworthFilter vis_iir_50;
    vis_iir_50.CreateNotch(2*M_PI*50.0/srate,5.0,4);
    iir_50_bqC = vis_iir_50.biquadsCascade;

    IIR::ButterworthFilter vis_iir_100;
    vis_iir_100.CreateNotch(2*M_PI*100.0/srate,5.0,2);
    iir_100_bqC = vis_iir_100.biquadsCascade;


    IIR::ButterworthFilter vis_iir_150;
    vis_iir_150.CreateNotch(2*M_PI*150.0/srate,5.0,2);
    iir_150_bqC = vis_iir_150.biquadsCascade;

    IIR::ButterworthFilter vis_iir_200;
    vis_iir_200.CreateNotch(2*M_PI*200.0/srate,5.0,2);
    iir_200_bqC = vis_iir_200.biquadsCascade;

    curlenwin = srate * defaultwinlen; //50 sec

    timedata.resize(curlenwin);
    for  (int i = 0; i < curlenwin; i++)
    {
        timedata[i] = ((double)i)/((double)srate);
    }
    
    processeddata.resize(curlenwin);
    envelopedata.resize(curlenwin);
    phasedata.resize(curlenwin);
    
    plots_processed[0]->graph(0)->setData(timedata,processeddata);
    plots_processed[0]->rescaleAxes();
    plots_processed[1]->graph(0)->setData(timedata,envelopedata);
    plots_processed[1]->rescaleAxes();
    plots_processed[2]->graph(0)->setData(timedata,phasedata);
    plots_processed[2]->rescaleAxes();

    QPen tickPen(Qt::red);
    chNames = new QCPItemText*[configuration->numberOfChannes];

    rng = 5.0; //scale of one plot
    scale = 1.0;

    QString channel_name;
    lsl::stream_info stream_info = datareceiver->inlet->info();
    lsl::xml_element channels = stream_info.desc().child("channels");



    for (uint i = 0; i < configuration->numberOfChannes; i++) {
                
        visdata[i].resize(curlenwin);
        visdata[i].fill(0.0+rng*i);

        plot->addGraph();
        plot->graph(i)->setData(timedata, visdata[i]);

        if (i % 2 == 0)
        {
            plot->graph(i)->setPen(QPen(QColor(0x3D,0xED,0x97),1));
        }
        else
        {
            plot->graph(i)->setPen(QPen(QColor(0x82,0xEE,0xFD),1));
        }
    }

    verticalLine = new QCPItemLine(plot);
    verticalLine->start->setCoords(timedata[curwinidx], 0-rng/2.0);
    verticalLine->end->setCoords(timedata[curwinidx], configuration->numberOfChannes * rng-rng / 2.0);


    QPen pen;
    pen.setStyle(Qt::DotLine); // set dotted line style
    pen.setColor(Qt::red); // set line color to red
    pen.setWidth(1); // set line width to 1 pixel
    verticalLine->setPen(pen);
 
    plot->setBackground(QBrush(QColor(20, 20, 20)));

    plot->yAxis->setTickPen(tickPen);
    plot->yAxis->setVisible(false);


    QPen penx(Qt::white);
    plot->xAxis->setBasePen(penx);

    QColor tickColor(Qt::white); // set the color you want
    plot->xAxis->setLabelColor(tickColor);
    plot->xAxis->setTickLabelColor(tickColor);
    plot->xAxis->setTickPen(penx);
    plot->xAxis->setSubTickPen(penx);


    plot->rescaleAxes();
    plot->yAxis->setRange(-rng, configuration->numberOfChannes * rng);

    ch_names_string.reserve(configuration->numberOfChannes);


    lsl::xml_element channel = channels.child("channel");
    for (uint i = 0; i < configuration->numberOfChannes; i++) {

        chNames[i] = new QCPItemText(plot);


        channel_name = channel.child_value("label");

        ch_names_string.append(channel_name);

        chNames[i]->position->setType(QCPItemPosition::ptPlotCoords);
        chNames[i]->position->setCoords(0.45, ((rng)*i)+rng/5);
        chNames[i]->setText(channel_name);
        chNames[i]->setColor(QColor(255, 255, 255)); // set color to red
        chNames[i]->setFont(QFont("Arial", 12)); //QFont::Bold));

        channel = channel.next_sibling("channel");
    }

}



void MainWindow::updGraphs()
{
    prevbufidx = curbufidx;
    curbufidx = datareceiver->curposidx;
    int n_samples_in_chunk= curbufidx-prevbufidx;
    if (n_samples_in_chunk < 0)
    {
        n_samples_in_chunk += datareceiver->maxbufsamples;
    }
    
    //if (!isShowProcessed)
    {
        for (uint i = 0; i < configuration->numberOfChannes; i++)
        {
            for (int j = 0; j <n_samples_in_chunk; j++)
            {
                //Тут нужно определиться с буферами, их длиной и т д
                double prefiltered = datareceiver->databuffer[i][(j+prevbufidx) % datareceiver->maxbufsamples];
               
                if (to_Low)
                {prefiltered = iir_low_bqC.ComputeOutput(prefiltered);}

                if (to_High)
                {prefiltered = iir_high_bqC.ComputeOutput(prefiltered);}

                if (to_Notch)
                {prefiltered = iir_50_bqC.ComputeOutput(prefiltered);
                prefiltered = iir_100_bqC.ComputeOutput(prefiltered);
                prefiltered = iir_150_bqC.ComputeOutput(prefiltered);
                prefiltered = iir_200_bqC.ComputeOutput(prefiltered);}

                visdata[i][(j+curwinidx)%curlenwin] = prefiltered*scale+rng*i;
            }
        }
    }
    //else
    {
        for (int j = 0; j <n_samples_in_chunk; j++)
        {
             processeddata[(j+curwinidx)%curlenwin] =(datareceiver->processedbuffer[(j+prevbufidx)% datareceiver->maxbufsamples]);
             envelopedata[(j+curwinidx)%curlenwin] =(datareceiver->envelopebuffer[(j+prevbufidx)% datareceiver->maxbufsamples]);
             phasedata[(j+curwinidx)%curlenwin] =(datareceiver->phasebuffer[(j+prevbufidx)% datareceiver->maxbufsamples]);
        }
    }

    verticalLine->start->setCoords(timedata[(curwinidx+n_samples_in_chunk)%curlenwin], 0-rng/2.0);
    verticalLine->end->setCoords(timedata[(curwinidx+n_samples_in_chunk)%curlenwin], configuration->numberOfChannes *rng-rng/2.0);

    if (isRecorded)
    {
        for (uint i = 0; i < configuration->numberOfChannes; i++)
        {
            for (int j = 0; j <n_samples_in_chunk; j++)
            {

                data(i,record_pos+j) = datareceiver->databuffer[i][(j+prevbufidx) % datareceiver->maxbufsamples];
            }
        }

    }

    if (isRecorded)
    {
        record_pos += n_samples_in_chunk;
    }

    curwinidx = (curwinidx+n_samples_in_chunk)%curlenwin;

    if (!isShowProcessed)
    {
        for (uint i = 0; i < configuration->numberOfChannes; i++) {
            plot->graph(i)->setData(timedata, visdata[i]);

        }
        plot->replot();
    }
    else
    {
        plots_processed[0]->graph(0)->setData(timedata,processeddata);
        plots_processed[0]->replot();
        plots_processed[1]->graph(0)->setData(timedata,envelopedata);
        plots_processed[1]->replot();
        plots_processed[2]->graph(0)->setData(timedata,phasedata);
        plots_processed[2]->replot();
    }
}

void MainWindow::onzoomInButtonclicked()
{
    rng = rng/2.0;

    for (uint i = 0; i < configuration->numberOfChannes; i++)
    {
        plot->yAxis->setRange(-rng, configuration->numberOfChannes * rng);
        plots_processed[0]->yAxis->setRange(-rng,rng);
        plots_processed[1]->yAxis->setRange(-rng,rng);
    }

    for (uint i = 0; i < configuration->numberOfChannes; i++)
    {
        chNames[i]->position->setCoords(0.45, ((rng)*i)+rng/5);
        for (int j = 0; j <curlenwin; j++)
        {
            visdata[i][j] = ((visdata[i][j]-rng*i*2.0)+rng*i);

        }

    }
}

void MainWindow::onzoomOutButtonclicked()
{
    rng = rng*2.0;
    for (uint i = 0; i < configuration->numberOfChannes; i++)
    {
        plot->yAxis->setRange(-rng, configuration->numberOfChannes * rng);
        plots_processed[0]->yAxis->setRange(-rng,rng);
        plots_processed[1]->yAxis->setRange(-rng,rng);
    }


    for (uint i = 0; i < configuration->numberOfChannes; i++)
    {
        chNames[i]->position->setCoords(0.45, ((rng)*i)+rng/5);
        for (int j = 0; j <curlenwin; j++)
        {
                visdata[i][j] = ((visdata[i][j]-rng*i/2.0)+rng*i);
        }

    }
}

void MainWindow::onstartButtonclicked()
{
    if (!is_started)
    {
        QThread* thread = new QThread;

        // Move the object to the new thread
        datareceiver->moveToThread(thread);

        // Connect the thread's started() signal to the method you want to run
        connect(thread, &QThread::started, datareceiver, &DataReceiver::lslDataReceive);

        // Start the thread
        thread->start();


        //samplesfromstart = 0;
        curwinidx = 0;
        prevbufidx = 0;
        curbufidx = datareceiver->curposidx;
        // накопившееся семплы, которые мы уже визуализировали
        //cumwinsamples =0;
        timer.start(50); // Interval 0 means to refresh as fast as possible
        is_started = true;    
    }
    else
    {
        timer.stop();
    }
}

void MainWindow::onrecordButtonclicked()
{
    if (isRecorded)
    {

       Eigen::MatrixXd cut_data = data(Eigen::all, Eigen::seq(0,record_pos));
       savedata->saveToFif(
           cut_data, 
           configuration->fileToSave,
           datareceiver->Nch,
           datareceiver->srate,
           this->ch_names_string);

       recordButton->setText("Start record");
       
       isRecorded = false;
    }
    else
    {
        // Resize the matrix to the required size
        data.resize(configuration->numberOfChannes, int(datareceiver->srate * 1200));

        record_pos = 0;
        isRecorded = true;
        recordButton->setText("Остановить запись");
    }
}


void MainWindow::onshowProcessedRaw()
{
    if (!isShowProcessed)
    {

        showProcessedRaw->setText("Получено");
        stackedLayout->setCurrentIndex(1);
        isShowProcessed = true;
    }
    else
    {
        showProcessedRaw->setText("Обработано");
        stackedLayout->setCurrentIndex(0);
        isShowProcessed = false;
    }
    zoomInButton->raise();
    zoomOutButton->raise();
    zoomLeftButton->raise();
    zoomRightButton->raise();

}


void MainWindow::onLowCutEntered()
{
    QString text_freq = low_cut_edit->text();

    bool ok;


    double freq = text_freq.toDouble(&ok);

    if (! ok) {
        // The conversion failed, handle the error
        qDebug() << "Invalid value entered";
    } else{
        low_cutoff = freq;
        //firwin_bp = new FirWin(bandpass_len,low_cutoff,high_cutoff,(double)srate,Nch);
        IIR::ButterworthFilter vis_iir_high;
        vis_iir_high.CreateHighPass(
            2*M_PI*(low_cutoff*2) / datareceiver->srate, 
            1.0, 
            2*M_PI*(low_cutoff) / datareceiver->srate, 
            -6.0);

        iir_high_bqC = vis_iir_high.biquadsCascade;


    }

}


void MainWindow::onHighCutEntered()
{
    QString text_freq = high_cut_edit->text();

    bool ok;
    double freq = text_freq.toDouble(&ok);

    if (! ok) {
        // The conversion failed, handle the error
        qDebug() << "Invalid value entered";
    } else{
        high_cutoff = freq;

        IIR::ButterworthFilter vis_iir_low;
        vis_iir_low.CreateLowPass(
            2*M_PI*high_cutoff / datareceiver->srate, 
            1.0, 
            2*M_PI*(high_cutoff*2) / datareceiver->srate,
            -6.0);

        iir_low_bqC = vis_iir_low.biquadsCascade;
    }
}



void MainWindow::resizeEvent(QResizeEvent *event)
{
    // Call the base class implementation first
    QWidget::resizeEvent(event);


    zoomInButton->setGeometry(this->width() - 100,
                        this->height() - 100,
                        zoomInButton->width(), zoomInButton->height());



    zoomOutButton->setGeometry(this->width() - 100,
                        this->height() - 50,
                        zoomOutButton->width(), zoomOutButton->height());



    zoomLeftButton->setGeometry(this->width() - 125,
                        this->height() - 75,
                        zoomLeftButton->width(), zoomLeftButton->height());

    zoomRightButton->setGeometry(this->width() - 75,
                        this->height() - 75,
                        zoomRightButton->width(), zoomRightButton->height());




}


//ONSTOPBUTTON CLICKED AND ALSO ON THE QUIT DELETE THREADS!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ! !!!!!!!!!!!!!!!!!
// ! !!!!!!!!!!!!!!!!



