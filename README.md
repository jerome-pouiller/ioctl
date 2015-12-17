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

    $ ioctl /dev/video0 0x80685600 | hexdump -C
    Decoded values: ioctl=0x80685600, direction=R, arg size=104 bytes, device number=0x56 ('V'), function number=0
    Used values: ioctl=0x80685600, direction=R, arg size=104 bytes, device number=0x56 ('V'), function number=0
    Returned 0
    00000000  75 76 63 76 69 64 65 6f  00 00 00 00 00 00 00 00  |uvcvideo........|
    00000010  42 75 69 6c 74 2d 69 6e  20 69 53 69 67 68 74 00  |Built-in iSight.|
    00000020  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
    00000030  75 73 62 2d 30 30 30 30  3a 30 30 3a 30 34 2e 31  |usb-0000:00:04.1|
    00000040  2d 34 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |-4..............|
    00000050  07 10 03 00 01 00 00 84  01 00 00 04 00 00 00 00  |................|
    00000060  00 00 00 00 00 00 00 00                           |........|
    00000068

`0x80685600` is numerical value of `VIDIOC_QUERYCAP`. `ioctl` also understand
symbolic names. Thus, you can use:

    $ ioctl /dev/video0 VIDIOC_QUERYCAP | hexdump -C
    [... same output...]

Sure, output is far less understandable than output of `v4l2-ctl` :-).

In order to work, symbolic have to defined during compilation (See "Compilation"
below). You can run `ioctl -L` in order to get list of all symbolic ioctls
supported.

    $ ioctl -L
    [...]
    SISFB_GET_AUTOMAXIMIZE_OLD               0x80046efa,  R,    4 bytes, 0x6e 'n', 250
    SISFB_SET_AUTOMAXIMIZE_OLD               0x40046efa,  W,    4 bytes, 0x6e 'n', 250
    MTRRIOC_ADD_ENTRY                        0x40104d00,  W,   16 bytes, 0x4d 'M', 0
    MTRRIOC_SET_ENTRY                        0x40104d01,  W,   16 bytes, 0x4d 'M', 1
    MTRRIOC_DEL_ENTRY                        0x40104d02,  W,   16 bytes, 0x4d 'M', 2
    MTRRIOC_GET_ENTRY                        0xc0184d03, RW,   24 bytes, 0x4d 'M', 3
    [...]

Direction and buffer size are deduced from `IOCTL_NUM` (see `asm/ioctl.h` for
more information about ioctl number encoding). It is however possible to force
these parameters using `-d` and `-s`.

Sometime it is conveniant to pass directly a value to an ioctl instead of a
pointer. You can use option `-v` for this.

`ioctl` try to always provide a maximum of information about possible error you
are facing (permission, value returned, value of `errno`, etc...). You make a
little less verbose using `-q`.

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

