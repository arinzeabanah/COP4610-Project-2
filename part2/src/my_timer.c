// SPDX-License-Identifier: GPL-2.0
/*
 * my_timer.c: creates /proc/timer.
 *
 * Each read prints the current time from ktime_get_real_ts64(). Every read
 * after the first also prints the time elapsed since the previous read.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/timekeeping.h>
#include <linux/mutex.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 14");
MODULE_DESCRIPTION("Reports current time and time since the last read");

#define ENTRY_NAME "timer"
#define PERMS 0444
#define PARENT NULL

static struct proc_dir_entry *timer_entry;
static struct timespec64 last_time;	/* time from the previous read */
static bool has_last;			/* false until the first read */
static DEFINE_MUTEX(timer_lock);

static int timer_show(struct seq_file *m, void *v)
{
	struct timespec64 now, elapsed;

	mutex_lock(&timer_lock);

	ktime_get_real_ts64(&now);
	seq_printf(m, "current time: %lld.%09ld\n",
		   (long long)now.tv_sec, now.tv_nsec);

	if (has_last) {
		elapsed = timespec64_sub(now, last_time);
		seq_printf(m, "elapsed time: %lld.%09ld\n",
			   (long long)elapsed.tv_sec, elapsed.tv_nsec);
	}

	last_time = now;
	has_last = true;

	mutex_unlock(&timer_lock);
	return 0;
}

/* single_open runs timer_show once per read, so one cat updates last_time once. */
static int timer_open(struct inode *inode, struct file *file)
{
	return single_open(file, timer_show, NULL);
}

/* Kernel 5.6 replaced file_operations with proc_ops for proc files. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops timer_fops = {
	.proc_open    = timer_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
};
#else
static const struct file_operations timer_fops = {
	.owner   = THIS_MODULE,
	.open    = timer_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};
#endif

static int __init timer_init(void)
{
	timer_entry = proc_create(ENTRY_NAME, PERMS, PARENT, &timer_fops);
	if (!timer_entry)
		return -ENOMEM;
	return 0;
}

static void __exit timer_exit(void)
{
	proc_remove(timer_entry);
}

module_init(timer_init);
module_exit(timer_exit);
