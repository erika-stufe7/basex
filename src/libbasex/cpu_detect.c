#include "../include/basex.h"
#include <stdio.h>
#include <string.h>
#include <cpuid.h>

basex_cpu_features_t basex_detect_cpu_features(void) {
    basex_cpu_features_t features = {0};
    
    unsigned int eax, ebx, ecx, edx;
    
    // Get CPU vendor and model name
    if (__get_cpuid(0x80000000, &eax, &ebx, &ecx, &edx) && eax >= 0x80000004) {
        unsigned int brand[12];
        __get_cpuid(0x80000002, &brand[0], &brand[1], &brand[2], &brand[3]);
        __get_cpuid(0x80000003, &brand[4], &brand[5], &brand[6], &brand[7]);
        __get_cpuid(0x80000004, &brand[8], &brand[9], &brand[10], &brand[11]);
        memcpy(features.cpu_name, brand, sizeof(brand));
        
        // Trim leading spaces
        char* name = features.cpu_name;
        while (*name == ' ') name++;
        if (name != features.cpu_name) {
            memmove(features.cpu_name, name, strlen(name) + 1);
        }
    }
    
    // Check for SSE4.2 (CPUID.01H:ECX.SSE42[bit 20])
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        features.has_sse42 = (ecx & (1 << 20)) != 0;
    }
    
    // Check for AVX2 (CPUID.07H:EBX.AVX2[bit 5])
    if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
        features.has_avx2 = (ebx & (1 << 5)) != 0;
        features.has_bmi1 = (ebx & (1 << 3)) != 0;
        features.has_bmi2 = (ebx & (1 << 8)) != 0;
    }
    
    return features;
}

void basex_print_cpu_info(void) {
    basex_cpu_features_t features = basex_detect_cpu_features();
    
    printf("BaseX v%d.%d.%d - CPU Information\n", 
           BASEX_VERSION_MAJOR, BASEX_VERSION_MINOR, BASEX_VERSION_PATCH);
    printf("=====================================\n");
    printf("CPU Model: %s\n", features.cpu_name[0] ? features.cpu_name : "Unknown");
    printf("\nSupported Features:\n");
    printf("  SSE4.2:  %s\n", features.has_sse42 ? "✓ Yes" : "✗ No");
    printf("  AVX2:    %s\n", features.has_avx2 ? "✓ Yes" : "✗ No");
    printf("  BMI1:    %s\n", features.has_bmi1 ? "✓ Yes" : "✗ No");
    printf("  BMI2:    %s\n", features.has_bmi2 ? "✓ Yes" : "✗ No");
    printf("\nOptimization Status:\n");
    
    if (features.has_avx2 && features.has_bmi2) {
        printf("  Using SIMD-accelerated code paths (AVX2+BMI2)\n");
        printf("  Expected performance: 5-8 GB/s\n");
    } else {
        printf("  Using portable code paths\n");
        printf("  Expected performance: 1-2 GB/s\n");
    }
}
