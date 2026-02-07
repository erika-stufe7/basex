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
