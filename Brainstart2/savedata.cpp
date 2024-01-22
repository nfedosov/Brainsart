#include "stdafx.h"

#include "FIFF/fiff.h"
#include "FIFF/fiff_info.h"
#include "FIFF/fiff_id.h"

SaveData::SaveData(QObject *parent)
    : QObject{parent}
{

}

void SaveData::saveToFif(Eigen::MatrixXd& data,std::string saveDir, int Nch,double Fs, QList<QString> ch_names)
{
    QString filename = QString::fromLocal8Bit(saveDir.c_str());
    QFile file_base(filename);
    FiffStream::SPtr file =FiffStream::start_file(file_base);
    FiffInfo info;

    info.sfreq = Fs;
    info.nchan = Nch;

    for (int i = 0; i < info.nchan; ++i) {

        FiffChInfo channelInfo;

        channelInfo.kind = FIFFV_EEG_CH;
        channelInfo.unit = FIFF_UNIT_V;
        channelInfo.range = (float)1e-3;
        channelInfo.cal = 1.0;
        channelInfo.ch_name =ch_names[i];

        info.chs.append(channelInfo);
    }
    
    // Write the FiffInfo object to the file
    file->write_id(FIFF_FILE_ID);
    info.writeToStream(file.data());

    file->start_block(FIFFB_RAW_DATA);
    file->write_raw_buffer(data);
    file->end_block(FIFFB_RAW_DATA);

    file->end_file();
}
