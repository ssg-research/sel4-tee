#
# Copyright 2021, Max Crone, Aalto University
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

project(tee C)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/Attestation)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/Driver)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/Loader)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/REEAPI)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/Runner)

DeclareCAmkESRootserver(tee.camkes)
add_simulate_test([=[wait_for "After the client"]=])
