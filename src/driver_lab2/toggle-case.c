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
#include <asm/uaccess.h>

MODULE_AUTHOR("USTC");
MODULE_LICENSE ("GPL");

#define TC_MAJOR 250
#define TC_MINOR 0
#define TC_NUM 4
#define TC_BUF_SIZE 256

//ichar data[128] = "foobar not equal to barfoo";

/*
 * TODO: in my opinion, we should alloc them dynamically, and get them by inode pointer.
 * Maybe my opinion above is wrong, it's useless here.
 */
static struct toggle_case{
    char buf[TC_BUF_SIZE];
    int  offset;
} tc[TC_NUM] = {0};

static dev_t devno;
static struct cdev cdev;

/*
 * TODO:we should check if we can open this device.
 */
static int tc_open (struct inode *inode, struct file *file)
{
  return 0;
}

/*
 * TODO:we should release the resources we got.
 */
static int tc_release (struct inode *inode, struct file *file)
{ 
  return 0;
}

ssize_t tc_read (struct file *filp, char *buff, size_t count, loff_t *offp)
{
#if 0
  ssize_t result = 0;
  
  if (count > 127) count = 127;

  if (copy_to_user (buff, data, count)) 
  {
    result = -EFAULT;
  }
  else
  {
    printk (KERN_INFO "wrote %d bytes\n", count);
	result = count;
  }
   
  return result;
#endif
}

ssize_t tc_write (struct file *filp, const char  *buf, size_t count, loff_t *f_pos)
{
#if 0
  ssize_t ret = 0;

  //printk (KERN_INFO "Writing %d bytes\n", count);
  if (count > 127) return -ENOMEM;
  if (copy_from_user (data, buf, count)) {
    ret = -EFAULT;
  }
  else {
    data[count] = '\0';
	int i=0;
	for (i=0;i<count;i++){
	   data[i]=data[i]+'A'-'a' ;
	}
    printk (KERN_INFO"Received: %s\n", data);
    ret = count;
  }

  printk("ret = %x\n",ret);

  return ret;
#endif
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
static void tc_setup(void)
{
    int error;
    cdev_init (&cdev, &tc_fops);
    cdev.owner = THIS_MODULE;
    error = cdev_add (&cdev, devno , TC_NUM);
    if (error)
    {
        printk (KERN_NOTICE "Error %d adding char_reg_setup_cdev", error);
    }
    return;
}

static int __init tc_init (void)
{
  int result;

  result= alloc_chrdev_region(&devno,0, 3,"toggle-case");

  if (result < 0) {
    printk ("ERROR:%s %d, can't get devno\n", __FUNCTION__, __LINE__);
    return result;
  }

  tc_setup ();
  printk (KERN_INFO "char device registered\n");
  
  return 0;
}

static void __exit tc_exit (void)
{
  cdev_del (&cdev);

  unregister_chrdev_region (devno, TC_NUM);
}

module_init (tc_init);
module_exit (tc_exit);
