#include<linux/module.h> /* Define module_init(), module_exit() */
#include<linux/fs.h> /* Define alloc_chrdev_region(), register_chrdev_region() */
#include<linux/device.h>/* Define device_create(), class_create() */
#include<linux/cdev.h>/* Define cdev_init(), cdev_add() */
#include<linux/uaccess.h>/* Define copy_to_user(), copy_from_user() */
#include<linux/slab.h>/* Define kfree() */
#include<linux/err.h>
#include<linux/init.h>
#include<linux/kernel.h>


#define DRIVER_AUTHOR "jerry jerryfromUET@gmail.com"
#define DRIVER_DESC "Hello world kernel module"
#define DRIVER_VERS "1.0"

#define memsize 1024 /*Memory size*/

struct m_foo_dev{
    int32_t size;
    char *kmalloc_ptr;
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
}mdev;

/*  Function Prototypes */
static int      __init chdev_init(void);
static void     __exit chdev_exit(void);
static int      m_open(struct inode *inode, struct file *file);
static int      m_release(struct inode *inode, struct file *file);
static ssize_t  m_read(struct file *filp, char __user *user_buf, size_t size,loff_t * offset);
static ssize_t  m_write(struct file *filp, const char *user_buf, size_t size, loff_t * offset);

static struct file_operations fops = 
{
    .owner = THIS_MODULE,
    .read = m_read,
    .write = m_write,
    .open = m_open,
    .release = m_release,
};
/* This function will be called when we open the Device file */
static int m_open(struct inode *inode, struct file *file)
{
    pr_info("System call open() called...!!!\n");
    return 0;
}
/* This function will be called when we close the Device file */
static int m_release(struct inode *inode, struct file *file)
{
    pr_info("System call close() called...!!!\n");
    return 0;
}
/* This function will be called when we read the Device file */
static ssize_t  m_read(struct file *filp, char __user *user_buf, size_t size,loff_t * offset)
{
    pr_info("System call read() called...!!!\n");
    if(copy_to_user(user_buf,mdev.kmalloc_ptr,memsize)){
        pr_err("Data Read : Err!\n");
    }
    pr_info("Data Read : Done!\n");
    return memsize;
}
/* This function will be called when we write the Device file */
static ssize_t  m_write(struct file *filp, const char __user *user_buf, size_t size, loff_t * offset)
{
    pr_info("System call write() called...!!!\n");
    if(copy_from_user(mdev.kmalloc_ptr,user_buf,size)){
        pr_err("Data Write : Err!\n");
    }
    pr_info("Data Write : Done!\n");
    //pr_info("Data from usr: %s",mdev.kmalloc_ptr);
    return size;
}

/*Constructor*/
static int  __init chdev_init(void)
{
    /*1.0 Dynamic allocating device number*/
    if(alloc_chrdev_region(&mdev.dev_num,0,1,"m_cdev")<0){
        pr_err("Failed to alloc chrdev region\n");
        return -1;
    }
    pr_info("Major: %d Minor: %d\n",MAJOR(mdev.dev_num),MINOR(mdev.dev_num));

    /*2.0 Creating struct class*/
    if((mdev.m_class=class_create(THIS_MODULE,"m_class"))==NULL){
        pr_err("Cannot create the struct class for my device\n");
        goto rm_device_numb;
    }

    /*3.0 Creating device*/
    if(device_create(mdev.m_class,NULL,mdev.dev_num,NULL,"m_device")==NULL){
        pr_err("Cannot create my device\n");
        goto rm_class;
    }

    /*4.0 Creating cdev structure*/
    cdev_init(&mdev.m_cdev,&fops);

    /*4.1 Adding character device to the system*/
    if(cdev_add(&mdev.m_cdev,mdev.dev_num,1)<0){
        pr_err("Cannot add the device to the system\n");
        goto rm_device;
    }
    mdev.kmalloc_ptr=kmalloc(memsize, GFP_KERNEL);
    strcpy(mdev.kmalloc_ptr, "Hello_World");
    pr_info("Hello world kernel module\n");
    return 0;

rm_device:
    device_destroy(mdev.m_class,mdev.dev_num);
rm_class:
    class_destroy(mdev.m_class);
rm_device_numb:
    unregister_chrdev_region(mdev.dev_num, 1);
    return -1;
}

/*Destructor*/
static void __exit chdev_exit(void)
{
    kfree(mdev.kmalloc_ptr);
    cdev_del(&mdev.m_cdev);
    device_destroy(mdev.m_class,mdev.dev_num);
    class_destroy(mdev.m_class);
    unregister_chrdev_region(mdev.dev_num, 1);
    pr_info("Goodbye\n");
}

module_init(chdev_init);
module_exit(chdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);  
MODULE_VERSION(DRIVER_VERS);