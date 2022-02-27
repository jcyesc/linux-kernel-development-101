
/*
 * Launching User Mode Processes: (Example: wall or cp)
 *
 * This kernel module launches user mode process that were passed using the sysfs
 * filesystem.
 *
 * To do this we use the call_usermodehelper() function.
 *
 * The kernel module will be implemented using the miscelaneous framework.
 *
 * To run the user mode process, execute:
 *
 *     $ sudo insmod launch_usermode_from_module.ko
 *     $ sudo chmod 777 /dev/launch_usermode
 *     $ echo "/bin/wall Hello!!!" > /dev/launch_usermod
 *
 *     Broadcast message from root@raspberrypi (somewhere) (Thu May 13 13:51:19 2021):
 *
 *     hello
 *
 *     # Prints the last command executed and the result.
 *     $ cat /dev/launch_usermod
 *     0
 *     $ touch /home/pi/Development/test.txt
 *     $ echo "/bin/mv /home/pi/Development/test.txt /home/pi/Development/awesome.txt" > /dev/launch_usermode
 *     $ ls
 *     awesome.txt
 *
 * Is it possible to redirect standard input/output while doing this? It's not possible.
 *
 * Note: This module is NOT thread-safe.
 */

#include <linux/cdev.h>     // char dev utilities.
#include <linux/fs.h>       // File operations.
// https://elixir.bootlin.com/linux/v5.15.12/source/include/linux/init.h
#include <linux/init.h>     // Macros for the module.
#include <linux/kernel.h>   // sprintf
#include <linux/kmod.h>
#include <linux/list.h>     // struct list_head
#include <linux/miscdevice.h>
#include <linux/module.h>   // For modules
#include <linux/printk.h>   // printk, pr_info
#include <linux/slab.h>     // kmalloc, kfree
#include <linux/string.h>   // memcpy
#include <linux/types.h>    // loff_t is defined here.
#include <linux/uaccess.h>  // copy_(to, from)_user

#define LAUNCH_USERMODE_DEV_NAME "launch_usermode"

#define COMMAND_BUFFER_SIZE ((size_t) (2 * PAGE_SIZE))
#define COMMAND_RESULT_SIZE ((size_t) (30))

static struct device *launch_usermode_dev;
static char *cmd_buffer;
static char cmd_result[COMMAND_RESULT_SIZE];

/* Keeps the commands that are passed. */
struct cmd_param {
	char *param;
	struct list_head node;
};

static int launch_usermode_dev_open(struct inode *indoe, struct file *file)
{
	dev_info(launch_usermode_dev,
			"launch_usermode_dev_open(): Opening device %s\n",
			LAUNCH_USERMODE_DEV_NAME);
	return 0;
}

static int launch_usermode_dev_release(struct inode *indoe, struct file *file)
{
	dev_info(launch_usermode_dev,
			"launch_usermode_dev_release(): Releasing device %s\n",
			LAUNCH_USERMODE_DEV_NAME);
	return 0;
}

/*
 * Reads the result of executing the previous command and populate it in the
 * user's buffer.
 *
 * @file The file to read from. In this example is not used.
 * @buf  The user's buffer that will be populated with the result of the execution
 *       of the command.
 * @lbuf The length of the user's buffer.
 * @ppos The present position pointer.  It is assumed that it starts in ZERO.
 * @return The number of characters that were read.
 */

static ssize_t launch_usermode_dev_read(
		struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int nbytes;
	int bytes_to_read;
	int maxbytes;

	dev_info(launch_usermode_dev,
			"launch_usermode_dev_read(): Stars lbuf=%d, *ppos=%d\n",
			(int)lbuf,
			(int)*ppos);

	maxbytes = strlen(cmd_result) - *ppos;
	bytes_to_read = maxbytes > lbuf ? lbuf : maxbytes;

	if (bytes_to_read == 0) {
		dev_info(launch_usermode_dev,
				"launch_usermode_dev_read(): The command result was copied!");
		return 0;
	}

	/* copy_to_user() returns the amount left to copy. */
	nbytes = bytes_to_read - copy_to_user(buf, cmd_result + *ppos, bytes_to_read);
	*ppos += nbytes;
	dev_info(launch_usermode_dev,
			"launch_usermode_dev_read(): Ends nbytes=%d, *ppos=%d\n",
			nbytes,
			(int)*ppos);

	return nbytes;
}

/*
 * Execute the command string. The steps to execute the command are:
 *
 * 1. Tokenize the command string
 * 2. Store the tokens in a list.
 * 3. Pass all the tokens to an array that will be passed to call_usermodehelper().
 * 4. Call to call_usermodehelper().
 * 5. Store the result
 * 6. Release resources.
 *
 * It assumes that the command string was populated in {@code cmd_buffer}.
 */
