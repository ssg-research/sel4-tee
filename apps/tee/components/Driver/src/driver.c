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

#define CLIENT_BUF 20
#define SERIAL_BUF (0x1000 - 8)
#define MAGIC_NUMBER 01

struct {
    uint32_t head;
    uint32_t tail;
    char buf[SERIAL_BUF];
} extern volatile *interrupt_getchar_buf;

void pre_init(void)
{
    set_putchar(interrupt_putchar_putchar);
}

int offload_sign(const char* fingerprint, char **signature)
{
    printf("%s: Offloading attestation to hardware\n", get_instance_name());
    printf("%c%s", MAGIC_NUMBER, fingerprint);

    char buf[CLIENT_BUF];
    while (1) {
        seL4_Word badge = 0;
        int i = 0;
        do {
            if (interrupt_getchar_buf->head == interrupt_getchar_buf->tail) {
                seL4_Wait(interrupt_getchar_notification(), &badge);
            }
            buf[i] = interrupt_getchar_buf->buf[interrupt_getchar_buf->head];
            interrupt_getchar_buf->head = (interrupt_getchar_buf->head + 1) % sizeof(interrupt_getchar_buf->buf);
        } while (buf[i] != '\r' && ++i < CLIENT_BUF - 1);

        buf[i] = 0;
        printf("%s: got '%s'\n", get_instance_name(), buf);

        *signature = malloc(strlen(buf) + 1);
        strncpy(*signature, buf, strlen(buf) + 1);
        (*signature)[strlen(buf)] = '\0';
        return 0;
    }
}
