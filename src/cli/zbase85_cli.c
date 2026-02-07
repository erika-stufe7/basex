#define _POSIX_C_SOURCE 200809L
#include "../../include/basex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <zstd.h>
#include <stdbool.h>

#define CHUNK_SIZE (128 * 1024)

static void print_usage(const char* prog) {
    printf("Usage: %s [OPTION]... [FILE]\n", prog);
    printf("Compress and encode data to Base85 format.\n\n");
    printf("Options:\n");
    printf("  -d, --decode       Decode and decompress data\n");
    printf("  -w, --wrap=COLS    Wrap encoded lines after COLS characters (default 76, 0 for no wrap)\n");
    printf("  -i, --ignore-garbage   Ignore non-alphabet characters when decoding\n");
    printf("  -l, --level=NUM    Compression level (1-19, default 9)\n");
    printf("  -T, --threads=NUM  Number of compression threads (default 0 = auto)\n");
    printf("  -v, --verbose      Show compression statistics\n");
    printf("  --version          Output version information\n");
    printf("  --help             Display this help and exit\n\n");
    printf("With no FILE, or when FILE is -, read standard input.\n");
}

static void print_version(void) {
    printf("zbase85 (BaseX) %d.%d.%d with zstd %s\n", 
           BASEX_VERSION_MAJOR, BASEX_VERSION_MINOR, BASEX_VERSION_PATCH,
           ZSTD_versionString());
}

