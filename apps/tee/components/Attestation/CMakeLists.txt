#
# Copyright 2021, Max Crone, Aalto University
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

project(ComponentAttestation C)

DeclareCAmkESComponent(Attestation SOURCES src/attestation.c INCLUDES ../../include)
