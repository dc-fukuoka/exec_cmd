#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <asm/uaccess.h>   /* copy_from_user()    */
#include <linux/proc_fs.h> /* create_proc_entry() */

MODULE_LICENSE("GPL");

#define BUFSIZE  256
#define PROCNAME "exec_cmd"

static int debug = 0;
/* S_IRUGO: read S_IWUSR: write */
module_param(debug, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(debug, "debug flag, 0:disabled, 1:enabled");

static int exec_userspace_func(int argc, char **argv)
{
	struct subprocess_info *sub_info;
	char *envp[4];
	int i;
	
	envp[0] = "HOME=/";
	envp[1] = "TERM=linux";
	envp[2] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
	envp[3] = NULL;

	if (debug) {
		printk(KERN_DEBUG "argc:%d\n", argc);
		for (i=0; i<argc; i++)
			printk(KERN_DEBUG "argv[%d]: \"%s\"\n", i, argv[i]);
	}
	
	sub_info = call_usermodehelper_setup(argv[0], argv, envp, GFP_ATOMIC);
	if (!sub_info)
		return -ENOMEM;

	return call_usermodehelper_exec(sub_info, UMH_WAIT_PROC);
}

static int myparse_args(char *arg, int *argc, char ***argv)
{
        char *p;
        int i;

        /* count argc */
        for (p=arg; *p; p++) {
                if (*p == ' ') {
                        while (*p == ' ') {
                                *p = '\0';
                                p++;
                        }
                        (*argc)++;
                } else if (*(p+1) == '\0') {
                        (*argc)++;
                }
		if (*p == '\n')
			*p = '\0';
        }
	*argc = 0;
        (*argc)++; /* for additional NULL argument */
        *argv = (char**)kmalloc(sizeof(**argv)*(*argc), GFP_KERNEL);
	if (!(*argv))
		return -ENOMEM;
        (*argv)[(*argc)-1] = NULL;
        p = arg;
        for (i=0; i<(*argc)-1; i++) {
		(*argv)[i] = p;
                /* go to next string */
                while(*p)
                        p++;
                while(!*p)
                        p++;
        }
        return 0;
}

static inline void myfree_args(char **argv)
{
        kfree(argv);
}

/* write_proc(), read_proc(): callbacks for entry */
static int write_proc(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char buf[BUFSIZE];
	int argc = 0;
	char **argv = NULL;
	int err = 0;
	memset(buf, '\0', sizeof(buf));
	if (copy_from_user(buf, buffer, count)) {
		printk(KERN_ERR "copy_from_user() failed in %s\n", __func__);
		return -EFAULT;
	}
	if (debug)
		printk(KERN_DEBUG "%s:%d in %s(): buf:%s\n", __FILE__, __LINE__, __func__, buf);
	err = myparse_args(buf, &argc, &argv);
	if (err)
		goto err;
	err = exec_userspace_func(argc, argv);
	if (err)
		goto err;
	myfree_args(argv);
	return count;
err:
	return err;
}

static int read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	return 0;
}

static int __init exec_cmd_init(void)
{
	struct proc_dir_entry *entry = NULL;

        entry = create_proc_entry(PROCNAME, 0666, NULL);
        if (entry) {
                entry->write_proc = write_proc;
                entry->read_proc  = read_proc;
        } else {
                printk(KERN_ERR "%s(): create_proc_entry() failed.\n", __func__);
                return -EBUSY;
        }
	return 0;
}


static void __exit exec_cmd_exit(void)
{
	remove_proc_entry(PROCNAME, NULL);
}

module_init(exec_cmd_init);
module_exit(exec_cmd_exit);
