#include <liblog.h>
#include <stdlib.h>

Bool SaveLog(const char *filename, int flags)
{
    FILE    *fp;
    char    fileRoot[256];
    time_t  *destTime;
    Log     *log;

    sprintf(fileRoot, "%s/%s", LOG_FOLDER, FILE_LOG_NAME);
#ifdef WIN32
    if (_access(LOG_FOLDER, 0))
    {
        _mkdir(LOG_FOLDER);
    }
#elif __linux__
    DIR *dir;
    if (!(dir = opendir(LOG_FOLDER)))
    {
        mkdir(LOG_FOLDER, S_IRWXU);
    }
    else
    {
        closedir(dir);
    }
#endif

    fp = fopen(fileRoot, "a+");
    log = (Log *)malloc(sizeof(Log));
    destTime = (time_t *)malloc(sizeof(time_t));

    if (fp)
    {
        time(destTime);
        switch (flags)
        {
        case JPEG_FORMAT_ERROR:
            sprintf(log->msg, "%I64d: %s.\n", (unsigned long long int)*destTime, MSG_JPEG_FORMAT_ERROR);
            break;
        case JPEG_OPEN_SUCCEED:
            sprintf(log->msg, "%I64d: %s%s.\n", (unsigned long long int)*destTime, MSG_JPEG_OPEN_SUCCEED, filename);
            break;
        case FILE_NOT_FOUND:
            sprintf(log->msg, "%I64d: %s: %s.\n", (unsigned long long int)*destTime, filename, MSG_FILE_NOT_FOUND);
            break;
        case JPEG_SCAN_FAILED:
            sprintf(log->msg, "%I64d: %s: %s.\n", (unsigned long long int)*destTime, filename, MSG_JPEG_SCAN_FAILED);
        default:
            return FALSE;
        }
        fwrite(log->msg, cslen((BYTE *)log->msg), 1, fp);
        printf("%s", log->msg);
    }

    free(log);
    free(destTime);
    fclose(fp);
    return TRUE;
}
