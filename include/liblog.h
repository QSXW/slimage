#pragma once

#ifndef __LIBLOG_H__
#define __LIBLOG_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>
#elif __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#endif

#include "typedefs.h"
#include "sequence.h"
#include "stream.h"
#include <time.h>

#if defined( __cplusplus )
extern "C" {
#endif /* __cplusplus */
enum { FILE_NOT_FOUND = 0x194 };
enum { JPEG_FORMAT_ERROR = 0x0, JPEG_OPEN_SUCCEED = 0x1, JPEG_SCAN_FAILED = 0x2 };
#define MSG_JPEG_FORMAT_ERROR   "The input file was not of Jpeg format"
#define MSG_JPEG_OPEN_SUCCEED   "Opened Jpeg format file successfully. File name: "
#define MSG_JPEG_SCAN_FAILED    "Faid to scan the Jpeg file. The input file may be not a standard Jpeg file."
#define MSG_FILE_NOT_FOUND      "No such file or directory"

#define LOG_FOLDER "./logs"
#define FILE_LOG_NAME "logs.txt"

typedef struct Log
{
    char msg[256];
} Log;

Bool
SaveLog(
    const char *filename,
    int flags
    );

#if defined( __cplusplus )
}
#endif /* __cplusplus */
#endif
/* end of the file */
