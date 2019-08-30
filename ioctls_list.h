/* vim: set sw=4 expandtab: */
/*
 * Licence: GPL
 * Created: 2015-12-17 14:41:27+01:00
 * Main authors:
 *     - Jérôme Pouiller <jezz@sysmic.org>
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

