#!/usr/bin/expect -f
#
# Copyright 2016, Data61, CSIRO
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(D61_GPL)
#

source [file join [file dirname [info script]] procs.tcl]

set timeout 600

spawn make arm_binary_semaphore_defconfig
check_exit

source [file join [file dirname [info script]] build.tcl]

source [file join [file dirname [info script]] run-arm.tcl]
wait_for "Echoing: 42 == 42"