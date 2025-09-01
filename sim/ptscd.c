#include "ptscd.h"

#include <errno.h>
#include <string.h>

#include "littlefs/lfs.h"

/**********************************
 * Internal functions declarations
 **********************************/

 int _ptscd_lfs_to_errno(int lfs_err);

/*******************************
 * Public functions definitions
 *******************************/
ptscd_t *ptscd_init(lfs_t *lfs)
{
    ptscd_t *ptscd = (ptscd_t *)malloc(sizeof(ptscd_t));
    if(ptscd == NULL)
    {
        return NULL;
    }

    ptscd->ctx = (ptscd_context_t *)malloc(sizeof(ptscd_context_t));
    if(ptscd->ctx == NULL)
    {
        free(ptscd);
        printf("Failed to malloc ctx.\r\n");
        return NULL;
    }

    printf("Done ctx\r\n");

    ptscd->ptscd_dat = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if(ptscd->ptscd_dat == NULL)
    {
        free(ptscd->ctx);
        free(ptscd);
        printf("Failed to malloc ptscd_dat.\r\n");
        return NULL;
    }

    printf("Done ptscd_dat\r\n");

    ptscd->channels_dat = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if(ptscd->channels_dat == NULL)
    {
        free(ptscd->ptscd_dat);
        free(ptscd->ctx);
        free(ptscd);
        printf("Failed to malloc channels_dat.\r\n");
        return NULL;
    }

    printf("Done channels\r\n");

    ptscd->contacts_dat = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if(ptscd->contacts_dat == NULL)
    {
        free(ptscd->ptscd_dat);
        free(ptscd->channels_dat);
        free(ptscd->ctx);
        free(ptscd);
        printf("Failed to malloc contacts_dat.\r\n");
        return NULL;
    }

    printf("Done contacts\r\n");

    ptscd->groups_dat = (lfs_file_t *)malloc(sizeof(lfs_file_t));
    if(ptscd->groups_dat == NULL)
    {
        free(ptscd->ptscd_dat);
        free(ptscd->contacts_dat);
        free(ptscd->channels_dat);
        free(ptscd->ctx);
        free(ptscd);
        printf("Failed to malloc groups_dat.\r\n");
        return NULL;
    }

    printf("Done groups\r\n");

    ptscd->lfs = lfs;

    return ptscd;
}

