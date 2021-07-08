/*
 * Copyright 2021 Aalto University & University of Waterloo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <camkes.h>
#include <stdio.h>

int load_load(int application)
{
    printf("%s: loading TA %d...\n", get_instance_name(), application);

    return 0;
}
