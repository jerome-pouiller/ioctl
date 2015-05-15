IOCTL
=====

The missing tool to call arbitrary ioctl on devices

Usage
------

    ioctl [OPTIONS] FILEDEV IOCTL_NUM

Call ioctl `IOCTL_NUM` on `FILEDEV`.

A buffer is allocated and passed as argument of ioctl. If direction is
not 'NONE', buffer content is read/write on standard input/ouput.
Direction and buffer size are deduced from `IOCTL_NUM`. It is however
possible to force these parameters.

  * `-d DIR`   force direction: `0 = NONE`, `1 = R`, `2 = W`, `3 = RW`
  * `-s SIZE`  force buffer size
  * `-v VALUE` pass this value as ioctl argument instead of a pointer on a 
    buffer. Force direction to `NONE`
  * `-q`       quiet




