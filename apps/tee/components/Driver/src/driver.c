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

void post_init(void) {}

void send_msg(struct message *m) {
  FILE *stream = stdout;
#ifdef ADD_NEWLINE_ON_POST
  fprintf(stream, "[");
#endif // ADD_NEWLINE_ON_POST
  fprintf(stream, "%c", MAGIC_NUMBER);
  fprintf(stream, "%c%c", m->type[0], m->type[1]);
  fprintf(stream, "%c%c", (char)(m->len & 0xff), (char)((m->len >> 8) & 0xff));
  for (int i = 0; i < m->len; ++i)
    fprintf(stream, "%c", m->msg[i]);
#ifdef ADD_NEWLINE_ON_POST
  fprintf(stream, "]\n");
#endif // ADD_NEWLINE_ON_POST
}

int wait_for_response(struct message *message, char expected_tag[3])
{
    seL4_Word badge = 0;
    int data_pos = -1;
    int discarded = 0;
    char next;
    int state = 0; // Waiting for magick number
    unsigned char length[2];

    debug_printf("Waiting for %c%c tag\n", expected_tag[0], expected_tag[1]);

    while (1)
    {
        if (interrupt_getchar_buf->head == interrupt_getchar_buf->tail) {
            seL4_Wait(interrupt_getchar_notification(), &badge);
        }

        next = interrupt_getchar_buf->buf[interrupt_getchar_buf->head];
        ++data_pos;

        // debug_printf("Got a %d '%c'\n", next, next);

        switch (state) {
        case 0: {
            // debug_printf("%s: Looking for %d ('%c') at pos %d -> %d ('%c')\n", __FUNCTION__, expected_tag[data_pos], expected_tag[data_pos], data_pos, next, next);

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
                debug_printf("Skipped %d characters before tag found\n", discarded);
                debug_printf("Read type field (%c%c), changing to state 1\n", message->type[0], message->type[1]);
                state = 1;
                data_pos = -1;
            }
            break;
        }
        case 1: {
            length[data_pos] = next;

            if (data_pos == 1) {
                message->len = length[0] | length[1] << 8;
                debug_printf("Read size field (%lu), chaning to state 2\n", message->len);
                state = 2;
                data_pos = -1;
            }
            break;
        }
        case 2: {
            if (data_pos >= MAX_MSG_SIZE) {
                debug_printf("Maximum message size exceeded (data_pos: %u < %u)\n", data_pos, MAX_MSG_SIZE);
                state = -1;
            } else {
                debug_printf("Saving data at %d\n", data_pos);
                message->msg[data_pos] = next;

                if (data_pos + 1 == message->len) {
                    debug_printf("Read data (type: %c%c, size: %lu)\n", message->type[0],
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

    debug_printf("ERROR: Expected message of type %s, but saw %c%c\n",
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
    debug_printf("Success!\n");

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

    debug_printf("Retrieving public key\n");
    if (setup_public_key() != 0) {
        printf("Public key setup failed!!!");
        return -1;
    }

    debug_printf("Asking to sign hash");

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

    debug_printf("got response\n");
    return 0;
}
