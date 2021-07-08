/*
 * Copyright 2021 Aalto University & University of Waterloo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <camkes.h>
#include <stdio.h>
#include <trusted_apps.h>

int ree_load(int application)
{
    printf("%s: call Loader\n", get_instance_name());
    load_load(application);
    
    return 0;
}

char* ree_attest(int application)
{
    printf("%s: call Attestation\n", get_instance_name());
    char* res = attest_attest(application);
    printf("%s: Received %s from Attestation\n", get_instance_name(), res);

    return res;
}

int ree_start(int application)
{
    int ret = 0;

    printf("%s: start TA %d\n", get_instance_name(), application);

    switch (application)
    {
        case ID_TA1:
	    ret = ta_start();
	    break;
	case ID_TA2:
	    ret = ta2_start(12);
	    break;
	default:
	    printf("%s: no TA registered with ID %d\n", get_instance_name(), application);
	    break;
    }

    return ret;
}
