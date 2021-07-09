#
# Copyright 2021 Aalto University & University of Waterloo
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.7.2)

project(ComponentAttestation C)

DeclareCAmkESComponent(Attestation SOURCES src/attestation.c src/tweetnacl.c src/randombytes.c src/base64.c LIBS RegisteredTAs)
