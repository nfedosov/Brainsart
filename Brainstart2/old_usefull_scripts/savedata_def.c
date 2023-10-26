#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "savedata_def.h"

HDRTYPE* constructHDR(const unsigned NS, const unsigned N_EVENT)
{
/*
    HDR is initialized, memory is allocated for
    NS channels and N_EVENT number of events.

    The purpose is to define all parameters at an initial step.
    No parameters must remain undefined.
 */
    HDRTYPE* hdr = (HDRTYPE*)malloc(sizeof(HDRTYPE));

    union {
        uint32_t testword;
        uint8_t  testbyte[sizeof(uint32_t)];
    } EndianTest;
        int k,k1;
    uint8_t	LittleEndian;
    size_t BitsPerBlock;

    EndianTest.testword = 0x4a3b2c1d;
    LittleEndian = (EndianTest.testbyte[0]==0x1d && EndianTest.testbyte[1]==0x2c  && EndianTest.testbyte[2]==0x3b  && EndianTest.testbyte[3]==0x4a );

    assert (  ( LittleEndian && __BYTE_ORDER == __LITTLE_ENDIAN)
           || (!LittleEndian && __BYTE_ORDER == __BIG_ENDIAN   ) );

    hdr->FileName = NULL;
    hdr->FILE.OPEN = 0;
    hdr->FILE.FID = 0;
    hdr->FILE.POS = 0;
    hdr->FILE.Des = 0;
    hdr->FILE.COMPRESSION = 0;
    hdr->FILE.size = 0;
#ifdef ZLIB_H
    hdr->FILE.gzFID = 0;
#endif

    hdr->AS.B4C_ERRNUM = B4C_NO_ERROR;
    hdr->AS.B4C_ERRMSG = NULL;
    hdr->AS.Header = NULL;
    hdr->AS.rawEventData = NULL;
    hdr->AS.auxBUF = NULL;
    hdr->AS.bpb    = 0;

    hdr->TYPE = noFile;
    hdr->VERSION = 2.0;
    hdr->AS.rawdata = NULL; 		//(uint8_t*) malloc(0);
    hdr->AS.flag_collapsed_rawdata = 0;	// is rawdata not collapsed
    hdr->AS.first = 0;
    hdr->AS.length  = 0;  			// no data loaded
    memset(hdr->AS.SegSel,0,sizeof(hdr->AS.SegSel));
    hdr->Calib = NULL;
    hdr->rerefCHANNEL = NULL;

    hdr->NRec = 0;
    hdr->SPR  = 0;
    hdr->NS = NS;
    hdr->SampleRate = 4321.5;
    hdr->Patient.Id[0]=0;
    strcpy(hdr->ID.Recording,"00000000");
    hdr->data.size[0] = 0; 	// rows
    hdr->data.size[1] = 0;  // columns
    hdr->data.block = NULL;
#if __FreeBSD__ || __APPLE__ || __NetBSD__
    time_t t=time(NULL);
    struct tm *tt = localtime(&t);
    hdr->tzmin    = tt->tm_gmtoff/60;
    hdr->T0       = t_time2gdf_time(time(NULL)-tt->tm_gmtoff); // localtime
#else
    hdr->T0    = t_time2gdf_time(time(NULL)-timezone); // localtime
    hdr->tzmin = -timezone/60;      // convert from seconds west of UTC to minutes east;
#endif
    {
    uint8_t Equipment[8] = "b4c_1.5 ";
    Equipment[4] = BIOSIG_VERSION_MAJOR+'0';
    Equipment[6] = BIOSIG_VERSION_MINOR+'0';
    memcpy(&(hdr->ID.Equipment), &Equipment, 8);
    }

    hdr->ID.Manufacturer._field[0]    = 0;
    hdr->ID.Manufacturer.Name         = NULL;
    hdr->ID.Manufacturer.Model        = NULL;
    hdr->ID.Manufacturer.Version      = NULL;
    hdr->ID.Manufacturer.SerialNumber = NULL;
    hdr->ID.Technician 	= NULL;
    hdr->ID.Hospital 	= NULL;

    memset(hdr->IPaddr, 0, 16);
    {	// some local variables are used only in this block
#ifdef _WIN32
   #if 1
    // getlogin() a flawfinder level [4] issue, recommended to use getpwuid(geteuid()) but not available on Windows
    hdr->ID.Technician = strdup("brainstart"); // !!!
   #else	// this compiles but stops with "Program error" on wine
    char str[1001];
    GetUserName(str,1000);
    if (VERBOSE_LEVEL>7)  fprintf(stdout,"Name:%s\n",str);
    hdr->ID.Technician = strdup(str);
   #endif
#else
    char *username = NULL;
    struct passwd *p = getpwuid(geteuid());
    if (p != NULL)
        username = p->pw_name;
    if (username)
        hdr->ID.Technician = strdup(username);

#endif
    }



    hdr->Patient.Name[0] 	= 0;
    //hdr->Patient.Id[0] 	= 0;
    hdr->Patient.Birthday 	= (gdf_time)0;        // Unknown;
        hdr->Patient.Medication = 0;	// 0:Unknown, 1: NO, 2: YES
        hdr->Patient.DrugAbuse 	= 0;	// 0:Unknown, 1: NO, 2: YES
        hdr->Patient.AlcoholAbuse=0;	// 0:Unknown, 1: NO, 2: YES
        hdr->Patient.Smoking 	= 0;	// 0:Unknown, 1: NO, 2: YES
        hdr->Patient.Sex 	= 0;	// 0:Unknown, 1: Male, 2: Female
        hdr->Patient.Handedness = 0;	// 0:Unknown, 1: Right, 2: Left, 3: Equal
        hdr->Patient.Impairment.Visual = 0;	// 0:Unknown, 1: NO, 2: YES, 3: Corrected
        hdr->Patient.Impairment.Heart  = 0;	// 0:Unknown, 1: NO, 2: YES, 3: Pacemaker
        hdr->Patient.Weight 	= 0;	// 0:Unknown
        hdr->Patient.Height 	= 0;	// 0:Unknown

        for (k1=0; k1<3; k1++) {
            hdr->Patient.Headsize[k1] = 0;        // Unknown;
            hdr->ELEC.REF[k1] = 0.0;
            hdr->ELEC.GND[k1] = 0.0;
        }
    hdr->LOC[0] = 0x00292929;
    hdr->LOC[1] = 48*3600000+(1<<31); 	// latitude
    hdr->LOC[2] = 15*3600000+(1<<31); 	// longitude
    hdr->LOC[3] = 35000; 	 	//altitude in centimeter above sea level

    hdr->FLAG.UCAL = 0; 		// un-calibration OFF (auto-scaling ON)
    hdr->FLAG.OVERFLOWDETECTION = 1; 	// overflow detection ON
    hdr->FLAG.ANONYMOUS = 1; 	// <>0: no personal names are processed
    hdr->FLAG.TARGETSEGMENT = 1;	// read 1st segment
    hdr->FLAG.ROW_BASED_CHANNELS=0;

        // define variable header
    hdr->CHANNEL = (CHANNEL_TYPE*)calloc(hdr->NS, sizeof(CHANNEL_TYPE));
    BitsPerBlock = 0;
    for (k=0;k<hdr->NS;k++)	{
        size_t nbits;
        CHANNEL_TYPE *hc = hdr->CHANNEL+k;
            hc->Label[0]  = 0;
            hc->LeadIdCode= 0;
            strcpy(hc->Transducer, "EEG: Ag-AgCl electrodes");
            hc->PhysDimCode = 19+4256; // uV
            hc->PhysMax   = +100;
            hc->PhysMin   = -100;
            hc->DigMax    = +2047;
            hc->DigMin    = -2048;
            hc->Cal	      = NAN;
            hc->Off	      = 0.0;
            hc->TOffset   = 0.0;
            hc->GDFTYP    = 3;	// int16
            hc->SPR       = 1;	// one sample per block
            hc->bi 	      = 2*k;
            hc->bi8	      = BitsPerBlock;
        nbits = GDFTYP_BITS[hc->GDFTYP]*hc->SPR;
        BitsPerBlock   += nbits;
            hc->OnOff     = 1;
            hc->HighPass  = 0.16;
            hc->LowPass   = 70.0;
            hc->Notch     = 50;
            hc->Impedance = INFINITY;
            hc->fZ        = NAN;
            hc->XYZ[0] 	= 0.0;
            hc->XYZ[1] 	= 0.0;
            hc->XYZ[2] 	= 0.0;
    }

    // define EVENT structure
    hdr->EVENT.N = N_EVENT;
    hdr->EVENT.SampleRate = 0;
    hdr->EVENT.CodeDesc = NULL;
    hdr->EVENT.LenCodeDesc = 0;
    if (hdr->EVENT.N) {
        hdr->EVENT.POS = (uint32_t*) calloc(hdr->EVENT.N, sizeof(*hdr->EVENT.POS));
        hdr->EVENT.TYP = (uint16_t*) calloc(hdr->EVENT.N, sizeof(*hdr->EVENT.TYP));
        hdr->EVENT.DUR = (uint32_t*) calloc(hdr->EVENT.N, sizeof(*hdr->EVENT.DUR));
        hdr->EVENT.CHN = (uint16_t*) calloc(hdr->EVENT.N, sizeof(*hdr->EVENT.CHN));
#if (BIOSIG_VERSION >= 10500)
        hdr->EVENT.TimeStamp = (gdf_time*) calloc(hdr->EVENT.N, sizeof(gdf_time));
#endif
    } else {
        hdr->EVENT.POS = NULL;
        hdr->EVENT.TYP = NULL;
        hdr->EVENT.DUR = NULL;
        hdr->EVENT.CHN = NULL;
#if (BIOSIG_VERSION >= 10500)
        hdr->EVENT.TimeStamp = NULL;
#endif
    }

    // initialize specialized fields
    hdr->aECG = NULL;
    hdr->AS.bci2000 = NULL;

#if (BIOSIG_VERSION >= 10500)
    hdr->SCP.Section7  = NULL;
    hdr->SCP.Section8  = NULL;
    hdr->SCP.Section9  = NULL;
    hdr->SCP.Section10 = NULL;
    hdr->SCP.Section11 = NULL;
    hdr->SCP.Section7Length  = 0;
    hdr->SCP.Section8Length  = 0;
    hdr->SCP.Section9Length  = 0;
    hdr->SCP.Section10Length = 0;
    hdr->SCP.Section11Length = 0;
#endif

    return(hdr);
}

