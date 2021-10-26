# Kernel Architecture II


## How to find the kernel configuration


In Raspbian, we run the commands below:


```shell
$ uname -r
5.10.75-v8+
$ more /etc/os-release 
PRETTY_NAME="Raspbian GNU/Linux 10 (buster)"
NAME="Raspbian GNU/Linux"
VERSION_ID="10"
VERSION="10 (buster)"
VERSION_CODENAME=buster
ID=raspbian
ID_LIKE=debian
HOME_URL="http://www.raspbian.org/"
SUPPORT_URL="http://www.raspbian.org/RaspbianForums"
BUG_REPORT_URL="http://www.raspbian.org/RaspbianBugs"
```

The `kernel configuration` can be found in:


```shell
# Go to the location where the kernel was compiled.
$ pwd
/home/parallels/Development/lsf-kernel-dev/linux
$ grep PREEMP ./.config
# CONFIG_PREEMPT_NONE is not set
# CONFIG_PREEMPT_VOLUNTARY is not set
CONFIG_PREEMPT=y
CONFIG_PREEMPT_COUNT=y
CONFIG_PREEMPTION=y
CONFIG_PREEMPT_RCU=y
CONFIG_DEBUG_PREEMPT=y
CONFIG_PREEMPTIRQ_TRACEPOINTS=y
# CONFIG_PREEMPT_TRACER is not set
# CONFIG_PREEMPTIRQ_DELAY_TEST is not set
```

In Ubuntu, we can run the commands below:

```shell
$ more /etc/os-release 
NAME="Ubuntu"
VERSION="16.04.6 LTS (Xenial Xerus)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 16.04.6 LTS"
VERSION_ID="16.04"
HOME_URL="http://www.ubuntu.com/"
SUPPORT_URL="http://help.ubuntu.com/"
BUG_REPORT_URL="http://bugs.launchpad.net/ubuntu/"
VERSION_CODENAME=xenial
UBUNTU_CODENAME=xenial

$ uname -r
4.15.0-142-generic
```

The `kernel configuration` can be found in:

```shell
$ more /boot/config-$(uname -r) | grep PREEMPT
CONFIG_PREEMPT_NOTIFIERS=y
# CONFIG_PREEMPT_NONE is not set
CONFIG_PREEMPT_VOLUNTARY=y
# CONFIG_PREEMPT is not set
# CONFIG_PREEMPTIRQ_EVENTS is not set
```
