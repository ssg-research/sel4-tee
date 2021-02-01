/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>

int run(void)
{
    printf("%s: Call Load\n", get_instance_name());
    if (ree_load("TestApplication"))
        printf("%s: Error when calling load api\n");

    printf("%s: Call Attest\n", get_instance_name());
    if (ree_attest("TestApplication"))
        printf("%s: Error when calling attest api\n");

    printf("%s: Finished running\n", get_instance_name());

    return 0;
}
