#ifndef FIFF_H
#define FIFF_H


//=============================================================================================================
// INCLUDES
//=============================================================================================================

//#include "fiff_global.h"
#include "fiff_types.h"
#include "fiff_id.h"


//#include "fiff_dir_node.h"
//#include "fiff_dir_entry.h"

//=============================================================================================================
// EIGEN INCLUDES
//=============================================================================================================

//#include <Eigen/Core>
//#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Core>
//#include <Eigen/SparseCore>
//#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/SparseCore>


//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

//=============================================================================================================
// DEFINE NAMESPACE FIFFLIB
//=============================================================================================================



//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

class FiffStream;
class FiffTag;
class FiffCtfComp;
class FiffRawData;
class FiffInfo;
class FiffInfoBase;
class FiffCov;
class FiffProj;
class FiffNamedMatrix;
class FiffDigPoint;
class FiffChInfo;
class FiffChPos;
class FiffCoordTrans;
class FiffDigitizerData;


using namespace Eigen;
//=============================================================================================================
/**
 * FiffStream provides an interface for reading from and writing to fiff files
 * Comparable to: fiffFile (struct *fiffFile,fiffFileRec)
 *
 * @brief FIFF File I/O routines.
 **/

class FiffStream : public QDataStream
{
public:
    typedef QSharedPointer<FiffStream> SPtr;            /**< Shared pointer type for FiffStream. */
    typedef QSharedPointer<const FiffStream> ConstSPtr; /**< Const shared pointer type for FiffStream. */

    //=========================================================================================================
    /**
     * Constructs a fiff stream that uses the I/O device p_pIODevice.
     *
     * @param[in] p_pIODevice    A fiff IO device like a fiff QFile or QTCPSocket.
     */
    explicit FiffStream(QIODevice *p_pIODevice);

    static FiffStream::SPtr start_file(QIODevice& p_IODevice);

    QString streamName();
    fiff_long_t write_id(fiff_int_t kind, const FiffId& id = FiffId::getDefault());
    fiff_long_t write_int(fiff_int_t kind, const fiff_int_t* data, fiff_int_t nel = 1, fiff_int_t next = FIFFV_NEXT_SEQ);
    fiff_long_t write_string(fiff_int_t kind, const QString& data);
    fiff_long_t write_name_list(fiff_int_t kind, const QStringList& data);
    fiff_long_t write_float(fiff_int_t kind, const float* data, fiff_int_t nel = 1);
    fiff_long_t write_ch_info(const FiffChInfo& ch);
    bool write_raw_buffer(const MatrixXd& buf);
    fiff_long_t write_ch_pos(const FiffChPos &chpos);

    void end_file();

    fiff_long_t start_block(fiff_int_t kind);
    fiff_long_t end_block(fiff_int_t kind, fiff_int_t next = FIFFV_NEXT_SEQ);


};


#endif // FIFF_H
