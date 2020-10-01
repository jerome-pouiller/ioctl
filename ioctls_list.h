/* vim: set sw=4 expandtab: */
/*
 * Licence: GPLv2+
 * Copyright (c) 2015-2020, Jérôme Pouiller
 */
#ifndef IOCTLS_LIST_H
# define IOCTLS_LIST_H

struct ioctl_entry {
    const char *name;
    int val;
    int dir;
    int size;
};
extern const struct ioctl_entry ioctls_list[];

#endif /* IOCTLS_LIST_H */

