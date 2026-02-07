#include "../../include/basex.h"
#include <string.h>

// Base64 encoding (RFC 4648)
// Encodes 3 bytes into 4 ASCII characters

static const char BASE64_ALPHABET[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static const int8_t BASE64_DECODE_TABLE[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

size_t basex_base64_encode_len(size_t input_len) {
    return ((input_len + 2) / 3) * 4;
}

size_t basex_base64_decode_len(size_t input_len) {
    return (input_len / 4) * 3 + 3;
}

ssize_t basex_base64_encode(const uint8_t* input, size_t input_len, char* output) {
    if (!input || !output) return -1;
    
    size_t out_pos = 0;
    size_t i;
    
    for (i = 0; i + 2 < input_len; i += 3) {
        uint32_t value = ((uint32_t)input[i] << 16) |
                        ((uint32_t)input[i+1] << 8) |
                        ((uint32_t)input[i+2]);
        
        output[out_pos++] = BASE64_ALPHABET[(value >> 18) & 0x3F];
        output[out_pos++] = BASE64_ALPHABET[(value >> 12) & 0x3F];
        output[out_pos++] = BASE64_ALPHABET[(value >> 6) & 0x3F];
        output[out_pos++] = BASE64_ALPHABET[value & 0x3F];
    }
    
    if (i < input_len) {
        uint32_t value = (uint32_t)input[i] << 16;
        if (i + 1 < input_len) value |= (uint32_t)input[i+1] << 8;
        
        output[out_pos++] = BASE64_ALPHABET[(value >> 18) & 0x3F];
        output[out_pos++] = BASE64_ALPHABET[(value >> 12) & 0x3F];
        output[out_pos++] = (i + 1 < input_len) ? BASE64_ALPHABET[(value >> 6) & 0x3F] : '=';
        output[out_pos++] = '=';
    }
    
    return out_pos;
}

ssize_t basex_base64_decode(const char* input, size_t input_len, uint8_t* output) {
    if (!input || !output) return -1;
    
    size_t out_pos = 0;
    uint32_t value = 0;
    int bits = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == '=') break;
        
        int8_t c = BASE64_DECODE_TABLE[(uint8_t)input[i]];
        if (c < 0) continue;
        
        value = (value << 6) | c;
        bits += 6;
        
        if (bits >= 8) {
            bits -= 8;
            output[out_pos++] = (value >> bits) & 0xFF;
        }
    }
    
    return out_pos;
}
