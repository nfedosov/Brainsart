#ifndef DATARECEIVER_H
#define DATARECEIVER_H



using namespace lsl;


class DataReceiver: public QObject
{
public:
    DataReceiver(std::vector<double> values, stream_info lslStreamInfo, QObject *parent = nullptr);

    void fakeDataReceive();

    void lslDataReceive();

    int stream_idx = 0;
    //stream_info* info;
    stream_inlet* inlet;
    stream_outlet *outlet;
    std::vector<float> out_sample = {0.0};   //size 1, filled with zero

    float q0 = 1.03829590e-06;
    float q1 =  3.85767676e-06*1.5;

    unsigned long long totalsamplesreceived;
    int curposidx = 0;

    unsigned int maxbufsec = 5;
    unsigned int maxbufsamples;

    double srate; // ATTENTION!!!!
    unsigned int Nch;

    bool to_prefilter = 0;


    QVector<QVector<double>> databuffer;
    VectorXd envelopebuffer;
    VectorXd phasebuffer;
    VectorXd processedbuffer;


    Vector2d x;




    // ПОКА БЕЗ ВИРТУАЛЬНОГО КЛАССА
    IDataProcessor *dataprocessor;
    //WhiteKF *dataprocessor;
    //CFIR *dataprocessor;

    VectorXd spat_filter;



};

#endif // DATARECEIVER_H
