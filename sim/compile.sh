#!/bin/bash
gcc main.c -I ./littlefs/ littlefs/*.c littlefs/bd/*.c ptscd.c -Og -g -o sim
