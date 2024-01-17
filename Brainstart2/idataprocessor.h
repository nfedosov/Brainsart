#ifndef IDATAPROCESSOR_H
#define IDATAPROCESSOR_H

using namespace Eigen;

class IDataProcessor
{
public:
    virtual ~IDataProcessor() {}

    virtual Vector2d step(double y);

};

#endif // IDATAPROCESSOR_H
