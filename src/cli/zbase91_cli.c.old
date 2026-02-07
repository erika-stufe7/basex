#include "../../include/basex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include <zstd.h>

#define BUFFER_SIZE (128 * 1024)
#define ZSTD_DEFAULT_LEVEL 9

static void print_usage(const char* progname) {
    printf("Usage: %s [OPTION]... [FILE]\n", progname);
    printf("Compress with zstd and encode with Base91, or decode and decompress FILE, or standard input, to standard output.\n\n");
    printf("Compression options:\n");
    printf("  -#                    Compression level (1-19, default: %d)\n", ZSTD_DEFAULT_LEVEL);
    printf("  -T, --threads=NUM     Number of threads (0=auto, default: 0)\n");
    printf("  -d, --decode          Decode and decompress data\n");
    printf("\nBase91 encoding options:\n");
    printf("  -w, --wrap=COLS       Wrap encoded lines after COLS characters (default 76)\n");
    printf("                        use 0 to disable line wrapping\n");
    printf("  -i, --ignore-garbage  When decoding, ignore non-alphabet characters\n");
    printf("\nGeneral options:\n");
    printf("  -c, --stdout          Write to stdout (default)\n");
    printf("  -k, --keep            Keep input file (not applicable for stdin)\n");
    printf("  -v, --verbose         Verbose mode\n");
    printf("  -q, --quiet           Quiet mode\n");
    printf("      --cpu-info        Show CPU features and exit\n");
    printf("      --help            Display this help and exit\n");
    printf("      --version         Output version information and exit\n\n");
    printf("With no FILE, or when FILE is -, read standard input.\n\n");
    printf("Pipeline: INPUT -> zstd compress (level %d) -> base91 encode -> OUTPUT\n", ZSTD_DEFAULT_LEVEL);
    printf("Report bugs to: https://github.com/yourusername/basex\n");
}

static void print_version(void) {
    printf("zbase91 (BaseX) %s with zstd %s\n", basex_version(), ZSTD_versionString());
    printf("Fast zstd compression with Base91 encoding and CPU optimizations\n");
}

