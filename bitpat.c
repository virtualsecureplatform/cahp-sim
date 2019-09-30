#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool bitpat_match_s(uint16_t t, const char* s)
{
    int idx = 0;
    int bit_cnt = 0;

    if (s[idx++] != '0') {
        fprintf(stderr, "Invalid pattern\n");
        return false;
    }
    if (s[idx++] != 'b') {
        fprintf(stderr, "Invalid pattern\n");
        return false;
    }

    while (bit_cnt < 16) {
        if (s[idx] == '_') {
            idx++;
            continue;
        }
        if (s[idx] == 'x') {
            idx++;
            bit_cnt++;
            continue;
        }
        if (s[idx] == '1') {
            if ((t >> (15 - bit_cnt)) & 1) {
                idx++;
                bit_cnt++;
                continue;
            }
            return false;
        }
        else if (s[idx] == '0') {
            if (((t >> (15 - bit_cnt)) & 1) == 0) {
                idx++;
                bit_cnt++;
                continue;
            }
            return false;
        }
    }
    return true;
}
