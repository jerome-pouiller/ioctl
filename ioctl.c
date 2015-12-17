/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2015-05-12 09:53:35+02:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <error.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ioctls_list.h"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof(A[0]))

static const char *dir_str[] = {
  "NONE", "W", "R", "RW"
};

void usage(FILE *out, int code) {
    fprintf(out,
            "Usage: %s [OPTIONS] FILEDEV IOCTL_NUM\n"
            "Call ioctl IOCTL_NUM on FILEDEV.\n"
            "A buffer is allocated and passed as argument of ioctl. If direction is\n"
            "not 'NONE', buffer content is read/write on standard input/ouput.\n"
            "Direction and buffer size are deduced from IOCTL_NUM. It is however\n"
            "possible to force these parameters.\n"
            "\n"
            "\t-L       Instead of normal behavior, list known ioctls\n"
            "\t-d DIR   force direction: 0 = NONE, 1 = W, 2 = R, 3 = RW\n"
            "\t-s SIZE  force buffer size\n"
            "\t-v VALUE pass this value as ioctl argument instead of a pointer on a buffer. Force direction to NONE\n"
            "\t-q       quiet\n",
            program_invocation_short_name);
    exit(code);
}

void sighandler(int signum, siginfo_t *pinfo, void *context) {
    signum = signum;
    context = context;
    psiginfo(pinfo, "ioctl returned with signal");
    // fprintf(stderr, "ioctl returned with signal %s (%d)\n", strsignal(sig), sig);
}

void doit(const char *file, unsigned long ioctl_nr, void *buf) {
    struct sigaction act = { }, oldact;
    int ret;
    int fd;
    int i;

    fd = open(file, O_RDWR);
    if (fd < 0 && errno == EPERM)
        fd = open(file, O_RDONLY);
    if (fd < 0) {
        error(1, errno, "Cannot open %s: ", file);
    }

    memset(&act, 0, sizeof(act));
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    for (i = 0; i < NSIG; i++) {
        sigaction(i, NULL, &oldact);
        if (oldact.sa_handler == SIG_DFL)
            sigaction(i, &act, NULL);
    }
    ret = ioctl(fd, ioctl_nr, buf);
    memset(&act, 0, sizeof(act));
    act.sa_handler = NULL;
    for (i = 0; i < NSIG; i++) {
        sigaction(i, NULL, &oldact);
        if (oldact.sa_sigaction == sighandler)
            sigaction(i, &act, NULL);
    }
    if (ret)
        fprintf(stderr, "Returned %d (errno: %d, \"%m\")\n", ret, errno);
    else
        fprintf(stderr, "Returned 0\n");
}

void display_parms(const char *prefix, unsigned ioctl_nr, int dir, int size, void *force_value) {
    int type = _IOC_TYPE(ioctl_nr);
    int nr = _IOC_NR(ioctl_nr);

    fprintf(stderr, "%s: ioctl=0x%08x, ", prefix, ioctl_nr);
    if (force_value == (void *) -1)
        fprintf(stderr, "direction=%s, arg size=%d bytes, ", dir_str[dir], size);
    else
        fprintf(stderr, "arg value=%p, ", force_value);
    fprintf(stderr, "device number=0x%02x", type);
    if (isprint(type))
        fprintf(stderr, " ('%c')", (char) type);
    fprintf(stderr, ", function number=%u\n", nr);
}

void list_ioctls() {
    unsigned ioctl_nr;
    int dir;
    int size;
    int type;
    int nr;
    int i;

    for (i = 0; ioctls_list[i].name; i++) {
        ioctl_nr = ioctls_list[i].val;
        dir = ioctls_list[i].dir;
        size = ioctls_list[i].size;
        type = _IOC_TYPE(ioctl_nr);
        nr = _IOC_NR(ioctl_nr);
        if (dir == -1)
            dir = _IOC_DIR(ioctl_nr);
        if (size == -1)
            size = _IOC_SIZE(ioctl_nr);
        printf("%-40s 0x%08x, ", ioctls_list[i].name, ioctl_nr);
        if (!dir)
            printf("%4s, %2d bytes, 0x%02x ", dir_str[dir], size, type);
        else
            printf("%2s, %4d bytes, 0x%02x ", dir_str[dir], size, type);
        if (isprint(type))
            printf("'%c'", (char) type);
        else
            printf("   ");
        printf(", %u\n", nr);
    }
}

