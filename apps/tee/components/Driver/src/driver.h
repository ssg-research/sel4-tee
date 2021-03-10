#ifndef TEE_DRIVER_DRIVER_H
#define TEE_DRIVER_DRIVER_H

#include <camkes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <autoconf.h>
#include <camkes.h>
#include <stdio.h>

#define CLIENT_BUF 20
#define SERIAL_BUF (0x1000 - 8)

#define FINGERPRINT_SIZE 64
#define MAX_MSG_SIZE 4096
#define MAGIC_NUMBER 16

#define RECEIVE_SECRET_KEY_FPGA "RK"
#define RECEIVE_VERIFIED_PUBLIC_KEY_FPGA "RV"
#define RECEIVE_MESSAGE_SIGNATURE_FPGA "RM"
#define RECEIVE_SIGNED_MESSAGE_HOST "RS"
#define RECEIVE_REQUEST_VERIFIED_PK_FPGA "RR"
#define RECEIVE_VERIFIED_PUBLIC_KEY_HOST "RP"

#define crypto_sign_SECRETKEYBYTES 32
#define crypto_sign_PUBLICKEYBYTES 32

unsigned char sk[crypto_sign_SECRETKEYBYTES];
unsigned char pk[crypto_sign_PUBLICKEYBYTES];

struct message {
    char type[3];
    size_t len;
    char msg[MAX_MSG_SIZE];
};

int check_type(struct message *m, const char expected[3]);
void send_msg(struct message *m);
int wait_for_response(struct message *message, char expected_tag[3]);
int setup_public_key(void);

struct {
    uint32_t head;
    uint32_t tail;
    char buf[SERIAL_BUF];
} extern volatile *interrupt_getchar_buf;

#endif //TEE_DRIVER_DRIVER_H