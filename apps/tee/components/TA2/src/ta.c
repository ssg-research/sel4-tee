/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

int app_start(int x)
{
    printf("%s: hello! magic number is %d!\n", get_instance_name(), x);

    return 0;
}
