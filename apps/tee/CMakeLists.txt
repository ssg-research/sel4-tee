#
# Copyright 2021, Max Crone, Aalto University
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

project(tee C)

# Interface for registered TA id's
add_library(RegisteredTAs INTERFACE)
target_include_directories(RegisteredTAs INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/Attestation)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/Driver)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/Loader)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/REEAPI)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/Runner)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/TA)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/TA2)

DeclareCAmkESRootserver(tee.camkes CPP_INCLUDES include)
add_simulate_test([=[wait_for "After the client"]=])
