#include "savedata.h"
//#include "biosig-dev.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

/*
#define T1T2

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>


#include "savedata_def.h"
#include "savedata/fiff_constant.h"
*/

SaveData::SaveData(QObject *parent)
    : QObject{parent}
{

}




#include <iostream>
#include "savedata/fiff.h"
#include "savedata/fiff_info.h"

void SaveData::saveToFif(Eigen::MatrixXd& data)
{
    //const QString & filename = "C:/Users/Fedosov/Documents/projects/Brainstart/records/probe.fif";

    QString filename = "C:/Users/Fedosov/Documents/projects/Brainstart/records/probe.fif";
    QFile file_base(filename);
    FiffStream::SPtr file =FiffStream::start_file(file_base);
    FiffInfo info;


    //Eigen::MatrixXd data(1000, 5);


    info.sfreq = 500.0;
    info.nchan = 5;
    info.chs.resize(info.nchan);
    info.ch_names << "CH1" << "CH2" << "CH3" << "CH4" << "CH5";


    for (int i = 0; i < info.nchan; ++i) {
        info.chs[i].kind = FIFFV_EEG_CH;
        info.chs[i].unit = FIFF_UNIT_V;
        info.chs[i].range = 1e-3;
        info.chs[i].cal = 1.0;
    }

    //file->write_ch_info(info.chs[0]);

        // Write the FiffInfo object to the file
    file->write_id(FIFF_FILE_ID);
 //   file->start_block(FIFFB_MEAS_INFO);

    //file->write_int(FIFF_DATA_BUFFER);
    info.writeToStream(file.data());
//file->end_block(FIFFB_MEAS_INFO);

    file->start_block(FIFFB_RAW_DATA);
    file->write_raw_buffer(data);
    file->end_block(FIFFB_RAW_DATA);

        file->end_file();
}
/*
void saveToFif(const Eigen::MatrixXd& data, const std::string& filename) {
    // Create an empty FIFF file
    FiffStream::SPtr file(new FiffStream);

    // Open the file for writing
    if (!file->open(filename)) {
        std::cerr << "Failed to open file " << filename << " for writing" << std::endl;
        return;
    }

    // Create an info object for the EEG data
    FiffInfo info;
    info.set_meas_date(QDateTime::currentDateTime());
    info.set_ch_names({"Fp1", "Fp2", "F3", "F4", "C3", "C4", "P3", "P4", "O1", "O2", "F7", "F8", "T7", "T8", "P7", "P8", "Fz", "Cz", "Pz"});
    info.set_chs(Eigen::RowVectorXi::LinSpaced(19, 1, 19));
    info.set_nchan(19);
    info.set_sfreq(250.0f);
    info.set_highpass(0.5f);
    info.set_lowpass(40.0f);

    // Write the info object to the FIFF file
    if (!file->start_block(FIFFB_MEAS)) {
        std::cerr << "Failed to start block FIFFB_MEAS" << std::endl;
        return;
    }
    if (!file->write_id(FIFF_BLOCK_ID)) {
        std::cerr << "Failed to write block ID" << std::endl;
        return;
    }
    if (!file->write_int(FIFF_BLOCK_KIND, FIFFB_MEAS)) {
        std::cerr << "Failed to write block kind" << std::endl;
        return;
    }
    if (!info.writeToStream(file)) {
        std::cerr << "Failed to write FIFF info to file" << std::endl;
        return;
    }
    if (!file->end_block(FIFFB_MEAS)) {
        std::cerr << "Failed to end block FIFFB_MEAS" << std::endl;
        return;
    }

    // Write the EEG data to the FIFF file
    if (!file->start_block(FIFFB_DATA)) {
        std::cerr << "Failed to start block FIFFB_DATA" << std::endl;
        return;
    }
    if (!file->write_int(FIFF_BLOCK_KIND, FIFFB_DATA)) {
        std::cerr << "Failed to write block kind" << std::endl;
        return;
    }
    if (!file->start_block(FIFFB_RAW_DATA)) {
        std::cerr << "Failed to start block FIFFB_RAW_DATA" << std::endl;
        return;
    }
    if (!file->write_double(FIFF_MNE_SAMPLE_DATA, data)) {
        std::cerr << "Failed to write EEG data to file" << std::endl;
        return;
    }
    if (!file->end_block(FIFFB_RAW_DATA)) {
        std::cerr << "Failed to end block FIFFB_RAW_DATA" << std::endl;
        return;
    }
    if (!file->end_block(FIFFB_DATA)) {
        std::cerr << "Failed to end block FIFFB_DATA" << std::endl;
        return;
    }

    // Close the file
    file->

*/


