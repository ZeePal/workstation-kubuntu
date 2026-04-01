/*
 * Compile with:
 * cc -O2 -Wall -Wextra $(pkg-config --cflags libseccomp) \
 *   -o /opt/agentwrap/bin/agentwrap-seccomp /opt/agentwrap/agentwrap-seccomp.c \
 *   $(pkg-config --libs libseccomp)
 *
 * The binary writes a seccomp BPF program to stdout for bwrap --seccomp FD.
 *
 * This profile starts from a baseline container-style allowlist and then
 * applies a very small number of extra argument-filtered rules.
 */

#include <stdio.h>

#if __has_include(<seccomp.h>)

#include <errno.h>
#include <linux/sched.h>
#include <seccomp.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef AF_VSOCK
#define AF_VSOCK 40
#endif

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

static int allow_by_name(scmp_filter_ctx ctx, const char *syscall_name) {
  int syscall_nr = seccomp_syscall_resolve_name(syscall_name);

  if (syscall_nr == __NR_SCMP_ERROR) {
    return 0;
  }

  return seccomp_rule_add(ctx, SCMP_ACT_ALLOW, syscall_nr, 0);
}

static int allow_all_by_name(scmp_filter_ctx ctx, const char *const *syscalls,
                             size_t count) {
  size_t i;

  for (i = 0; i < count; i++) {
    if (allow_by_name(ctx, syscalls[i]) != 0) {
      return -1;
    }
  }

  return 0;
}

static int allow_clone_non_namespace(scmp_filter_ctx ctx) {
  const uint64_t namespace_mask = CLONE_NEWCGROUP | CLONE_NEWIPC | CLONE_NEWNET |
                                  CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWUSER |
                                  CLONE_NEWUTS;

  return seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clone), 1,
                          SCMP_A0_64(SCMP_CMP_MASKED_EQ, namespace_mask, 0));
}

static int allow_socket_non_vsock(scmp_filter_ctx ctx) {
  int syscall_nr = seccomp_syscall_resolve_name("socket");

  if (syscall_nr == __NR_SCMP_ERROR) {
    return 0;
  }

  return seccomp_rule_add(ctx, SCMP_ACT_ALLOW, syscall_nr, 1,
                          SCMP_A0_64(SCMP_CMP_NE, AF_VSOCK));
}

