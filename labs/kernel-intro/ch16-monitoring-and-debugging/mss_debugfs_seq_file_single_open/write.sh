#!/bin/bash

# Script to write to the debugfs.
#
# Remember to run the file with sudo.

echo -n 77777 > /sys/kernel/debug/mssdbg/mc2/lpddr4-2