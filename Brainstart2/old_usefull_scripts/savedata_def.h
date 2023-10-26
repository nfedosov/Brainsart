#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>


#ifndef SAVEDATA_DEF_H
#define SAVEDATA_DEF_H

#define MAX_LENGTH_LABEL 	80	// TMS: 40, AXG: 79
#define MAX_LENGTH_TRANSDUCER 	80
#define MAX_LENGTH_PID	 	80  	// length of Patient ID: MFER<65, GDF<67, EDF/BDF<81, etc.
#define MAX_LENGTH_RID		80	// length of Recording ID: EDF,GDF,BDF<80, HL7 ?
#define MAX_LENGTH_NAME 	132	// max length of personal name: MFER<=128, EBS<=33*4
#define MAX_LENGTH_MANUF 	128	// max length of manufacturer field: MFER<128
#define MAX_LENGTH_TECHNICIAN 	128	// max length of manufacturer field: SCP<41

#define BIOSIG_VERSION_MAJOR 3
#define BIOSIG_VERSION_MINOR 0
#define BIOSIG_PATCHLEVEL    1
// for backward compatibility
#define BIOSIG_VERSION_STEPPING BIOSIG_PATCHLEVEL
#define BIOSIG_VERSION (BIOSIG_VERSION_MAJOR * 10000 + BIOSIG_VERSION_MINOR * 100 + BIOSIG_PATCHLEVEL)

#define t_time2gdf_time(t)	((gdf_time)floor(ldexp(((double)(t))/86400.0 + 719529.0, 32)))

#ifdef NDEBUG
#define VERBOSE_LEVEL 0 	// turn off debugging information, but its only used without NDEBUG
#else
extern int VERBOSE_LEVEL; 	// used for debugging, variable is always defined
#endif

#  define __BIG_ENDIAN		4321
#  define __LITTLE_ENDIAN	1234
#  define __BYTE_ORDER		__LITTLE_ENDIAN



typedef struct {
    const char *free_text_event_limiter;
} biosig_options_type;
typedef double biosig_data_type;
typedef int64_t nrec_t;	/* type for number of records */
typedef int64_t gdf_time, gdftime_t; /* gdf time is represented in 64 bits */


enum B4C_ERROR {
    B4C_NO_ERROR=0,
    B4C_FORMAT_UNKNOWN,
    B4C_FORMAT_UNSUPPORTED,
    B4C_CANNOT_OPEN_FILE,
    B4C_CANNOT_WRITE_FILE,
    B4C_CANNOT_APPEND_FILE,
    B4C_INSUFFICIENT_MEMORY,
    B4C_ENDIAN_PROBLEM,
    B4C_CRC_ERROR,
    B4C_DATATYPE_UNSUPPORTED,
    B4C_SCLOSE_FAILED,
    B4C_DECOMPRESSION_FAILED,
    B4C_MEMORY_ALLOCATION_FAILED,
    B4C_RAWDATA_COLLAPSING_FAILED,
    B4C_REREF_FAILED,
    B4C_INCOMPLETE_FILE,
    B4C_UNSPECIFIC_ERROR,
    B4C_CHAR_ENCODING_UNSUPPORTED
};


enum FileFormat {
    noFile, unknown,
    ABF, ABF2, ACQ, ACR_NEMA, AIFC, AIFF, AINF, alpha, ARFF,
    ASCII_IBI, ASCII, AU, ASF, ATES, ATF, AVI, AXG, Axona,
    BCI2000, BDF, BESA, BIN, BKR, BLSC, BMP, BNI, BSCS,
    BrainVision, BrainVisionVAmp, BrainVisionMarker, BZ2,
    CDF, CFS, CFWB, CNT, CTF, DICOM, DEMG,
    EBS, EDF, EEG1100, EEProbe, EEProbe2, EEProbeAvr, EGI,
    EGIS, ELF, EMBLA, EMSA, ePrime, ET_MEG, ETG4000, EVENT, EXIF,
    FAMOS, FEF, FIFF, FITS, FLAC, GDF, GDF1,
    GIF, GTF, GZIP, HDF, HL7aECG, HEKA,
    IBW, ISHNE, ITX, JPEG, JSON, Lexicor,
    Matlab, MFER, MIDI, MIT, MM, MSI, MSVCLIB, MS_LNK, MX,
    native, NeuroLoggerHEX, NetCDF, NEURON, NEV, NEX1, NIFTI, NUMPY,
    OGG, OpenXDF,
    PBMA, PBMN, PDF, PDP, Persyst, PGMA, PGMB,
    PLEXON, PNG, PNM, POLY5, PPMA, PPMB, PS,
    RDF, RIFF,
    SASXPT, SCP_ECG, SIGIF, Sigma, SMA, SMR, SND, SQLite,
    SPSS, STATA, SVG, SXI, SYNERGY,
    TDMS, TIFF, TMS32, TMSiLOG, TRC, UNIPRO, VRML, VTK,
    WAV, WCP, WG1, WinEEG, WMF, XML, XPM,
    Z, ZIP, ZIP2, RHD2000, RHS2000, IntanCLP,
    EBNEURO, SigViewerEventsCSV, XDF,
    EAS, EZ3, ARC, WFT, BiosigDump,
    LastPlaceHolder, invalid=0xffff
};


