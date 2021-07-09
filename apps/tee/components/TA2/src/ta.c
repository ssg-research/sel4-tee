/*
 * Copyright 2021 Aalto University & University of Waterloo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <camkes.h>
#include <stdio.h>

int app_start(int x)
{
    printf("%s: hello! magic number is %d!\n", get_instance_name(), x);

    return 0;
}
