#include "../../include/basex.h"
#include <string.h>

// Base32 encoding (RFC 4648)
// Encodes 5 bytes into 8 ASCII characters

static const char BASE32_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

static const int8_t BASE32_DECODE_TABLE[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 26, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

size_t basex_base32_encode_len(size_t input_len) {
    return ((input_len + 4) / 5) * 8;
}

size_t basex_base32_decode_len(size_t input_len) {
    return (input_len / 8) * 5 + 5;
}

ssize_t basex_base32_encode(const uint8_t* input, size_t input_len, char* output) {
    if (!input || !output) return -1;
    
    size_t out_pos = 0;
    uint64_t buffer = 0;
    int bits = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        buffer = (buffer << 8) | input[i];
        bits += 8;
        
        while (bits >= 5) {
            bits -= 5;
            output[out_pos++] = BASE32_ALPHABET[(buffer >> bits) & 0x1F];
        }
    }
    
    if (bits > 0) {
        output[out_pos++] = BASE32_ALPHABET[(buffer << (5 - bits)) & 0x1F];
    }
    
    while (out_pos % 8 != 0) {
        output[out_pos++] = '=';
    }
    
    return out_pos;
}

ssize_t basex_base32_decode(const char* input, size_t input_len, uint8_t* output) {
    if (!input || !output) return -1;
    
    size_t out_pos = 0;
    uint64_t buffer = 0;
    int bits = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == '=') break;
        
        int8_t c = BASE32_DECODE_TABLE[(uint8_t)input[i]];
        if (c < 0) continue;
        
        buffer = (buffer << 5) | c;
        bits += 5;
        
        if (bits >= 8) {
            bits -= 8;
            output[out_pos++] = (buffer >> bits) & 0xFF;
        }
    }
    
    return out_pos;
}