static void process_cmd_string(void) {
	char *tmp_cmd;
	char *token;
	int i;
	int result;
	LIST_HEAD(params_list);
	struct list_head *list_ptr;
	struct list_head *list_pos;
	struct list_head *list_tmp;
	struct cmd_param *cmd_param_ptr;
	char **cmd_param_array;
	int counter;
	// Environment for the command. Because UMH_NO_WAIT is being used, this
	// variable has to live after this function has completed.
	static char *envp[] = { NULL };

	tmp_cmd = cmd_buffer;
	dev_info(launch_usermode_dev,
			"launch_usermode_dev_write(): Command = %s\n",
			tmp_cmd);

	// strsep() modifies the given pointer, so it is necessary to pass a temp
	// pointer.
	while ((token = strsep(&tmp_cmd, " ")) != NULL) {
		token = strim(token);
		counter++;

		cmd_param_ptr = kmalloc(sizeof(struct cmd_param), GFP_KERNEL);
		if(!cmd_param_ptr) {
			dev_info(launch_usermode_dev, "Unable to allocate memory for cmd_param\n");
			goto list_clean;
		}

		cmd_param_ptr->param = token;
		list_add_tail(&cmd_param_ptr->node, &params_list);
	}

	// Allocating space for the commands. The last element of the cmd parameters
	// passed to call_usermodehelper() has to be NULL.
	cmd_param_array = kmalloc_array(
			sizeof(char *), counter + 1 /* One extra for the NULL */, GFP_KERNEL);
	if(!cmd_param_array) {
		dev_info(launch_usermode_dev, "Unable to allocate memory for cmd_param_array\n");
		goto list_clean;
	}

	// Populating the array of commands.
	i = 0;
	list_for_each(list_ptr, &params_list) {
		cmd_param_ptr = list_entry(list_ptr, struct cmd_param, node);
		cmd_param_array[i] = cmd_param_ptr->param;
		dev_info(launch_usermode_dev,
			"param[%d]: %s\n", i, cmd_param_array[i]);
		i++;
	}
	cmd_param_array[i] = NULL;

	// Execute command.
	// https://elixir.bootlin.com/linux/latest/source/kernel/umh.c#L462
	// If we want to wait till the program finishes, use UMH_WAIT_PROC.
	result = call_usermodehelper(
				cmd_param_array[0], cmd_param_array, envp, UMH_NO_WAIT);
	if (result) {
		pr_info("Failed to run the program\n");
		goto clean_all;
	}

	// Save the result.
	counter = snprintf(cmd_result, COMMAND_RESULT_SIZE, "%d\n", result);
	cmd_result[counter] = '\0';

clean_all:
	kfree(cmd_param_array);

list_clean:
	list_for_each_safe(list_pos, list_tmp, &params_list) {
		cmd_param_ptr = list_entry(list_pos, struct cmd_param, node);
		list_del(list_pos);
		kfree(cmd_param_ptr);
	}
}

/*
 * The command written to the launch_usermode_dev is tokenized and executed.
 *
 * @file    The file where the user's buffer will be written. It is not used in
 *          this example.
 * @buf     The user's buffer to write to the character device.
 * @lbuf    The length of the buffer.
 * @ppos    The present position pointer.
 * @return  The number of characters that were written.
 */
static ssize_t launch_usermode_dev_write(
		struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
	int nbytes;
	int bytes_to_write;
	int maxbytes;

	dev_info(launch_usermode_dev,
		"launch_usermode_dev_write(): Starts lbuf=%d, *ppos=%d\n",
		(int)lbuf,
		(int)*ppos);

	// The null terminator is substracted from COMMAND_BUFFER_SIZE.
	maxbytes = (COMMAND_BUFFER_SIZE - 1)- *ppos;
	bytes_to_write = maxbytes > lbuf ? lbuf : maxbytes;

	// Check if we finish copying the command.
	if (lbuf == *ppos) {
		dev_info(launch_usermode_dev,
				"launch_usermode_dev_write(): The command and args were copied!\n");
		return 0;
	}

	/* copy_from_user() returns the amount left to copy. */
	nbytes = bytes_to_write - copy_from_user(cmd_buffer + *ppos, buf, bytes_to_write);
	*ppos += nbytes;
	dev_info(launch_usermode_dev,
			"launch_usermode_dev_write(): Ends nbytes=%d, *ppos=%d\n",
			nbytes, (int)*ppos);

	if (lbuf == *ppos) {
		// Sets the NULL terminator
		cmd_buffer[lbuf] = '\0';
		process_cmd_string();
	}

	return nbytes;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = launch_usermode_dev_read,
	.write = launch_usermode_dev_write,
	.open = launch_usermode_dev_open,
	.release = launch_usermode_dev_release,
};

static struct miscdevice misc_launch_usermode_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = LAUNCH_USERMODE_DEV_NAME,
	.fops = &fops,
};

static int __init launch_usermode_init(void)
{
	char *default_result = "Nothing to report yet!\n";
	ssize_t default_result_size = strlen(default_result);

	cmd_buffer = kmalloc(COMMAND_BUFFER_SIZE, GFP_KERNEL);
	if (!cmd_buffer) {
		pr_err("launch_usermode_init(): kmalloc() failed to allocate memory!\n");
		return -EINVAL;
	}

	memcpy(cmd_result, default_result, default_result_size);
	cmd_result[default_result_size] = '\0';

	if (misc_register(&misc_launch_usermode_dev)) {
		pr_err("launch_usermode_init(): failed to register the %s\n",
				LAUNCH_USERMODE_DEV_NAME);
		kfree(cmd_buffer);
		return -EBUSY;
	}

	launch_usermode_dev = misc_launch_usermode_dev.this_device;
	dev_info(launch_usermode_dev,
			"launch_usermode_init(): succeedded in registering device %s\n",
			LAUNCH_USERMODE_DEV_NAME);

	return 0;
}

static void __exit launch_usermode_exit(void)
{
	dev_info(launch_usermode_dev,
			"launch_usermode_exit(): unregistering device %s\n",
			LAUNCH_USERMODE_DEV_NAME);
	misc_deregister(&misc_launch_usermode_dev);
	kfree(cmd_buffer);
}

module_init(launch_usermode_init);
module_exit(launch_usermode_exit);

MODULE_AUTHOR("John");
MODULE_DESCRIPTION("Launch usermode programs");
MODULE_LICENSE("GPL v2");
