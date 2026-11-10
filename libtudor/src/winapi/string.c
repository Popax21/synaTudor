#include <limits.h>
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

//Both conversion functions follow real Win32 semantics: a source length of -1 means "NUL-terminated,
//convert including the terminator", and a destination size of 0 means "only return the required
//size". (They used to convert just the first character - mbrtoc16/c16rtomb handle one character
//per call - leaving the rest of the caller's buffer untouched.)
__winfnc int MultiByteToWideChar(UINT code_page, DWORD flags, const char *mbstr, int mblen, char16_t *wstr, int wlen) {
    if(mblen < 0) mblen = (int) strlen(mbstr) + 1;
    if(mblen == 0) {
        winerr_set();
        return 0;
    }

    mbstate_t mbs = {0};
    const char *src = mbstr;
    size_t remaining = (size_t) mblen;
    int out_count = 0;
    while(remaining > 0) {
        char16_t c16;
        size_t consumed = mbrtoc16(&c16, src, remaining, &mbs);
        if(consumed == (size_t) -1 || consumed == (size_t) -2) { winerr_set(); return 0; }
        if(consumed == 0) consumed = 1; //mbrtoc16 returns 0 for an embedded NUL that was actually consumed
        if(wlen > 0) {
            if(out_count >= wlen) { winerr_set(); return 0; }
            wstr[out_count] = c16;
        }
        out_count++;
        src += consumed;
        remaining -= consumed;
    }
    return out_count;
}
WINAPI(MultiByteToWideChar)

__winfnc int WideCharToMultiByte(UINT code_page, DWORD flags, const char16_t *wstr, int wlen, char *mbstr, int mblen) {
    if(wlen < 0) wlen = winstr_len(wstr) + 1;
    if(wlen == 0) {
        winerr_set();
        return 0;
    }

    mbstate_t mbs = {0};
    int out_count = 0;
    for(int i = 0; i < wlen; i++) {
        char buf[MB_LEN_MAX];
        size_t sz = c16rtomb(buf, wstr[i], &mbs);
        if(sz == (size_t) -1) { winerr_set(); return 0; }
        if(mblen > 0) {
            if(out_count + (int) sz > mblen) { winerr_set(); return 0; }
            memcpy(mbstr + out_count, buf, sz);
        }
        out_count += (int) sz;
    }
    return out_count;
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