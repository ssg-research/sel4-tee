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


void send_message(char *msg, size_t len)
{
    (void) len;
    // FIXME: Needs to handle null characters!!!
    printf("%c%s", MAGIC_NUMBER, msg);
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
    unsigned char req[2] = RECEIVE_REQUEST_VERIFIED_PK_FPGA;
    send_message(req, 2);

    // Wait for message
    struct message response;
    wait_for_response(&response);

    // TODO: Store, verify, or something...
    return;
}

int offload_sign(const char *fingerprint, char **signature)
{
    printf("%s: Retrieving public key\n", get_instance_name());
    setup_public_key();

    printf("%s: Offloading attestation to hardware\n", get_instance_name());

    // Construct request
    u_int16_t length_int = 64;
    unsigned char tag[2] = RECEIVE_MESSAGE_SIGNATURE_FPGA;
    unsigned char length[2];
    length[0] = length_int & 0xff;
    length[1] = (length_int >> 8) & 0xff;
    // example msg
    unsigned char msg[64] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                             'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                             'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                             'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                             'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                             'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                             'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                             'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    // construct the TLV message
    unsigned char message[4 + sizeof(msg)];
    memcpy(message, tag, 2);
    memcpy(&message[2], length, 2);
    memcpy(&message[4], msg, (size_t)length_int);
    send_message(message, 4 + sizeof(msg));

    struct message response;
    wait_for_response(&response);

    *signature = malloc(response.len);
    strncpy(*signature, response.msg, response.len);

    printf("%s: got response\n", get_instance_name());
    return 0;
}
