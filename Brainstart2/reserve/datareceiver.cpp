#include "cfir.h"
#include "datareceiver.h"
#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>
#include "liblsl-master/include/lsl_cpp.h"
#include "qlistwidget.h"


using namespace Eigen;
using namespace lsl;


#include <random>
#include <chrono>
#include <thread>
#include <QDebug>

DataReceiver::DataReceiver(IDataProcessor *dataproc_obj, QObject *parent)
    : QObject{parent}
{
    //do there choicing


    dataprocessor = dataproc_obj;
 //   if (1)//...?
  //  {
  //      dataprocessor = new WhiteKF();
  //  }
  //  else
  //  {
   //     dataprocessor = new CFIR(100, 8.0, 12.0, 500.0,"hamming");//new WhiteKF();

 //   }
//


    //lsl::stream_inlet inlet(lsl::stream_info("my_stream_name", "my_stream_type", 1), 0, true);


    maxbufsamples = ((unsigned int)srate)*maxbufsec;

    envelopebuffer.resize(maxbufsamples);
    phasebuffer.resize(maxbufsamples);
    processedbuffer.resize(maxbufsamples);

/*
    int n_channels = 1;
    std::string name = "Game_Ctrl";
    std::string type = "Ctrl";

    stream_info *info = new stream_info(name, type, n_channels, srate, lsl::cf_float32, std::string(name) += type);
    outlet = new stream_outlet(*info, 0, 5000); //5000 - max buffered samples
*/

}


int DataReceiver::resolve_and_print_streams(QListWidget* streamList)
{

    results = resolve_streams();
    streamList->clear();
    qInfo()<< "Found " << results.size() << " streams:";
      for (auto& result : results) {

        streamList->addItem(QString::fromStdString(result.name()) + " (" + QString::fromStdString(result.type()) + ")");
        qInfo()<< QString::fromStdString(result.name()) << " (" << QString::fromStdString(result.type()) << ")";
    }





    //lsl_streaminfo info; /* the streaminfo returned by the resolve call */
    //    /* resolve the stream of interest (result array: info, array capacity: 1 element, type shall be
     //    * EEG, resolve at least 1 stream, wait forever if necessary) */
    //printf("Now waiting for an EEG stream...\n");
    //lsl_resolve_byprop(&info, 5, "type", "BCI", 1, 5000.0);
    //qInfo()<<info;


    return 0;
}




void DataReceiver::memStreamInfo()
{
    inlet = new stream_inlet(results[stream_idx]);//new stream_inlet(*info, maxbufsamples, 0, 1);
    //info = inlet->info();
    //const char* label = info.desc().first_child().first_child().child_value("label");
    //info = new stream_info(results[stream_idx].name(), results[stream_idx].type(), results[stream_idx].channel_count());
    //info->desc() = results[stream_idx].desc();
}

