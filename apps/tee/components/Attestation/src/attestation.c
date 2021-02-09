/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>

char* attest_attest(const char *application)
{
    printf("%s: Taking measurements..\n", get_instance_name());
    printf("%s: call Driver\n", get_instance_name());
    char *out = NULL;
    offload_sign(application, &out);
    
    return out;
}
