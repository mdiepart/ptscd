#ifndef __MODES_H__
#define __MODES_H__

typedef enum
{
    NBFM = 0,   /** Narrow-band FM, 12.5 kHz bandwidth */
    FM,         /** 25 kHz bandwidth FM */
    BCFM,       /** Broadcast FM */
    USB,        /** Upper sideband */
    LSB,        /** Lower sideband */
    CW,         /** Continuous Wave / Morse */
    RTTY,       /** Radioteletype */
    APRS,       /** Automatic Packet Reporting System */
    M17,        /** M17 */
    DSTAR,      /** D-STAR */
    DMR,        /** DMR */
    YSF_NB,     /** Yaesu System Fusion, narrow-band */
    YSF_WB,     /** Yaesu System Fusion, wideband */
    P25,        /** P25 */
    NXDN,       /** NXDN */
    TETRA,      /** Tetra */
    POCSAG,     /** POCSAG */
    DPMR,       /** dPMR */
    PMR446,     /** PMR446 */

    USER0 = 250,    /** Reserved for custom user modes */
    USER1 = 251,    /** Reserved for custom user modes */
    USER2 = 252,    /** Reserved for custom user modes */
    USER3 = 253,    /** Reserved for custom user modes */
    USER4 = 254,    /** Reserved for custom user modes */
    USER5 = 255,    /** Reserved for custom user modes */
} PTSCD_Modes_e;


#endif /* __MODES_H__ */