void DataReceiver::lslDataReceive()
{
    totalsamplesreceived = 0;
    std::vector<std::vector<float>> samples;
    curposidx = 0;

    //double timestamp = 0;



    int vec_len;
    //std::vector<float> sample;

    //inlet->pull_sample(sample);
    while(1) {

        //std::this_thread::sleep_for(std::chrono::milliseconds(10));

        inlet->pull_chunk(samples);
        vec_len = samples.size();

        //qDebug()<<samples;

        for (int i = 0; i<Nch; i++)
        {
            for(int j = 0; j<vec_len;j++)
            {
                databuffer[i][(curposidx+j)%maxbufsamples] = samples[j][i];

            }
        }


        double spat_filtered;
        for(int i = 0; i<vec_len;i++)
        {
            spat_filtered = 0.0;
            for (int j = 0; j<Nch; j++)
            {
                spat_filtered += spat_filter[j]*samples[i][j]*1.0;
            }
            x = dataprocessor->step(spat_filtered);
            //dataprocessor->x(0);
            processedbuffer((curposidx+i)%maxbufsamples) =x(0);

            envelopebuffer((curposidx+i)%maxbufsamples) = sqrt(x(0)*x(0)+x(1)*x(1));
            phasebuffer((curposidx+i)%maxbufsamples) = atan(x(0)/x(1)); // ПОка что
            //qInfo()<<envelopebuffer((curposidx+i)%maxbufsamples)<<"\n"<<"x0";


            /*out_sample[0] = (float)envelopebuffer((curposidx+i)%maxbufsamples);

            float q0 = 0;
            float q1 = 1e-8;
            out_sample[0] =(out_sample[0]-q0)/(q1-q0);
            if (out_sample[0] <0)
            {
                out_sample[0] = 0.0;
            }
            if (out_sample[0] > 1.0)
            {
                out_sample[0] = 1.0;
            }
            (*outlet).push_sample(out_sample);*/
        }



        totalsamplesreceived += vec_len;
        curposidx = (curposidx+vec_len)%maxbufsamples;



        //timestamp = inlet->pull_sample(sample);

    }



/*




    stream_inlet inlet(results[0]);



    if (results.empty()) {
            std::cout << "No streams found." << std::endl;
            return 1;
        }

        // Get the first result
        stream_info info = results.front();

        // Print some information about the stream
        std::cout << "Stream name: " << info.name() << std::endl;
        std::cout << "Stream type: " << info.type() << std::endl;
        std::cout << "Number of channels: " << info.channel_count() << std::endl;

        // Open an inlet to read from the stream
        inlet inlet(info);

        // Read some data from the stream
        std::vector<float> sample;
        while (true) {
            inlet.pull_sample(sample);
            std::cout << "Sample: ";
            for (float value : sample) {
                std::cout << value << " ";
            }
            std::cout << std::endl;
        }



    while(true)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
        counter =int(elapsed_time.count()/2.0);

        // ПРЕДПОЛАГАЕМ, ЧТО БУФЕР LSL меньше равно буфера тут
        if (counter> totalsamplesreceived)
        {
            difsamples = counter-totalsamplesreceived;
            for (int i = 0; i<Nch; i++)
            {
                for(int j = 0; j<difsamples;j++)
                {
                    databuffer[i][(curposidx+j)%maxbufsamples] = dist(generator)*1.0;

                }

            }
            totalsamplesreceived += difsamples;
            curposidx = (curposidx+difsamples)%maxbufsamples;
        }

        double spat_filtered;
        for(int i = 0; i<difsamples;i++)
        {
            spat_filtered = 0.0;
            for (int j = 0; j<Nch; j++)
            {
                spat_filtered += spat_filter[j]*dist(generator)*1.0;
            }
            dataprocessor->step(spat_filtered);
            //dataprocessor->x(0);
            processedbuffer((curposidx+i)%maxbufsamples) =dataprocessor->x(0);
            envelopebuffer((curposidx+i)%maxbufsamples) = sqrt(dataprocessor->x(0)*dataprocessor->x(0)+dataprocessor->x(1)*dataprocessor->x(1));
            phasebuffer((curposidx+i)%maxbufsamples) = atan(dataprocessor->x(0)/dataprocessor->x(1)); // ПОка что
        }

*/
}

void DataReceiver::fakeDataReceive()
{
    totalsamplesreceived = 0;



    // Initialize a counter variable
    int counter = 0;

    // Connect the timeout() signal of the QTimer to a slot that increments the counter








    int difsamples;
    curposidx = 0;

    const double mean = 0.0;
    const double stddev = 1.0;
    static std::default_random_engine generator;
    static std::normal_distribution<double> dist(mean, stddev);

    // Start the timer
    auto start_time = std::chrono::high_resolution_clock::now();



    while(true)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
        counter =int(elapsed_time.count()/2.0);

        // ПРЕДПОЛАГАЕМ, ЧТО БУФЕР LSL меньше равно буфера тут
        if (counter> totalsamplesreceived)
        {
            difsamples = counter-totalsamplesreceived;
            for (int i = 0; i<Nch; i++)
            {
                for(int j = 0; j<difsamples;j++)
                {
                    databuffer[i][(curposidx+j)%maxbufsamples] = dist(generator)*1.0;

                }

            }
            totalsamplesreceived += difsamples;
            curposidx = (curposidx+difsamples)%maxbufsamples;
        }

        double spat_filtered;
        for(int i = 0; i<difsamples;i++)
        {
            spat_filtered = 0.0;
            for (int j = 0; j<Nch; j++)
            {
                spat_filtered += spat_filter[j]*dist(generator)*1.0;
            }
            x = dataprocessor->step(spat_filtered);
            //dataprocessor->x(0);
            processedbuffer((curposidx+i)%maxbufsamples) =x(0);
            envelopebuffer((curposidx+i)%maxbufsamples) = sqrt(x(0)*x(0)+x(1)*x(1));
            phasebuffer((curposidx+i)%maxbufsamples) = atan(x(0)/x(1)); // ПОка что
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    }

}
