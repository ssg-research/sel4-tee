/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

int ree_load(char* application)
{
    printf("%s: Call Loader\n", get_instance_name());
    load_load("TestApplication");
    
    return 0;
}

int ree_attest(char* application)
{
    printf("%s: Call Attestation\n", get_instance_name());
    char* res = attest_attest("TestApplication");
    printf("%s: Received %s from Attestation\n", get_instance_name(), res);

    return 0;
}
