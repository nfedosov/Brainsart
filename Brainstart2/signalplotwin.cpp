#include "signalplotwin.h"
#include "qcustomplot.h"
#include <QPushButton.h>
#include <QTimer>
#include <vector>
#include <QThread>
#include "datareceiver.h"
#include "firwin.h"
#include "simplebarfbwin.h"

#include "constants.h"
#include "code_iir/IIR.h"





SignalPlotWin::SignalPlotWin(uint Nch, DataReceiver* datareceiver, string fileToSave,QWidget *parent)
    : QWidget{parent}
{


    this->fileSaveDir = fileToSave;

    this->datareceiver = datareceiver;
    this->srate = datareceiver->srate;

    this->savedata = new SaveData();

    //data.resize(Nch);
    visdata.resize(Nch);




    bandpass_len = (int)(def_bandpass_lensec*((double)srate));
    //firwin_bp = new FirWin(bandpass_len,low_cutoff,high_cutoff,(double)srate,Nch);
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






    this->Nch = Nch;
    this->srate = datareceiver->srate;
    resize(800, 400);
    startButton = new QPushButton("Начать мониторинг", this);
    recordButton = new QPushButton("Запись", this);
    showProcessedRaw = new QPushButton("Обработано", this);

    scale = 1.0;
    zoomInButton = new QToolButton(this);
    zoomOutButton = new QToolButton(this);
    zoomLeftButton = new QToolButton(this);
    zoomRightButton = new QToolButton(this);

    QCheckBox *check50Hz = new QCheckBox(this);
    check50Hz->setFixedSize(QSize(50, 50));

    check50Hz->setChecked(true); // set checkbox to 'on' by default

    zoomInButton->setGeometry(this->width() - 100,
                        this->height() - 100,
                        zoomInButton->width(), zoomInButton->height());

    QIcon upIcon("C:/Users/Fedosov/Documents/projects/brainstart2/Brainstart2/zoom_up.png");
    zoomInButton->setIcon(upIcon);
    zoomInButton->setStyleSheet("QToolButton { border: none; }");



    zoomOutButton->setGeometry(this->width() - 100,
                        this->height() - 50,
                        zoomOutButton->width(), zoomOutButton->height());

    QIcon downIcon("C:/Users/Fedosov/Documents/projects/brainstart2/Brainstart2/zoom_down.png");
    zoomOutButton->setIcon(downIcon);
    zoomOutButton->setStyleSheet("QToolButton { border: none; }");



    zoomLeftButton->setGeometry(this->width() - 125,
                        this->height() - 75,
                        zoomLeftButton->width(), zoomLeftButton->height());

    QIcon leftIcon("C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart2/zoom_left.png");
    zoomLeftButton->setIcon(leftIcon);
    zoomLeftButton->setStyleSheet("QToolButton { border: none; }");

    zoomRightButton->setGeometry(this->width() - 75,
                        this->height() - 75,
                        zoomRightButton->width(), zoomRightButton->height());

    QIcon rightIcon("C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart2/zoom_right.png");
    zoomRightButton->setIcon(rightIcon);
    zoomRightButton->setStyleSheet("QToolButton { border: none; }");


    QIcon recIcon(".\\Data\\icons\\record.png");
    recordButton->setIcon(recIcon);


    //zoomInButton->setIcon(QIcon(":/icons/zoomin.png"));
    //zoomInButton->setToolTip("Zoom In");
    //zoomInButton->setAutoRaise(true);


    low_cut_edit = new QLineEdit(QString::number(this->low_cutoff), this);
    high_cut_edit = new QLineEdit(QString::number(this->high_cutoff), this);



    //zoomOutButton->setIcon(QIcon(":/icons/zoomout.png"));
    //zoomOutButton->setToolTip("Zoom Out");
    //zoomOutButton->setAutoRaise(true);
    //customPlot->plotLayout()->addElement(2, 0, zoomOutButton);

    // Connect the buttons to slots that zoom in or out
    //connect(zoomInButton, &QToolButton::clicked, [=]() {
    //   customPlot->axis


    // Create a QVBoxLayout to hold the plot widgets

    QVBoxLayout *globalay = new QVBoxLayout(this);
    QHBoxLayout *buttonlay = new QHBoxLayout(this);
    //QVBoxLayout *scalelay = new QVBoxLayout();
    //QVBoxLayout *cutfreqlay = new QVBoxLayout();

    QFont font("Arial", 12); //QFont::Bold);

    QLabel *bp_low_label = new QLabel("Нижн");
    QLabel *bp_high_label = new QLabel("Верх");
    QLabel *notch_label = new QLabel("50Hz");
    bp_low_label->setFont(font);
    bp_high_label->setFont(font);
    notch_label->setFont(font);

    bp_low_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

    bp_high_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

    notch_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);


    // set the label's alignment
    //bp_label->setAlignment(Qt::AlignCenter);

    // add the label to a layout or widget


    buttonlay->addWidget(startButton);
    buttonlay->addWidget(showProcessedRaw);
    buttonlay->addWidget(recordButton);
    QWidget *emptyWidget1 = new QWidget(this);
    // add the widget to the layout
    buttonlay->addWidget(emptyWidget1);
    //buttonlay->addLayout(cutfreqlay);
    buttonlay->addWidget(bp_low_label);
    buttonlay->addWidget(low_cut_edit);
    buttonlay->addWidget(bp_high_label);
    buttonlay->addWidget(high_cut_edit);
    //buttonlay->addLayout(scalelay);
    buttonlay->addWidget(notch_label);
    buttonlay->addWidget(check50Hz);

    //scalelay->addWidget(zoomInButton);
    //scalelay->addWidget(zoomOutButton);
    buttonlay->setStretch(0,5);
    buttonlay->setStretch(1,5);
    buttonlay->setStretch(2,5);
    buttonlay->setStretch(3,4);
    buttonlay->setStretch(4,2);
    buttonlay->setStretch(5,2);
    buttonlay->setStretch(6,2);
    buttonlay->setStretch(7,2);
    buttonlay->setStretch(8,2);
    buttonlay->setStretch(9,2);
    //buttonlay->setStretch(4,5);


    connect(low_cut_edit,&QLineEdit::returnPressed,this,&SignalPlotWin::onLowCutEntered);
    connect(high_cut_edit,&QLineEdit::returnPressed, this,&SignalPlotWin::onHighCutEntered);

    connect(recordButton, &QToolButton::clicked,this,&SignalPlotWin::onrecordButtonclicked);

    connect(zoomInButton, &QToolButton::clicked, this, &SignalPlotWin::onzoomInButtonclicked);
    connect(zoomOutButton, &QToolButton::clicked, this, &SignalPlotWin::onzoomOutButtonclicked);

    connect(showProcessedRaw, &QToolButton::clicked, this, &SignalPlotWin::onshowProcessedRaw);
    //buttonlay->
    //customPlot->plotLayout()->addElement(1, 0, zoomInButton);

    //hbox->addLayout(vbox);

    //plotLayoutWidget = new QWidget();// PARENT??



    stackedLayout = new QStackedLayout();

    QVBoxLayout *plotProcLayout = new QVBoxLayout();
    plotProcLayoutWidget = new QWidget();
    plotProcLayoutWidget->setLayout(plotProcLayout);

    plot = new QCustomPlot();
    stackedLayout->addWidget(plot);

    stackedLayout->addWidget(plotProcLayoutWidget);



    // set the first layout as the current layout
    stackedLayout->setCurrentIndex(0);



    // Create the main plot widget and add it to the plot layout






    plots_processed = new QCustomPlot*[3];


    curlenwin = srate*defaultwinlen; //50 sec

    //timedata = QVector<double>::fromStdVector(std::vector<double>(101));
    timedata.resize(curlenwin);
    for  (int i = 0; i < curlenwin; i++)
    {
        timedata[i] = ((double)i)/((double)srate);
    }
    //timedata = QVector<double>::range(0.0, double(maxLen)/double(srate), 1.0/double(srate));
    // Create and set up each QCustomPlot in the array




    processeddata.resize(curlenwin);
    envelopedata.resize(curlenwin);
    phasedata.resize(curlenwin);
    for (int i = 0; i< 3; i++)
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

        //QCPItemText *label = new QCPItemText(plots_processed[i]);
        //label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
        //label->position->setType(QCPItemPosition::ptAxisRectRatio);
        //label->position->setCoords(0.2, 0.0); // Set the position of the label
        QLabel *label = new QLabel("");
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
        // Customize the label properties
        //label->setAlignment(Qt::AlignH);

        label->setFont(QFont("Arial", 8));
        //label->setColor(Qt::red);
        plotProcLayout->addWidget(label);
        plotProcLayout->addWidget(plots_processed[i]);



    }
    //plotProcLayout->setAlignment(Qt::AlignCenter);
    plotProcLayout->setStretch(0,1);
    plotProcLayout->setStretch(1,4);
    plotProcLayout->setStretch(2,1);
    plotProcLayout->setStretch(3,4);
    plotProcLayout->setStretch(4,1);
    plotProcLayout->setStretch(5,4);


    plots_processed[0]->graph(0)->setData(timedata,processeddata);
    plots_processed[0]->rescaleAxes();
    plots_processed[1]->graph(0)->setData(timedata,envelopedata);
    plots_processed[1]->rescaleAxes();
    plots_processed[2]->graph(0)->setData(timedata,phasedata);
    plots_processed[2]->rescaleAxes();





    QPen tickPen(Qt::red);


    chNames = new QCPItemText*[Nch];

    rng = 5.0; //scale of one plot
    scale = 1.0;

    QString channel_name;
    lsl::stream_info stream_info = this->datareceiver->inlet->info();
    lsl::xml_element channels = stream_info.desc().child("channels");



    for (int i = 0; i < Nch; i++) {

        //data[i].resize(1000000);
        //data[i].fill(0.0);

        visdata[i].resize(curlenwin);
        visdata[i].fill(0.0+rng*i);

        plot->addGraph();
        plot->graph(i)->setData(timedata, visdata[i]);
        //datareceiver->info->desc().child("channels");

        //plots[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        //plots[i]->setGeometry(0,0,plots[i]->width(), 500);



        //QCPLayoutGrid *gridLayout = new QCPLayoutGrid;
        //plots[i]->plotLayout()->addElement(0, 0, gridLayout); //0
        //QCPAxisRect *axisRect = new QCPAxisRect(plots[i]);
        //gridLayout->addElement(0, 0, axisRect);
        //gridLayout->setRowStretchFactor(0, 1);
        //gridLayout->setRowStretchFactor(1, 3);
        if (i%2 == 0)
        {
            plot->graph(i)->setPen(QPen(QColor(0x3D,0xED,0x97),1));
        }
        else
        {
            plot->graph(i)->setPen(QPen(QColor(0x82,0xEE,0xFD),1));
        }



    }

    //plot->addGraph()

    //QVector<double> buf1 = {timedata[curwinidx],timedata[curwinidx]};
    //QVector<double> buf2 = {0-rng/2.0,Nch*rng-rng/2.0};
    //plot->graph(Nch)->setData(buf1,buf2);
    //plot->graph(Nch)->lineStyle();

    verticalLine = new QCPItemLine(plot);
    verticalLine->start->setCoords(timedata[curwinidx], 0-rng/2.0);
    verticalLine->end->setCoords(timedata[curwinidx], Nch*rng-rng/2.0);

            // set the line style
    QPen pen;
    pen.setStyle(Qt::DotLine); // set dotted line style
    pen.setColor(Qt::red); // set line color to red
    pen.setWidth(1); // set line width to 1 pixel
    verticalLine->setPen(pen);
    //plot->addItem(verticalLine);

    plot->setBackground(QBrush(QColor(20, 20, 20)));

    plot->yAxis->setTickPen(tickPen);
    plot->yAxis->setVisible(false);

    //QColor tickColor(Qt::white); // set the color you want
    //plot->xAxis->setTickLabelColor(tickColor);
    //plot->xAxis->
    QPen penx(Qt::white);
    //pen.setWidth(2);
    plot->xAxis->setBasePen(penx);

    QColor tickColor(Qt::white); // set the color you want
    plot->xAxis->setLabelColor(tickColor);
    plot->xAxis->setTickLabelColor(tickColor);
    plot->xAxis->setTickPen(penx);
    plot->xAxis->setSubTickPen(penx);



    plot->rescaleAxes();
    plot->yAxis->setRange(-rng, Nch*rng);

    ch_names_string.resize(Nch);


    lsl::xml_element channel = channels.child("channel");
    for (int i = 0; i < Nch; i++) {

        chNames[i] = new QCPItemText(plot);


        channel_name = channel.child_value("label");

        ch_names_string[i] = channel_name;

        //textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);

        //chNames[i]->position->setType(QCPItemPosition::ptAxisRectRatio);
        //chNames[i]->position->setCoords(0.05, ((double)i)/Nch);
        chNames[i]->position->setType(QCPItemPosition::ptPlotCoords);
        chNames[i]->position->setCoords(0.45, ((rng)*i)+rng/5);
        chNames[i]->setText(channel_name);
        chNames[i]->setColor(QColor(255, 255, 255)); // set color to red
        chNames[i]->setFont(QFont("Arial", 12)); //QFont::Bold));

        channel = channel.next_sibling("channel");
}






    //QCPMarginGroup *marginGroup = new QCPMarginGroup(plots[0]);

    //for(int i = 0; i < Nch-1; i++) {
     //   plots[i]->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    //}

    //plots[Nch-1]->axisRect()->setMargins(QMargins(0, 0, 0, 30));
    //plots[Nch-1]->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

    //QCPAxis* sharedAxis = plots[Nch-1]->xAxis;
    //sharedAxis->setLabel("Time (s)");


 /*   for(int i = 0; i<Nch; i++)
{
            plots[i]->addGraph();
            plots[i]->graph(0)->setData(timedata, visdata[i]);
            plots[i]->rescaleAxes();
            plots[i]->yAxis->setRange(-rng, rng);
}*/

    //QCPAxisRect *sharedAxisRect = new QCPAxisRect(plots[0]);
    //sharedAxisRect->setMargins(QMargins(0, 0, 0, 0));
    //for (int i = 0; i < Nch; i++) {
    //    plots[i]->plotLayout()->addElement(i, 0, sharedAxisRect);
    //}
    //QCPAxis *sharedXAxis = new QCPAxis(sharedAxisRect, QCPAxis::AxisType::atBottom);
    //sharedXAxis->setLabel("Shared X-Axis");
    //sharedAxisRect->addAxis(QCPAxis::AxisType::atBottom, sharedXAxis);



    // Set up the layout for the main plot widget

    // Set the layout for the widget containing the plots
    //QWidget *plotWidget = new QWidget();
    globalay->addLayout(buttonlay);
    globalay->addLayout(stackedLayout);
    this->setLayout(globalay);

    // Add the plot widget to the main window
    //setCentralWidget(plotWidget);


    zoomInButton->raise();
    zoomOutButton->raise();
    zoomLeftButton->raise();
    zoomRightButton->raise();

    connect(startButton, &QPushButton::clicked, this, &SignalPlotWin::onstartButtonclicked);
    connect(&timer,&QTimer::timeout,this,&SignalPlotWin::updGraphs);
}