typedef struct CHANNEL_STRUCT {
    double 		PhysMin ;	/* physical minimum */
    double 		PhysMax ;	/* physical maximum */
    double 		DigMin 	;	/* digital minimum */
    double	 	DigMax 	;	/* digital maximum */
    double		Cal 	;	/* gain factor */
    double		Off 	;	/* bias */

    char		Label[MAX_LENGTH_LABEL+1] ; 	/* Label of channel */
    char		OnOff	;	/* 0: channel is off, not consider for data output; 1: channel is turned on; 2: channel containing time axis */
    uint16_t	LeadIdCode ;	/* Lead identification code */
    char 		Transducer[MAX_LENGTH_TRANSDUCER+1] ;	/* transducer e.g. EEG: Ag-AgCl electrodes */
#ifdef MAX_LENGTH_PHYSDIM
        char            PhysDim[MAX_LENGTH_PHYSDIM+1] ATT_ALI ATT_DEPREC;       /* DONOT USE - use PhysDim3(PhysDimCode) instead */
#endif
    uint16_t	PhysDimCode ;	/* code for physical dimension - PhysDim3(PhysDimCode) returns corresponding string */

    float 		TOffset 	;	/* time delay of sampling */
    float 		LowPass		;	/* lowpass filter */
    float 		HighPass	;	/* high pass */
    float 		Notch		;	/* notch filter */
    float 		XYZ[3]		;	/* sensor position */

    union {
        /* context specific channel information */
    float 		Impedance	;   	/* Electrode Impedance in Ohm, defined only if PhysDim = _Volt */
    float 		fZ        	;   	/* ICG probe frequency, defined only if PhysDim = _Ohm */
    } ATT_ALI;

    /* this part should not be used by application programs */
    uint8_t*	bufptr		;	/* pointer to buffer: NRec<=1 and bi,bi8 not used */
    uint32_t 	SPR 		;	/* samples per record (block) */
    uint32_t	bi          ;	/* start byte (byte index) of channel within data block */
    uint32_t	bi8 		;	/* start bit  (bit index) of channel within data block */
    uint16_t 	GDFTYP 		;	/* data type */
} CHANNEL_TYPE;



