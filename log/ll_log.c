#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "ll_log.h"

#define LOG_PRINT_BUFF_LEN 512

int g_log_fd = -1;
int g_log_level = LL_LOG_DEBUG;

static int ll_log_open(const char *log_dir)
{
    char *p = NULL;
    char *dir = NULL;
    char c;
    int ret;
    dir = (char*)malloc(strlen(log_dir) + 1);
    if (dir == NULL) {
        return -errno;
    }
    memcpy(dir, log_dir, strlen(log_dir) + 1);
    p = dir;

    while(*p != '\0') {
        if (*p == '/') {
            c = *(p + 1);
            *(p + 1) = 0;
            ret = mkdir(dir, (S_IRUSR | S_IWUSR | S_IXUSR | S_IXGRP | S_IXOTH));
            if (ret < 0) {
                if (errno != EEXIST) {
                    ret = -errno;
                    goto err;
                }
            }
            *(p + 1) = c;
        }
        p ++;
    }
    ret = open(dir, O_APPEND | O_RDWR | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH));
    if (ret < 0) {
        ret = -errno;
        goto err;
    }
  err:
    if (dir) {
        free(dir);
        dir = NULL;
    }
        
    return ret;
}


int ll_log_init(int log_fd, const char *log_dir)
{
    int ret;
    if (log_fd >= 0) {
        g_log_fd = log_fd;
        return 0;
    } else if (log_dir) {
        ret = ll_log_open((char *)log_dir);
        assert(ret >= 0);
        g_log_fd = ret;
    } else {
        ret = ll_log_open(LL_LOG_DIR);
        assert(ret >= 0);
        g_log_fd = ret;
    }
    return 0;
}

static void ll_log(int level, const char *buff, size_t len)
{

    if (g_log_fd < 0) {
        ll_log_init(-1, NULL);
    }
    if (g_log_level >= level) {
        write(g_log_fd, buff, len);
    }
}

void ll_log_print(int level, const char *format, ...)
{
    char buff[LOG_PRINT_BUFF_LEN];
    int ret = 0;
    va_list ap;

    time_t now = time(NULL);
    struct tm tm;
    localtime_r(&now, &tm);

    ret = strftime(buff, LOG_PRINT_BUFF_LEN - 1, "%Y-%m-%d %H:%M:%S ", &tm);

    switch(level) {
        case LL_LOG_ERROR:
            ret += snprintf(buff + ret, LOG_PRINT_BUFF_LEN - 1 - ret, "ERROR: "); 
            break;
        case LL_LOG_WARNING:
            ret += snprintf(buff + ret, LOG_PRINT_BUFF_LEN - 1 - ret, "WARNING: "); 
            break;
        case LL_LOG_INFO:
            ret += snprintf(buff + ret, LOG_PRINT_BUFF_LEN - 1 - ret, "INFO: "); 
            break;
        case LL_LOG_DEBUG:
            ret += snprintf(buff + ret, LOG_PRINT_BUFF_LEN - 1 - ret, "DEBUG: ");
            break;
        default:
            ret += snprintf(buff + ret, LOG_PRINT_BUFF_LEN - 1 - ret, "UNKOWN");
    }
    va_start(ap, format);
    ret += vsnprintf(buff + ret, LOG_PRINT_BUFF_LEN - 1 - ret, format, ap);
    ll_log(level, buff, ret);
    va_end(ap);
}

void ll_log_print_debug(int level, const char *file_name, const char *func_name,
                        const int line_num, const char *format, ...)
{
    char buff[LOG_PRINT_BUFF_LEN];
    int ret = 0;
    va_list ap;

    time_t now = time(NULL);
    struct tm tm;
    localtime_r(&now, &tm);

    ret = strftime(buff, LOG_PRINT_BUFF_LEN - 1, "%Y-%m-%d %H:%M:%S ", &tm);
    ret += snprintf(buff + ret, LOG_PRINT_BUFF_LEN - 1 - ret, "DEBUG: file: %s, func: %s, line: %d: ",
                        file_name, func_name, line_num);
    va_start(ap, format);
    ret += vsnprintf(buff + ret, LOG_PRINT_BUFF_LEN - 1 - ret, format, ap);
    ll_log(level, buff, ret);
    va_end(ap);
}
                            
/*
int main()
{
    ll_log_print(LL_LOG_ERROR, "hello log %s %s %s\n", "adsa", "ftrre", "tdfgt");
    DEBUG("%s\n", "test");
    return 0;
}
*/