void SignalPlotWin::updGraphs()
{
    prevbufidx = curbufidx;
    curbufidx = datareceiver->curposidx;
    int n_samples_in_chunk= curbufidx-prevbufidx;
    if (n_samples_in_chunk < 0)
    {
        n_samples_in_chunk +=datareceiver->maxbufsamples;
    }
    //qDebug() << 2;
    //qInfo()<<datareceiver->envelopebuffer[(n_samples_in_chunk-1+prevbufidx)%datareceiver->maxbufsamples];
    //fbwin->setBarHeight(datareceiver->envelopebuffer[(n_samples_in_chunk-1+prevbufidx)%datareceiver->maxbufsamples]*1e7); //jump 8
    //qDebug()<<(datareceiver->envelopebuffer[(n_samples_in_chunk-1+prevbufidx)%datareceiver->maxbufsamples]*1e7); //jump

    if (!isShowProcessed)
    {
        for (int i = 0; i < Nch; i++)
        {
            for (int j = 0; j <n_samples_in_chunk; j++)
            {
                //visdata[i][(j+curwinidx)%curlenwin] =(datareceiver->databuffer[i][(j+prevbufidx)%datareceiver->maxbufsamples]);
                //Тут нужно определиться с буферами, их длиной и т д
                double prefiltered = datareceiver->databuffer[i][(j+prevbufidx)%datareceiver->maxbufsamples];
                //for (int lag = 0; lag < bandpass_len; lag++)
                //{
                    // Перевернуть h!!
                    //qDebug() << 4 << ' ' <<(j+prevbufidx-lag)%datareceiver->maxbufsamples;
                //    prefiltered += (datareceiver->databuffer[i][(j+prevbufidx-lag)%datareceiver->maxbufsamples])*firwin_bp->h[lag];
                //}
                if (to_Low)
                {prefiltered = iir_low_bqC.ComputeOutput(prefiltered);}

                if (to_High)
                {prefiltered = iir_high_bqC.ComputeOutput(prefiltered);}

                if (to_Notch)
                {prefiltered = iir_50_bqC.ComputeOutput(prefiltered);
                prefiltered = iir_100_bqC.ComputeOutput(prefiltered);
                prefiltered = iir_150_bqC.ComputeOutput(prefiltered);
                prefiltered = iir_200_bqC.ComputeOutput(prefiltered);}

                //qDebug() << i;
                //qDebug() << (datareceiver->databuffer[i][(j+prevbufidx)%datareceiver->maxbufsamples]);
                //qDebug() << 1;

                visdata[i][(j+curwinidx)%curlenwin] = prefiltered*scale+rng*i;//(datareceiver->databuffer[i][(j+prevbufidx)%datareceiver->maxbufsamples]);
                //qDebug() << 0;


                // Здесь или отдельно??

            }

        }
    }

    else
    {

        for (int j = 0; j <n_samples_in_chunk; j++)
        {
             processeddata[(j+curwinidx)%curlenwin] =(datareceiver->processedbuffer[(j+prevbufidx)%datareceiver->maxbufsamples]);
             envelopedata[(j+curwinidx)%curlenwin] =(datareceiver->envelopebuffer[(j+prevbufidx)%datareceiver->maxbufsamples]);
             phasedata[(j+curwinidx)%curlenwin] =(datareceiver->phasebuffer[(j+prevbufidx)%datareceiver->maxbufsamples]);


        }


    }

    verticalLine->start->setCoords(timedata[(curwinidx+n_samples_in_chunk)%curlenwin], 0-rng/2.0);
    verticalLine->end->setCoords(timedata[(curwinidx+n_samples_in_chunk)%curlenwin], Nch*rng-rng/2.0);

    if (isRecorded)
    {
        for (int i = 0; i < Nch; i++)
        {
            for (int j = 0; j <n_samples_in_chunk; j++)
            {

                data(i,record_pos+j) = datareceiver->databuffer[i][(j+prevbufidx)%datareceiver->maxbufsamples];
                //if( i == 1){
                //data(i,record_pos+j) = (datareceiver->databuffer[i][(j+prevbufidx)%datareceiver->maxbufsamples]);
                //}
                //if( i == 0){
                //data(i,record_pos+j) = (datareceiver->envelopebuffer[(j+prevbufidx)%datareceiver->maxbufsamples]);
                //}

                //envelopedata[(j+curwinidx)%curlenwin] =(datareceiver->envelopebuffer[(j+prevbufidx)%datareceiver->maxbufsamples]);
            }
        }

    }





   /*
        //QVector<int> v{1, 2, 3, 4, 5, 6};
        // Replace the range of elements from index 2 to index 4 with the values 9, 8, and 7.
        //v.replace(2, 3, {9, 8, 7});

    }*/

    //samplesfromstart += n_samples_in_chunk;

    if (isRecorded)
    {
        record_pos += n_samples_in_chunk;
    }


    //CHECK THE INDEX
    //qInfo() << envelopedata[(curwinidx+n_samples_in_chunk-1)%curlenwin]*10000000;

    curwinidx = (curwinidx+n_samples_in_chunk)%curlenwin;
    //prevbufidx = curbufpos;

    if (!isShowProcessed)
    {
        for (int i = 0; i < Nch; i++) {
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

void SignalPlotWin::onzoomInButtonclicked()
{
    rng = rng/2.0;
    for (int i = 0; i < Nch; i++)
    {
        plot->yAxis->setRange(-rng, Nch*rng);
        plots_processed[0]->yAxis->setRange(-rng,rng);
        plots_processed[1]->yAxis->setRange(-rng,rng);
    }



    for (int i = 0; i < Nch; i++)
    {
        chNames[i]->position->setCoords(0.45, ((rng)*i)+rng/5);
        for (int j = 0; j <curlenwin; j++)
        {


                visdata[i][j] = ((visdata[i][j]-rng*i*2.0)+rng*i);//(datareceiver->databuffer[i][(j+prevbufidx)%datareceiver->maxbufsamples]);


                // Здесь или отдельно??

            }

        }
}

void SignalPlotWin::onzoomOutButtonclicked()
{
    rng = rng*2.0;
    for (int i = 0; i < Nch; i++)
    {
        plot->yAxis->setRange(-rng, Nch*rng);
        plots_processed[0]->yAxis->setRange(-rng,rng);
        plots_processed[1]->yAxis->setRange(-rng,rng);
    }


    for (int i = 0; i < Nch; i++)
    {
        chNames[i]->position->setCoords(0.45, ((rng)*i)+rng/5);
        for (int j = 0; j <curlenwin; j++)
        {


                visdata[i][j] = ((visdata[i][j]-rng*i/2.0)+rng*i);//(datareceiver->databuffer[i][(j+prevbufidx)%datareceiver->maxbufsamples]);


                // Здесь или отдельно??

            }

        }







}


void SignalPlotWin::onstartButtonclicked()
{
    if (!is_started)
    {

    fbwin =new SimpleBarFBWin();
    fbwin->show();


    QThread* thread = new QThread;

    datareceiver->fbwin = fbwin;

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

void SignalPlotWin::onrecordButtonclicked()
{


    if (isRecorded)
    {

       Eigen::MatrixXd cut_data = data(Eigen::all, Eigen::seq(0,record_pos));
       savedata->saveToFif(cut_data,fileSaveDir,this->datareceiver->Nch,this->datareceiver->srate,this->ch_names_string);
       recordButton->setText("Start record");
       //record_pos = 0;
       isRecorded = false;
    }
    else
    {

        // Resize the matrix to the required size
        data.resize(Nch, int(srate*1200));


        record_pos = 0;
        isRecorded = true;
        recordButton->setText("Остановить запись");
    }


}


void SignalPlotWin::onshowProcessedRaw()
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


void SignalPlotWin::onLowCutEntered()
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
        vis_iir_high.CreateHighPass(2*M_PI*(low_cutoff*2)/srate, 1.0, 2*M_PI*(low_cutoff)/srate, -6.0);
        iir_high_bqC = vis_iir_high.biquadsCascade;


    }

}


void SignalPlotWin::onHighCutEntered()
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
        vis_iir_low.CreateLowPass(2*M_PI*high_cutoff/srate, 1.0, 2*M_PI*(high_cutoff*2)/srate, -6.0);
        iir_low_bqC = vis_iir_low.biquadsCascade;

    }

}



void SignalPlotWin::resizeEvent(QResizeEvent *event)
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



