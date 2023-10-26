#include "fiff.h"
#include "fiff_constants.h"




#define MALLOC_54(x,t) (t *)malloc((x)*sizeof(t))

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include <iostream>
#include <time.h>
#include "fiff_ch_info.h"


#include <QFile>

//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace FIFFLIB;

//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

FiffStream::FiffStream(QIODevice *p_pIODevice)
: QDataStream(p_pIODevice)
{
    this->setFloatingPointPrecision(QDataStream::SinglePrecision);
    this->setByteOrder(QDataStream::BigEndian);
    this->setVersion(QDataStream::Qt_5_0);
}

void FiffStream::end_file()
{
    fiff_int_t datasize = 0;

     *this << (qint32)FIFF_NOP;
     *this << (qint32)FIFFT_VOID;
     *this << (qint32)datasize;
     *this << (qint32)FIFFV_NEXT_NONE;
}
fiff_long_t FiffStream::start_block(fiff_int_t kind)
{
    return this->write_int(FIFF_BLOCK_START,&kind);
}



FiffStream::SPtr FiffStream::start_file(QIODevice& p_IODevice)
{
    FiffStream::SPtr p_pStream(new FiffStream(&p_IODevice));
    QString t_sFileName = p_pStream->streamName();

    if(!p_pStream->device()->open(QIODevice::WriteOnly))
    {
        qWarning("Cannot write to %s\n", t_sFileName.toUtf8().constData());//consider throw
        FiffStream::SPtr p_pEmptyStream;
        return p_pEmptyStream;
    }

    //
    //   Write the compulsory items
    //
    p_pStream->write_id(FIFF_FILE_ID);//1
    int null_pointer = FIFFV_NEXT_NONE;
    p_pStream->write_int(FIFF_DIR_POINTER,&null_pointer);//2
    p_pStream->write_int(FIFF_FREE_LIST,&null_pointer);//3
    //
    //   Ready for more
    //
    return p_pStream;
}

QString FiffStream::streamName()
{
    QFile* t_pFile = qobject_cast<QFile*>(this->device());
    QString p_sFileName;

    p_sFileName = t_pFile->fileName();
    return p_sFileName;
}




fiff_long_t FiffStream::write_id(fiff_int_t kind, const FiffId& id)
{
    fiff_long_t pos = this->device()->pos();

    FiffId t_id = id;
/*
    if(t_id.isEmpty()) {
        //
        // Create a new one
        //
        t_id = FiffId::new_file_id();
    }*/

    //
    //
    fiff_int_t datasize = 5*4;                       //   The id comprises five integers

     *this << (qint32)kind;
     *this << (qint32)FIFFT_ID_STRUCT;
     *this << (qint32)datasize;
     *this << (qint32)FIFFV_NEXT_SEQ;
    //
    // Collect the bits together for one write
    //
    qint32 data[5];
    data[0] = t_id.version;
    data[1] = t_id.machid[0];
    data[2] = t_id.machid[1];
    data[3] = t_id.time.secs;
    data[4] = t_id.time.usecs;

    for(qint32 i = 0; i < 5; ++i)
        *this << data[i];

    return pos;
}



fiff_long_t FiffStream::write_int(fiff_int_t kind, const fiff_int_t* data, fiff_int_t nel, fiff_int_t next)
{
    fiff_long_t pos = this->device()->pos();

    fiff_int_t datasize = nel * 4;

     *this << (qint32)kind;
     *this << (qint32)FIFFT_INT;
     *this << (qint32)datasize;
     *this << (qint32)next;

    for(qint32 i = 0; i < nel; ++i)
        *this << data[i];

    return pos;
}


fiff_long_t FiffStream::write_string(fiff_int_t kind,
                                     const QString& data)
{
    fiff_long_t pos = this->device()->pos();

    fiff_int_t datasize = data.size();
     *this << (qint32)kind;
     *this << (qint32)FIFFT_STRING;
     *this << (qint32)datasize;
     *this << (qint32)FIFFV_NEXT_SEQ;

    this->writeRawData(data.toUtf8().constData(),datasize);

    return pos;
}

fiff_long_t FiffStream::end_block(fiff_int_t kind, fiff_int_t next)
{
    return this->write_int(FIFF_BLOCK_END,&kind,1,next);
}


