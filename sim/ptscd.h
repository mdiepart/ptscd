#ifndef __PTSCD_H__
#define __PTSCD_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include "littlefs/lfs.h"

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
    uint16_t id;        /**< ID of the channel */
    uint32_t freq_rx;   /**< RX frequency of the channel */
    uint32_t freq_tx;   /**< TX frequency of the channel */
    uint8_t squelch;    /**< Squelch level */
    uint8_t power;      /**< TX power */
    uint8_t mode;       /**< Mode to use for this channel */
    uint16_t qa_params; /**< Quick Access parameters, mode dependant */
    uint16_t blob_id;   /**< Optional: ID of the blob containing additional mode data (NULL if unused) */
} ptscd_channel_t;

/**
 * Structure representing a group of channels
 */
typedef struct __attribute__((packed))
{
    uint16_t id;        /**< ID of the group */
    uint16_t blob_id;   /**< ID of the blob containing the IDs of the channels making the group */
} ptscd_group_t;

/**
 * Structure representing a PTSCD blob.
 */
typedef struct __attribute__((packed))
{
    uint16_t id;        /**< ID of the blob */
    uint8_t length;     /**< Data length (not including id and length byte itself) */
    uint8_t *data;      /**< Data contained in the blob */
} ptscd_blob_t;

/**
 * Structure containing the handle to a PTSCD codeplug
 */
typedef struct __attribute__((packed))
{
    ptscd_context_t *ctx;
    lfs_t *lfs;
    lfs_file_t *ptscd_dat;
    lfs_file_t *channels_dat;
    lfs_file_t *groups_dat;
    lfs_file_t *contacts_dat;
} ptscd_t;

/**
 * Initializes a PTSCD codeplug handle
 * 
 * @param *ptscd pointer to a PTSCD handle
 * 
 * @return 
 */
ptscd_t *ptscd_init(lfs_t *lfs);

/**
 * Destroy a PTSCD handle, freeing all internal resources
 */
void ptscd_destroy(ptscd_t *ptscd);

/**
 * Opens a PTSCD codeplug
 * 
 * @param *ptscd initialized PTSCD handle
 * 
 * @return 
 */
int ptscd_open(ptscd_t *ptscd, const ptscd_context_t *context);

void ptscd_close(ptscd_t *ptscd);

/**
 * Return the number of channels contained in a PTSCD codeplug
 */
int ptscd_get_channel_count(const ptscd_t *ptscd);

int ptscd_get_channel(const ptscd_t *ptscd, const uint16_t id, ptscd_channel_t *channel);
int ptscd_del_channel(ptscd_t *ptscd, const uint16_t id);
int ptscd_add_channel(ptscd_t *ptscd, const ptscd_channel_t *channel);

#endif /* __PTSCD_H__ */