#
# Copyright 2021, Max Crone, Aalto University
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

project(ComponentREEAPI C)

DeclareCAmkESComponent(REEAPI SOURCES src/api.c LIBS RegisteredTAs)
