#include <stdlib.h>
#include <string.h>
#include "internal.h"

//The driver builds security descriptors for its named objects. There is no
//access control to enforce in the sandbox, so these only need to produce
//well-formed (if permissive) structures that the driver can free again.

typedef struct {
    BYTE Value[6];
} SID_IDENTIFIER_AUTHORITY;

typedef struct {
    BYTE Revision;
    BYTE SubAuthorityCount;
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
    DWORD SubAuthority[];
} SID;

typedef struct {
    BYTE AclRevision;
    BYTE Sbz1;
    USHORT AclSize;
    USHORT AceCount;
    USHORT Sbz2;
} ACL;

#define SECURITY_DESCRIPTOR_SIZE 40

__winfnc BOOL AllocateAndInitializeSid(SID_IDENTIFIER_AUTHORITY *auth, BYTE count, DWORD sub0, DWORD sub1, DWORD sub2, DWORD sub3, DWORD sub4, DWORD sub5, DWORD sub6, DWORD sub7, SID **out) {
    if(count > 8) { winerr_set_code(E_INVALIDARG); return FALSE; }

    SID *sid = (SID*) malloc(sizeof(SID) + count * sizeof(DWORD));
    if(!sid) { winerr_set_errno(); return FALSE; }

    DWORD subs[8] = { sub0, sub1, sub2, sub3, sub4, sub5, sub6, sub7 };
    sid->Revision = 1;
    sid->SubAuthorityCount = count;
    sid->IdentifierAuthority = *auth;
    memcpy(sid->SubAuthority, subs, count * sizeof(DWORD));

    *out = sid;
    return TRUE;
}
WINAPI(AllocateAndInitializeSid)

__winfnc void *FreeSid(SID *sid) {
    free(sid);
    return NULL;
}
WINAPI(FreeSid)

__winfnc DWORD SetEntriesInAclA(ULONG count, void *entries, ACL *old_acl, ACL **new_acl) {
    //Hand back an empty ACL - the caller releases it with LocalFree
    ACL *acl = (ACL*) malloc(sizeof(ACL));
    if(!acl) return winerr_from_errno();
    *acl = (ACL) { .AclRevision = 2, .AclSize = sizeof(ACL) };

    *new_acl = acl;
    return ERROR_SUCCESS;
}
WINAPI(SetEntriesInAclA)

__winfnc BOOL InitializeSecurityDescriptor(void *descr, DWORD revision) {
    memset(descr, 0, SECURITY_DESCRIPTOR_SIZE);
    *(BYTE*) descr = revision;
    return TRUE;
}
WINAPI(InitializeSecurityDescriptor)

__winfnc BOOL SetSecurityDescriptorDacl(void *descr, BOOL dacl_present, ACL *dacl, BOOL dacl_defaulted) {
    return TRUE;
}
WINAPI(SetSecurityDescriptorDacl)
