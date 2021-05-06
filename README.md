IOCTL
=====

The missing tool to call arbitrary ioctl on devices.

Since most data associated with ioctls are not human readable, this tool is
intended for driver developers who want to do quick tests on their drivers.

Usage
------

    ioctl [OPTIONS] FILEDEV IOCTL_NUM

Call ioctl `IOCTL_NUM` on `FILEDEV`.

A buffer is allocated and passed as an argument of ioctl. If direction is
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

`0x80685600` is the numerical value of `VIDIOC_QUERYCAP`. `ioctl` also understands
symbolic names. Thus, you can use:

    $ ioctl /dev/video0 VIDIOC_QUERYCAP | hexdump -C
    [... same output...]

Sure, output is far less understandable than output of `v4l2-ctl` :-).

In order to work, symbolic names have to be defined during compilation (See "Compilation"
below). You can run `ioctl -L` in order to get a list of all symbolic ioctls
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
more information about ioctl number encoding). However, it is possible to force
these parameters using `-d` and `-s`.

Sometimes it is convenient to pass a value directly to an ioctl instead of a
pointer. You can use option `-v` for this.

`ioctl` tries to always provide maximum information about possible errors you
are facing (permissions, value returned, value of `errno`, etc...). You can make it a
little less verbose using `-q`.

Try `-h` to list all options.

Examples
--------

Query capabilities of your webcam:

    ioctl /dev/video0 VIDIOC_QUERYCAP | hexdump -C

Save parameters of serial port (`0x5401 == TCGETS`):

    ioctl /dev/ttyS0 0x5401 -s 1024 -d R > params

Notice in this case, the ioctl number does not follow convention. We have to force
size and direction. Also note we allocate a buffer larger than
`sizeof(struct termios)`. In most cases, this is not a problem.

After doing some tests, restore parameters of serial port (`0x5402 = TCSETS`):

    ioctl /dev/ttyS0 0x5402 -s 1024 -d W < params

Compilation
-----------

`make && make install` should work as usual. However, you may encounter errors
in `ioctls_list.c`. This file is generated automaticaly by `gen_ioctls_list.sh`.
It tries to declare all available ioctls. However, some header files do not
work and some ioctls cannot be resolved because it lacks type definitions. Most
of these errors should addressed with patches to kernel headers. Meanwhile, you
may have to patch `gen_ioctls_list.sh` in order to make it work.

If you don't want to bother with `ioctls_list.c` and `gen_ioctls_list.sh` you
can compile with `IGNORE_IOCTLS_LIST=1`:

    make IGNORE_IOCTLS_LIST=1

You won't have symbolic ioctl names, but this is still sufficient for some tests.

Bugs
----

Currently all symbolic names are automaticaly generated. However some ioctls (like
`TCGETS`) cannot be detected automaticaly. We have to add manual entries for 
them (patches are welcome).

