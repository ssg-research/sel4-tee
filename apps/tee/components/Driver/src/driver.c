/*
 * Copyright 2021, Max Crone, Aalto University
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <autoconf.h>
#include <camkes.h>
#include <stdio.h>

#include "driver.h"

void pre_init(void)
{
    set_putchar(interrupt_putchar_putchar);
}

void send_msg(struct message *m) 
{
    printf("[");
    printf("%c", MAGIC_NUMBER);
    printf("%c%c", m->type[0], m->type[1]);
    printf("%c%c", (char) (m->len & 0xff), (char) ((m->len >> 8) & 0xff));
    for (int i = 0; i < m->len; ++i)
        printf("%c", m->msg[0]);
    printf("]\n");
}

int wait_for_response(struct message *message, char expected_tag[3])
{
    seL4_Word badge = 0;
    int data_pos = -1;
    int discarded = 0;
    char next;
    int state = 0; // Waiting for magick number
    unsigned char length[2];

    printf("Waiting for %c%c tag\n", expected_tag[0], expected_tag[1]);

    while (1)
    {
        if (interrupt_getchar_buf->head == interrupt_getchar_buf->tail) {
            seL4_Wait(interrupt_getchar_notification(), &badge);
        }

        next = interrupt_getchar_buf->buf[interrupt_getchar_buf->head];
        ++data_pos;

        // printf("Got a %d '%c'\n", next, next);

        switch (state) {
        case 0: {
            printf("%s: Looking for %d ('%c') at pos %d -> %d ('%c')\n", __FUNCTION__, expected_tag[data_pos], expected_tag[data_pos], data_pos, next, next);

            if (expected_tag[data_pos] != next) {
                ++discarded;
                if (data_pos == 1 && expected_tag[0] == next) {
                    data_pos = 0;
                } else {
                    data_pos = -1;
                }
            }

            if (data_pos == 1) {
                message->type[0] = expected_tag[0];
                message->type[1] = expected_tag[1];
                printf("%s: Skipped %d characters before tag found\n", __FUNCTION__, discarded);
                printf("%s: Read type field (%c%c), changing to sate 1\n", __FUNCTION__, message->type[0], message->type[1]);
                state = 1;
                data_pos = -1;
            }
            break;
        }
        case 1: {
            length[data_pos] = next;

            if (data_pos == 1) {
                message->len = length[0] | length[1] << 8;
                printf("%s: Read size field (%lu), chaning to state 2\n", __FUNCTION__, message->len);
                state = 2;
                data_pos = -1;
            }
            break;
        }
        case 2: {
            if (data_pos >= MAX_MSG_SIZE) {
                printf("%s: Maximum message size exceeded (data_pos: %u < %u)\n", __FUNCTION__, data_pos, MAX_MSG_SIZE);
                state = -1;
            } else {
                message->msg[data_pos] = next;

                if (data_pos + 1 == message->len) {
                    printf("%s: Read data (type: %c%c, size: %lu)\n", __FUNCTION__, message->type[0],
                           message->type[1], message->len);
                    state = 100;
                }
            }
            break;
        }
        }

        interrupt_getchar_buf->head = (interrupt_getchar_buf->head + 1) % sizeof(interrupt_getchar_buf->buf);

        if (state == 100) // Successfull
            return 0;
        if (state < 0) // Fail
            return state;
    };
}

int check_type(struct message *m, const char expected[3]) 
{
    if (m->type[0] == expected[0], m->type[1] == expected[1])
        return 0;

    printf("err: Expected message of type %s, but saw %c%c\n",
           expected, m->type[0], m->type[1]);
    return -1;
}

int setup_public_key(void)
{
    // SeL4 pk request flow
    unsigned char pk_sel4[crypto_sign_PUBLICKEYBYTES];
    unsigned char verified_public_key_sel4[crypto_sign_PUBLICKEYBYTES];

    // Send request for key
    // unsigned char req[2] = RECEIVE_REQUEST_VERIFIED_PK_FPGA;
    struct message req; 
    strncpy(req.type, RECEIVE_REQUEST_VERIFIED_PK_FPGA, 2);
    req.len = 0;
    send_msg(&req);

    // Wait for message
    struct message response;
    wait_for_response(&response, RECEIVE_VERIFIED_PUBLIC_KEY_HOST);

    if (check_type(&response, RECEIVE_VERIFIED_PUBLIC_KEY_HOST) != 0)
        return -1;

    // TODO: Store, verify, or something...
    return 0;
}

int offload_sign(const char *fingerprint, char **signature)
{
    struct message m;

    // Replace signature
    unsigned char dummy_fingerprint[FINGERPRINT_SIZE] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    fingerprint = dummy_fingerprint;

    printf("%s: Retrieving public key\n", get_instance_name());
    if (setup_public_key() != 0) {
        printf("Public key setup failed!!!");
        return -1;
    }

    printf("%s: Asking to sign hash", get_instance_name());

    // Construct request
    strncpy(m.type, RECEIVE_MESSAGE_SIGNATURE_FPGA, 2);
    m.len = 64;
    strncpy(m.msg, fingerprint, 64);
    send_msg(&m);

    //struct message response;
    wait_for_response(&m, RECEIVE_SIGNED_MESSAGE_HOST);
    if (check_type(&m, RECEIVE_SIGNED_MESSAGE_HOST) != 0)
        return -1;

    *signature = malloc(m.len);
    strncpy(*signature, m.msg, m.len);
    *signature[m.len] = 0;

    printf("%s: got response\n", get_instance_name());
    return 0;
}