int ptscd_open(ptscd_t *ptscd, const ptscd_context_t *context)
{
    if(ptscd == NULL)
    {
        fprintf(stderr, "provided ptscd is NULL.\r\n");
        return EINVAL;
    }

    int err = lfs_file_open(ptscd->lfs, ptscd->ptscd_dat, "ptscd.dat", LFS_O_RDWR);
    if(err != LFS_ERR_OK)
    {
        fprintf(stderr, "Error opening ptscd.dat.\r\n");
        return _ptscd_lfs_to_errno(err);
    }

    lfs_file_rewind(ptscd->lfs, ptscd->ptscd_dat); // Go to beginning of file

    // Check header, must be "PTSCD x.y pz"

    char buffer[35] = {'\0'};
    int read_cnt = lfs_file_read(ptscd->lfs, ptscd->ptscd_dat, buffer, sizeof(buffer)-1);

    // Header must begin with "PTSCD " ... 
    if(read_cnt < 6 || strncmp(buffer, "PTSCD ", 6) != 0)
    {
        fprintf(stderr, "Malformed ptscd.dat file. Must start with PTSCD.\r\n");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        return -1;
    }

    // ... followed by x.y for major.minor version number ...
    char *p = strchr(buffer+6, 'p'); // search for letter p following version 
    if(p == NULL)
    {
        fprintf(stderr, "Malformed ptscd.dat header.\r\n");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        return -1;
    }

    int cnt = p - (buffer + 6);
    if(cnt >= 8)
    {
        fprintf(stderr, "PTSCD version field is too long.\r\n");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        return -1;
    }

    // Parse version (x.y), length of x and y unspecified 
    char ver[8] = {'\0'};
    memcpy(ver, buffer + 6, cnt); // S
    
    char *version = strtok(ver, "."); // Split into major and minor based on '.'
    unsigned long major = strtoul(version, NULL, 10);
    version = strtok(NULL, ".");
    unsigned long minor = strtoul(version, NULL, 10);
    if(strtok(NULL, ".") != NULL)
    {
        fprintf(stderr, "PTSCD version must be in format \"major.minor\".\r\n");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        return -1;
    }

    // ... followed by p1\n or p2\n ...
    if(strncmp(p, "p1\n", 3) == 0)
    {
        printf("Detected PTSCD file version %lu.%lu, p1.\r\n", major, minor);

    }
    else if(strncmp(p, "p2\n", 3) == 0)
    {
        fprintf(stderr, "PTSCD p2 unsupported.\r\n");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        return -1;
    }

    // ... followed by a list of files for channels, contacts, groups
    p += 3; // add 3 to skip "px\n"
    int leftover = read_cnt - (p - buffer); // leftover in the buffer
    char filename[33] = {'\0'};

    // Moves whatever was left in the buffer to the front of it
    if(leftover > 0)
    {
        memmove(buffer, p, leftover);
    }
    
    // Parse channel file name
    read_cnt = lfs_file_read(ptscd->lfs, ptscd->ptscd_dat, buffer + leftover, sizeof(buffer)-leftover-1);
    char *newline = strchr(buffer, '\n');
    if(newline == NULL)
    {
        fprintf(stderr, "channel filename too long?");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        return -1;
    }
    int filename_len = newline-buffer;

    // Copy filename to another buffer
    memcpy(filename, buffer, filename_len);
    filename[filename_len] = '\0';
    
    // Open file
    err = lfs_file_open(ptscd->lfs, ptscd->channels_dat, filename, LFS_O_RDWR);
    if(err != LFS_ERR_OK)
    {
        fprintf(stderr, "Error opening channel file.\r\n");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        return _ptscd_lfs_to_errno(err);
    }

    // Moves whatever was left in the buffer to the front of it
    newline++; // Skip actual newline
    leftover = (sizeof(buffer)-1) - filename_len - 1; // Subtract 1 to skip newline
    if(leftover > 0)
    {
        memmove(buffer, newline, leftover);
    }

    // Parse contacts file name
    read_cnt = lfs_file_read(ptscd->lfs, ptscd->ptscd_dat, buffer + leftover, sizeof(buffer)-leftover-1);
    newline = strchr(buffer, '\n');
    if(newline == NULL)
    {
        fprintf(stderr, "contacts filename too long?");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        lfs_file_close(ptscd->lfs, ptscd->channels_dat);
        return -1;
    }

    filename_len = newline-buffer;
    memcpy(filename, buffer, filename_len);
    filename[filename_len] = '\0';

    err = lfs_file_open(ptscd->lfs, ptscd->contacts_dat, filename, LFS_O_RDWR);
    if(err != LFS_ERR_OK)
    {
        fprintf(stderr, "Error opening contacts file.\r\n");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        return _ptscd_lfs_to_errno(err);
    }

    // Moves whatever was left in the buffer to the front of it
    newline++; // Skip actual newline
    leftover = (sizeof(buffer)-1) - filename_len - 1; // Subtract 1 to skip newline
    if(leftover > 0)
    {
        memmove(buffer, newline, leftover);
    }
    
    // Parse groups file name
    read_cnt = lfs_file_read(ptscd->lfs, ptscd->ptscd_dat, buffer + leftover, sizeof(buffer)-leftover-1);
    newline = strchr(buffer, '\n');
    if(newline == NULL)
    {
        fprintf(stderr, "groups filename too long?");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        lfs_file_close(ptscd->lfs, ptscd->channels_dat);
        return -1;
    }
    
    filename_len = newline-buffer;
    memcpy(filename, buffer, filename_len);
    filename[filename_len] = '\0';

    err = lfs_file_open(ptscd->lfs, ptscd->groups_dat, filename, LFS_O_RDWR);
    if(err != LFS_ERR_OK)
    {
        fprintf(stderr, "Error opening groups file.\r\n");
        lfs_file_close(ptscd->lfs, ptscd->ptscd_dat);
        lfs_file_close(ptscd->lfs, ptscd->channels_dat);
        lfs_file_close(ptscd->lfs, ptscd->contacts_dat);
        return _ptscd_lfs_to_errno(err);
    }    

    printf("Successfuly opened all PTSCD files.\n");

    if(context != NULL)
    {
        memcpy(ptscd->ctx, context, sizeof(ptscd_context_t));
    }
    else
    {
        memset(ptscd->ctx, 0, sizeof(ptscd_context_t));
    }

    return 0;
}

/*********************************
 * Internal functions definitions
 *********************************/

 int _ptscd_lfs_to_errno(int lfs_err)
 {
    switch(lfs_err)
    {
        case LFS_ERR_OK:
            return 0;
        case LFS_ERR_IO:
        case LFS_ERR_CORRUPT:
            return EIO;
        case LFS_ERR_NOENT:
            return ENOENT;
        case LFS_ERR_EXIST:
            return EEXIST;
        case LFS_ERR_NOTDIR:
            return ENOTDIR;
        case LFS_ERR_ISDIR:
            return EISDIR;
        case LFS_ERR_NOTEMPTY:
            return ENOTEMPTY;
        case LFS_ERR_BADF:
            return EBADF;
        case LFS_ERR_FBIG:
            return EFBIG;
        case LFS_ERR_INVAL:
            return EINVAL;
        case LFS_ERR_NOSPC:
            return ENOSPC;
        case LFS_ERR_NOMEM:
            return ENOMEM;
        case LFS_ERR_NOATTR:
            return ENODATA;
        case LFS_ERR_NAMETOOLONG:
            return ENAMETOOLONG;
        default:
            return lfs_err;
    }
 }