//=============================================================================================================
/**
 * @file     fiff_constants.h
 * @author   Lorenz Esch <lesch@mgh.harvard.edu>;
 *           Matti Hamalainen <msh@nmr.mgh.harvard.edu>;
 *           Christoph Dinh <chdinh@nmr.mgh.harvard.edu>
 * @since    0.1.0
 * @date     July, 2012
 *
 * @section  LICENSE
 *
 * Copyright (C) 2012, Lorenz Esch, Matti Hamalainen, Christoph Dinh. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 * the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 *       following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 *       the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
 *       to endorse or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @brief    Fiff constants
 *
 */

#ifndef FIFF_CONSTANTS_H
#define FIFF_CONSTANTS_H

//=============================================================================================================
// DEFINE NAMESPACE FIFFLIB
//=============================================================================================================

namespace FIFFLIB
{

//=============================================================================================================
// FIFF constants
//=============================================================================================================

#define FIFFT_VOID           0
#define FIFFT_INT            3
#define FIFFT_FLOAT          4
#define FIFFT_STRING         10
#define FIFFT_CH_INFO_STRUCT 30
#define FIFFT_ID_STRUCT      31
#define FIFF_FILE_ID         100
#define FIFFB_MEAS           100
#define FIFF_DIR_POINTER     101
#define FIFFB_MEAS_INFO      101
#define FIFFB_RAW_DATA       102
#define FIFF_BLOCK_ID        103
#define FIFF_BLOCK_START     104
#define FIFF_BLOCK_END       105
#define FIFF_FREE_LIST       106
#define FIFF_NOP             108
#define FIFF_PARENT_BLOCK_ID 110
#define FIFFB_DACQ_PARS      117
#define FIFF_DACQ_PARS       150
#define FIFF_DACQ_STIM       151
#define FIFF_CH_INFO         203


#define FIFF_NCHAN           200        /**< Number of channels. */
#define FIFF_SFREQ           201        /**< Sampling frequency (Hz). */
#define FIFF_DATA_PACK       202        /**< How the raw data is packed. */
#define FIFF_CH_INFO         203        /**< Channel descriptor. */
#define FIFF_MEAS_DATE       204        /**< Measurement date. */
#define FIFF_SUBJECT         205        /**< This might be deleted later. */
#define FIFF_COMMENT         206        /**< This is used in a questionable way... */
#define FIFF_NAVE            207        /**< Number of averages. */
#define FIFF_FIRST_SAMPLE    208        /**< The first sample of an epoch. */
#define FIFF_LAST_SAMPLE     209        /**< The last sample of an epoch. */
#define FIFF_ASPECT_KIND     210        /**< Aspect label. */
#define FIFF_REF_EVENT       211        /**< Reference event. */
#define FIFF_EXPERIMENTER    212        /**< Experimenter name. */
#define FIFF_DIG_POINT       213        /**< Digitization point. */
#define FIFF_CH_POS_VEC      214        /**< Channel positions. */
#define FIFF_HPI_SLOPES      215        /**< HPI data. */
#define FIFF_HPI_NCOIL       216        /**< Number of HPI coils. */
#define FIFF_REQ_EVENT       217        /**< Required event. */
#define FIFF_REQ_LIMIT       218        /**< Window for required event. */
#define FIFF_LOWPASS         219        /**< Analog lowpass. */
#define FIFF_BAD_CHS         220        /**< List of bad channels. */
#define FIFF_ARTEF_REMOVAL   221        /**< Artifact removal. */
#define FIFF_COORD_TRANS     222        /**< Coordinate transformation. */
#define FIFF_HIGHPASS        223        /**< Analog highpass. */
#define FIFF_CH_CALS_VEC     224        /**< This will not occur in new files. */
#define FIFF_HPI_BAD_CHS     225        /**< List of channels considered to be bad in hpi. */
#define FIFF_HPI_CORR_COEFF  226        /**< Hpi curve fit correlations. */
#define FIFF_EVENT_COMMENT   227        /**< Comment about the events used in averaging. */
#define FIFF_NO_SAMPLES      228        /**< Number of samples in an epoch. */
#define FIFF_FIRST_TIME      229        /**< Time scale minimum. */
#define FIFF_SUBAVE_SIZE     230        /**< Size of a subaverage. */
#define FIFF_SUBAVE_FIRST    231        /**< The first epoch # contained in the subaverage. */
#define FIFF_NAME            233          /**< Intended to be a short name. */
#define FIFF_DESCRIPTION     FIFF_COMMENT /**< (Textual) Description of an object. */
#define FIFF_DIG_STRING      234          /**< String of digitized points. */

#define FIFF_LINE_FREQ       235          /**< Line interference frequency. */
#define FIFF_HPI_COIL_FREQ   236          /**< HPI coil excitation frequency. */
#define FIFF_SIGNAL_CHANNEL  237          /**< Signal channel name. */

#define FIFF_PROJ_ID           500        /**< Project ID. */
#define FIFF_PROJ_NAME         501        /**< Project name. */
#define FIFF_PROJ_AIM          502        /**< Projct description. */
#define FIFF_PROJ_PERSONS      503        /**< Persons participating in the project. */
#define FIFF_PROJ_COMMENT      504        /**< Comment about the project. */

#define FIFF_GANTRY_ANGLE      282        /**< Tilt angle of the dewar in degrees. */
#define FIFF_DATA_BUFFER       300        /**< Buffer containing measurement data. */

#define FIFFC_MAJOR_VERSION 1L
#define FIFFC_MINOR_VERSION 3L

#define FIFFC_VERSION (FIFFC_MAJOR_VERSION<<16 | FIFFC_MINOR_VERSION)



#define  FIFFC_FAIL -1
#define  FIFFC_OK 0

#define  FIFFV_TRUE  1
#define  FIFFV_FALSE 0

/* Compatibility definitions */

#define  FIFF_FAIL -1
#define  FIFF_OK    0

#define FIFFC_TAG_INFO_SIZE (sizeof(fiffTagRec) - sizeof(fiff_data_t *))
#define FIFFC_DATA_OFFSET FIFFC_TAG_INFO_SIZE
#define FIFFM_TAG_INFO(x) &((x)->kind)

/*
 * Quaternion channels for head position monitoring
 */
#define FIFFV_QUAT_0   700  /**< Quaternion parameter q0; obsolete for unit quaternion. */
#define FIFFV_QUAT_1   701  /**< Quaternion parameter q1; rotation. */
#define FIFFV_QUAT_2   702  /**< Quaternion parameter q2; rotation. */
#define FIFFV_QUAT_3   703  /**< Quaternion parameter q3; rotation. */
#define FIFFV_QUAT_4   704  /**< Quaternion parameter q4; translation. */
#define FIFFV_QUAT_5   705  /**< Quaternion parameter q5; translation. */
#define FIFFV_QUAT_6   706  /**< Quaternion parameter q6; translation. */
#define FIFFV_HPI_G    707  /**< Goodness-of-fit in continuous hpi. */
#define FIFFV_HPI_ERR  708  /**< Estimation error in continuous hpi. */
#define FIFFV_HPI_MOV  709  /**< Estimated head movement speed in continuous hpi. */

#define FIFFM_QUAT_CH(X) ((X >= FIFFV_QUAT_0) && (X <= FIFFV_HPI_MOV))   /* Check for a quaternion channel */

#define FIFFV_DIPOLE_WAVE_CH   1000	   /* Dipole time curve */
#define FIFFV_GOODNESS_FIT_CH  1001	   /* Goodness of fit */

#define FIFFM_CHPOS(x) &((x)->chpos)

/*
 * Units of measurement
 */
#define FIFF_UNIT_NONE -1

/*
 * SI base units
 */
#define FIFF_UNIT_M   1
#define FIFF_UNIT_KG  2
#define FIFF_UNIT_SEC 3
#define FIFF_UNIT_A   4
#define FIFF_UNIT_K   5
#define FIFF_UNIT_MOL 6

/*
 * SI Supplementary units
 */
#define FIFF_UNIT_RAD 7
#define FIFF_UNIT_SR  8

/*
 * SI base candela
 */
#define FIFF_UNIT_CD  9

/*
 * SI derived units
 */
#define FIFF_UNIT_HZ  101
#define FIFF_UNIT_N   102
#define FIFF_UNIT_PA  103
#define FIFF_UNIT_J   104
#define FIFF_UNIT_W   105
#define FIFF_UNIT_C   106
#define FIFF_UNIT_V   107
#define FIFF_UNIT_F   108
#define FIFF_UNIT_OHM 109
#define FIFF_UNIT_MHO 110
#define FIFF_UNIT_WB  111
#define FIFF_UNIT_T   112
#define FIFF_UNIT_H   113
#define FIFF_UNIT_CEL 114
#define FIFF_UNIT_LM  115
#define FIFF_UNIT_LX  116

/*
 * Others we need
 */
#define FIFF_UNIT_T_M   201   /* T/m */
#define FIFF_UNIT_AM    202   /* Am  */
#define FIFF_UNIT_AM_M2 203   /* Am/m^2  */
#define FIFF_UNIT_AM_M3 204   /* Am/m^3  */

/*
 * Multipliers
 */
#define FIFF_UNITM_E   18
#define FIFF_UNITM_PET 15
#define FIFF_UNITM_T   12
#define FIFF_UNITM_MEG  6
#define FIFF_UNITM_K    3
#define FIFF_UNITM_H    2
#define FIFF_UNITM_DA   1
#define FIFF_UNITM_NONE 0
#define FIFF_UNITM_D   -1
#define FIFF_UNITM_C   -2
#define FIFF_UNITM_M   -3
#define FIFF_UNITM_MU  -6
#define FIFF_UNITM_N   -9
#define FIFF_UNITM_P  -12
#define FIFF_UNITM_F  -15
#define FIFF_UNITM_A  -18

/*
 * Coil types
 */
#define FIFFV_COIL_NONE                         0       /**< The location info contains no data. */
#define FIFFV_COIL_EEG                          1       /**< EEG electrode position in r0. */
#define FIFFV_COIL_NM_122                       2       /**< Neuromag 122 coils. */
#define FIFFV_COIL_NM_24                        3       /**< Old 24 channel system in HUT. */
#define FIFFV_COIL_NM_MCG_AXIAL                 4       /**< The axial devices in the HUCS MCG system. */
#define FIFFV_COIL_EEG_BIPOLAR                  5       /**< Bipolar EEG lead. */
#define FIFFV_COIL_DIPOLE                       200     /**< Time-varying dipole definition The coil info contains dipole location (r0) and direction (ex). */
#define FIFFV_COIL_MCG_42                       1000    /**< For testing the MCG software. */
#define FIFFV_COIL_POINT_MAGNETOMETER           2000    /**< Simple point magnetometer. */
#define FIFFV_COIL_AXIAL_GRAD_5CM               2001    /**< Generic axial gradiometer. */
#define FIFFV_COIL_VV_PLANAR_W                  3011    /**< VV prototype wirewound planar sensor. */
#define FIFFV_COIL_VV_PLANAR_T1                 3012    /**< Vectorview SQ20483N planar gradiometer. */
#define FIFFV_COIL_VV_PLANAR_T2                 3013    /**< Vectorview SQ20483N-A planar gradiometer. */
#define FIFFV_COIL_VV_PLANAR_T3                 3014    /**< Vectorview SQ20950N planar gradiometer. */
#define FIFFV_COIL_VV_MAG_W                     3021    /**< VV prototype wirewound magnetometer. */
#define FIFFV_COIL_VV_MAG_T1                    3022    /**< Vectorview SQ20483N magnetometer. */
#define FIFFV_COIL_VV_MAG_T2                    3023    /**< Vectorview SQ20483-A magnetometer. */
#define FIFFV_COIL_VV_MAG_T3                    3024    /**< Vectorview SQ20950N magnetometer. */
#define FIFFV_COIL_MAGNES_MAG                   4001    /**< Magnes WH magnetometer. */
#define FIFFV_COIL_MAGNES_GRAD                  4002    /**< Magnes WH gradiometer . */
#define FIFFV_COIL_CTF_GRAD                     5001    /**< CTF axial gradiometer. */
#define FIFFV_COIL_CTF_REF_MAG                  5002    /**< CTF reference magnetometer. */
#define FIFFV_COIL_CTF_REF_GRAD                 5003    /**< CTF reference gradiometer. */
#define FIFFV_COIL_CTF_REF_GOFF                 5004    /**< CTF offdiagonal reference gradiometer. */
#define FIFFV_COIL_KIT_GRAD                     6001    /**< KIT MIT gradiometer. */
#define FIFFV_COIL_KIT_REF_GRAD                 6002    /**< KIT MIT reference magnetometer. */
#define FIFFV_COIL_BABY_GRAD                    7001    /**< BabySQUID gradiometers. */
#define FIFFV_COIL_BABY_MAG                     7002    /**< BabyMEG inner layer magnetometers. */
#define FIFFV_COIL_BABY_REF_MAG                 7003    /**< BabyMEG outer layer magnetometers. */
#define FIFFV_COIL_BABY_REF_MAG2                7004    /**< BabyMEG reference magnetometer. */
#define FIFFV_COIL_ARTEMIS123_GRAD              7501    /**< Artemis123 gradiometer. */
#define FIFFV_COIL_ARTEMIS123_REF_MAG           7502    /**< Artemis123 reference magnetometer. */
#define FIFFV_COIL_ARTEMIS123_REF_GRAD          7503    /**< Artemis123 reference gradiometer. */
#define FIFFV_COIL_QUSPIN_ZFOPM_MAG             8001    /**< QuSpin ZFOPM magnetometer. */
#define FIFFV_COIL_KRISS_GRAD                   9001    /**< KRISS gradiometer. */
#define FIFFV_COIL_COMPUMEDICS_ADULT_GRAD       9101    /**< Compumedics adult gradiometer. */
#define FIFFV_COIL_COMPUMEDICS_PEDIATRIC_GRAD   9102    /**< Compumedics pediatric gradiometer. */

#define FIFFM_IS_VV_COIL(c) ((c)/1000 == 3)

/*
 * These are the cardinal points for MCG data
 * Left and right are supposed to be located below the sternum point
 */
#define FIFFV_POINT_CHEST_LEFT      1
#define FIFFV_POINT_CHEST_STERNUM   2
#define FIFFV_POINT_CHEST_RIGHT     3

/*
 * Coordinate transformation descriptor
 */
#define FIFFV_COORD_UNKNOWN     0
#define FIFFV_COORD_DEVICE      1
#define FIFFV_COORD_ISOTRAK     2
#define FIFFV_COORD_HPI         3
#define FIFFV_COORD_HEAD        4
#define FIFFV_COORD_MRI         5
#define FIFFV_COORD_MRI_SLICE   6
#define FIFFV_COORD_MRI_DISPLAY 7
#define FIFFV_COORD_XRAY        FIFF_COORD_MRI
#define FIFFV_COORD_XRAY_SLICE  FIFF_COORD_MRI_SLICE
#define FIFFV_COORD_CT          FIFF_COORD_MRI
#define FIFFV_COORD_CT_SLICE    FIFF_COORD_MRI_SLICE

#define FIFFV_COORD_DICOM_DEVICE     8
#define FIFFV_COORD_IMAGING_DEVICE   9

#define FIFFV_COORD_TORSO      100

#define FIFFV_MOVE         1
#define FIFFV_NO_MOVE      0

/*
 * Artefact removal parameters
 */
#define FIFFV_ARTEF_MAX   100
#define FIFFV_ARTEF_NONE   -1
#define FIFFV_ARTEF_KEEP   0
#define FIFFV_ARTEF_JUMP   1
#define FIFFV_ARTEF_NOJUMP 2

/*
 * The layered sphere model
 */
#define FIFFV_LAYER_BRAIN   1
#define FIFFV_LAYER_CSF     2
#define FIFFV_LAYER_SKULL   3
#define FIFFV_LAYER_HEAD    4

#define FIFFV_XFIT_MAP_SURF_SENSORS 0  /**< Xfit contours on the sensor array. */
#define FIFFV_XFIT_MAP_SURF_HEAD    1  /**< Xfit contours on a head surface. */
#define FIFFV_XFIT_MAP_SURF_SPHERE  2  /**< Xfit contours on a spherical surface. */

/*
 * Pointers
 */
#define FIFFV_NEXT_SEQ    0
#define FIFFV_NEXT_NONE   -1

/*
 * Channel types
 */
#define FIFFV_MAGN_CH    1
#define FIFFV_EL_CH      2
#define FIFFV_MEG_CH     FIFFV_MAGN_CH
#define FIFFV_MCG_CH     201
#define FIFFV_REF_MEG_CH 301    /**< CTF coil and channel type. */
#define FIFFV_EEG_CH     FIFFV_EL_CH
#define FIFFV_STIM_CH      3

#define FIFFV_EOG_CH     202
#define FIFFV_EMG_CH     302
#define FIFFV_ECG_CH     402
#define FIFFV_MISC_CH    502
#define FIFFV_RESP_CH    602   /**< Respiration monitoring*/

/*
 * More of those defined in MNE
 */
#define FIFFV_MNE_SURF_UNKNOWN       -1
#define FIFFV_MNE_SURF_LEFT_HEMI     101
#define FIFFV_MNE_SURF_RIGHT_HEMI    102

/*
 * These relate to the Isotrak data
 */
#define FIFFV_POINT_CARDINAL 1
#define FIFFV_POINT_HPI      2
#define FIFFV_POINT_EEG      3
#define FIFFV_POINT_ECG      FIFFV_POINT_EEG
#define FIFFV_POINT_EXTRA    4

#define FIFFV_POINT_LPA         1
#define FIFFV_POINT_NASION      2
#define FIFFV_POINT_RPA         3

/*
 * These are the MNE fiff definitions
 */
#define FIFFB_MNE                    350
#define FIFFB_MNE_SOURCE_SPACE       351
#define FIFFB_MNE_FORWARD_SOLUTION   352
#define FIFFB_MNE_PARENT_MRI_FILE    353
#define FIFFB_MNE_PARENT_MEAS_FILE   354
#define FIFFB_MNE_COV                355
#define FIFFB_MNE_INVERSE_SOLUTION   356
#define FIFFB_MNE_NAMED_MATRIX       357
#define FIFFB_MNE_ENV                358
#define FIFFB_MNE_BAD_CHANNELS       359
#define FIFFB_MNE_VERTEX_MAP         360
#define FIFFB_MNE_EVENTS             361
#define FIFFB_MNE_MORPH_MAP          362

/*
 * CTF compensation data
 */
#define FIFFB_MNE_CTF_COMP           370
#define FIFFB_MNE_CTF_COMP_DATA      371

/*
 * Fiff tags associated with MNE computations (3500...)
 * 3500... Bookkeeping
 */
#define FIFF_MNE_ROW_NAMES              3502
#define FIFF_MNE_COL_NAMES              3503
#define FIFF_MNE_NROW                   3504
#define FIFF_MNE_NCOL                   3505
#define FIFF_MNE_COORD_FRAME            3506    /**< Coordinate frame employed. Defaults:*/
//FIFFB_MNE_SOURCE_SPACE   #define FIFFV_COORD_MRI
//FIFFB_MNE_FORWARD_SOLUTION   FIFFV_COORD_HEAD
//FIFFB_MNE_INVERSE_SOLUTION   FIFFV_COORD_HEAD
#define FIFF_MNE_CH_NAME_LIST           3507
#define FIFF_MNE_FILE_NAME              3508    /**< This removes the collision with fiff_stream.h (used to be 3501)*/

/*
 * 3510... 3590... Source space or surface
 */
#define FIFF_MNE_SOURCE_SPACE_POINTS        3510    /**< The vertices*/
#define FIFF_MNE_SOURCE_SPACE_NORMALS       3511    /**< The vertex normals*/
#define FIFF_MNE_SOURCE_SPACE_NPOINTS       3512    /**< How many vertices*/
#define FIFF_MNE_SOURCE_SPACE_SELECTION     3513    /**< Which are selected to the source space*/
#define FIFF_MNE_SOURCE_SPACE_NUSE          3514    /**< How many are in use*/
#define FIFF_MNE_SOURCE_SPACE_NEAREST       3515    /**< Nearest source space vertex for all vertices*/
#define FIFF_MNE_SOURCE_SPACE_NEAREST_DIST  3516    /**< Distance to the Nearest source space vertex for all vertices*/
#define FIFF_MNE_SOURCE_SPACE_ID            3517    /**< Identifier*/
#define FIFF_MNE_SOURCE_SPACE_TYPE          3518    /**< Surface or volume*/

#define FIFF_MNE_SOURCE_SPACE_NTRI          3590    /**< Number of triangles*/
#define FIFF_MNE_SOURCE_SPACE_TRIANGLES     3591    /**< The triangulation*/
#define FIFF_MNE_SOURCE_SPACE_NUSE_TRI      3592    /**< Number of triangles corresponding to the number of vertices in use*/
#define FIFF_MNE_SOURCE_SPACE_USE_TRIANGLES 3593    /**< The triangulation of the used vertices in the source space*/

#define FIFF_MNE_SOURCE_SPACE_VOXEL_DIMS    3596    /**< Voxel space dimensions in a volume source space*/
#define FIFF_MNE_SOURCE_SPACE_INTERPOLATOR  3597    /**< Matrix to interpolate a volume source space into a mri volume*/
#define FIFF_MNE_SOURCE_SPACE_MRI_FILE      3598    /**< MRI file used in the interpolation*/

#define FIFF_MNE_SOURCE_SPACE_DIST          3599    /**< Distances between vertices in use (along the surface)*/
#define FIFF_MNE_SOURCE_SPACE_DIST_LIMIT    3600    /**< If distance is above this limit (in the volume) it has not been calculated*/

/*
 * 3520... Forward solution
 */
#define FIFF_MNE_FORWARD_SOLUTION       3520
#define FIFF_MNE_SOURCE_ORIENTATION     3521    /**< Fixed or free*/
#define FIFF_MNE_INCLUDED_METHODS       3522
#define FIFF_MNE_FORWARD_SOLUTION_GRAD  3523

/*
 * 3530... Covariance matrix
 */
#define FIFF_MNE_COV_KIND               3530    /**< What kind of a covariance matrix*/
#define FIFF_MNE_COV_DIM                3531    /**< Matrix dimension*/
#define FIFF_MNE_COV                    3532    /**< Full matrix in packed representation (lower triangle)*/
#define FIFF_MNE_COV_DIAG               3533    /**< Diagonal matrix*/
#define FIFF_MNE_COV_EIGENVALUES        3534    /**< Eigenvalues and eigenvectors of the above*/
#define FIFF_MNE_COV_EIGENVECTORS       3535
#define FIFF_MNE_COV_NFREE              3536    /**< Number of degrees of freedom*/

/*
 * 3540... Inverse operator
 * We store the inverse operator as the eigenleads  eigenfields and weights
 */
#define FIFF_MNE_INVERSE_LEADS              3540     /**< The eigenleads*/
#define FIFF_MNE_INVERSE_LEADS_WEIGHTED     3546     /**< The eigenleads (already weighted with R^0.5)*/
#define FIFF_MNE_INVERSE_FIELDS             3541     /**< The eigenfields*/
#define FIFF_MNE_INVERSE_SING               3542     /**< The singular values*/
#define FIFF_MNE_PRIORS_USED                3543     /**< Which kind of priors have been used for the source covariance matrix*/
#define FIFF_MNE_INVERSE_FULL               3544     /**< Inverse operator as one matrix*/
                                   // This matrix includes the whitening operator as well
                               // The regularization is applied
#define FIFF_MNE_INVERSE_SOURCE_ORIENTATIONS 3545    /**<  orientation of one source per row*/
                               // The source orientations must be expressed in the coordinate system
                               // given by FIFF_MNE_COORD_FRAME

/*
 * 3550... Saved environment info
 */
#define FIFF_MNE_ENV_WORKING_DIR        3550       /**< Working directory where the file was created*/
#define FIFF_MNE_ENV_COMMAND_LINE       3551       /**< The command used to create the file*/

/*
 * 3560... Miscellaneous
 */
#define FIFF_MNE_PROJ_ITEM_ACTIVE       3560       /**< Is this projection item active?*/
#define FIFF_MNE_EVENT_LIST             3561       /**< An event list (for STI 014)*/
#define FIFF_MNE_HEMI                   3562       /**< Hemisphere association for general purposes*/

/*
 * 3570... Morphing maps
 */
#define FIFF_MNE_MORPH_MAP              3570       /**< Mapping of closest vertices on the sphere*/
#define FIFF_MNE_MORPH_MAP_FROM         3571       /**< Which subject is this map from*/
#define FIFF_MNE_MORPH_MAP_TO           3572       /**< Which subject is this map to*/

/*
 * 3580... CTF compensation data
 */
#define FIFF_MNE_CTF_COMP_KIND         3580       /**< What kind of compensation*/
#define FIFF_MNE_CTF_COMP_DATA         3581       /**< The compensation data itself*/
#define FIFF_MNE_CTF_COMP_CALIBRATED   3582       /**< Are the coefficients calibrated?*/

/*
 * 3700... Real-Time Communication
 */
#define FIFF_MNE_RT_COMMAND         3700              /**< Fiff Real-Time Command. */
#define FIFF_MNE_RT_CLIENT_ID       3701              /**< Fiff Real-Time mne_t_server client id. */

/*
 * 3710... Real-Time Blocks
 */
#define FIFFB_MNE_RT_MEAS_INFO      3710              /**< Fiff Real-Time Measurement Info. */

/*
 * Fiff values associated with MNE computations
 */
#define FIFFV_MNE_FIXED_ORI            1
#define FIFFV_MNE_FREE_ORI             2

#define FIFFV_MNE_MEG                  1
#define FIFFV_MNE_EEG                  2
#define FIFFV_MNE_MEG_EEG              3

#define FIFFV_MNE_UNKNOWN_COV          0
#define FIFFV_MNE_SENSOR_COV           1
#define FIFFV_MNE_NOISE_COV            1           /**< This is what it should have been called*/
#define FIFFV_MNE_SOURCE_COV           2
#define FIFFV_MNE_FMRI_PRIOR_COV       3
#define FIFFV_MNE_SIGNAL_COV           4           /**< This will be potentially employed in beamformers*/
#define FIFFV_MNE_DEPTH_PRIOR_COV      5           /**< The depth weighting prior*/
#define FIFFV_MNE_ORIENT_PRIOR_COV     6           /**< The orientation prior*/

/*
 * Source space types (values of FIFF_MNE_SOURCE_SPACE_TYPE)
 */
#define FIFFV_MNE_SPACE_UNKNOWN  -1
#define FIFFV_MNE_SPACE_SURFACE  1
#define FIFFV_MNE_SPACE_VOLUME   2
#define FIFFV_MNE_SPACE_DISCRETE 3

/*
 * Covariance matrix channel classification
 */
#define FIFFV_MNE_COV_CH_UNKNOWN  -1    /**< No idea*/
#define FIFFV_MNE_COV_CH_MEG_MAG   0    /**< Axial gradiometer or magnetometer [T]*/
#define FIFFV_MNE_COV_CH_MEG_GRAD  1    /**< Planar gradiometer [T/m]*/
#define FIFFV_MNE_COV_CH_EEG       2    /**< EEG [V]*/

/*
 * Projection item kinds
 */
#define FIFFV_MNE_PROJ_ITEM_EEG_AVREF  10

/*
 * Additional coordinate frames
 */
#define FIFFV_MNE_COORD_TUFTS_EEG    300           /**< For Tufts EEG data*/
#define FIFFV_MNE_COORD_CTF_DEVICE  1001           /**< CTF device coordinates*/
#define FIFFV_MNE_COORD_CTF_HEAD    1004           /**< CTF head coordinates*/
#define FIFFV_MNE_COORD_MRI_VOXEL   2001           /**< The MRI voxel coordinates*/
#define FIFFV_MNE_COORD_RAS         2002           /**< Surface RAS coordinates with non-zero origin*/
#define FIFFV_MNE_COORD_MNI_TAL     2003           /**< MNI Talairach coordinates*/
#define FIFFV_MNE_COORD_FS_TAL_GTZ  2004           /**< FreeSurfer Talairach coordinates (MNI z > 0)*/
#define FIFFV_MNE_COORD_FS_TAL_LTZ  2005           /**< FreeSurfer Talairach coordinates (MNI z < 0)*/
#define FIFFV_MNE_COORD_FS_TAL      2006           /**< FreeSurfer Talairach coordinates*/
} // NAMESPACE

#endif // FIFF_CONSTANTS_H
