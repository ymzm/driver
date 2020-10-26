/*
 * char-read.c
 *
 * Character driver with toggle case
 *
 * Copyright (C) 2020 USTC
 *
 * by howif
 *    dzw
 *
 */


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>		/* kmalloc() */


MODULE_AUTHOR("USTC");
MODULE_LICENSE ("GPL");

#define TC_MAJOR 250
#define TC_MINOR 0
#define TC_NUM 1
#define TC_MAX_BUF_SIZE 256

typedef struct toggle_case{
    char buf[TC_MAX_BUF_SIZE];
    int  w_offset; // offset to write
    int  offset;   // offset to read
    int  size;
    struct cdev cdev;
} tc;

static tc * tc_devices;
char tmp[256];
static dev_t devno;

/*
 * TODO:Should we check if we can open this device?
 */
int tc_open(struct inode *inode, struct file *filp)
{
    tc  *dev; /* device information */

    dev = container_of(inode->i_cdev, tc, cdev);
    filp->private_data = dev; /* for other methods */

    return 0;
}

/*
 * TODO:we should release the resources we got.
 */
static int tc_release (struct inode *inode, struct file *file)
{ 
  return 0;
}

ssize_t tc_read (struct file *filp, char *buf, size_t count, loff_t *offp)
{
    tc *dev = filp->private_data;
    
    if (count == 0)
    {
        return 0;
    }

    count = count > dev->size ? dev->size : count;

    if (dev->offset + count > TC_MAX_BUF_SIZE)
    {
        if (copy_to_user (buf, dev->buf + dev->offset, TC_MAX_BUF_SIZE-dev->offset) != 0)
	{
	    return -EFAULT;
	}
	if (copy_to_user (buf + TC_MAX_BUF_SIZE - dev->offset, dev->buf, count - (TC_MAX_BUF_SIZE-dev->offset)) != 0)
	{
	    return -EFAULT;
	}
	dev->offset = (dev->offset + count) % TC_MAX_BUF_SIZE;
	dev->size = dev->size - count;
    }
    else
    {
        if (copy_to_user (buf, dev->buf + dev->offset, count) != 0)
	{
	    return -EFAULT;
	}
        dev->offset = (dev->offset + count) % TC_MAX_BUF_SIZE;
        dev->size = dev->size - count;
    }

    return count;
}

static void toggle(char *begin, int length)
{
    int i;
    for (i=0; i<length; i++)
    {
        if (begin[i] >= 'A' && begin[i] <= 'Z')
        {
            begin[i]=begin[i]-'A'+'a';
        }
        else if (begin[i] >= 'a' && begin[i] <= 'z')
        {
            begin[i]=begin[i]+'A'-'a';
        }
        else
        {
            ;
        } 
    }
}

ssize_t tc_write (struct file *filp, const char  *buf, size_t count, loff_t *f_pos)
{
    tc *dev = filp->private_data;
  
    count = count > (TC_MAX_BUF_SIZE - dev->size) ? (TC_MAX_BUF_SIZE - dev->size) : count ;

    if (dev->w_offset + count > TC_MAX_BUF_SIZE)
    {
        if (copy_from_user (dev->buf + dev->w_offset, buf,  TC_MAX_BUF_SIZE-dev->w_offset) != 0)
	{
	    return -EFAULT;
	}
	toggle (dev->buf + dev->w_offset,  TC_MAX_BUF_SIZE-dev->w_offset);
        if (copy_from_user (dev->buf,buf + TC_MAX_BUF_SIZE-dev->w_offset, count - (TC_MAX_BUF_SIZE-dev->w_offset)) != 0)
	{
	    return -EFAULT;
	}
        toggle (dev->buf, count - (TC_MAX_BUF_SIZE-dev->w_offset));
	dev->w_offset = (dev->w_offset + count) % TC_MAX_BUF_SIZE;
        dev->size = dev->size + count;
    }
    else
    {
        if (copy_from_user (dev->buf + dev->w_offset, buf, count) != 0)
	{
	    return -EFAULT;
	}
        toggle (dev->buf + dev->w_offset, count);
	dev->w_offset = (dev->w_offset + count) % TC_MAX_BUF_SIZE;
        dev->size = dev->size + count;
    }        
    return count;
}


struct file_operations tc_fops = {
  .owner = THIS_MODULE,
  .open  = tc_open,
  .release = tc_release,
  .read  = tc_read,
  .write = tc_write
};

/*
 * init cdev and call cdev_add;
 * TODO: know what does the kernel really do.
 */
static void tc_setup_cdev(tc *dev, int index)
{
    int err;
    
    cdev_init(&dev->cdev, &tc_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &tc_fops;
    err = cdev_add (&dev->cdev, devno + index, 1);
    
    if (err)
    {
        printk(KERN_NOTICE "Error %d adding tc%d", err, index);
    }

    return;
}

static int __init tc_init (void)
{
    int result, i;

    result= alloc_chrdev_region(&devno, 0, TC_NUM,"toggle-case");

    if (result < 0) 
    {
        printk ("ERROR:%s %d, can't get devno\n", __FUNCTION__, __LINE__);
        return result;
    }

    tc_devices = kmalloc(TC_NUM * sizeof(tc), GFP_KERNEL);
    if (tc_devices == NULL)
    {
        printk("ERROR:%s %d, kmalloc failed\n", __FUNCTION__, __LINE__);
    }

    for (i = 0; i < TC_NUM; i++) 
    {
	tc_devices[i].w_offset = 0;
        tc_devices[i].offset = 0;
        tc_devices[i].size = 0;
        tc_setup_cdev(&tc_devices[i], i);
    }
    printk (KERN_INFO "char device registered\n");
  
    return 0;
}

static void __exit tc_exit (void)
{
    int i;

    if (tc_devices) 
    {
        for (i = 0; i < TC_NUM; i++) 
	{
            cdev_del(&tc_devices[i].cdev);
        }

        kfree(tc_devices);
    }

    unregister_chrdev_region(devno, TC_NUM);
}

module_init (tc_init);
module_exit (tc_exit);
