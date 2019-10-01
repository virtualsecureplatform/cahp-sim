#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t getbit(uint8_t n, uint8_t i) { return ((n >> i) & 1); }

int bitpat_match_s(int nbits, uint32_t t, const char* s)
{
    for (int i = 0, bi = 0; bi < nbits; i++) {
        switch (s[i]) {
            case '_':  // Skip '_' as separator.
                break;

            case 'x':  // Don't care 'x'
                bi++;
                break;

            case '1':
                if (!getbit(t, nbits - 1 - bi)) return 0;
                bi++;
                break;

            case '0':
                if (getbit(t, nbits - 1 - bi)) return 0;
                bi++;
                break;

            default:
                assert(0 && "Invalid bit pattern.");
        }
    }

    return 1;
}
