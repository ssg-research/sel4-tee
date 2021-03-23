/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

int load_load(int application)
{
    printf("%s: loading TA %d...\n", get_instance_name(), application);

    return 0;
}
