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

#define BUFFER_SIZE (1024 * 1024)

static void print_usage(const char* prog) {
    printf("Usage: %s [OPTION]... [FILE]\n", prog);
    printf("Compress and encode data to Base32 format.\n\n");
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
    printf("zbase32 (BaseX) %d.%d.%d\n", BASEX_VERSION_MAJOR, BASEX_VERSION_MINOR, BASEX_VERSION_PATCH);
}

int main(int argc, char* argv[]) {
    int decode_mode = 0;
    int wrap = 76;
    int ignore_garbage = 0;
    int compression_level = 9;
    int threads = 0;
    int verbose = 0;
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
            case 'd': decode_mode = 1; break;
            case 'w': wrap = atoi(optarg); break;
            case 'i': ignore_garbage = 1; break;
            case 'l': 
                compression_level = atoi(optarg);
                if (compression_level < 1 || compression_level > 19) {
                    fprintf(stderr, "Invalid compression level: %s (must be 1-19)\n", optarg);
                    return 1;
                }
                break;
            case 'T': threads = atoi(optarg); break;
            case 'v': verbose = 1; break;
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
    
    uint8_t* buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("malloc");
        if (input != stdin) fclose(input);
        return 1;
    }
    
    if (!decode_mode) {
        size_t input_len = fread(buffer, 1, BUFFER_SIZE, input);
        
        size_t compressed_bound = ZSTD_compressBound(input_len);
        uint8_t* compressed = malloc(compressed_bound);
        if (!compressed) {
            perror("malloc");
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        ZSTD_CCtx* cctx = ZSTD_createCCtx();
        if (threads > 0) {
            ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, threads);
        }
        
        size_t compressed_size = ZSTD_compressCCtx(cctx, compressed, compressed_bound, 
                                                     buffer, input_len, compression_level);
        ZSTD_freeCCtx(cctx);
        
        if (ZSTD_isError(compressed_size)) {
            fprintf(stderr, "Compression error: %s\n", ZSTD_getErrorName(compressed_size));
            free(compressed);
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        size_t encoded_len = basex_base32_encode_len(compressed_size);
        char* encoded = malloc(encoded_len + 1);
        if (!encoded) {
            perror("malloc");
            free(compressed);
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        ssize_t result = basex_base32_encode(compressed, compressed_size, encoded);
        if (result < 0) {
            fprintf(stderr, "Encoding error\n");
            free(encoded);
            free(compressed);
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        if (wrap > 0) {
            for (size_t i = 0; i < (size_t)result; i++) {
                putchar(encoded[i]);
                if ((i + 1) % wrap == 0) putchar('\n');
            }
            if (result % wrap != 0) putchar('\n');
        } else {
            fwrite(encoded, 1, result, stdout);
            putchar('\n');
        }
        
        if (verbose) {
            fprintf(stderr, "Input: %zu bytes → Compressed: %zu bytes (%.1f%%) → Encoded: %zd bytes\n",
                    input_len, compressed_size, 
                    (100.0 * compressed_size) / input_len,
                    result);
        }
        
        free(encoded);
        free(compressed);
    } else {
        size_t input_len = fread(buffer, 1, BUFFER_SIZE, input);
        
        char* filtered = malloc(input_len);
        size_t filtered_len = 0;
        for (size_t i = 0; i < input_len; i++) {
            if (!isspace((unsigned char)buffer[i])) {
                filtered[filtered_len++] = buffer[i];
            }
        }
        
        size_t decoded_len = basex_base32_decode_len(filtered_len);
        uint8_t* decoded = malloc(decoded_len);
        if (!decoded) {
            perror("malloc");
            free(filtered);
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        ssize_t result = basex_base32_decode(filtered, filtered_len, decoded);
        free(filtered);
        
        if (result < 0) {
            fprintf(stderr, "Decoding error\n");
            free(decoded);
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        size_t decompressed_bound = BUFFER_SIZE;
        uint8_t* decompressed = malloc(decompressed_bound);
        if (!decompressed) {
            perror("malloc");
            free(decoded);
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        ZSTD_DCtx* dctx = ZSTD_createDCtx();
        size_t decompressed_size = ZSTD_decompressDCtx(dctx, decompressed, decompressed_bound,
                                                         decoded, result);
        ZSTD_freeDCtx(dctx);
        
        if (ZSTD_isError(decompressed_size)) {
            fprintf(stderr, "Decompression error: %s\n", ZSTD_getErrorName(decompressed_size));
            free(decompressed);
            free(decoded);
            free(buffer);
            if (input != stdin) fclose(input);
            return 1;
        }
        
        fwrite(decompressed, 1, decompressed_size, stdout);
        
        if (verbose) {
            fprintf(stderr, "Encoded: %zu bytes → Decoded: %zd bytes → Decompressed: %zu bytes\n",
                    filtered_len, result, decompressed_size);
        }
        
        free(decompressed);
        free(decoded);
    }
    
    free(buffer);
    if (input != stdin) fclose(input);
    
    return 0;
}
