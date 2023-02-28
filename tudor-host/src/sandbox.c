#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sched.h>
#include <fcntl.h>
#include <asm/prctl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <sys/capability.h>
#include <sys/utsname.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <linux/netlink.h>
#include <seccomp.h>
#include <tudor/log.h>
#include "sandbox.h"
#include "ipc.h"

#define strfy(x) _strfy(x)
#define _strfy(x) #x

static struct utsname sbox_utsname;

int uname(struct utsname *oname) {
    *oname = sbox_utsname;
    return 0;
}

static int sbox_usb_fd;
static uint8_t sbox_usb_bus, sbox_usb_addr;

ssize_t readlink(const char *path, char *buf, size_t size) {
    //Check if path is /proc/self/fd/<FD>
    int fd;
    if(sscanf(path, "/proc/self/fd/%d", &fd) != 1) {
        log_error("Tried to readlink prohibited path '%s'!", path);
        abort();
    }

    //Check for the USB FD
    if(fd != sbox_usb_fd) {
        log_error("Tried to get path of prohibited FD %d!", fd);
        abort();
    }

    //Return a string immetating a device with the specific bus / address
    return snprintf(buf, size, "/dev/bus/usb/%hhu/%hhu", sbox_usb_bus, sbox_usb_addr);
}

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
#ifdef UNMOUNTFS
    //Pivot root
    cant_fail(mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL));
    cant_fail(mount("/sbin/tudor", "/sbin/tudor", NULL, MS_BIND | MS_NOSUID | MS_RDONLY, NULL));
    cant_fail(chdir("/sbin/tudor"));
    cant_fail(syscall(SYS_pivot_root, ".", "."));
    cant_fail(umount2(".", MNT_DETACH));
    cant_fail(chdir("/"));
#endif
}

static void setup_seccomp() {
    //Initialize filter
    scmp_filter_ctx scmp_ctx = seccomp_init(SCMP_ACT_KILL_PROCESS);
    if(!scmp_ctx) abort_perror("Couldn't initialize the SECCOMP context");

    //Add rules
    pid_t pid = getpid();
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(kill), 1, SCMP_A0_32(SCMP_CMP_EQ, pid)));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(tkill), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(tgkill), 1, SCMP_A0_32(SCMP_CMP_EQ, pid)));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigaction), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigprocmask), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigreturn), 0));

    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(getpid), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(gettid), 0));

    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 1, SCMP_A3_32(SCMP_CMP_MASKED_EQ, MAP_PRIVATE | MAP_ANONYMOUS, MAP_PRIVATE | MAP_ANONYMOUS)));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(madvise), 0));

    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(clone), 1, SCMP_A0_32(SCMP_CMP_MASKED_EQ, CLONE_THREAD, CLONE_THREAD)));
#if defined(SYS_clone3)
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(clone3), 0));
#endif
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(rseq), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(futex), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(get_robust_list), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(set_robust_list), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(prctl), 1, SCMP_A0_32(SCMP_CMP_EQ, PR_SET_NAME)))
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(arch_prctl), 1, SCMP_A0_32(SCMP_CMP_EQ, ARCH_SET_GS)))

    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(getrandom), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(eventfd), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(eventfd2), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(timerfd_create), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(timerfd_gettime), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(timerfd_settime), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(gettimeofday), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(clock_getres), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(clock_gettime), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(clock_nanosleep), 0));

    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(access), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(stat), 0));
#if defined(SYS_statx)
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(statx), 0));
#endif
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_fstat, 0));
#if defined(SYS_newfstatat)
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SYS_newfstatat, 0));
#endif
#if defined(SYS_statfs) && defined(SYS_fstatfs)
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(statfs), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(fstatfs), 0));
#endif
#if defined(SYS_name_to_handle_at) && defined(SYS_open_by_handle_at)
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(name_to_handle_at), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(open_by_handle_at), 0));
#endif

    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(poll), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(ppoll), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(open), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(openat), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ERRNO(EPERM), SCMP_SYS(openat2), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(fcntl), 1, SCMP_A1_32(SCMP_CMP_EQ, F_GETFD)));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(fcntl), 1, SCMP_A1_32(SCMP_CMP_EQ, F_SETFD)));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(fcntl), 1, SCMP_A1_32(SCMP_CMP_EQ, F_GETFL)));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(fcntl), 1, SCMP_A1_32(SCMP_CMP_EQ, F_SETFL)));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(ioctl), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(lseek), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0));
#if defined(SYS_readv) && defined(SYS_writev)
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(readv), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(writev), 0));
#endif
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0));

    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(sendmsg), 0));
    cant_fail(seccomp_rule_add(scmp_ctx, SCMP_ACT_ALLOW, SCMP_SYS(recvmsg), 0));

    //Load policy
    cant_fail(seccomp_load(scmp_ctx));
    seccomp_release(scmp_ctx);
}

void activate_sandbox() {
    //Query the system uname
    cant_fail(syscall(SYS_uname, &sbox_utsname));
    strncpy(sbox_utsname.nodename, "tudor-host", sizeof(sbox_utsname.nodename)); //Don't leak host name

    //Setup resource limits
    cant_fail(setrlimit(RLIMIT_DATA, &(struct rlimit) { .rlim_cur = SANDBOX_DATA_LIMIT, .rlim_max = SANDBOX_DATA_LIMIT }));
    cant_fail(setrlimit(RLIMIT_STACK, &(struct rlimit) { .rlim_cur = SANDBOX_STACK_LIMIT, .rlim_max = SANDBOX_STACK_LIMIT }));
    cant_fail(setrlimit(RLIMIT_NOFILE, &(struct rlimit) { .rlim_cur = SANDBOX_MAX_FDS, .rlim_max = SANDBOX_MAX_FDS }));
    cant_fail(setrlimit(RLIMIT_NPROC, &(struct rlimit) { .rlim_cur = SANDBOX_MAX_THREADS, .rlim_max = SANDBOX_MAX_THREADS }));
    cant_fail(setrlimit(RLIMIT_FSIZE, &(struct rlimit) { .rlim_cur = 0, .rlim_max = 0 }));

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
    setup_seccomp();
}

void setup_usb_sbox(int usb_fd, uint8_t usb_bus, uint8_t usb_addr) {
    sbox_usb_fd = usb_fd;
    sbox_usb_bus = usb_bus;
    sbox_usb_addr = usb_addr;
}
