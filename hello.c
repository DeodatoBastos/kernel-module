/**
 * hello.c
 *
 * Kernel module that communicates with /proc file system.
 *
 * The makefile must be modified to compile this program.
 * Change the line "simple.o" to "hello.o"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/random.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <asm/param.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128

#define PROC_NAME "hello"

/**
 * Function prototypes
 */
static ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos);

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
};

/* This function is called when the module is loaded. */
static int proc_init(void)
{

  // creates the /proc/hello entry
  // the following function call is a wrapper for
  // proc_create_data() passing NULL as the last argument
  proc_create(PROC_NAME, 0, NULL, &proc_fops);

  printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

  return 0;
}

/* This function is called when the module is removed. */
static void proc_exit(void)
{

  // removes the /proc/hello entry
  remove_proc_entry(PROC_NAME, NULL);

  printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/hello is read.
 *
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the
 * corresponding /proc file.
 *
 * params:
 *
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
static ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos)
{
  int rv = 0;
  char buffer[BUFFER_SIZE];
  static int completed = 0;
  int i, sleep_value;
  unsigned long timeout;

  if (completed)
  {
    completed = 0;
    return 0;
  }

  completed = 1;
  unsigned long t = (unsigned long)jiffies / HZ;

  rv = sprintf(buffer, "jiffies: %ld\nsecond since first kearnel load: %ld\n", jiffies, t);

  // copies the contents of buffer to userspace usr_buf
  int err = copy_to_user(user_buf, buffer, rv);
  if (err < 0)
    printk(KERN_ERR "/proc/%s removed\n", PROC_NAME);

  timeout = jiffies + 5 * HZ; // timeout in 5s
  get_random_bytes(&i, sizeof(i));
  sleep_value = i % 10000; // sleep between 0 and 10s
  usleep_range(sleep_value, sleep_value + 1);

  if (time_before(jiffies, timeout))
    rv = sprintf(buffer, "We didnt timeout. Yayy\n");
  else
    rv = sprintf(buffer, "We timed out. Jiffies are awesome\n");
  err = copy_to_user(user_buf, buffer, rv);

  if (err < 0)
    printk(KERN_ERR "/proc/%s removed\n", PROC_NAME);
  return rv;
}

/* Macros for registering module entry and exit points. */
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("DFG");
