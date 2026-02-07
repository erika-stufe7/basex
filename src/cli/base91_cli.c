#include "../../include/basex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE (64 * 1024)

static void print_usage(const char* progname) {
    printf("Usage: %s [OPTION]... [FILE]\n", progname);
    printf("Base91 encode or decode FILE, or standard input, to standard output.\n\n");
    printf("  -d, --decode          decode data\n");
    printf("  -w, --wrap=COLS       wrap encoded lines after COLS characters (default 76)\n");
    printf("                        use 0 to disable line wrapping\n");
    printf("  -i, --ignore-garbage  when decoding, ignore non-alphabet characters\n");
    printf("      --cpu-info        show CPU features and exit\n");
    printf("      --help            display this help and exit\n");
    printf("      --version         output version information and exit\n\n");
    printf("With no FILE, or when FILE is -, read standard input.\n\n");
    printf("Report bugs to: https://github.com/yourusername/basex\n");
}

static void print_version(void) {
    printf("base91 (BaseX) %s\n", basex_version());
    printf("Fast Base91 encoding with CPU optimizations\n");
}

int main(int argc, char* argv[]) {
    bool decode = false;
    int wrap_cols = 76;
    bool ignore_garbage = false;
    const char* filename = NULL;
    
    static struct option long_options[] = {
        {"decode", no_argument, 0, 'd'},
        {"wrap", required_argument, 0, 'w'},
        {"ignore-garbage", no_argument, 0, 'i'},
        {"cpu-info", no_argument, 0, 'c'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "dw:i", long_options, NULL)) != -1) {
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
            case 'c':
                basex_print_cpu_info();
                return 0;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'v':
                print_version();
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    if (optind < argc) {
        filename = argv[optind];
    }
    
    FILE* input = stdin;
    if (filename && strcmp(filename, "-") != 0) {
        input = fopen(filename, "rb");
        if (!input) {
            perror("fopen");
            return 1;
        }
    }
    
    uint8_t* in_buffer = malloc(BUFFER_SIZE);
    char* out_buffer = malloc(BUFFER_SIZE * 2);
    
    if (!in_buffer || !out_buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    int line_pos = 0;
    
    // Buffer for decoded input (filter whitespace)
    uint8_t* filtered_buffer = malloc(BUFFER_SIZE * 2);
    if (!filtered_buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    while (1) {
        size_t bytes_read = fread(in_buffer, 1, BUFFER_SIZE, input);
        if (bytes_read == 0) break;
        
        ssize_t result;
        if (decode) {
            // Filter out whitespace when decoding
            size_t filtered_len = 0;
            for (size_t i = 0; i < bytes_read; i++) {
                if (!isspace(in_buffer[i])) {
                    filtered_buffer[filtered_len++] = in_buffer[i];
                }
            }
            
            result = basex_base91_decode((char*)filtered_buffer, filtered_len, (uint8_t*)out_buffer);
            if (result < 0) {
                fprintf(stderr, "Decoding error\n");
                free(in_buffer);
                free(out_buffer);
                if (input != stdin) fclose(input);
                return 1;
            }
            fwrite(out_buffer, 1, result, stdout);
        } else {
            result = basex_base91_encode(in_buffer, bytes_read, out_buffer);
            if (result < 0) {
                fprintf(stderr, "Encoding error\n");
                free(in_buffer);
                free(out_buffer);
                if (input != stdin) fclose(input);
                return 1;
            }
            
            if (wrap_cols > 0) {
                for (ssize_t i = 0; i < result; i++) {
                    putchar(out_buffer[i]);
                    line_pos++;
                    if (line_pos >= wrap_cols) {
                        putchar('\n');
                        line_pos = 0;
                    }
                }
            } else {
                fwrite(out_buffer, 1, result, stdout);
            }
        }
    }
    
    if (!decode && wrap_cols > 0 && line_pos > 0) {
        putchar('\n');
    }
    
    free(in_buffer);
    free(out_buffer);
    free(filtered_buffer);
    if (input != stdin) fclose(input);
    
    return 0;
}
