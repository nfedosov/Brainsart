#include "firwin.h"
#include <cmath>
#include <string>
#include <vector>


#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>
#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/unsupported/Eigen/FFT>



using namespace std;

FirWin::FirWin(int numtaps, double low_cutoff, double high_cutoff, double fs, int Nch, string window)
{




    h.resize(numtaps);
    double low = low_cutoff / fs;
    double high = high_cutoff / fs;
    double pi = M_PI;
    double n = numtaps - 1;
    for (int i = 0; i <= n; i++) {
        if (i == n / 2) {
            h[i] = 2.0 * (high - low);
        } else {
            h[i] = (sin(2.0 * pi * high * (i - n / 2.0)) - sin(2.0 * pi * low * (i - n / 2.0))) / (pi * (i - n / 2.0));
        }
    }
    std::vector<double> w(numtaps);
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
    for (int i = 0; i <= n; i++) {
        h[i] *= w[i];
    }







    // INIT FILTERING UTILS, BUFFER
    buf.resize(Nch);
        for (int i = 0; i < Nch; i++) {
            buf[i].resize(numtaps);
            fill(buf[i].begin(), buf[i].end(), 0);
        }
}










