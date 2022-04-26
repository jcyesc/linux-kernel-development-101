#!/bin/bash
#/* **************** LFD420:5.14 s_17/lab_testloop.sh **************** */
#/*
# * The code herein is: Copyright the Linux Foundation, 2021
# *
# * This Copyright is retained for the purpose of protecting free
# * redistribution of source.
# *
# *     URL:    https://training.linuxfoundation.org
# *     email:  info@linuxfoundation.org
# *
# * This code is distributed under Version 2 of the GNU General Public
# * License, which you should have received with the source.
# *
# */
#!/bin/bash 

set -x

P=./lab_task 
[[ -f $P ]] || P=./lab_task

# Take your choice of looping methods:

# n=19
# while [ $n -ge -20 ] ; do ($P $n &) ;  n=$((n-4)) ; done

for n in 19 15 10 5 0 -5 -10 -15 -20 ; do ($P $n &) ; done


