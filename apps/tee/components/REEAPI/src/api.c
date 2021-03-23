/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

int ree_load(const char* application)
{
    printf("%s: call Loader\n", get_instance_name());
    load_load(application);
    
    return 0;
}

char* ree_attest(const char* application)
{
    printf("%s: call Attestation\n", get_instance_name());
    char* res = attest_attest(application);
    printf("%s: Received %s from Attestation\n", get_instance_name(), res);

    return res;
}

int ree_start(const char *application)
{
    printf("%s: start TA %s\n", get_instance_name(), application);
    return ta2_start(12);
}
