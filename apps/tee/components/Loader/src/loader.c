/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

int Load_load(char* application)
{
    printf("%s: Loading application \"%s\"\n", get_instance_name(), application);

    return 0;
}
