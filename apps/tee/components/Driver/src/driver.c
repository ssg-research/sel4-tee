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

struct {
    uint32_t head;
    uint32_t tail;
    char buf[SERIAL_BUF];
} extern volatile *interrupt_getchar_buf;

void pre_init(void)
{
    set_putchar(interrupt_putchar_putchar);
}

void send_msg(struct message *m) {
    printf("%c", MAGIC_NUMBER);
    printf("%c%c", m->type[0], m->type[1]);
    printf("%c%c", (char) (m->len & 0xff), (char) ((m->len >> 8) & 0xff));
    for (int i = 0; i < m->len; ++i)
        printf("%c", m->msg[0]);
}

int wait_for_response(struct message *message)
{
    seL4_Word badge = 0;
    int i = 0;
    char next = 0;
    int state = 0;
    unsigned char length[2];

    while (1)
    {
        if (interrupt_getchar_buf->head == interrupt_getchar_buf->tail)
        {
            seL4_Wait(interrupt_getchar_notification(), &badge);
        }
        next = interrupt_getchar_buf->buf[interrupt_getchar_buf->head];
        printf("Got me a %c\n", next);

        switch (state)
        {
        case 0:
        {
            printf("We're in state 0\n");
            message->type[i] = next;

            if (i == 1) 
                state = 1;
            break;
        }
        case 1:
        {
            printf("We're in state 1\n");
            length[i - 2] = next;

            if (i == 3) {
                message->len = length[0] | length[1] << 8;
                printf("Message size is %lu\n", message->len);
                state = 2;
            }
            break;
        }
        case 2:
        {
            printf("We're in state 2\n");
            unsigned msg_pos = i - 4;
            if (msg_pos >= MAX_MSG_SIZE) {
                printf("Maximum message size exceeded (msg_pos is %u < %u)\n", msg_pos, MAX_MSG_SIZE);
                state = -1;
            }

            message->msg[msg_pos] = next;
            break;

        }
        }

        interrupt_getchar_buf->head = (interrupt_getchar_buf->head + 1) % sizeof(interrupt_getchar_buf->buf);
        ++i;

        if (state == 2 && (i - 4) == message->len)
            return 0;
        if (state < 0)
            return state;
    };
}

void setup_public_key(void)
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
    wait_for_response(&response);

    // TODO: Store, verify, or something...
    return;
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
    setup_public_key();

    printf("%s: Asking to sign hash", get_instance_name());

    // Construct request
    strncpy(m.type, RECEIVE_MESSAGE_SIGNATURE_FPGA, 2);
    m.len = 64;
    strncpy(m.msg, fingerprint, 64);
    send_msg(&m);

    //struct message response;
    wait_for_response(&m);

    *signature = malloc(m.len);
    strncpy(*signature, m.msg, m.len);
    *signature[m.len] = 0;

    printf("%s: got response\n", get_instance_name());
    return 0;
}
