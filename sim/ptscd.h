#ifndef __PTSCD_H__
#define __PTSCD_H__

#include <stdlib.h>
#include <stdint.h>

#include "modes.h"

typedef struct __attribute__((packed))
{
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t part;

    uint16_t last_blob_id;
    uint32_t last_blob_address;
    uint16_t first_empty_blob_id;
    uint32_t first_empty_blob_address;

    uint16_t first_empty_channel_id;
    uint16_t first_empty_contact_id;

} ptscd_context_t;

/**
 * Structure representing a channel, can be used as in-memory or in-file representation
 */
typedef struct __attribute__((packed))
{
    uint16_t id;        /** ID of the channel */
    uint32_t freq_rx;   /** RX frequency of the channel */
    uint32_t freq_tx;   /** TX frequency of the channel */
    uint8_t squelch;    /** Squelch level */
    uint8_t power;      /** TX power */
    uint8_t mode;       /** Mode to use for this channel */
    uint16_t qa_params; /** Quick Access parameters, mode dependant */
    uint16_t blob_id;   /** Optional: ID of the blob containing additional mode data (NULL if unused) */
} ptscd_channel_t;

typedef struct {
    uint16_t id;        /** ID of the group */
    uint16_t blob_id;   /** ID of the blob containing the IDs of the channels making the group */
} ptscd_group_t;

typedef struct
{
    union
    {
        struct __attribute__((packed)) fm
        {
            uint16_t ctcss_rx:6; /** FM CTCSS code to open the squelch (0 if disabled) */
            uint16_t ctcss_tx:6; /** FM CTCSS code during TX (0 if disabled) */
            uint16_t null:4;
        };

        struct __attribute__((packed)) m17
        {
            uint16_t can:3; /** M17 Channel access number */
        };
    };

} ptscd_qa_t;

typedef struct
{
    uint8_t dst[6]; /** M17 encoded callsign */
    uint8_t module;
} ptscd_blob_m17_t;

typedef struct
{
    uint32_t dmr_id:24;
    uint32_t reserved:8;
    uint8_t reserved[10];

} ptscd_blob_dmr_t;

#endif /* __PTSCD_H__ */