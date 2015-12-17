#!/bin/bash
# vim: set sw=4 expandtab:
#
# Licence: GPL
# Created: 2015-12-17 14:19:58+01:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#

CC=$1
SYSROOT=$($CC -print-sysroot)
ARCH=$($CC -print-multiarch)
EXCLUDE_FILES+=" --exclude auto_fs.h"           # compat_ulong_t undeclared
EXCLUDE_FILES+=" --exclude coda.h"              # Include linux/time.h which conflict with time.h
# EXCLUDE_FILES+=" --exclude if_ppp.h"          # net/if_ppp.h conflict with linux/ppp-ioctl.h
EXCLUDE_FILES+=" --exclude ppp-ioctl.h"         # linux/ppp-ioctl.h conflict with net/if_ppp.h
EXCLUDE_FILES+=" --exclude ioctl.h"             # Does not contains any ioctl in fact
EXCLUDE_FILES+=" --exclude mount.h"             # sys/mount.h conflict with linux/fs.h
EXCLUDE_FILES+=" --exclude asequencer.h"        # Lack of type definition
EXCLUDE_FILES+=" --exclude-dir Peripherals"     # Old versions of linux/fmd/Peripherals/ missed many files
EXCLUDE_IOCTLS+=" -e USBDEVFS_CONTROL32"        # Lack type size
EXCLUDE_IOCTLS+=" -e USBDEVFS_BULK32"           # Lack type size
EXCLUDE_IOCTLS+=" -e USBDEVFS_SUBMITURB32"      # Lack type size
EXCLUDE_IOCTLS+=" -e USBDEVFS_DISCSIGNAL32"     # Lack type size
EXCLUDE_IOCTLS+=" -e USBDEVFS_IOCTL32"          # Lack type size
EXCLUDE_IOCTLS+=" -e KVM_CREATE_SPAPR_TCE"      # Lack type size
EXCLUDE_IOCTLS+=" -e KVM_ALLOCATE_RMA"          # Lack type size
EXCLUDE_IOCTLS+=" -e KVM_PPC_GET_HTAB_FD"       # Lack type size
EXCLUDE_IOCTLS+=" -e KVM_PPC_RTAS_DEFINE_TOKEN" # Lack type size
EXCLUDE_IOCTLS+=" -e KVM_ARM_VCPU_INIT"         # Lack type size
EXCLUDE_IOCTLS+=" -e KVM_ARM_PREFERRED_TARGET"  # Lack type size
EXCLUDE_IOCTLS+=" -e BLKTRACESETUP"             # Lack type size
EXCLUDE_IOCTLS+=" -e BTRFS_IOC_SET_FSLABEL"     # Lack type size
EXCLUDE_IOCTLS+=" -e BTRFS_IOC_GET_FSLABEL"     # Lack type size
EXCLUDE_IOCTLS+=" -e BTRFS_IOC_DEFRAG_RANGE"    # Lack type size
EXCLUDE_IOCTLS+=" -e BLKELVGET"                 # Inside #if 0
EXCLUDE_IOCTLS+=" -e BLKELVSET"                 # Inside #if 0

# There are multiple problems:
#  - some delaration does not match regular expression -> Not yet supported
#    (possibility to add them manually to list, but there are problem with
#    foreign platforms)
#  - some declaration appear twice -> There are generally the same

echo '#include <unistd.h>'  # size_t
echo '#include <net/if.h>'  # IFNAMSIZ at linux/isdn_ppp.h:38
echo '#include <stdint.h>'  # For alsa headers
# echo '#include <time.h>'    # alsa headers
# echo '#include <termios.h>' # struct termios2
echo '#include "ioctls_list.h"'
grep -lr $EXCLUDE_FILES --exclude-dir \*-linux-\* '^#define[^(].*[ \t]_IO[RW]*(' "$SYSROOT/usr/include" |
    sed -e "s|$SYSROOT/usr/include/\(.*\)|#include <\1>|"
if [ -n $ARCH ]; then
    grep -lr $EXCLUDE_FILES '^#define[^(].*[ \t]_IO[RW]*(' "$SYSROOT/usr/include/$ARCH" |
        sed -e "s|$SYSROOT/usr/include/$ARCH/\(.*\)|#include <\1>|"
fi
echo
echo "const struct ioctl_entry ioctl_list[] = {"
grep -nr $EXCLUDE_FILES --exclude-dir \*-linux-\* '^#define[^(]*[ \t]_IO[RW]*(' "$SYSROOT/usr/include" |
    grep -v $EXCLUDE_IOCTLS |
    sed -e "s|$SYSROOT/usr/include/\(.*:.*\):#define[ \t]*\([A-Z0-9x_]*\).*|    DECLARE_IOCTL(\2), // \1|"
if [ -n $ARCH ]; then
    grep -nr $EXCLUDE_FILES '^#define[^(]*[ \t]_IO[RW]*(' "$SYSROOT/usr/include/$ARCH" |
        grep -v $EXCLUDE_IOCTLS |
        sed -e "s|$SYSROOT/usr/include/\(.*:.*\):#define[ \t]*\([A-Z0-9x_]*\).*|    DECLARE_IOCTL(\2), // \1|"
fi
echo "};"
