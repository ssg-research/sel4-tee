/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>

#include <trusted_apps.h>
#include <buffer.h>
#include "tweetnacl.h"
#include "base64.h"

static int attest(const char *mem, size_t mem_len, char *signature, int *sig_len)
{
    // Calculate a hash over the code pages of the TA
    unsigned char hash[crypto_hashblocks_sha256_tweet_BLOCKBYTES];
    if (crypto_hash(hash, mem, mem_len)) {
	printf("%s: Error while calculating hash\n", get_instance_name());
	return -1;
    }

    // Print fingerprint in base64 for observability purposes
    unsigned char *hash_base64;
    hash_base64 = base64_encode((const unsigned char *) hash, crypto_hashblocks_sha256_tweet_BLOCKBYTES, NULL);
    printf("%s: fingerprint is %s\n", get_instance_name(), hash_base64);
    free(hash_base64);

    printf("%s: call Driver\n", get_instance_name());
    if (offload_sign(hash, crypto_hashblocks_sha256_tweet_BLOCKBYTES, &signature, sig_len)) {
	printf("%s: Error in driver\n", get_instance_name());
	return -1;
    }

    // Print signature in base64 for observability purposes
    unsigned char *out_base64;
    out_base64 = base64_encode((const unsigned char *) signature, *sig_len, NULL);
    printf("%s: signature is %s\n", get_instance_name(), out_base64);
    free(out_base64);

    return 0;
}

char* attest_attest(int application)
{
    int ret = 0;
    const char *ta_str;
    size_t ta_mem_size;

    printf("%s: Taking measurements of TA %d..\n", get_instance_name(), application);

    switch (application)
    {
        case ID_TA1:
	    ta_str = (const char *) ta;
	    ta_mem_size = TA_BUFSIZE;
	    break;
	case ID_TA2:
	    ta_str = (const char *) ta2;
	    ta_mem_size = TA2_BUFSIZE;
	    break;
	default:
	    printf("%s: no TA registered with ID %d\n", get_instance_name(), application);
	    return NULL;
    }

    char *signature = NULL;
    int sig_len;
    if (attest(ta_str, ta_mem_size, signature, &sig_len)) {
        printf("%s: Failed to attest TA %d\n", get_instance_name(), application);
	return NULL;
    }

    return signature;
}