int main(int argc, char **argv) {
    unsigned long ioctl_nr;
    int dir = -1, force_dir = -1;
    int size = -1, force_size = -1;
    void *buf = NULL, *force_value = (void *) -1;
    const char *file;
    int quiet = 0;
    int opt;
    char *endptr;
    int i;

    while ((opt = getopt(argc, argv, "d:s:v:qLh")) != -1) {
        switch (opt) {
            case 'd':
                force_dir = strtol(optarg, &endptr, 0);
                if (*endptr) {
                    force_dir = -1;
                    printf("optarg %s\n", optarg);
                    for (i = 0; i < ARRAY_SIZE(dir_str); i++)
                        if (!strcmp(optarg, dir_str[i]))
                            force_dir = i;
                    if (force_dir == -1 || force_dir > ARRAY_SIZE(dir_str))
                        error(1, 0, "Invalid direction");
                }
                break;
            case 's':
                force_size = strtol(optarg, &endptr, 0);
                if (*endptr)
                    usage(stderr, EXIT_FAILURE);
                break;
            case 'v':
                force_value = (void *) strtol(optarg, &endptr, 0);
                if (*endptr)
                    usage(stderr, EXIT_FAILURE);
                break;
            case 'q':
                quiet = 1;
                break;
            case 'L':
                list_ioctls();
                exit(0);
                break;
            case 'h':
                usage(stdout, 0);
                break;
            default: /* '?' */
                usage(stderr, EXIT_FAILURE);
                break;
        }
    }
    if (optind + 2 != argc)
        usage(stderr, EXIT_FAILURE);
    file = argv[optind];
    ioctl_nr = strtoul(argv[optind + 1], &endptr, 0);
    if (*endptr) {
        ioctl_nr = 0;
        for (i = 0; ioctls_list[i].name; i++) {
            if (!strcmp(argv[optind + 1], ioctls_list[i].name)) {
                ioctl_nr = ioctls_list[i].val;
                dir = ioctls_list[i].dir;
                size = ioctls_list[i].size;
                break;
            }
        }
        if (!ioctl_nr)
            error(1, 0, "Cannot find %s ioctl value", argv[optind + 1]);
    }
    if (dir == -1)
        dir = _IOC_DIR(ioctl_nr);
    if (size == -1)
        size = _IOC_SIZE(ioctl_nr);
    if (!quiet)
        display_parms("Decoded values", ioctl_nr, dir, size, (void *) -1);
    if (force_value != (void *) -1 && force_size != -1)
        error(1, 0, "Options -v and -s are incompatible");
    if (force_value != (void *) -1 && force_dir != -1)
        error(1, 0, "Options -v and -d are incompatible");
    if (force_dir != -1)
        dir = force_dir;
    if (force_size != -1)
        size = force_size;
    if (force_value != (void *) -1) {
        buf = force_value;
        dir = 0;
        size = 0;
    } else if (!size) {
        buf = NULL;
    } else {
        buf = malloc(size);
        memset(buf, 0, size);
    }
    if (dir == 0 && size != 0)
        fprintf(stderr, "Warning: Direction is NONE but buffer size is not 0\n");
    if (!quiet)
        display_parms("Used values", ioctl_nr, dir, size, force_value);

    if (dir == 1 || dir == 3)
        read(0, buf, size);
    doit(file, ioctl_nr, buf);
    if (dir == 2 || dir == 3)
        write(1, buf, size);

    return 0;
}

