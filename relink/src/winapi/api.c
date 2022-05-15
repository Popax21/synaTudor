#include <stdlib.h>
#include <string.h>
#include "internal.h"

static struct __winapi_descr *descr_head;

void __register_windows_api(struct __winapi_descr *descr) {
    descr->next = descr_head;
    descr_head = descr;
}

void *resolve_windows_api(const char *name) {
    for(struct __winapi_descr *d = descr_head; d; d = d->next) {
        if(strcmp(d->name, name) == 0) return d->func;
    }
    return NULL;
}

int winstr_len(const char16_t *str) {
    int len = 0;
    for(const char16_t *p = str; *p; p++) len++;
    return len;
}

char16_t *winstr_from_str(const char *str) {
    if(!str) return NULL;
    mbstate_t mstate;

    //Determine length of converted string
    int len = 0;
    mstate = (mbstate_t) {0};
    for(const char *p = str; *p;) {
        p += mbrtoc16(NULL, p, MB_CUR_MAX, &mstate);
        len++;
    }

    //Convert string
    char16_t *wstr = (char16_t*) malloc((len + 1) * sizeof(char16_t));
    if(!wstr) { perror("Couldn't allocate memory for string"); abort(); }
    memset(wstr, 0, (len + 1) * sizeof(char16_t));
    mstate = (mbstate_t) {0};
    for(char16_t *d = wstr; *str; d++) {
        str += mbrtoc16(d, str, MB_CUR_MAX, &mstate);
    }

    return wstr;
}

char *winstr_to_str(const char16_t *wstr) {
    if(!wstr) return NULL;
    mbstate_t mstate;

    //Determine length of converted string
    int len = 0;
    mstate = (mbstate_t) {0};
    for(const char16_t *p = wstr; *p; p++) {
        len += c16rtomb(NULL, *p, &mstate);
    }

    //Convert string
    char *str = (char*) malloc(len + 1);
    if(!str) { perror("Couldn't allocate memory for string"); abort(); }
    memset(str, 0, len + 1);
    mstate = (mbstate_t) {0};
    for(char *d = str; *wstr; wstr++) {
        d += c16rtomb(d, *wstr, &mstate);
    }

    return str;
}