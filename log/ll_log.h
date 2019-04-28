#ifndef __LL_LOH_H__
#define __LL_LOH_H__

#define LL_LOG_ERROR    1
#define LL_LOG_WARNING  2
#define LL_LOG_INFO     3
#define LL_LOG_DEBUG    4

#define LL_LOG_DIR  "/var/log/ll/my.log"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void ll_log_print(int level, const char *format, ...);
void ll_log_print_debug(int level, const char *file_name, const char *func_name,
                                const int line_num, const char *format, ...);

#define ERROR(fmt, ...) \
    do { \
        ll_log_print(LL_LOG_ERROR, (fmt), ##__VA_ARGS__); \
    } while(0);
#define WARNING(fmt, ...) \
    do { \
        ll_log_print(LL_LOG_WARNING, (fmt), ##__VA_ARGS__); \
    } while(0);
#define INFO(fmt, ...) \
    do { \
        ll_log_print(LL_LOG_INFO, (fmt), ##__VA_ARGS__); \
    } while(0);
#define DEBUG(fmt, ...) \
    do { \
        ll_log_print_debug(LL_LOG_DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__); \
    } while(0);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //__LL_LOH_H__
