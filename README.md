IOCTL
=====

The missing tool to call arbitrary ioctl on devices.

Since most of data associated with ioctl are not human readable, this tools is
intended for driver developpers who want to do quick tests on their drivers.

Usage
------

    ioctl [OPTIONS] FILEDEV IOCTL_NUM

Call ioctl `IOCTL_NUM` on `FILEDEV`.

A buffer is allocated and passed as argument of ioctl. If direction is
not `NONE`, buffer content is read/write on standard input/ouput.

For example, to query capabilities of a webcam, you can call:

    ioctl /dev/video0 0x80685600 | hexdump -C

(Note `0x80685600` is numerical value of `VIDIOC_QUERYCAP`). Sure, output is
far less understandable than output of `v4l2-ctl` :-).

Direction and buffer size are deduced from `IOCTL_NUM` (see `asm/ioctl.h` for
more information about ioctl number encoding). It is however possible to force
these parameters using `-d` and `-s`.

Sometime it is conveniant to pass directly a value to an ioctl instead of a
pointer. You can use option `-v` for this.

Try `-h` to list all options.

Examples
--------

Query capability of you webcam:

    ioctl /dev/video0 0x80685600 | hexdump -C

Get current capture format (`0xc0cc5604 = VIDIOC_G_FMT`):

    ioctl /dev/video0 0xc0cc5604 > format

Restore capture format after some tests (`0xc0cc5605 = VIDIOC_S_FMT`):

    ioctl /dev/video0 0xc0cc5605 < format

Future work
-----------

We want to include synbolic name of known ioctls. We will be able to call
`ioctl` command with ioctl symbolic name instead of numerical values.