int main(int argc, char* argv[]) {
    bool decode = false;
    int wrap_cols = 76;
    bool ignore_garbage = false;
    int compression_level = ZSTD_DEFAULT_LEVEL;
    int threads = 0;
    bool verbose = false;
    bool quiet = false;
    const char* filename = NULL;
    
    static struct option long_options[] = {
        {"decode", no_argument, 0, 'd'},
        {"wrap", required_argument, 0, 'w'},
        {"ignore-garbage", no_argument, 0, 'i'},
        {"threads", required_argument, 0, 'T'},
        {"stdout", no_argument, 0, 'c'},
        {"keep", no_argument, 0, 'k'},
        {"verbose", no_argument, 0, 'v'},
        {"quiet", no_argument, 0, 'q'},
        {"cpu-info", no_argument, 0, 'C'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "dw:iT:1234567890ckvqhV", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
                decode = true;
                break;
            case 'w':
                wrap_cols = atoi(optarg);
                break;
            case 'i':
                ignore_garbage = true;
                break;
            case 'T':
                threads = atoi(optarg);
                break;
            case 'c':
            case 'k':
                // Already default behavior
                break;
            case 'v':
                verbose = true;
                break;
            case 'q':
                quiet = true;
                break;
            case 'C':
                basex_print_cpu_info();
                printf("\nzstd version: %s\n", ZSTD_versionString());
                return 0;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'V':
                print_version();
                return 0;
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
                // Handle -# compression level
                compression_level = opt - '0';
                if (compression_level == 0) compression_level = 10;
                // Check for two-digit levels (11-19)
                if (optind < argc && argv[optind][0] >= '0' && argv[optind][0] <= '9') {
                    compression_level = compression_level * 10 + (argv[optind][0] - '0');
                    optind++;
                }
                if (compression_level < 1 || compression_level > 19) {
                    fprintf(stderr, "Invalid compression level: %d (must be 1-19)\n", compression_level);
                    return 1;
                }
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    if (optind < argc) {
        filename = argv[optind];
    }
    
    if (verbose && !quiet) {
        fprintf(stderr, "zbase91: zstd v%s, compression level %d, threads %d\n", 
                ZSTD_versionString(), compression_level, threads);
    }
    
    // Open input file or use stdin
    FILE* input = stdin;
    if (filename && strcmp(filename, "-") != 0) {
        input = fopen(filename, "rb");
        if (!input) {
            perror("fopen");
            return 1;
        }
    }
    
    uint8_t* in_buffer = malloc(BUFFER_SIZE);
    uint8_t* zstd_buffer = malloc(ZSTD_compressBound(BUFFER_SIZE));
    uint8_t* base91_buffer = malloc(BUFFER_SIZE * 2);
    uint8_t* filtered_buffer = malloc(BUFFER_SIZE * 2);
    
    if (!in_buffer || !zstd_buffer || !base91_buffer || !filtered_buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    // Create zstd context
    ZSTD_CCtx* cctx = NULL;
    ZSTD_DCtx* dctx = NULL;
    
    if (!decode) {
        cctx = ZSTD_createCCtx();
        if (!cctx) {
            fprintf(stderr, "Failed to create zstd compression context\n");
            return 1;
        }
        ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, compression_level);
        if (threads > 0 || threads == 0) {
            ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, threads);
        }
    } else {
        dctx = ZSTD_createDCtx();
        if (!dctx) {
            fprintf(stderr, "Failed to create zstd decompression context\n");
            return 1;
        }
    }
    
    int line_pos = 0;
    size_t total_in = 0, total_out = 0;
    
    while (1) {
        size_t bytes_read = fread(in_buffer, 1, BUFFER_SIZE, input);
        if (bytes_read == 0) break;
        total_in += bytes_read;
        
        if (decode) {
            // Decode: Base91 -> zstd decompress
            // Filter whitespace
            size_t filtered_len = 0;
            for (size_t i = 0; i < bytes_read; i++) {
                if (!isspace(in_buffer[i])) {
                    filtered_buffer[filtered_len++] = in_buffer[i];
                }
            }
            
            // Base91 decode
            ssize_t decoded_len = basex_base91_decode((char*)filtered_buffer, filtered_len, zstd_buffer);
            if (decoded_len < 0) {
                fprintf(stderr, "Base91 decoding error\n");
                goto cleanup;
            }
            
            // zstd decompress
            size_t decompressed_size = ZSTD_decompressDCtx(dctx, base91_buffer, BUFFER_SIZE * 2, 
                                                            zstd_buffer, decoded_len);
            if (ZSTD_isError(decompressed_size)) {
                fprintf(stderr, "zstd decompression error: %s\n", ZSTD_getErrorName(decompressed_size));
                goto cleanup;
            }
            
            fwrite(base91_buffer, 1, decompressed_size, stdout);
            total_out += decompressed_size;
            
        } else {
            // Encode: zstd compress -> Base91
            size_t compressed_size = ZSTD_compressCCtx(cctx, zstd_buffer, ZSTD_compressBound(BUFFER_SIZE),
                                                        in_buffer, bytes_read, compression_level);
            if (ZSTD_isError(compressed_size)) {
                fprintf(stderr, "zstd compression error: %s\n", ZSTD_getErrorName(compressed_size));
                goto cleanup;
            }
            
            // Base91 encode
            ssize_t encoded_len = basex_base91_encode(zstd_buffer, compressed_size, (char*)base91_buffer);
            if (encoded_len < 0) {
                fprintf(stderr, "Base91 encoding error\n");
                goto cleanup;
            }
            
            // Handle line wrapping
            if (wrap_cols > 0) {
                for (ssize_t i = 0; i < encoded_len; i++) {
                    putchar(base91_buffer[i]);
                    line_pos++;
                    if (line_pos >= wrap_cols) {
                        putchar('\n');
                        line_pos = 0;
                    }
                }
            } else {
                fwrite(base91_buffer, 1, encoded_len, stdout);
            }
            
            total_out += encoded_len;
        }
    }
    
    if (!decode && wrap_cols > 0 && line_pos > 0) {
        putchar('\n');
    }
    
    if (verbose && !quiet) {
        fprintf(stderr, "Total: %zu bytes -> %zu bytes (%.2f%% %s)\n",
                total_in, total_out, 
                total_in > 0 ? (100.0 * total_out / total_in) : 0.0,
                decode ? "decompressed" : "compressed+encoded");
    }
    
cleanup:
    free(in_buffer);
    free(zstd_buffer);
    free(base91_buffer);
    free(filtered_buffer);
    if (cctx) ZSTD_freeCCtx(cctx);
    if (dctx) ZSTD_freeDCtx(dctx);
    if (input != stdin) fclose(input);
    
    return 0;
}
