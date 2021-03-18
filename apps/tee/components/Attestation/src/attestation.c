/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>

#include "tweetnacl.h"
#include <buffer.h>

char* attest_attest(const char *application)
{
    printf("%s: Taking measurements..\n", get_instance_name());

    // Test our access to the SharedData via dataport ta
    const char *ta_str = (const char *) ta;
    /*
    printf("First %d bytes of TA (%p):", TA_BUFSIZE, &ta_str);
    for (int i = 0; i < TA_BUFSIZE; i++) {
        if (i % 16 == 0)
            printf("\n");
        printf("%02hhx ", *ta_str);
        ta_str++;
    }
    printf("\n");
    */

    // Calculate a hash over the code pages of the TA
    unsigned char hash[crypto_hashblocks_sha256_tweet_BLOCKBYTES];
    if (crypto_hash(hash, ta_str, TA_BUFSIZE)) {
	printf("%s: Error while calculating hash\n", get_instance_name());
    }

    printf("%s: Hash = %lx\n", get_instance_name(), (unsigned long long) hash);

    printf("%s: call Driver\n", get_instance_name());
    char *out = NULL;
    offload_sign(hash, crypto_hashblocks_sha256_tweet_BLOCKBYTES, &out);
    
    return out;
}
