#
# Copyright 2021, Max Crone, Aalto University
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

project(tee C)

DeclareCAmkESComponent(Attestation SOURCES components/Attestation/src/attestation.c)
DeclareCAmkESComponent(Driver SOURCES components/Driver/src/driver.c)
DeclareCAmkESComponent(Loader SOURCES components/Loader/src/loader.c)
DeclareCAmkESComponent(REEAPI SOURCES components/REEAPI/src/api.c)
DeclareCAmkESComponent(Runner SOURCES components/Runner/src/runner.c)

DeclareCAmkESRootserver(tee.camkes)
add_simulate_test([=[wait_for "After the client"]=])
