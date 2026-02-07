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
#include "../../include/basex.h"
#include <string.h>

// Base122 encoding - Portable implementation
// Uses 7-bit encoding with illegal UTF-8 byte markers
// ~12.5% overhead (most efficient)

// Base122 uses 122 valid UTF-8 continuation bytes
// Encodes 7 bits per byte (vs 6 bits for Base64)

static const uint8_t BASE122_ILLEGAL[] = {
    0x00, 0x0A, 0x0D, 0x22, 0x26, 0x5C // NUL, LF, CR, ", &, backslash
};

static inline bool is_illegal(uint8_t byte) {
    for (size_t i = 0; i < sizeof(BASE122_ILLEGAL); i++) {
        if (byte == BASE122_ILLEGAL[i]) return true;
    }
    return false;
}

size_t basex_base122_encode_len(size_t input_len) {
    // 7 bits per output byte → 8/7 expansion
    return ((input_len * 8) + 6) / 7 + 2; // +2 for markers
}

size_t basex_base122_decode_len(size_t input_len) {
    // Approximately 7/8 bytes per character
    return input_len + 4;
}

ssize_t basex_base122_encode(const uint8_t* input, size_t input_len, char* output) {
    if (!input || !output) return -1;
    
    size_t out_pos = 0;
    uint32_t accumulator = 0;
    int bits = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        accumulator = (accumulator << 8) | input[i];
        bits += 8;
        
        while (bits >= 7) {
            bits -= 7;
            uint8_t byte = (accumulator >> bits) & 0x7F;
            
            // Map to valid UTF-8 continuation bytes (0x80-0xFF range)
            byte |= 0x80;
            
            // Handle illegal bytes by escaping
            if (is_illegal(byte)) {
                output[out_pos++] = (char)0xC2; // Escape marker
                output[out_pos++] = byte;
            } else {
                output[out_pos++] = byte;
            }
            
            accumulator &= (1 << bits) - 1;
        }
    }
    
    // Flush remaining bits
    if (bits > 0) {
        uint8_t byte = ((accumulator << (7 - bits)) & 0x7F) | 0x80;
        if (is_illegal(byte)) {
            output[out_pos++] = (char)0xC2;
            output[out_pos++] = byte;
        } else {
            output[out_pos++] = byte;
        }
    }
    
    return out_pos;
}

ssize_t basex_base122_decode(const char* input, size_t input_len, uint8_t* output) {
    if (!input || !output) return -1;
    
    size_t out_pos = 0;
    uint32_t accumulator = 0;
    int bits = 0;
    bool escaped = false;
    
    for (size_t i = 0; i < input_len; i++) {
        uint8_t byte = (uint8_t)input[i];
        
        // Handle escape sequences
        if (byte == 0xC2 && !escaped) {
            escaped = true;
            continue;
        }
        
        // Extract 7 bits
        uint8_t value = byte & 0x7F;
        accumulator = (accumulator << 7) | value;
        bits += 7;
        escaped = false;
        
        // Output complete bytes
        while (bits >= 8) {
            bits -= 8;
            output[out_pos++] = (accumulator >> bits) & 0xFF;
            accumulator &= (1 << bits) - 1;
        }
    }
    
    return out_pos;
}
