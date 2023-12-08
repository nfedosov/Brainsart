#include "cfir.h"

#include <cmath>
#include <string>
#include <vector>
#include "utils.h"

#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>



using namespace std;
CFIR::CFIR()//(int numtaps, double low_cutoff, double high_cutoff, double fs, string window)

{
    int numtaps = 101 ;//100
    freq =10.0;
    fs = 500.0;
    string window = "hamming";
    Ntaps = numtaps;
    // default params
}

void CFIR::init_params()
{

    string window = "hamming";

    double  low_cutoff = freq-1.0;
    double high_cutoff = freq+1.0;

    h_real.resize(Ntaps);
    h_imag.resize(Ntaps);
    double low = low_cutoff / fs;
    double high = high_cutoff / fs;
    double pi = M_PI;
    double n = Ntaps - 1;
    for (int i = 0; i <= n; i++) {
        if (i == n / 2) {
            h_real[i] = 2.0 * (high - low);
        } else {
            h_real[i] = (sin(2.0 * pi * high * (i - n / 2.0)) - sin(2.0 * pi * low * (i - n / 2.0))) / (pi * (i - n / 2.0));
        }
    }
    std::vector<double> w(Ntaps);
    if (window == "hamming") {
        for (int i = 0; i <= n; i++) {
            w[i] = 0.54 - 0.46 * cos(2.0 * pi * i / n);
        }
    } else if (window == "hann") {
        for (int i = 0; i <= n; i++) {
            w[i] = 0.5 * (1.0 - cos(2.0 * pi * i / n));
        }
    } else if (window == "blackman") {
        double a0 = 0.42659;
        double a1 = 0.49656;
        double a2 = 0.076849;
        for (int i = 0; i <= n; i++) {
            w[i] = a0 - a1 * cos(2.0 * pi * i / n) + a2 * cos(4.0 * pi * i / n);
        }
    } else {
        for (int i = 0; i <= n; i++) {
            w[i] = 1.0;
        }
    }

    double sum = 0.0;
    for (int i = 0; i <= n; i++) {
        h_real[i] *= w[i];
        sum += h_real[i]*h_real[i];
    }

    sum = std::sqrt(sum);
    for (int i = 0; i <= n; i++) {

        h_real[i] /= sum;
    }


    buf.resize(Ntaps);
    buf.fill(0);





    Eigen::Map<Eigen::VectorXd> h_eigen(h_real.data(), h_real.size());
    Eigen::VectorXcd h_complex = hilbert(h_eigen);
    h_real = h_complex.real();
    h_imag = h_complex.imag();

    pos_in_buf = 0;



}


Vector2d CFIR::step(double y)
{

    buf(pos_in_buf) = y;

    x(0) =0;
    x(1) =0;
    for (int i = pos_in_buf; i < pos_in_buf+Ntaps; i++)
    {
        x(0) += h_real(i-pos_in_buf)*buf(i%Ntaps);
        x(1) += h_imag(i-pos_in_buf)*buf(i%Ntaps);
    }

    pos_in_buf ++;
    if (pos_in_buf >= Ntaps)
    {
       pos_in_buf %=Ntaps;
    }


    return x;
}



void CFIR::grid_search()
{

}







