/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

int app_start(void)
{
    printf("%s: Hello, world!\n", get_instance_name());

    return 0;
}
