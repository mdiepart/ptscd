# Portable, Transferable, Standardised Codeplug Description

PTSCD is a standardised codeplug format that aims to be portable, transferable and cross-platform. This standard contains two parts. Part 1 is the full standard. It is meant to be readable and writable from low-power devices (like handheld transceivers) that are powerful enough and have enough memory space. Part 2 is a condensed version of the part 1. It is meant as a read-only, condensed form of the full standard for lower-power devices or with limited memory space available.

## Part 1

This part of the standard is the "full" standard. It is implemented over a filesystem on which 5 files will be stored. Each file will store specific data. All files (except for the file containing the blobs) have fixed size entries. This is intended to keep fragmentation low.
The main file is named "ptscd.dat" and has a specific header identifying the version and the part of the standard as well as data about the other files.

When programmed into a user device, it must be at the root of the filesystem and the blob file must be named `ptscd.dat`.

## Part 2

This part of the standard is the "condensed" standard. It can be implemented over a filesystem or it can be implemented "baremetal" directly in the memory. It is read-only and cannot be modified on the device.

## Files

This section describe the layout of the various files used for the standard.

### Blob

The blob file is the main file of this standard. It must be named `ptscd.dat`. It contains the main header with important information for the device. In the case of part 2, this may be a file, or not.

#### Main header

The main header starts with text `PTSCD` followed by a single space followed by the version in format `major.minor`(currently, `0.1`) followed by the part of the standard (`p1` for part 1 and `p2` for part 2) followed .

The rest of the header depends on the part of the standard.

##### Part 1

In case the part 1 is followed, the rest of the header contains a list of filenames in the form of null-terminated strings. Each string must be at most 32 characters long and in ASCII. The filenames are in the following order:

 * channels
 * contacts
 * groups

If any of those filenames is 0 length, this means that the codeplug does not contain any such entries. This also means that the radio will not be able to write to any of those files. So if the codeplug simply contains no such data but that the radio must be able to create such entries, the codeplug should contain an empty file and its name should be present in the list.

##### Part 2

In case the part 2 of the standard is followed, the rest of the header contains offsets at which each section starts. Each offset is expressed as a 32 bits number and is the offset in bytes from the beginning of the file. If this offset is 0 this means that the codeplug does not contain any such entries. The offset must be in this order:

 * channels
 * contacts
 * groups

#### Content

The content of the blob file is defined as follows:

### channels

| ID  | FREQ RX | FREQ TX | Squelch | Power | Mode | QA Params | BLOB_ID | Total    |
|---- |---------|---------|---------|-------|------|-----------|---------|----------|
| 2 B | 4 B     | 4 B     | 1 B     | 1 B   | 1B   | 2 B       | 2 B     | 17 bytes |

 * `ID` is the channel number. Odd channels are active, even channels are deleted channels.
 * `Mode` is the operating mode.

    0) for NBFM (12.5 kHz BW)
    1) for FM (25 kHz BW)
    1) for BCFM (RX only?)
    1) for USB
    1) for LSB
    1) for CW
    1) for RTTY
    1) for APRS
    1) for M17
    1) for DStar
    1) for DMR
    1) for YSF (NB)
    1) for YSF (WB)
    1) for P25
    1) for NXDN
    1) for Tetra
    1) for POCSAG
    1) for dPMR
    1) for PMR446
    * Modes values 250 - 255 are explicitely not defined and are to be used for custom, implementation specific modes. That is, codeplugs using those values for Operating Modes are not expected to be portable and transferable


 * `QA Params` Are Quick Access parameters that need to be accessed fast when scanning frequencies. Those are mode dependant.

### groups

| ID | BLOB_ID | Total |
|----|---------|-------|
| 2B | 2B      | 4 bytes |

    blob contains the list of channels in the group. Bounded to 32 (or so) channels

### contacts

CS () | .. | ... | BLOB_ID


### Blobs

BLOB_ID (2 bytes) | BLOB_SIZE (1 byte) | BLOB_DATA (var length)


FM :
    QA Params: CTCSS RX code (6 bits) | CTCSS TX Code (6 bits)
    Blob: Name

M17 :
    QA Params: CAN (3 bits) |
    Blob: DST |

Note:
The smallest channel is 16 bytes and at least 20 bytes with a blob (16 in channel file and 4 in blob file). With blob ID of 2 bytes, it means that we can at most use 1.25 MB of flash. If we consider at least 10 bytes in blob, this still makes for 29 bytes per channel (65536 channels = 1.8 MB).
With large Flash memories, (i.e. 32 MB) we would need 65536 channels using 512 B each to fill-up the memory.
Should we consider using larger IDs / blob IDs ?

As a comparison, here are a few common radios with the size of their external flash and advertised number of channels / contacts

MD-UV380
16 MB -> 3000 channels, 10k contacts

GD-77
1 MB -> 1024 channels, ?? contacts

CS-7000
16 MB  -> (65280 contacts, CS760)
CS-7000 Plus
32 MB


## What needs to be done?

* Understand how DMR codeplugs work? Have an idea of what is stored in a typical DMR contact
* Define the values to use for Squelch, power, ...
* Does BCFM need to be split in EU/US BCFM?
* Another way to handle modes would be to have a mode and submode field so that FM bandwidth / modulation factors can be classified as submodes of FM, it also make it easier to add subtly different modes like Reverse RTTY, reverse CW, or (let's be crazy) Codec2 Over DStar. The common denominator for mode would be that they use the same QA params and blob content. Otherwise it is another mode.
    * Suggestion -> Merge QA Params and mode to a new 4B long Mode. 1B for mode, 4b for submode, 20b for QA Params
* Define content of contact entry