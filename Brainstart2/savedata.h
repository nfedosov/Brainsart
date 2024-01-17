#ifndef SAVEDATA_H
#define SAVEDATA_H


class SaveData : public QObject
{
    Q_OBJECT
public:
    explicit SaveData(QObject *parent = nullptr);


    void saveToFif(Eigen::MatrixXd&,std::string saveDir,int Nch, double srate, QList<QString> ch_names);

    //int save2gdf(const std::vector<std::string>& argv);

signals:

};

#endif // SAVEDATA_H