/* just for debugging
void debug_showptr(HDRTYPE* hdr) {
    fprintf(stdout,"=========================\n");
    fprintf(stdout,"&AS.Header=%p\n",hdr->AS.Header);
    fprintf(stdout,"&AS.auxBUF=%p\n",hdr->AS.auxBUF);
    fprintf(stdout,"&aECG=%p\n",hdr->aECG);
    fprintf(stdout,"&AS.bci2000=%p\n",hdr->AS.bci2000);
    fprintf(stdout,"&AS.rawEventData=%p\n",hdr->AS.rawEventData);
    fprintf(stdout,"&AS.rawData=%p\n",hdr->AS.rawdata);
    fprintf(stdout,"&data.block=%p\n",hdr->data.block);
    fprintf(stdout,"&CHANNEL=%p\n",hdr->CHANNEL);
    fprintf(stdout,"&EVENT.POS=%p\n",hdr->EVENT.POS);
    fprintf(stdout,"&EVENT.TYP=%p\n",hdr->EVENT.TYP);
    fprintf(stdout,"&EVENT.DUR=%p\n",hdr->EVENT.DUR);
    fprintf(stdout,"&EVENT.CHN=%p\n",hdr->EVENT.CHN);
    fprintf(stdout,"&EVENT.CodeDesc=%p\n",hdr->EVENT.CodeDesc);
    fprintf(stdout,"&FileName=%p %s\n",&hdr->FileName,hdr->FileName);
    fprintf(stdout,"&Hospital=%p\n",hdr->ID.Hospital);
}
*/
