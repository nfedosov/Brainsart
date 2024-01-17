#ifndef CFIR_H
#define CFIR_H

#include "idataprocessor.h"

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

    void grid_search();

};

#endif // CFIR_H
