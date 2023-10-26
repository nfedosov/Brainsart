#include "whitekf.h"
#include "math.h"
#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>
using namespace Eigen;



WhiteKF::WhiteKF()
{

    H << 1.0,0.0;

    init_params(default_freq,default_srate,default_A,default_r,default_q);

}

// y - observation
Vector2d WhiteKF::step(double y)
{
    x = Psi*x;
    P = Psi*P*(Psi.transpose())+Q;
    res(0,0) = y-H*x;
    S = H*P*H.transpose()+R;
    K = P*H.transpose()*S.completeOrthogonalDecomposition().pseudoInverse();
    x = x+K*res;
    P = (MatrixXd(2,2).Identity(2,2)-K*H)*P;
    return x;
}


//void WhiteKF::lfilter(QVector<double> data)
//{
    /*int i = 0;

    for(i = 0; i < size; i++)
    {
        step(data[i]);
        filtered[i] = x(0);
        envelope[i] = sqrt(x(0)*x(0)+x(1)*x(1));
    }*/
//}


void WhiteKF::reset_states()
{
    x.setZero();
    P.Identity();
}

void WhiteKF:: init_params(double freq, double srate, double A, double r, double q)
{
    R << r;
    Q << q,0.0,0.0,q;

    this->A = A;
    this->freq = freq;
    this->srate = srate;

    double arg = 2.0*M_PI*freq/srate;
    Psi<< cos(arg), -sin(arg), sin(arg), cos(arg);

    reset_states();
}


void grid_search(double *q_range, double *r_range)
{
    //q_size = sizeof(q_range);
    //r_size = sizeof(r_range);
    //for(int i= 0; i<q_size;i++)
    //init_states();
    //init_params(...)

}
