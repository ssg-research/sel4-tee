/*
 * Copyright 2021 Aalto University & University of Waterloo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <trusted_apps.h>
#include <buffer.h>
#include "tweetnacl.h"
#include "base64.h"

void print_buff(const char *buff, size_t size) {
  for (int i = 0; i < size; ++i)
    printf("%02x", buff[i] & 0xff);
}

static int attest(const char *mem, size_t mem_len, char **signature, size_t *sig_len)
{
    // Calculate a hash over the code pages of the TA
    unsigned char hash[crypto_hash_BYTES];
    if (crypto_hash(hash, mem, mem_len)) {
	printf("%s: Error while calculating hash\n", get_instance_name());
	return -1;
    }

    // Print fingerprint in base64 for observability purposes
    unsigned char *hash_base64;
    hash_base64 = base64_encode((const unsigned char *) hash, crypto_hash_BYTES, NULL);
    // printf("%s: fingerprint is %s\n", get_instance_name(), hash_base64);
    printf("%s: fingerprint is ", get_instance_name());
    print_buff(hash, crypto_hash_BYTES);
    printf("\n");

    free(hash_base64);

    printf("%s: call Driver\n", get_instance_name());
    if (offload_sign(crypto_hash_BYTES, hash, sig_len, signature)) {
	printf("%s: Error in driver\n", get_instance_name());
	return -1;
    }

    // Print signature in base64 for observability purposes
    char *out_base64;
    out_base64 = base64_encode((const unsigned char *) *signature, *sig_len, NULL);
    printf("%s: signature is %s\n", get_instance_name(), out_base64);
    // printf("%s: signature is ", get_instance_name());
    // print_buff(signature, sig_len);
    // printf("\n");
    //free(out_base64);
    *signature = out_base64;

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
    size_t sig_len;
    if (attest(ta_str, ta_mem_size, &signature, &sig_len)) {
        printf("%s: Failed to attest TA %d\n", get_instance_name(), application);
	return NULL;
    }

    return signature;
}
