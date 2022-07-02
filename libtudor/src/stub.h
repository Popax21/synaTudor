#ifndef LIBTUDOR_STUB_H
#define LIBTUDOR_STUB_H

#if !defined(__x86_64__) && defined(DBGIMPORT)
#warning Import debugging only supported on x86-64!
#undef DBGIMPORT
#endif
#if !defined(__x86_64__) && defined(DBGWDF)
#warning WDF debugging only supported on x86-64!
#undef DBGWDF
#endif

#ifdef DBGIMPORT
void *create_import_stub(const char *lib, const char *name);
#endif

#ifdef DBGWDF
void *create_wdf_stub(int num);
#endif

#endif