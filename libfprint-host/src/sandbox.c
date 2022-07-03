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
#include <tudor/log.h>
#include "sandbox.h"

#define strfy(x) _strfy(x)
#define _strfy(x) #x

static void write_to(const char *fname, const char *cnts) {
    int len = strlen(cnts);
    FILE *file = fopen(fname, "w");
    if(!file) abort_perror("Couldn't open file");
    cant_fail(fwrite(cnts, 1, len, file));
    cant_fail(fclose(file));
}

static void setup_uid_gid() {
    //Setup UID / GID mapping
    //We need to have a parent process write our UID / GID map, so we need to fork first
    sigset_t sigs, oldsigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGUSR1);
    cant_fail(sigprocmask(SIG_BLOCK, &sigs, &oldsigs));

    pid_t ppid = getpid(), cpid;
    cant_fail(cpid = fork());
    if(cpid == 0) {
        //Unshare user namespace
        cant_fail(unshare(CLONE_NEWUSER));

        //Notify and wait for parent process
        int sig;
        cant_fail(kill(ppid, SIGUSR1));
        cant_fail(sigwait(&sigs, &sig));
    } else {
        //Wait for child process to be ready
        int sig;
        cant_fail(sigwait(&sigs, &sig));

        //Write UID / GID map
        char path_buf[128];
        sprintf(path_buf, "/proc/%d/setgroups", cpid);
        write_to(path_buf, "deny");
        sprintf(path_buf, "/proc/%d/uid_map", cpid);
        write_to(path_buf, strfy(SANDBOX_UID) " " strfy(SANDBOX_UID) " 1");
        sprintf(path_buf, "/proc/%d/gid_map", cpid);
        write_to(path_buf, strfy(SANDBOX_GID) " " strfy(SANDBOX_GID) " 1");

        //Notify child process
        cant_fail(kill(cpid, SIGUSR1));

        //Wait for child
        int status;
        cant_fail(wait(&status));
        exit(status);
    }

    //Change UID / GID
    cant_fail(setresuid(SANDBOX_UID, SANDBOX_UID, SANDBOX_UID));
    cant_fail(setresgid(SANDBOX_GID, SANDBOX_GID, SANDBOX_GID));

    //Restore signal mask
    sigandset(&oldsigs, &oldsigs, &sigs);
    cant_fail(sigprocmask(SIG_UNBLOCK, &oldsigs, NULL));
}

static void unmount_root() {
    //Create a temporary directory and remount it as a bind mount point
    char tmpRoot[] = "/tmp/tudorRootXXXXXX";
    if(!mkdtemp(tmpRoot)) abort_perror("Couldn't create temporary root directory");
    cant_fail(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL));
    cant_fail(mount(tmpRoot, tmpRoot, NULL, MS_BIND | MS_NOSUID | MS_RDONLY, NULL));

    //Pivot root
    cant_fail(chdir(tmpRoot));
    cant_fail(syscall(SYS_pivot_root, ".", "."));
    cant_fail(umount2(".", MNT_DETACH));
    cant_fail(chdir("/"));
}

static void setup_seccomp(int *notif_fd) {
    //Initialize filter
    scmp_filter_ctx scmp_ctx = seccomp_init(SCMP_ACT_KILL_PROCESS);
    if(!scmp_ctx) abort_perror("Couldn't initialize the SECCOMP context");

    //Add rules
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_exit, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_exit_group, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_rt_sigreturn, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_NOTIFY, SYS_open, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_close, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_read, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_write, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_sendmsg, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_recvmsg, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_brk, 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_mmap, 1, SCMP_A3_32(SCMP_CMP_MASKED_EQ, MAP_PRIVATE | MAP_ANONYMOUS, MAP_PRIVATE | MAP_ANONYMOUS)));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_munmap, 0));

    //Load policy
    cant_fail(seccomp_load(scmp_ctx));
    cant_fail(*notif_fd = seccomp_notify_fd(scmp_ctx));

    seccomp_release(scmp_ctx);
}

void activate_sandbox(int *seccomp_notif_fd) {
    //Setup UID / GID
    setup_uid_gid();

    //Close all file descriptors, but preserve stdin/out/err
    closefrom(3);

    //Unshare all namspaces
    cant_fail(unshare(CLONE_FS | CLONE_FILES | CLONE_SYSVSEM | CLONE_NEWCGROUP | CLONE_NEWIPC | CLONE_NEWNET | CLONE_NEWNS | CLONE_NEWUTS));

    //Unmount root filesystem
    unmount_root();

    //Drop all capabilities
    cap_t cap = cap_init();
    if(!cap) abort_perror("Couldn't allocate capabilities");
    cant_fail(cap_clear(cap));
    cant_fail(cap_set_proc(cap));
    cant_fail(cap_free(cap));

    //Restrict all syscalls using SECCOMP, only keep the ones required to communicate with the module
    setup_seccomp(seccomp_notif_fd);
}