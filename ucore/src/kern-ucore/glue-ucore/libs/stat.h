#ifndef __LIBS_STAT_H__
#define __LIBS_STAT_H__

#include <types.h>
#include <arm-posix_types.h>

#if 0
typedef uint32_t      blksize_t;
typedef int64_t     blkcnt_t; 
typedef uint32_t __kernel_dev_t;

typedef __kernel_ino_t		ino_t;
typedef __kernel_nlink_t	nlink_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef __kernel_time_t  _st_time_t;
typedef __kernel_off_t   _st_off_t;
//typedef __kernel_timer_t	timer_t;
#endif

struct stat {
    uint32_t st_mode;                   // protection mode and file type
    size_t st_nlinks;                   // number of hard links
    size_t st_blocks;                   // number of blocks file is using
    size_t st_size;                     // file size (bytes)
};



#if 0
#define S_IFMT          070000          // mask for type of file
#define S_IFREG         010000          // ordinary regular file
#define S_IFDIR         020000          // directory
#define S_IFLNK         030000          // symbolic link
#define S_IFCHR         040000          // character device
#define S_IFBLK         050000          // block device

#define S_ISREG(mode)                   (((mode) & S_IFMT) == S_IFREG)      // regular file
#define S_ISDIR(mode)                   (((mode) & S_IFMT) == S_IFDIR)      // directory
#define S_ISLNK(mode)                   (((mode) & S_IFMT) == S_IFLNK)      // symlink
#define S_ISCHR(mode)                   (((mode) & S_IFMT) == S_IFCHR)      // char device
#define S_ISBLK(mode)                   (((mode) & S_IFMT) == S_IFBLK)      // block device
#endif

#if defined(__KERNEL__) || !defined(__GLIBC__) || (__GLIBC__ < 2)

#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#endif


#endif /* !__LIBS_STAT_H__ */

