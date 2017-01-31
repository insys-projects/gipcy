
#ifndef __IPCEXT_H__
#define __IPCEXT_H__

#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <asm/semaphore.h>

#ifdef GIPCY_2_4_X
struct device;
#endif

struct class {
    char m_drvname[32];
    char m_devname[32];
    int  m_flags;
    int  m_major;
    void *m_owner;
    struct device *m_dev;
    struct file_operations *m_fops;
};

struct device {
    struct class *m_class;
    struct device *m_parent;
    dev_t m_devno;
    void *m_data;
};

struct mutex {
    struct semaphore m_sem;
};

struct cdev {
    char m_drvname[32];
    char m_devname[32];
    int m_major;
    int m_count;
    struct file_operations *ops;
    void *owner;
};

typedef int bool;

enum {
    true = 1,
    false = 0
};

#define container_of(ptr, type, member) ({                      \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

void mutex_init(struct mutex *m);
void mutex_lock(struct mutex *m);
void mutex_unlock(struct mutex *m);
void *kzalloc(size_t size, int flags);
int down_timeout(struct semaphore *sem, int timeout);
int cdev_init(struct cdev *cd, struct file_operations *fops);
int cdev_add(struct cdev *cd, int major, int count);
int cdev_del(struct cdev *cd);
struct device* device_create(struct class *class, struct device *parent, dev_t devt, void *drvdata, const char *fmt, ...);
int device_destroy(struct class *cl, dev_t devno);
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
int unregister_chrdev_region(dev_t from, unsigned count);
struct class *class_create(void *owner, const char *name);
int class_destroy(struct class *cl);

#endif //__IPCEXT_H__
