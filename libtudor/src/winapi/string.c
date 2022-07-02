#include "internal.h"

#define MAX_DEFAULTCHAR 2
#define MAX_LEADBYTES 12

typedef struct {
    UINT MaxCharSize;
    BYTE DefaultChar[MAX_DEFAULTCHAR];
    BYTE LeadByte[MAX_LEADBYTES];
} CPINFO;

__winfnc UINT GetACP() {
    return 1200; //UTF-16
}
WINAPI(GetACP)

__winfnc BOOL IsValidCodePage(UINT code_page) {
    switch(code_page) {
        case 65001: return TRUE; //UTF-8
        case 1200: return TRUE; //UTF-16
        case 12000: return TRUE; //UTF-32
        default: return FALSE;
    }
}
WINAPI(IsValidCodePage)

__winfnc BOOL GetCPInfo(UINT code_page, CPINFO *info) {
    switch(code_page) {
        case 65001: *info = (CPINFO) { .MaxCharSize = 1, .DefaultChar = { '?' } }; return TRUE; //UTF-8
        case 1200: *info = (CPINFO) { .MaxCharSize = 2, .DefaultChar = { '?' } }; return TRUE; //UTF-16
        case 12000: *info = (CPINFO) { .MaxCharSize = 4, .DefaultChar = { '?' } }; return TRUE; //UTF-32
        default: return FALSE;
    }
}
WINAPI(GetCPInfo)

__winfnc int MultiByteToWideChar(UINT code_page, DWORD flags, const char *mbstr, int mblen, char16_t *wstr, int wlen) {
    if(mblen < 0) mblen = strlen(mbstr);
    if(mblen == 0) {
        winerr_set();
        return 0;
    }

    mbstate_t mbs = {0};
    int sz = mbrtoc16((0 < wlen) ? wstr : NULL, mbstr, mblen, &mbs);
    if(sz > 0) return sz;
    winerr_set();
    return 0;
}
WINAPI(MultiByteToWideChar)

__winfnc int WideCharToMultiByte(UINT code_page, DWORD flags, const char16_t *wstr, int wlen, char *mbstr, int mblen) {
    if(wlen < 0) wlen = winstr_len(wstr);
    if(wlen == 0) {
        winerr_set();
        return 0;
    }

    mbstate_t mbs = {0};
    int sz = c16rtomb((0 < mblen) ? mbstr : NULL, *wstr, &mbs);
    if(sz > 0) return sz;
    winerr_set();
    return 0;
}
WINAPI(WideCharToMultiByte)

__winfnc int GetStringTypeW(DWORD info_type, const char16_t *str, int strlen, WORD* char_types) {
    if(strlen < 0) strlen = winstr_len(str);

    //TODO
    memset(char_types, 0, strlen * sizeof(WORD));
    return 0;
}
WINAPI(GetStringTypeW)

__winfnc int LCMapStringW(DWORD lcid, DWORD flags, const char16_t *in, int inlen, char16_t *out, int outlen) {
    if(inlen < 0) inlen = winstr_len(in);
    if(outlen == 0) return (inlen + 1) * sizeof(char16_t);

    //TODO
    if(outlen < inlen) {
        winerr_set();
        return 0;
    }

    memcpy(out, in, (inlen + 1) * sizeof(char16_t));
    return outlen;
}
WINAPI(LCMapStringW)

__winfnc void RtlInitUnicodeString(UNICODE_STRING *dst, const char16_t *src) {
    if(src) {
        int len = winstr_len(src);
        dst->Length = dst->MaximumLength = len+1;
        dst->Buffer = (char16_t*) malloc((len+1) * sizeof(char16_t));
        if(!dst->Buffer) { perror("Couldn't allocate UNICODE_STRING buffer"); abort(); }
        memcpy(dst->Buffer, src, (len+1) * sizeof(char16_t));
    } else {
        dst->Length = dst->MaximumLength = 0;
        dst->Buffer = NULL;
    }
}
WINAPI(RtlInitUnicodeString)

__winfnc int lstrlenA(const char *str) {
    return (int) strlen(str);
}
WINAPI(lstrlenA)

__winfnc char16_t *lstrcpynW(char16_t *dst, const char16_t *src, int max_len) {
    for(; *src && max_len > 0; src++, dst++, max_len--) *dst = *src;
    *dst = 0;
    return dst;
}
WINAPI(lstrcpynW);

__winfnc int lstrcmpW(const char16_t *a, const char16_t *b) {
    int a_len = winstr_len(a), b_len = winstr_len(b);
    if(a_len < b_len) return -1;
    if(a_len > b_len) return -1;
    return memcmp(a, b, a_len * sizeof(char16_t));
}
WINAPI(lstrcmpW);