/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

char* attest_attest(const char *application)
{
    printf("%s: I do attestation\n", get_instance_name());
    char *out;
    offload_sign(application, &out);
    
    return "Attestation";
}
