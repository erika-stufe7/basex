#ifndef BASEX_H
#define BASEX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version information */
#define BASEX_VERSION_MAJOR 1
#define BASEX_VERSION_MINOR 0
#define BASEX_VERSION_PATCH 0

/* CPU feature detection */
typedef struct {
    bool has_sse42;
    bool has_avx2;
    bool has_bmi1;
    bool has_bmi2;
    char cpu_name[64];
} basex_cpu_features_t;

/**
 * Detect CPU features at runtime
 * @return CPU feature structure
 */
basex_cpu_features_t basex_detect_cpu_features(void);

/**
 * Print CPU features to stdout
 */
void basex_print_cpu_info(void);

/* Base85 encoding/decoding */

/**
 * Calculate required buffer size for Base85 encoding
 * @param input_len Length of input data in bytes
 * @return Required output buffer size in bytes
 */
size_t basex_base85_encode_len(size_t input_len);

/**
 * Calculate required buffer size for Base85 decoding
 * @param input_len Length of encoded input in bytes
 * @return Required output buffer size in bytes
 */
size_t basex_base85_decode_len(size_t input_len);

/**
 * Encode data to Base85 (RFC 1924)
 * @param input Input data buffer
 * @param input_len Input data length
 * @param output Output buffer (must be large enough)
 * @return Number of bytes written, or -1 on error
 */
ssize_t basex_base85_encode(const uint8_t* input, size_t input_len, char* output);

/**
 * Decode Base85 data
 * @param input Input Base85 string
 * @param input_len Input string length
 * @param output Output buffer (must be large enough)
 * @return Number of bytes written, or -1 on error
 */
ssize_t basex_base85_decode(const char* input, size_t input_len, uint8_t* output);

/* Base91 encoding/decoding */

/**
 * Calculate maximum buffer size for Base91 encoding
 * @param input_len Length of input data in bytes
 * @return Maximum required output buffer size
 */
size_t basex_base91_encode_len(size_t input_len);

/**
 * Calculate maximum buffer size for Base91 decoding
 * @param input_len Length of encoded input
 * @return Maximum required output buffer size
 */
size_t basex_base91_decode_len(size_t input_len);

/**
 * Encode data to Base91
 * @param input Input data buffer
 * @param input_len Input data length
 * @param output Output buffer (must be large enough)
 * @return Number of bytes written, or -1 on error
 */
ssize_t basex_base91_encode(const uint8_t* input, size_t input_len, char* output);

/**
 * Decode Base91 data
 * @param input Input Base91 string
 * @param input_len Input string length
 * @param output Output buffer (must be large enough)
 * @return Number of bytes written, or -1 on error
 */
ssize_t basex_base91_decode(const char* input, size_t input_len, uint8_t* output);

/* Base122 encoding/decoding */

/**
 * Calculate required buffer size for Base122 encoding
 * @param input_len Length of input data in bytes
 * @return Required output buffer size in bytes
 */
size_t basex_base122_encode_len(size_t input_len);

/**
 * Calculate required buffer size for Base122 decoding
 * @param input_len Length of encoded input
 * @return Required output buffer size in bytes
 */
size_t basex_base122_decode_len(size_t input_len);

/**
 * Encode data to Base122
 * @param input Input data buffer
 * @param input_len Input data length
 * @param output Output buffer (must be large enough)
 * @return Number of bytes written, or -1 on error
 */
ssize_t basex_base122_encode(const uint8_t* input, size_t input_len, char* output);

/**
 * Decode Base122 data
 * @param input Input Base122 string
 * @param input_len Input string length
 * @param output Output buffer (must be large enough)
 * @return Number of bytes written, or -1 on error
 */
ssize_t basex_base122_decode(const char* input, size_t input_len, uint8_t* output);

/* Common utilities */

/**
 * Get version string
 * @return Version string (e.g., "1.0.0")
 */
const char* basex_version(void);

#ifdef __cplusplus
}
#endif

#endif /* BASEX_H */
