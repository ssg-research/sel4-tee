/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <trusted_apps.h>

int run(void)
{
    printf("%s: request to load TA %d\n", get_instance_name(), ID_TA1);
    if (ree_load(1))
        printf("%s: Error when calling load api\n");

    /*
    printf("%s: call attest api\n", get_instance_name());
    char *sig = ree_attest(1);
    if (!sig)
        printf("%s: Error when calling attest api\n");

    printf("%s: Received attestation signature: %s\n", get_instance_name(), sig);
    */

    printf("%s: request to start TA\n", get_instance_name());
    ree_start(1);

    printf("%s: finished running\n", get_instance_name());

    return 0;
}
