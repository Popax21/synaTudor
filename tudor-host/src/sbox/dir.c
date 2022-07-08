#include <dirent.h>
#include "sandbox.h"
#include "ipc.h"

struct __dirstream {
    int cur_entry, num_entries;
    struct dirent entries[];
};

DIR *opendir(const char *path) {
    log_debug("Hooked opendir call for directory '%s'", path);

    //Send the host module a message
    struct ipc_msg_sbox_lsdir lsdir_msg = {
        .type = IPC_MSG_SBOX_LSDIR
    };
    strncpy(lsdir_msg.dir_path, path, IPC_SBOX_PATH_SIZE);
    ipc_send_msg(sbox_ipc_sock, &lsdir_msg, sizeof(lsdir_msg));

    //Receive the response
    struct ipc_msg_resp_sbox_lsdir resp_msg;
    ipc_recv_msg(sbox_ipc_sock, &resp_msg, IPC_MSG_RESP_SBOX_LSDIR, sizeof(resp_msg), sizeof(resp_msg), NULL);

    if(resp_msg.error != 0) {
        errno = resp_msg.error;
        return 0;
    }

    //Create directory stream / dirent array
    DIR *dir = (DIR*) malloc(sizeof(struct dirent) * resp_msg.num_entries);
    if(!dir) return NULL;

    dir->cur_entry = 0;
    dir->num_entries = resp_msg.num_entries;
    for(int i = 0; i < resp_msg.num_entries; i++) {
        struct dirent *ent = &dir->entries[i];
        ent->d_ino = 0;
        ent->d_off = 0;
        ent->d_reclen = sizeof(struct dirent);
        ent->d_type = resp_msg.entries[i].type;
        strncpy(ent->d_name, resp_msg.entries[i].name, sizeof(ent->d_name));
    }

    return dir;
}

int closedir(DIR *dir) {
    free(dir);
    return 0;
}

struct dirent *readdir(DIR *dir) {
    if(dir->cur_entry >= dir->num_entries) return NULL;
    return &dir->entries[dir->cur_entry++];
}