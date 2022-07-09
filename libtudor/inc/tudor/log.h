#ifndef LIBTUDOR_TUDOR_LOG_H
#define LIBTUDOR_TUDOR_LOG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

enum log_level {
    LOG_VERBOSE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR
};

extern pthread_mutex_t LOG_LOCK;
extern enum log_level LOG_LEVEL;

#define cant_fail(fnc) { if((fnc) < 0) { int __err = errno; log_error("'%s' failed: %d [%s]", #fnc, __err, strerror(__err)); abort(); } }
#define cant_fail_ret(fnc) { int __err =  (fnc); if(__err != 0) { log_error("'%s' failed: %d [%s]", #fnc, __err, strerror(__err)); abort(); } }
#define abort_perror(msg) { perror(msg); abort(); }

#define log_verbose(frmt, ...) { if(LOG_LEVEL <= LOG_VERBOSE)   { pthread_mutex_lock(&LOG_LOCK); fprintf(stdout, "[VRB] " frmt "\n", ##__VA_ARGS__); pthread_mutex_unlock(&LOG_LOCK); } }
#define log_debug(frmt, ...) { if(LOG_LEVEL <= LOG_DEBUG)       { pthread_mutex_lock(&LOG_LOCK); fprintf(stdout, "[DBG] " frmt "\n", ##__VA_ARGS__); pthread_mutex_unlock(&LOG_LOCK); } }
#define log_info(frmt, ...)  { if(LOG_LEVEL <= LOG_INFO )       { pthread_mutex_lock(&LOG_LOCK); fprintf(stdout, "[INF] " frmt "\n", ##__VA_ARGS__); pthread_mutex_unlock(&LOG_LOCK); } }
#define log_warn(frmt, ...)  { if(LOG_LEVEL <= LOG_WARN )       { pthread_mutex_lock(&LOG_LOCK); fprintf(stderr, "[WRN] " frmt "\n", ##__VA_ARGS__); pthread_mutex_unlock(&LOG_LOCK); } }
#define log_error(frmt, ...) { if(LOG_LEVEL <= LOG_ERROR)       { pthread_mutex_lock(&LOG_LOCK); fprintf(stderr, "[ERR] " frmt "\n", ##__VA_ARGS__); pthread_mutex_unlock(&LOG_LOCK); } }

#endif