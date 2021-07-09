#
# Copyright 2021 Aalto University & University of Waterloo
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.7.2)

project(ComponentREEAPI C)

DeclareCAmkESComponent(REEAPI SOURCES src/api.c LIBS RegisteredTAs)