typedef struct HDR_STRUCT {

    char* 	FileName;       /* FileName - dynamically allocated, local copy of file name */

    union {
        // workaround for transition to cleaner fieldnames
        float VERSION;		/* GDF version number */
        float Version;		/* GDF version number */
    };

    union {
        // workaround for transition to cleaner fieldnames
        enum FileFormat TYPE;		 	/* type of file format */
        enum FileFormat Type; 			/* type of file format */
    };

    struct {
        size_t 			size[2]; /* size {rows, columns} of data block	 */
        biosig_data_type* 	block; 	 /* data block */
    } data;

    uint8_t 	IPaddr[16]; 	/* IP address of recording device (if applicable) */
    double 		SampleRate;	/* Sampling rate */
    nrec_t  	NRec;	/* number of records/blocks -1 indicates length is unknown. */
    gdf_time 	T0; 	/* starttime of recording */
    uint32_t 	HeadLen;	/* length of header in bytes */
    uint32_t 	SPR 	    ;	/* samples per block (when different sampling rates are used, this is the LCM(CHANNEL[..].SPR) */
    uint32_t  	LOC[4] 	    ;	/* location of recording according to RFC1876 */
    uint16_t 	NS 	    ;	/* number of channels */
    int16_t 	tzmin 	    ;	/* time zone : minutes east of UTC */

#ifdef CHOLMOD_H
    cholmod_sparse  *Calib     ;                  /* re-referencing matrix */
#else
        void        *Calib     ;                  /* re-referencing matrix */
#endif
    CHANNEL_TYPE 	*rerefCHANNEL     ;

    /* Patient specific information */
    struct {
        gdf_time 	Birthday; 	/* Birthday of Patient */
        // 		Age;		// the age is HDR.T0 - HDR.Patient.Birthday, even if T0 and Birthday are not known
        uint16_t	Headsize[3]; 	/* circumference, nasion-inion, left-right mastoid in millimeter;  */

        /* Patient Name:
         * can consist of up to three components, separated by the unit separator ascii(31), 0x1f, containing in that order
            Last name, first name, second last name (see also SCP-ECG specification EN1064, Section 1, tag 0, 1, and 3)
         * for privacy protection this field is by default not supported, support can be turned on with FLAG.ANONYMOUS
                 */
        char		Name[MAX_LENGTH_NAME+1];

        char		Id[MAX_LENGTH_PID+1];	/* patient identification, identification code as used in hospital  */
        uint8_t		Weight;		/* weight in kilograms [kg] 0:unkown, 255: overflow  */
        uint8_t		Height;		/* height in centimeter [cm] 0:unkown, 255: overflow  */
        //		BMI;		// the body-mass index = weight[kg]/height[m]^2
        /* Patient classification */
        int8_t	 	Sex;		/* 0:Unknown, 1: Male, 2: Female */
        int8_t		Handedness;	/* 0:Unknown, 1: Right, 2: Left, 3: Equal */
        int8_t		Smoking;	/* 0:Unknown, 1: NO, 2: YES */
        int8_t		AlcoholAbuse;	/* 0:Unknown, 1: NO, 2: YES */
        int8_t		DrugAbuse;	/* 0:Unknown, 1: NO, 2: YES */
        int8_t		Medication;	/* 0:Unknown, 1: NO, 2: YES */
        struct {
            int8_t 	Visual;		/* 0:Unknown, 1: NO, 2: YES, 3: Corrected */
            int8_t 	Heart;		/* 0:Unknown, 1: NO, 2: YES, 3: Pacemaker */
        } Impairment;
    } Patient     ;

    struct {
        char		Recording[MAX_LENGTH_RID+1]; 	/* HL7, EDF, GDF, BDF replaces HDR.AS.RID */
        char* 		Technician;
        char* 		Hospital;	/* recording institution */
        uint64_t 	Equipment; 	/* identifies this software */
        struct {
            /* see
                SCP: section1, tag14,
                MFER: tag23:  "Manufacturer^model^version number^serial number"
            */
            const char*	Name;
            const char*	Model;
            const char*	Version;
            const char*	SerialNumber;
            char	_field[MAX_LENGTH_MANUF+1];	/* buffer */
        } Manufacturer;
    } ID     ;

    /* position of electrodes; see also HDR.CHANNEL[k].XYZ */
    struct {
        float		REF[3];	/* XYZ position of reference electrode */
        float		GND[3];	/* XYZ position of ground electrode */
    } ELEC     ;

    /* EVENTTABLE */
    struct {
        double  	SampleRate     ;	/* for converting POS and DUR into seconds  */
        uint16_t 	*TYP     ;	/* defined at http://biosig.svn.sourceforge.net/viewvc/biosig/trunk/biosig/doc/eventcodes.txt */
        uint32_t 	*POS     ;	/* starting position [in samples] using a 0-based indexing */
        uint32_t 	*DUR     ;	/* duration [in samples] */
        uint16_t 	*CHN     ;	/* channel number; 0: all channels  */
#if (BIOSIG_VERSION >= 10500)
        gdf_time        *TimeStamp     ;  /* store time stamps */
#endif
        const char*	*CodeDesc ;	/* describtion of "free text"/"user specific" events (encoded with TYP=0..255 */
        uint32_t  	N ;	/* number of events */
        uint16_t	LenCodeDesc  ;	/* length of CodeDesc Table */
    } EVENT  ;

    struct {	/* flags */
        char		OVERFLOWDETECTION; 	/* overflow & saturation detection 0: OFF, !=0 ON */
        char		UCAL; 		/* UnCalibration  0: scaling  !=0: NO scaling - raw data return  */
        char		ANONYMOUS; 	/* 1: anonymous mode, no personal names are processed */
        char		ROW_BASED_CHANNELS;     /* 0: column-based data [default]; 1: row-based data */
        char		TARGETSEGMENT; /* in multi-segment files (like Nihon-Khoden, EEG1100), it is used to select a segment */
    } FLAG   ;

    CHANNEL_TYPE 	*CHANNEL     ;
        // moving CHANNEL after the next struct (HDR.FILE) gives problems at AMD64 MEX-file.
        // perhaps some alignment problem.

    struct {	/* File specific data  */
#ifdef ZLIB_H
        gzFile		gzFID;
#else
        void*		gzFID;
#endif
#ifdef _BZLIB_H
//		BZFILE*		bzFID;
#endif
        FILE* 		FID;		/* file handle  */
        size_t 		size;		/* size of file - experimental: only partly supported */
        size_t 		POS;		/* current reading/writing position [in blocks] */
        //size_t 	POS2;		// current reading/writing position [in samples] */
        int		Des;		/* file descriptor */
        int		DES;		/* descriptor for streams */
        uint8_t		OPEN; 		/* 0: closed, 1:read, 2: write */
        uint8_t		LittleEndian;   /* 1 if file is LittleEndian data format and 0 for big endian data format*/
        uint8_t		COMPRESSION;    /* 0: no compression 9: best compression */
    } FILE  ;

    /*	internal variables (not public)  */
    struct {
        const char*	B4C_ERRMSG;	/* error message */
//		char 		PID[MAX_LENGTH_PID+1];	// use HDR.Patient.Id instead
//		char* 		RID;		// recording identification
        uint32_t 	bpb;  		/* total bytes per block */
        uint32_t 	bpb8;  		/* total bits per block */

        uint8_t*	Header;
        uint8_t*	rawEventData;
        uint8_t*	rawdata; 	/* raw data block */
        size_t		first;		/* first block loaded in buffer - this is equivalent to hdr->FILE.POS */
        size_t		length;		/* number of block(s) loaded in buffer */
        uint8_t*	auxBUF;  	/* auxillary buffer - used for storing EVENT.CodeDesc, MIT FMT infor, alpha:rawdata header */
        union {
            char*	bci2000;	/* application specific free text field */
            char*	fpulse;
            char*	stimfit;
        };
        uint32_t	SegSel[5];	/* segment selection in a hirachical data formats, e.g. sweeps in HEKA/PatchMaster format */
        enum B4C_ERROR	B4C_ERRNUM;	/* error code */
        char		flag_collapsed_rawdata; /* 0 if rawdata contain obsolete channels, too. 	*/
    } AS     ;

    void *aECG;				/* used as an pointer to (non-standard) auxilary information - mostly used for hacks */
    uint64_t viewtime; 			/* used by edfbrowser */

#if (BIOSIG_VERSION >= 10500)
    struct {
        /*
            This part contains Section 7-11 of the SCP-ECG format
            without its 16 byte "Section ID header".
            These sections are also stored in GDF Header 3 (tag 9-13)
            It is mostly used for SCP<->GDF conversion.

            The pointers points into hdr->AS.Header,
            so do not dynamically re-allocate the pointers.
        */
        const uint8_t* Section7;
        const uint8_t* Section8;
        const uint8_t* Section9;
        const uint8_t* Section10;
        const uint8_t* Section11;
        uint32_t Section7Length;
        uint32_t Section8Length;
        uint32_t Section9Length;
        uint32_t Section10Length;
        uint32_t Section11Length;
    } SCP;
#endif

} HDRTYPE;


HDRTYPE* sopen_extended(const char* FileName, const char* MODE, HDRTYPE* hdr, biosig_options_type* options) __attribute__ ((visibility ("default") ));


HDRTYPE* constructHDR(const unsigned NS, const unsigned N_EVENT);


#endif // SAVEDATA_DEF_H
