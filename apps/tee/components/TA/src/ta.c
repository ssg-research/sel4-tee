/*
 * Copyright 2021 Aalto University & University of Waterloo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <camkes.h>
#include <stdio.h>

int app_start(void)
{
    printf("%s: Hello, world!\n", get_instance_name());

    return 0;
}