int main(int argc, char* argv[]) {
    bool decode_mode = false;
    int wrap = 76;
    bool ignore_garbage = false;
    int compression_level = 9;
    int threads = 0;
    bool verbose = false;
    const char* input_file = NULL;
    
    static struct option long_options[] = {
        {"decode", no_argument, 0, 'd'},
        {"wrap", required_argument, 0, 'w'},
        {"ignore-garbage", no_argument, 0, 'i'},
        {"level", required_argument, 0, 'l'},
        {"threads", required_argument, 0, 'T'},
        {"verbose", no_argument, 0, 'v'},
        {"version", no_argument, 0, 'V'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "dw:il:T:vh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd': decode_mode = true; break;
            case 'w': wrap = atoi(optarg); break;
            case 'i': ignore_garbage = true; break;
            case 'l': 
                compression_level = atoi(optarg);
                if (compression_level < 1 || compression_level > 19) {
                    fprintf(stderr, "Invalid compression level: %s (must be 1-19)\n", optarg);
                    return 1;
                }
                break;
            case 'T': threads = atoi(optarg); break;
            case 'v': verbose = true; break;
            case 'V': print_version(); return 0;
            case 'h': print_usage(argv[0]); return 0;
            default: print_usage(argv[0]); return 1;
        }
    }
    
    if (optind < argc) {
        input_file = argv[optind];
    }
    
    FILE* input = stdin;
    if (input_file && strcmp(input_file, "-") != 0) {
        input = fopen(input_file, "rb");
        if (!input) {
            perror("fopen");
            return 1;
        }
    }
    
    if (!decode_mode) {
        // ENCODE MODE: Read all input, compress once, then encode
        
        // Read all input data into memory
        size_t buffer_capacity = CHUNK_SIZE;
        size_t buffer_size = 0;
        uint8_t* buffer = malloc(buffer_capacity);
        if (!buffer) {
            perror("malloc");
            if (input != stdin) fclose(input);
            return 1;
        }
        
        while (1) {
            if (buffer_size >= buffer_capacity) {
                buffer_capacity *= 2;
                uint8_t* new_buffer = realloc(buffer, buffer_capacity);
                if (!new_buffer) {
                    perror("realloc");
                    free(buffer);
                    if (input != stdin) fclose(input);
                    return 1;
                }
                buffer = new_buffer;
            }
            
            size_t bytes_read = fread(buffer + buffer_size, 1, buffer_capacity - buffer_size, input);
            if (bytes_read == 0) break;
            buffer_size += bytes_read;
        }
        
        // Compress the entire input
        size_t compressed_bound = ZSTD_compressBound(buffer_size);
        uint8_t* compressed = malloc(compressed_bound);
        if (!compressed) {
            perror("malloc compressed");
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        ZSTD_CCtx* cctx = ZSTD_createCCtx();
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, compression_level);
        if (threads > 0 || threads == 0) {
            ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, threads);
        }
        
        size_t compressed_size = ZSTD_compressCCtx(cctx, compressed, compressed_bound,
                                                     buffer, buffer_size, compression_level);
        ZSTD_freeCCtx(cctx);
        free(buffer);
        
        if (ZSTD_isError(compressed_size)) {
            fprintf(stderr, "zstd compression error: %s\n", ZSTD_getErrorName(compressed_size));
            free(compressed);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        // Base85 encode the compressed data
        size_t encoded_len = basex_base85_encode_len(compressed_size);
        char* encoded = malloc(encoded_len + 1);
        if (!encoded) {
            perror("malloc encoded");
            free(compressed);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        ssize_t result = basex_base85_encode(compressed, compressed_size, encoded);
        free(compressed);
        
        if (result < 0) {
            fprintf(stderr, "Base85 encoding error\n");
            free(encoded);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        // Write with line wrapping
        if (wrap > 0) {
            for (ssize_t i = 0; i < result; i++) {
                putchar(encoded[i]);
                if ((i + 1) % wrap == 0) {
                    putchar('\n');
                }
            }
            if (result % wrap != 0) {
                putchar('\n');
            }
        } else {
            fwrite(encoded, 1, result, stdout);
            putchar('\n');
        }
        
        if (verbose) {
            fprintf(stderr, "Input: %zu bytes → Compressed: %zu bytes (%.1f%%) → Encoded: %zd bytes\n",
                    buffer_size, compressed_size,
                    (100.0 * compressed_size) / buffer_size,
                    result);
        }
        
        free(encoded);
        
    } else {
        // DECODE MODE: Read all encoded input, decode, then decompress
        
        // Read all encoded input
        size_t buffer_capacity = CHUNK_SIZE;
        size_t buffer_size = 0;
        char* buffer = malloc(buffer_capacity);
        if (!buffer) {
            perror("malloc");
            if (input != stdin) fclose(input);
            return 1;
        }
        
        while (1) {
            if (buffer_size >= buffer_capacity) {
                buffer_capacity *= 2;
                char* new_buffer = realloc(buffer, buffer_capacity);
                if (!new_buffer) {
                    perror("realloc");
                    free(buffer);
                    if (input != stdin) fclose(input);
                    return 1;
                }
                buffer = new_buffer;
            }
            
            size_t bytes_read = fread(buffer + buffer_size, 1, buffer_capacity - buffer_size, input);
            if (bytes_read == 0) break;
            buffer_size += bytes_read;
        }
        
        // Filter whitespace
        size_t filtered_len = 0;
        for (size_t i = 0; i < buffer_size; i++) {
            if (!isspace((unsigned char)buffer[i])) {
                buffer[filtered_len++] = buffer[i];
            }
        }
        
        // Base85 decode
        size_t decoded_len = basex_base85_decode_len(filtered_len);
        uint8_t* decoded = malloc(decoded_len);
        if (!decoded) {
            perror("malloc decoded");
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        ssize_t result = basex_base85_decode(buffer, filtered_len, decoded);
        free(buffer);
        
        if (result < 0) {
            fprintf(stderr, "Base85 decoding error\n");
            free(decoded);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        // Decompress
        unsigned long long decompressed_size = ZSTD_getFrameContentSize(decoded, result);
        if (decompressed_size == ZSTD_CONTENTSIZE_ERROR) {
            fprintf(stderr, "Not compressed by zstd\n");
            free(decoded);
            if (input != stdin) fclose(input);
            return 1;
        }
        if (decompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
            decompressed_size = result * 20;  // Estimate
        }
        
        uint8_t* decompressed = malloc(decompressed_size);
        if (!decompressed) {
            perror("malloc decompressed");
            free(decoded);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        ZSTD_DCtx* dctx = ZSTD_createDCtx();
        size_t decomp_result = ZSTD_decompressDCtx(dctx, decompressed, decompressed_size,
                                                     decoded, result);
        ZSTD_freeDCtx(dctx);
        free(decoded);
        
        if (ZSTD_isError(decomp_result)) {
            fprintf(stderr, "zstd decompression error: %s\n", ZSTD_getErrorName(decomp_result));
            free(decompressed);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        fwrite(decompressed, 1, decomp_result, stdout);
        
        if (verbose) {
            fprintf(stderr, "Decoded: %zu bytes → Decompressed: %zu bytes\n",
                    (size_t)result, decomp_result);
        }
        
        free(decompressed);
    }
    
    if (input != stdin) fclose(input);
    
    return 0;
}
