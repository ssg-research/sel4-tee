/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int offload_sign(const char* fingerprint, char **signature)
{
    printf("%s: Offloading attestation to hardware\n", get_instance_name());
    char *sig = "abcdefgh";
    *signature = malloc(strlen(sig) + 1);
    strncpy(*signature, sig, strlen(sig) + 1);
    (*signature)[strlen(sig)] = '\0';

    return 0;
}
