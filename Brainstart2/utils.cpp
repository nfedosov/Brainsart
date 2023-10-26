#include <complex>
#include <cmath>

#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>

using namespace Eigen;
using namespace std;


struct cor_delay {
    double quotient;
    int remainder;
};



// z - previous state vector
//double simple_matsuda_generator(double *z)
//{
//
//}


Eigen::VectorXcd hilbert(const Eigen::VectorXd& x, int N = -1) {
    // Compute the Hilbert transform of the input signal x using the FFT method.

    if (N == -1) {
        N = x.size();
    }
    if (N <= 0) {
        throw std::invalid_argument("N must be positive.");
    }

    Eigen::VectorXd x_ = x;



    MatrixXcd dft(N, N);
    for (int k = 0; k < N; k++) {
      for (int n = 0; n < N; n++) {
        complex<double> arg = -(2.0 * M_PI * 1i * complex<double>(k * n)) / complex<double>(N);
        dft(k,n) = exp(arg);
      }
    }
/*
    MatrixXcd dft(N, N);
      for (int k = 0; k < N; k++) {
        for (int n = 0; n < N; n++) {

          //complex<double> arg = -(2.0 * M_PI * 1i * (complex<double>(k * n)))/(complex<double> N);
          complex<double> arg = -(2.0 * M_PI * 1i * (complex<double>(k * n))) / (complex<double>(N));
          dft(k,n) = exp((_Complex double)(arg));
          //dft(k, n) = static_cast<_Complex double>(exp(static_cast<_Complex double>((-2.0 * M_PI * 1i * k * n) / N)));
        }
     }*/

      // Compute the DFT of the input signal
     VectorXcd Xf = dft * x;


    Eigen::VectorXd h(N);

    if (N % 2 == 0) {
        h(0) = h(N / 2) = 1;
        h.segment(1, N / 2 - 1).setConstant(2);
    } else {
        h(0) = 1;
        h.segment(1, (N + 1) / 2 - 1).setConstant(2);
    }

    Eigen::VectorXcd y;
    y = Xf.array()*h.array();
    MatrixXcd idft(N, N);
    idft = dft.conjugate().transpose();
    Eigen::VectorXcd y_complex = idft*h;


    return y_complex;
}







//a - reference signal
//b - tested signal(with delay)

/*
cor_delay xcor(double *x, double *y, int shift, int Len)
{

    //int Len = sizeof(x)/sizeof(double);
    double cor[shift];

    double sumsqra = 0.0;
    double sumsqrb = 0.0;


    for(int bias = 0; bias < shift; bias++)
    {
        cor[bias] = 0.0;
        for(int i = 0; i<Len-bias; i++)
        {
            cor[bias] += x[i]*y[i+bias];
            sumsqra += x[i]*x[i];
            sumsqrb = y[i]*y[i];
        }
    }
}
*/
