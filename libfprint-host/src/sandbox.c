#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/capability.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <seccomp.h>
#include "sandbox.h"

#define strfy(x) _strfy(x)
#define _strfy(x) #x

#define ABORT_PERROR(msg) { perror(msg); abort(); }

static void write_to(const char *fname, const char *cnts) {
    int len = strlen(cnts);
    FILE *file = fopen(fname, "w");
    if(!file) ABORT_PERROR("Couldn't open file");
    if(fwrite(cnts, 1, len, file) < len) ABORT_PERROR("Couldn't write to file");
    if(fclose(file) < 0) ABORT_PERROR("Couldn't close file");
}

static void setup_uid_gid() {
    //Setup UID / GID mapping
    //We need to have a parent process write our UID / GID map, so we need to fork first
    sigset_t sigs, oldsigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGUSR1);
    if(sigprocmask(SIG_BLOCK, &sigs, &oldsigs) < 0) ABORT_PERROR("Error setting signal mask");

    int ppid = (int) getpid();
    int cpid = (int) fork();
    if(cpid < 0) ABORT_PERROR("Couldn't fork process");
    if(cpid == 0) {
        //Unshare user namespace
        if(unshare(CLONE_NEWUSER) < 0) ABORT_PERROR("Couldn't unshare user namspace");

        //Notify and wait for parent process
        int sig;
        if(kill(ppid, SIGUSR1) < 0) ABORT_PERROR("Couldn't notify parent process");
        if(sigwait(&sigs, &sig) < 0) ABORT_PERROR("Couldn't wait for parent process");
    } else {
        //Wait for child process to be ready
        int sig;
        if(sigwait(&sigs, &sig) < 0) ABORT_PERROR("Couldn't wait for child process");

        //Write UID / GID map
        char path_buf[128];
        sprintf(path_buf, "/proc/%d/setgroups", cpid);
        write_to(path_buf, "deny");
        sprintf(path_buf, "/proc/%d/uid_map", cpid);
        write_to(path_buf, strfy(SANDBOX_UID) " " strfy(SANDBOX_UID) " 1");
        sprintf(path_buf, "/proc/%d/gid_map", cpid);
        write_to(path_buf, strfy(SANDBOX_GID) " " strfy(SANDBOX_GID) " 1");

        //Notify child process
        if(kill(cpid, SIGUSR1) < 0) ABORT_PERROR("Couldn't notify child process");

        //Wait for child
        int status;
        if(wait(&status) < 0) ABORT_PERROR("Couldn't wait for child");
        exit(status);
    }

    //Change UID / GID
    if(setresuid(SANDBOX_UID, SANDBOX_UID, SANDBOX_UID) < 0) ABORT_PERROR("Couldn't change UID");
    if(setresgid(SANDBOX_GID, SANDBOX_GID, SANDBOX_GID) < 0) ABORT_PERROR("Couldn't change GID");

    //Restore signal mask
    sigandset(&oldsigs, &oldsigs, &sigs);
    if(sigprocmask(SIG_UNBLOCK, &oldsigs, NULL) < 0) ABORT_PERROR("Couldn't restore signal mask");
}

static void unmount_root() {
    //Create a temporary directory and remount it as a bind mount point
    char tmpRoot[] = "/tmp/tudorRootXXXXXX";
    if(!mkdtemp(tmpRoot)) ABORT_PERROR("Couldn't create temporary root directory");
    if(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) < 0) ABORT_PERROR("Couldn't change filesystem propagation");
    if(mount(tmpRoot, tmpRoot, NULL, MS_BIND | MS_NOSUID | MS_RDONLY, NULL) < 0) ABORT_PERROR("Couldn't remount tmp root");

    //Pivot root
    if(chdir(tmpRoot) < 0) ABORT_PERROR("Couldn't chdir into tmp root");
    if(syscall(SYS_pivot_root, ".", ".") < 0) ABORT_PERROR("Couldn't pivot root");
    if(umount2(".", MNT_DETACH) < 0) ABORT_PERROR("Couldn't unmount old rootfs");
    if(chdir("/") < 0) ABORT_PERROR("Couldn't chdir into new root");
}

static void setup_seccomp(int *notif_fd) {
    //Initialize filter
    scmp_filter_ctx scmp_ctx = seccomp_init(SCMP_ACT_KILL_PROCESS);
    if(scmp_ctx == NULL) ABORT_PERROR("Couldn't initialize the SECCOMP context");

    //Add rules
    if(
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_exit, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_exit_group, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_rt_sigreturn, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_NOTIFY, SYS_open, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_close, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_read, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_write, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_sendmsg, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_recvmsg, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_brk, 0) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_mmap, 1, SCMP_A3_32(SCMP_CMP_MASKED_EQ, MAP_PRIVATE | MAP_ANONYMOUS, MAP_PRIVATE | MAP_ANONYMOUS)) < 0) ||
        (seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_munmap, 0) < 0)
    ) ABORT_PERROR("Couldn't add SECCOMP rules");

    //Load policy
    if(seccomp_load(scmp_ctx) < 0) ABORT_PERROR("Couldn't load SECCOMP policy");
    *notif_fd = seccomp_notify_fd(scmp_ctx);
    if(*notif_fd < 0) ABORT_PERROR("Couldn't get SECCOMP notification FD");

    seccomp_release(scmp_ctx);
}

void activate_sandbox(int *sockfd, int *seccomp_notif_fd) {
    //Setup UID / GID
    setup_uid_gid();

    //Close all file descriptors, but preserve stdin/out/err and socket FDs
    if(sockfd && dup2(*sockfd, 3) < 0) ABORT_PERROR("Couldn't clone socket FD"); 
    closefrom(4);
    if(sockfd) *sockfd = 3;

    //Unshare all namspaces
    if(unshare(CLONE_FS | CLONE_FILES | CLONE_SYSVSEM | CLONE_NEWCGROUP | CLONE_NEWIPC | CLONE_NEWNET | CLONE_NEWNS | CLONE_NEWUTS) < 0) ABORT_PERROR("Couldn't unshare namspaces");

    //Unmount root filesystem
    unmount_root();

    //Drop all capabilities
    cap_t cap = cap_init();
    if(cap == NULL) ABORT_PERROR("Couldn't allocate capabilities");
    if(cap_clear(cap) < 0) ABORT_PERROR("Couldn't clear capabilities");
    if(cap_set_proc(cap) < 0) ABORT_PERROR("Couldn't set capabilities");
    if(cap_free(cap) < 0) ABORT_PERROR("Couldn't free capabilities");

    //Restrict all syscalls using seccomp, only keep the ones required to communicate with the module
    setup_seccomp(seccomp_notif_fd);
}