#include <complex>
#include <cmath>

#include "./eigen-3.4.0/Eigen/Dense"

using namespace Eigen;;

#ifndef UTILS_H
#define UTILS_H


Eigen::VectorXcd hilbert(const Eigen::VectorXd& x, int N = -1);

#endif // UTILS_H
