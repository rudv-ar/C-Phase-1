#include <stdio.h>
#include <stdint.h>


/*
Exact width integers — guaranteed exactly N bits:

int8_t     uint8_t      // 8 bits
int16_t    uint16_t     // 16 bits
int32_t    uint32_t     // 32 bits
int64_t    uint64_t     // 64 bits


Least width — at least N bits, whatever is efficient:

int_least8_t     uint_least8_t
int_least16_t    uint_least16_t
int_least32_t    uint_least32_t
int_least64_t    uint_least64_t


Fastest — fastest type with at least N bits:
int_fast8_t      uint_fast8_t
int_fast16_t     uint_fast16_t
int_fast32_t     uint_fast32_t
int_fast64_t     uint_fast64_t

Pointer-sized integers:
intptr_t     // signed, big enough to hold a pointer
uintptr_t    // unsigned, big enough to hold a pointer

Maximum width:
intmax_t     // largest signed integer the platform supports
uintmax_t    // largest unsigned integer the platform supports

In practice, 90% of the time you only use the exact width ones — int32_t, uint8_t etc. The rest exist for portability edge cases.

Use %PRId32, %PRIu64 etc. from <inttypes.h> to print them safely.

*/


int main(void) {
    return 0; // lol, just know the types for now.. se up.. only those types exists in the stdint lib.
}