fiff_long_t FiffStream::write_name_list(fiff_int_t kind, const QStringList& data)
{
    QString all = data.join(":");
    return this->write_string(kind,all);
}


fiff_long_t FiffStream::write_float(fiff_int_t kind, const float* data, fiff_int_t nel)
{
    fiff_long_t pos = this->device()->pos();

    qint32 datasize = nel * 4;

    *this << (qint32)kind;
    *this << (qint32)FIFFT_FLOAT;
    *this << (qint32)datasize;
    *this << (qint32)FIFFV_NEXT_SEQ;

    for(qint32 i = 0; i < nel; ++i)
        *this << data[i];

    return pos;
}



fiff_long_t FiffStream::write_ch_info(const FiffChInfo& ch)
{
    fiff_long_t pos = this->device()->pos();

    //typedef struct _fiffChPosRec {
    //  fiff_int_t   coil_type;          /*!< What kind of coil. */
    //  fiff_float_t r0[3];              /*!< Coil coordinate system origin */
    //  fiff_float_t ex[3];              /*!< Coil coordinate system x-axis unit vector */
    //  fiff_float_t ey[3];              /*!< Coil coordinate system y-axis unit vector */
    //  fiff_float_t ez[3];             /*!< Coil coordinate system z-axis unit vector */
    //} fiffChPosRec,*fiffChPos;        /*!< Measurement channel position and coil type */

    //typedef struct _fiffChInfoRec {
    //  fiff_int_t    scanNo;        /*!< Scanning order # */
    //  fiff_int_t    logNo;         /*!< Logical channel # */
    //  fiff_int_t    kind;          /*!< Kind of channel */
    //  fiff_float_t  range;         /*!< Voltmeter range (only applies to raw data ) */
    //  fiff_float_t  cal;           /*!< Calibration from volts to... */
    //  fiff_ch_pos_t chpos;         /*!< Channel location */
    //  fiff_int_t    unit;          /*!< Unit of measurement */
    //  fiff_int_t    unit_mul;      /*!< Unit multiplier exponent */
    //  fiff_char_t   ch_name[16];   /*!< Descriptive name for the channel */
    //} fiffChInfoRec,*fiffChInfo;   /*!< Description of one channel */
    fiff_int_t datasize= 4*13 + 4*7 + 16;

     *this << (qint32)FIFF_CH_INFO;
     *this << (qint32)FIFFT_CH_INFO_STRUCT;
     *this << (qint32)datasize;
     *this << (qint32)FIFFV_NEXT_SEQ;

    //
    //   Start writing fiffChInfoRec
    //
     *this << (qint32)ch.scanNo;
     *this << (qint32)ch.logNo;
     *this << (qint32)ch.kind;

     *this << ch.range;
     *this << ch.cal;

    //
    //   FiffChPos follows
    //
    write_ch_pos(ch.chpos);

    //
    //   unit and unit multiplier
    //
     *this << (qint32)ch.unit;
     *this << (qint32)ch.unit_mul;

    //
    //   Finally channel name
    //
    fiff_int_t len = ch.ch_name.size();
    QString ch_name;
    if(len > 15)
        ch_name = ch.ch_name.mid(0, 15);
    else
        ch_name = ch.ch_name;

    len = ch_name.size();

    this->writeRawData(ch_name.toUtf8().constData(),len);

    if (len < 16) {
        const char* chNull = "";
        for(qint32 i = 0; i < 16-len; ++i)
            this->writeRawData(chNull,1);
    }

    return pos;
}


fiff_long_t FiffStream::write_ch_pos(const FiffChPos &chpos)
{
    fiff_long_t pos = this->device()->pos();

    //
    //   FiffChPos
    //
     *this << (qint32)chpos.coil_type;

    qint32 i;
    // r0
    for(i = 0; i < 3; ++i)
        *this << chpos.r0[i];
    // ex
    for(i = 0; i < 3; ++i)
        *this << chpos.ex[i];
    // ey
    for(i = 0; i < 3; ++i)
        *this << chpos.ey[i];
    // ez
    for(i = 0; i < 3; ++i)
        *this << chpos.ez[i];

    return pos;
}


bool FiffStream::write_raw_buffer(const MatrixXd& buf)
{
    Eigen::MatrixXf tmp = buf.cast<float>();
    this->write_float(FIFF_DATA_BUFFER,tmp.data(),tmp.rows()*tmp.cols());
    return true;
}
