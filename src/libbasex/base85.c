#include "../../include/basex.h"
#include <string.h>

// Base85 encoding (RFC 1924) - Portable implementation
// Encodes 4 bytes into 5 ASCII characters

static const char BASE85_ALPHABET[] = 
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "!#$%&()*+-;<=>?@^_`{|}~";

static const int8_t BASE85_DECODE_TABLE[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 62, -1, 63, 64, 65, 66, -1, 67, 68, 69, 70, -1, 71, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, 72, 73, 74, 75, 76,
    77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, 78, 79,
    80, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 81, 82, 83, 84, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

size_t basex_base85_encode_len(size_t input_len) {
    return ((input_len + 3) / 4) * 5;
}

size_t basex_base85_decode_len(size_t input_len) {
    return (input_len / 5) * 4 + 4; // +4 for safety
}

ssize_t basex_base85_encode(const uint8_t* input, size_t input_len, char* output) {
    if (!input || !output) return -1;
    
    size_t out_pos = 0;
    size_t i;
    
    // Process full 4-byte blocks
    for (i = 0; i + 3 < input_len; i += 4) {
        uint32_t value = ((uint32_t)input[i] << 24) |
                        ((uint32_t)input[i+1] << 16) |
                        ((uint32_t)input[i+2] << 8) |
                        ((uint32_t)input[i+3]);
        
        // Encode as 5 base85 digits
        output[out_pos + 4] = BASE85_ALPHABET[value % 85]; value /= 85;
        output[out_pos + 3] = BASE85_ALPHABET[value % 85]; value /= 85;
        output[out_pos + 2] = BASE85_ALPHABET[value % 85]; value /= 85;
        output[out_pos + 1] = BASE85_ALPHABET[value % 85]; value /= 85;
        output[out_pos + 0] = BASE85_ALPHABET[value];
        
        out_pos += 5;
    }
    
    // Handle remaining bytes
    if (i < input_len) {
        uint32_t value = 0;
        size_t remaining = input_len - i;
        
        for (size_t j = 0; j < remaining; j++) {
            value |= (uint32_t)input[i + j] << (24 - j * 8);
        }
        
        // Encode partial block
        size_t chars_needed = remaining + 1;
        for (size_t j = 0; j < chars_needed; j++) {
            output[out_pos + chars_needed - 1 - j] = BASE85_ALPHABET[value % 85];
            value /= 85;
        }
        out_pos += chars_needed;
    }
    
    return out_pos;
}

ssize_t basex_base85_decode(const char* input, size_t input_len, uint8_t* output) {
    if (!input || !output) return -1;
    
    size_t out_pos = 0;
    size_t i;
    
    // Process full 5-character blocks
    for (i = 0; i + 4 < input_len; i += 5) {
        uint32_t value = 0;
        
        // Decode 5 base85 digits
        for (int j = 0; j < 5; j++) {
            int8_t digit = BASE85_DECODE_TABLE[(uint8_t)input[i + j]];
            if (digit < 0) return -1; // Invalid character
            value = value * 85 + digit;
        }
        
        // Output 4 bytes
        output[out_pos++] = (value >> 24) & 0xFF;
        output[out_pos++] = (value >> 16) & 0xFF;
        output[out_pos++] = (value >> 8) & 0xFF;
        output[out_pos++] = value & 0xFF;
    }
    
    // Handle remaining characters
    if (i < input_len) {
        uint32_t value = 0;
        size_t remaining = input_len - i;
        
        for (size_t j = 0; j < remaining; j++) {
            int8_t digit = BASE85_DECODE_TABLE[(uint8_t)input[i + j]];
            if (digit < 0) return -1;
            value = value * 85 + digit;
        }
        
        // Output partial bytes
        size_t bytes_out = remaining - 1;
        for (size_t j = 0; j < bytes_out; j++) {
            output[out_pos++] = (value >> (24 - j * 8)) & 0xFF;
        }
    }
    
    return out_pos;
}
