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

`0x80685600` is numerical value of `VIDIOC_QUERYCAP`. `ioctl` also understand
symbolic names. Thus, you can use:

    ioctl /dev/video0 VIDIOC_QUERYCAP | hexdump -C

Sure, output is far less understandable than output of `v4l2-ctl` :-).

In order to work, symbolic have to defined during compilation (See "Compilation"
below). You can run `ioctl -L` in order to get list of all symbolic ioctls
supported.

Direction and buffer size are deduced from `IOCTL_NUM` (see `asm/ioctl.h` for
more information about ioctl number encoding). It is however possible to force
these parameters using `-d` and `-s`.

Sometime it is conveniant to pass directly a value to an ioctl instead of a
pointer. You can use option `-v` for this.

Try `-h` to list all options.

Examples
--------

Query capability of you webcam:

    ioctl /dev/video0 VIDIOC_QUERYCAP | hexdump -C

Save parameters of serial port (`0x5401 == TCGETS`):

    ioctl /dev/ttyS0 0x5401 -s 1024 -d R > params

Notice in this case, ioctl number does not follow convention. We have to force
size and direction. Also note wa allocate a buffer larger than
`sizeof(struct termios)`. In most case, it is not a problem.

After doing some tests, restore parameters of serial port (`0x5402 = TCSETS`):

    ioctl /dev/ttyS0 0x5402 -s 1024 -d W < params

Compilation
-----------

`make && make install` should wokr as usual. However, you may encounter errors
in `ioctls_list.c`. This file is generated automaticaly by `gen_ioctls_list.sh`.
It try to declare all available ioctls. However, some headers files does not
work and some ioctls cannot be resolved because it lacks type definition. Most
of these error should addressed with patchs in kernel headers. Meanwhile, you
may have to patch `gen_ioctls_list.sh` in order to make work.

Bugs
----

Current all symbolic names are automaticaly generated. However some ioctls (like
`TCGETS`) cannot be detected automaticaly. We have to add manual entries for 
them (patches are welcome).

