#include <tudor/log.h>

pthread_mutex_t LOG_LOCK = PTHREAD_MUTEX_INITIALIZER;
enum log_level LOG_LEVEL = LOG_INFO;