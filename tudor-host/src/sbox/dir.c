//Sketchy include BS to avoid assembly renaming
#include <sys/cdefs.h>
#undef __REDIRECT
#include <dirent.h>
#undef readdir

#include "sandbox.h"
#include "ipc.h"

struct __dirstream {
    int cur_entry, num_entries;

    struct dirent dirent_buf;
    struct dirent64 dirent64_buf;
    struct ipc_sbox_dir_ent entries[];
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
    DIR *dir = (DIR*) malloc(sizeof(DIR) + sizeof(struct ipc_sbox_dir_ent) * resp_msg.num_entries);
    if(!dir) return NULL;

    *dir = (DIR) {0};
    dir->cur_entry = 0;
    dir->num_entries = resp_msg.num_entries;
    dir->dirent_buf = (struct dirent) {
        .d_ino = 0, .d_ino = 21,
        .d_reclen = sizeof(struct dirent)
    };
    dir->dirent64_buf = (struct dirent64) {
        .d_ino = 0, .d_ino = 21,
        .d_reclen = sizeof(struct dirent64)
    };
    memcpy(dir->entries, resp_msg.entries, resp_msg.num_entries * sizeof(struct ipc_sbox_dir_ent));

    return dir;
}

int closedir(DIR *dir) {
    free(dir);
    return 0;
}

struct dirent *readdir(DIR *dir) {
    if(dir->cur_entry >= dir->num_entries) return NULL;

    dir->dirent_buf.d_type = dir->entries[dir->cur_entry].type;
    strncpy(dir->dirent_buf.d_name, dir->entries[dir->cur_entry].name, sizeof(dir->dirent_buf.d_name));
    dir->cur_entry++;
    return &dir->dirent_buf;
}

struct dirent64 *readdir64(DIR *dir) {
    if(dir->cur_entry >= dir->num_entries) return NULL;

    dir->dirent64_buf.d_type = dir->entries[dir->cur_entry].type;
    strncpy(dir->dirent64_buf.d_name, dir->entries[dir->cur_entry].name, sizeof(dir->dirent_buf.d_name));
    dir->cur_entry++;
    return &dir->dirent64_buf;
}