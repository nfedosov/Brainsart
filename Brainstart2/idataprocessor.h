#ifndef IDATAPROCESSOR_H
#define IDATAPROCESSOR_H

#include <QVector>
#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>

using namespace Eigen;

class IDataProcessor
{
public:
    virtual ~IDataProcessor() {}
    //virtual void lfilter(QVector<double> data) = 0;
    //virtual void doSomething() = 0;
    virtual Vector2d step(double y);





};

#endif // IDATAPROCESSOR_H