static int install_rules(scmp_filter_ctx ctx) {
  static const char *const syscalls[] = {
      "accept",
      "accept4",
      "access",
      "alarm",
      "arch_prctl",
      "bind",
      "brk",
      "cachestat",
      "capget",
      "capset",
      "chdir",
      "chmod",
      "chown",
      "clock_getres",
      "clock_getres_time64",
      "clock_gettime",
      "clock_gettime64",
      "clock_nanosleep",
      "clock_nanosleep_time64",
      "clone3",
      "close",
      "close_range",
      "connect",
      "copy_file_range",
      "creat",
      "dup",
      "dup2",
      "dup3",
      "epoll_create",
      "epoll_create1",
      "epoll_ctl",
      "epoll_pwait",
      "epoll_pwait2",
      "epoll_wait",
      "eventfd",
      "eventfd2",
      "execve",
      "execveat",
      "exit",
      "exit_group",
      "faccessat",
      "faccessat2",
      "fadvise64",
      "fadvise64_64",
      "fallocate",
      "fanotify_mark",
      "fchdir",
      "fchmod",
      "fchmodat",
      "fchmodat2",
      "fchown",
      "fchownat",
      "fcntl",
      "fdatasync",
      "fgetxattr",
      "flistxattr",
      "flock",
      "fork",
      "fremovexattr",
      "fsetxattr",
      "fstat",
      "fstatfs",
      "fsync",
      "ftruncate",
      "futex",
      "futex_requeue",
      "futex_time64",
      "futex_wait",
      "futex_waitv",
      "futex_wake",
      "futimesat",
      "getcpu",
      "getcwd",
      "getdents",
      "getdents64",
      "getegid",
      "geteuid",
      "getgid",
      "getgroups",
      "getitimer",
      "getpeername",
      "getpgid",
      "getpgrp",
      "getpid",
      "getppid",
      "getpriority",
      "getrandom",
      "getresgid",
      "getresuid",
      "getrlimit",
      "get_robust_list",
      "getrusage",
      "getsid",
      "getsockname",
      "getsockopt",
      "gettid",
      "gettimeofday",
      "getuid",
      "getxattr",
      "getxattrat",
      "inotify_add_watch",
      "inotify_init",
      "inotify_init1",
      "inotify_rm_watch",
      "io_cancel",
      "ioctl",
      "io_destroy",
      "io_getevents",
      "io_pgetevents",
      "io_pgetevents_time64",
      "ioprio_get",
      "ioprio_set",
      "io_setup",
      "io_submit",
      "kill",
      "landlock_add_rule",
      "landlock_create_ruleset",
      "landlock_restrict_self",
      "lgetxattr",
      "link",
      "linkat",
      "listen",
      "listmount",
      "listxattr",
      "listxattrat",
      "llistxattr",
      "lremovexattr",
      "lseek",
      "lsetxattr",
      "lstat",
      "madvise",
      "map_shadow_stack",
      "membarrier",
      "memfd_create",
      "memfd_secret",
      "mincore",
      "mkdir",
      "mkdirat",
      "mknod",
      "mknodat",
      "mlock",
      "mlock2",
      "mlockall",
      "mmap",
      "mprotect",
      "mq_getsetattr",
      "mq_notify",
      "mq_open",
      "mq_timedreceive",
      "mq_timedreceive_time64",
      "mq_timedsend",
      "mq_timedsend_time64",
      "mq_unlink",
      "mremap",
      "mseal",
      "msgctl",
      "msgget",
      "msgrcv",
      "msgsnd",
      "msync",
      "munlock",
      "munlockall",
      "munmap",
      "name_to_handle_at",
      "nanosleep",
      "newfstatat",
      "open",
      "openat",
      "openat2",
      "pause",
      "pidfd_open",
      "pidfd_send_signal",
      "pipe",
      "pipe2",
      "pkey_alloc",
      "pkey_free",
      "pkey_mprotect",
      "poll",
      "ppoll",
      "ppoll_time64",
      "prctl",
      "pread64",
      "preadv",
      "preadv2",
      "prlimit64",
      "process_mrelease",
      "pselect6",
      "pselect6_time64",
      "pwrite64",
      "pwritev",
      "pwritev2",
      "read",
      "readahead",
      "readlink",
      "readlinkat",
      "readv",
      "recv",
      "recvfrom",
      "recvmmsg",
      "recvmmsg_time64",
      "recvmsg",
      "remap_file_pages",
      "removexattr",
      "removexattrat",
      "rename",
      "renameat",
      "renameat2",
      "restart_syscall",
      "riscv_hwprobe",
      "rmdir",
      "rseq",
      "rt_sigaction",
      "rt_sigpending",
      "rt_sigprocmask",
      "rt_sigqueueinfo",
      "rt_sigreturn",
      "rt_sigsuspend",
      "rt_sigtimedwait",
      "rt_sigtimedwait_time64",
      "rt_tgsigqueueinfo",
      "sched_getaffinity",
      "sched_getattr",
      "sched_getparam",
      "sched_get_priority_max",
      "sched_get_priority_min",
      "sched_getscheduler",
      "sched_rr_get_interval",
      "sched_rr_get_interval_time64",
      "sched_setaffinity",
      "sched_setattr",
      "sched_setparam",
      "sched_setscheduler",
      "sched_yield",
      "seccomp",
      "select",
      "semctl",
      "semget",
      "semop",
      "semtimedop",
      "semtimedop_time64",
      "send",
      "sendfile",
      "sendmmsg",
      "sendmsg",
      "sendto",
      "setfsgid",
      "setfsuid",
      "setgid",
      "setgroups",
      "setitimer",
      "setpgid",
      "setpriority",
      "setregid",
      "setresgid",
      "setresuid",
      "setreuid",
      "setrlimit",
      "set_robust_list",
      "setsid",
      "setsockopt",
      "set_tid_address",
      "setuid",
      "setxattr",
      "setxattrat",
      "shmat",
      "shmctl",
      "shmdt",
      "shmget",
      "shutdown",
      "sigaltstack",
      "signalfd",
      "signalfd4",
      "splice",
      "stat",
      "statfs",
      "statmount",
      "statx",
      "symlink",
      "symlinkat",
      "sync",
      "sync_file_range",
      "syncfs",
      "sysinfo",
      "tee",
      "tgkill",
      "time",
      "timer_create",
      "timer_delete",
      "timer_getoverrun",
      "timer_gettime",
      "timer_gettime64",
      "timer_settime",
      "timer_settime64",
      "timerfd_create",
      "timerfd_gettime",
      "timerfd_gettime64",
      "timerfd_settime",
      "timerfd_settime64",
      "times",
      "tkill",
      "truncate",
      "ugetrlimit",
      "umask",
      "uname",
      "unlink",
      "unlinkat",
      "uretprobe",
      "utime",
      "utimensat",
      "utimensat_time64",
      "utimes",
      "vfork",
      "vmsplice",
      "wait4",
      "waitid",
      "waitpid",
      "write",
      "writev",
  };

  if (seccomp_attr_set(ctx, SCMP_FLTATR_CTL_NNP, 0) != 0) {
    return -1;
  }

  if (seccomp_attr_set(ctx, SCMP_FLTATR_CTL_TSYNC, 1) != 0) {
    return -1;
  }

  if (allow_all_by_name(ctx, syscalls, ARRAY_LEN(syscalls)) != 0) {
    return -1;
  }

  if (allow_clone_non_namespace(ctx) != 0) {
    return -1;
  }

  if (allow_socket_non_vsock(ctx) != 0) {
    return -1;
  }

  if (allow_by_name(ctx, "socketpair") != 0) {
    return -1;
  }

  return 0;
}

int main(void) {
  scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ERRNO(EPERM));

  if (ctx == NULL) {
    perror("seccomp_init");
    return 1;
  }

  if (install_rules(ctx) != 0) {
    perror("install_rules");
    seccomp_release(ctx);
    return 1;
  }

  if (seccomp_export_bpf(ctx, STDOUT_FILENO) != 0) {
    perror("seccomp_export_bpf");
    seccomp_release(ctx);
    return 1;
  }

  seccomp_release(ctx);
  return 0;
}

#else

int main(void) {
  fputs("libseccomp headers are required to build agentwrap-seccomp\n", stderr);
  return 1;
}

#endif
