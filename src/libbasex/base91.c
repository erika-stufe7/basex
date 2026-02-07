#include "../../include/basex.h"
#include <string.h>

// Base91 encoding - Portable implementation
// Variable-length encoding with ~23% overhead

static const char BASE91_ALPHABET[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!#$%&()*+,-./:;<=>?@[]^_`{|}~";

static const int8_t BASE91_DECODE_TABLE[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 62, -1, 63, 64, 65, 66, -1, 67, 68, 69, 70, 71, 72, 73, 74,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 75, 76, 77, 78, 79, 80,
    81,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 82, -1, 83, 84, 85,
    86, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 87, 88, 89, 90, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

size_t basex_base91_encode_len(size_t input_len) {
    // Worst case: each byte becomes ~1.23 characters
    return input_len + (input_len / 4) + 4;
}

size_t basex_base91_decode_len(size_t input_len) {
    // Approximately 0.81 bytes per character
    return input_len + 4;
}

ssize_t basex_base91_encode(const uint8_t* input, size_t input_len, char* output) {
    if (!input || !output) return -1;
    
    uint32_t accumulator = 0;
    int bits = 0;
    size_t out_pos = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        accumulator |= (uint32_t)input[i] << bits;
        bits += 8;
        
        if (bits > 13) {
            uint32_t value = accumulator & 0x1FFF; // 13 bits
            
            if (value > 88) {
                accumulator >>= 13;
                bits -= 13;
            } else {
                value = accumulator & 0x3FFF; // 14 bits
                accumulator >>= 14;
                bits -= 14;
            }
            
            output[out_pos++] = BASE91_ALPHABET[value % 91];
            output[out_pos++] = BASE91_ALPHABET[value / 91];
        }
    }
    
    // Flush remaining bits
    if (bits > 0) {
        output[out_pos++] = BASE91_ALPHABET[accumulator % 91];
        if (bits > 7 || accumulator > 90) {
            output[out_pos++] = BASE91_ALPHABET[accumulator / 91];
        }
    }
    
    return out_pos;
}

ssize_t basex_base91_decode(const char* input, size_t input_len, uint8_t* output) {
    if (!input || !output) return -1;
    
    uint32_t accumulator = 0;
    int bits = 0;
    int value = -1;
    size_t out_pos = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        int8_t c = BASE91_DECODE_TABLE[(uint8_t)input[i]];
        if (c < 0) return -1; // Invalid character
        
        if (value < 0) {
            value = c;
        } else {
            value += c * 91;
            accumulator |= value << bits;
            
            if ((value & 0x1FFF) > 88) {
                bits += 13;
            } else {
                bits += 14;
            }
            
            while (bits > 7) {
                output[out_pos++] = accumulator & 0xFF;
                accumulator >>= 8;
                bits -= 8;
            }
            
            value = -1;
        }
    }
    
    // Flush remaining bits
    if (value >= 0) {
        output[out_pos++] = (accumulator | (value << bits)) & 0xFF;
    }
    
    return out_pos;
}
