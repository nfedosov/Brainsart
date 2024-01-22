#include "stdafx.h"


DataReceiver::DataReceiver(std::vector<double> values, stream_info lslStreamInfo, QObject *parent)
    : QObject{parent}
{
    int filterType = (int)values[1];

    Nch = values[0];
    spat_filter.resize(Nch);

    inlet = new stream_inlet(lslStreamInfo);

    srate = inlet->info().nominal_srate();

    maxbufsamples = ((unsigned int)srate) * maxbufsec;

    envelopebuffer.resize(maxbufsamples);
    phasebuffer.resize(maxbufsamples);
    processedbuffer.resize(maxbufsamples);
    databuffer.resize(Nch);

    for (uint i = 0; i < Nch; i++) 
    {        
        databuffer[i].resize(maxbufsamples);
        databuffer[i].fill(0.0);
    }

    switch (filterType)
    {
    case 0:
    {
        WhiteKF* dataproc_kf = new WhiteKF();

        to_prefilter = values[2];

        dataproc_kf->freq = values[3];

        q0 = values[4];
        q1 = values[5];

        dataproc_kf->q = values[6];
        dataproc_kf->r = values[7];

        for (int k = 9; k < values.size(); k++)
        {
            spat_filter[k - 9] = values[k];
        }

        dataproc_kf->srate = srate;
        dataproc_kf->init_params();

        dataprocessor = dataproc_kf;
    }
    break;

    case 1:
    {
        CFIR* dataproc_cfir = new CFIR();

        to_prefilter = values[2];

        dataproc_cfir->freq = values[3];

        q0 = values[4];
        q1 = values[5];

        dataproc_cfir->Ntaps = values[6];

        for (int k = 8; k < values.size(); k++)
        {
            spat_filter[k - 8] = values[k];
        }

        dataproc_cfir->fs = srate;
        dataproc_cfir->init_params();

        dataprocessor = dataproc_cfir;
    }
    break;
    }


}

void DataReceiver::lslDataReceive()
{
    totalsamplesreceived = 0;
    std::vector<std::vector<float>> samples;
    curposidx = 0;


    IIR::ButterworthFilter low_filter;
    low_filter.CreateLowPass(2*M_PI*50.0/srate, 1.0, 2*M_PI*100.0/srate, -6.0);
    IIR::BiquadsCascade low_bqC = low_filter.biquadsCascade;

    IIR::ButterworthFilter high_filter;
    high_filter.CreateHighPass(2*M_PI*4.0/srate, 1.0, 2*M_PI*2.0/srate, -6.0);
    IIR::BiquadsCascade high_bqC = high_filter.biquadsCascade;


    IIR::ButterworthFilter iir_50;
    iir_50.CreateNotch(2*M_PI*50.0/srate,5.0,4);
    IIR::BiquadsCascade iir_50_bqC = iir_50.biquadsCascade;

    IIR::ButterworthFilter iir_100;
    iir_100.CreateNotch(2*M_PI*100.0/srate,5.0,2);
    IIR::BiquadsCascade iir_100_bqC = iir_100.biquadsCascade;


    IIR::ButterworthFilter iir_150;
    iir_150.CreateNotch(2*M_PI*150.0/srate,5.0,2);
    IIR::BiquadsCascade iir_150_bqC = iir_150.biquadsCascade;

    IIR::ButterworthFilter iir_200;
    iir_200.CreateNotch(2*M_PI*200.0/srate,5.0,2);
    IIR::BiquadsCascade iir_200_bqC = iir_200.biquadsCascade;





    /*ButterworthFilter *lowpass = new ButterworthFilter(2, 50.0, 1000.0, Lowpass);
    lowpass->b[0] =  0.020083365564211232;
    lowpass->b[1] =  0.040166731128422464;
    lowpass->b[2] =  0.020083365564211232;

    lowpass->a[0] = 1.0;
    lowpass->a[1] = -1.5610180758007182;
    lowpass->a[2] = 0.6413515380575631;*/

          /*
    ButterworthFilter *bandstop = new ButterworthFilter(2, 50.0, 1000.0, Bandstop);
    bandstop->b[0] =  0.9875889380903246;
    bandstop->b[1] = -1.8786541206154759;
    bandstop->b[2] =  0.9875889380903248;

    bandstop->a[0] = 1.0;
    bandstop->a[1] = -1.8786541206154757;
    bandstop->a[2] = 0.9751778761806491;
*/





        int n_channels = 1;
        std::string name = "SpeedStream";
        std::string type = "Ctrl";

        stream_info *info = new stream_info(name, type, n_channels, srate, lsl::cf_float32, std::string(name) += type);
        outlet = new stream_outlet(*info, 0, 1); //5000 - max buffered samples


    int vec_len;

    //fbwin =new SimpleBarFBWin();
    //fbwin->show();
    //std::vector<float> sample;

    //inlet->pull_sample(sample);
    while(1) {

        //std::this_thread::sleep_for(std::chrono::milliseconds(10));

        inlet->pull_chunk(samples);
        vec_len = (int)samples.size();

        //qDebug()<<samples;

        for (uint i = 0; i<Nch; i++)
        {
            for(int j = 0; j<vec_len;j++)
            {
                //qDebug()<<'here';
                databuffer[i][(curposidx+j)%maxbufsamples] = samples[j][i];

            }

        }


        double spat_filtered;
        for(int i = 0; i<vec_len;i++)
        {
            spat_filtered = 0.0;
            for (uint j = 0; j<Nch; j++)
            {
                spat_filtered += spat_filter[j]*samples[i][j]*1.0;
            }


            if (to_prefilter)
            {

                spat_filtered = low_bqC.ComputeOutput(spat_filtered);
                spat_filtered = high_bqC.ComputeOutput(spat_filtered);
                spat_filtered = iir_50_bqC.ComputeOutput(spat_filtered);
                spat_filtered = iir_100_bqC.ComputeOutput(spat_filtered);
                spat_filtered = iir_150_bqC.ComputeOutput(spat_filtered);
                spat_filtered = iir_200_bqC.ComputeOutput(spat_filtered);
            }

            x = dataprocessor->step(spat_filtered);

            processedbuffer((curposidx+i)%maxbufsamples) =x(0);
            envelopebuffer((curposidx+i)%maxbufsamples) = sqrt(x(0)*x(0)+x(1)*x(1));
            phasebuffer((curposidx+i)%maxbufsamples) = atan(x(0)/x(1)); // ПОка что
            
            out_sample[0] = (float)envelopebuffer((curposidx+i)%maxbufsamples);


                        //out_sample[0] =(out_sample[0]-q0)/(q1-q0);
                        out_sample[0] =(out_sample[0]/q1);// /(q1);
                        if (out_sample[0] <0)
                        {
                            out_sample[0] = 0.0;
                        }
                        //if (out_sample[0] > 1.0)
                        //{
                        //    out_sample[0] = 1.0;
                        //}
                        (*outlet).push_sample(out_sample);
                        if ((curposidx%50) == 0)
                        {
                        //qDebug()<<out_sample[0];
                        }

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
            for (uint i = 0; i<Nch; i++)
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
            for (uint j = 0; j<Nch; j++)
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
