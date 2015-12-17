#!/bin/sh
# vim: set sw=4 expandtab:
#
# Licence: GPL
# Created: 2015-12-17 14:19:58+01:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#

CC=$1
ARCH=$($CC -print-multiarch)
SYSROOT=$($CC -print-sysroot)
EXTRA_GREP_ARGS="--exclude via_drm.h --exclude if_ppp.h --exclude-dir Peripherals --exclude-dir libdrm --exclude-dir *-linux-*"

# There are multiple problems:
#  - some delaration does not match regular expression -> Not yet supported
#    (possibility to add them manually to list, but there are problem with
#    foreign platforms)
#  - some declaration appear twice -> There are generally the same

echo '#include <stdint.h>' # intX_t
echo '#include <unistd.h>' # size_t
echo '#include <time.h>'   # alsa headers
echo '#include <termios.h>'   # struct termios2
echo '#include <linux/termios.h>'   # struct termios2
grep -lr $EXTRA_GREP_ARGS '^#define.*_IO[RW]*(' "$SYSROOT/usr/include" | sed "s|$SYSROOT/usr/include/\(.*\)|#include <\1>|"
echo '#include "ioctls_list.h"'
echo
echo "const struct ioctl_entry ioctl_list[] = {"
grep -hr $EXTRA_GREP_ARGS '^#define.*_IO[RW]*(' "$SYSROOT/usr/include" | sed "s|#define[ \t]*\([A-Z0-9_]*\).*|    DECLARE_IOCTL(\1),|"
echo "};"
