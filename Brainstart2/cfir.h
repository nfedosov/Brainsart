#ifndef CFIR_H
#define CFIR_H
#include "idataprocessor.h"
#include <string>

#include <string>
#include <vector>

#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>


using namespace std;
using namespace Eigen;

class CFIR: public IDataProcessor
{
public:
    CFIR();//(int numtaps, double low_cutoff, double high_cutoff, double fs, string window);


    Eigen::VectorXd h_real;
    Eigen::VectorXd h_imag;
    Vector2d x = VectorXd(2);
    VectorXd buf;
    int pos_in_buf = 0;
    int Ntaps;
    double fs;

    double freq;


    Vector2d step(double) override;
    void init_params();



    //step();

    //lfilter(init_z);
    //filtfilt();
    void grid_search();


};

#endif // CFIR_H
