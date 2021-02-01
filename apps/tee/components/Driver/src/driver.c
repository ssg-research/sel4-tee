/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

int offload_sign(const char* fingerprint, char **signature)
{
    printf("%s: Offloading attestation to hardware\n", get_instance_name());
    *signature = "abcdefgh";

    return 0;
}
