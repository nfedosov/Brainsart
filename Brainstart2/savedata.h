#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <QObject>
#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>


class SaveData : public QObject
{
    Q_OBJECT
public:
    explicit SaveData(QObject *parent = nullptr);


    void saveToFif(Eigen::MatrixXd&,std::string saveDir,int Nch, double srate, QStringList ch_names);

    //int save2gdf(const std::vector<std::string>& argv);

signals:

};

#endif // SAVEDATA_H
