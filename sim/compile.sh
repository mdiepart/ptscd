#!/bin/bash
gcc main.c -I ./littlefs/ littlefs/*.c littlefs/bd/*.c -o sim
