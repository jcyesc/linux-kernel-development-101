#!/bin/bash
#/* **************** LFD420:5.14 s_24/lab_script_handler.sh **************** */
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

killall lab_handler_test

echo Running lab_handler_test
./lab_handler_test &

PID=$(ps | grep lab_handler | awk '{print $1}' | head -1)
#echo "         PID=$PID"

echo "" ; echo ps after first kill
kill -2 $PID
ps | grep -e PID -e handler

echo "" ; echo ps after Second kill
kill -2 $PID
ps | grep -e PID -e handler

echo Installing kernel module
sudo insmod lab_handler.ko pid=$PID signo=2

echo "" ; echo ps after final kill
kill -2 $PID
ps | grep -e PID -e handler

echo Removing kernel module
sudo rmmod lab_handler

