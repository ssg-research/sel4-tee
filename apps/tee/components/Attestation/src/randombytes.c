#include <stddef.h>
#include "tweetnacl.h"

/**
 *  used by the tweetnacl to generate sk and pk
 **/
void randombytes(unsigned char *buf, size_t size)
{
        //getrandom(buf, size, 0);
        // stdlib.h provides "long int random()"
        // TODO: fill buf with random bytes
}
