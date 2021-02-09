/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

void hello(void)
{
    printf("%s: Hello, world!\n", get_instance_name());
}
