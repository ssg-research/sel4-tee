/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <trusted_apps.h>

static void test(int app)
{
    printf("%s: request to load TA %d\n", get_instance_name(), app);
    if (ree_load(app))
        printf("%s: Error when calling load api\n");

    printf("%s: request to attest TA %d\n", get_instance_name(), app);
    char *sig = ree_attest(app);
    if (!sig)
        printf("%s: Error when calling attest api\n");

    printf("%s: Received attestation signature: %s\n", get_instance_name(), sig);

    printf("%s: request to start TA %d\n", get_instance_name(), app);
    ree_start(app);

}

int run(void)
{
    test(ID_TA1);
    test(ID_TA2);

    printf("%s: finished running\n", get_instance_name());

    return 0;
}
