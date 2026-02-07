# BaseX - Multi-Algorithm Base Encoding Tool

## Projektübersicht
Debian-Binary für Base-Kodierungen mit CPU-optimierten Codepfaden.

### Funktionsumfang
- **Algorithmen:** Base85 (RFC 1924, ASCII85, Z85), Base91, weitere moderne Standards
- **CLI-Interface:** Kompatibel mit `base64`/`base32` Syntax
- **CPU-Optimierung:**
  - Intel Xeon E5-2685/2696 v4 (Broadwell-EP): AVX2, BMI2
  - Intel Core i3-7100U (Kaby Lake): AVX2, BMI1, BMI2
  - Automatische Runtime-Detection via CPUID
  - Fallback auf portable Implementation

## Technische Architektur

### 1. Projektstruktur
```
basex/
├── CMakeLists.txt           # Build system mit CPU-Feature-Detection
├── src/
│   ├── main.c               # CLI entry point
│   ├── base85.c             # Base85 implementations
│   ├── base91.c             # Base91 implementations
│   ├── cpu_detect.c         # Runtime CPU feature detection
│   └── simd/
│       ├── base85_avx2.c    # AVX2-optimized Base85
│       └── base91_avx2.c    # AVX2-optimized Base91
├── include/
│   ├── basex.h              # Public API
│   └── cpu_features.h       # CPU detection header
├── man/
│   └── basex.1              # Man page
├── debian/
│   ├── control              # Package metadata
│   ├── rules                # Build rules
│   ├── changelog            # Debian changelog
│   └── copyright            # License info
└── tests/
    └── test_encodings.c     # Unit tests

```

### 2. CPU-Optimierungsstrategie

#### Runtime CPU Detection
```c
typedef struct {
    bool has_avx2;
    bool has_bmi1;
    bool has_bmi2;
    bool has_sse4_2;
    char cpu_name[64];
} cpu_features_t;

cpu_features_t detect_cpu_features(void);
```

#### Function Pointers für optimierte Pfade
```c
typedef void (*encode_func_t)(const uint8_t* input, size_t len, char* output);
typedef void (*decode_func_t)(const char* input, size_t len, uint8_t* output);

// Runtime selection
encode_func_t select_encoder(int algorithm, cpu_features_t* cpu);
```

### 3. CLI-Interface (base64-kompatibel)

```bash
# Encoding
basex [OPTIONS] [FILE]
basex --base85 input.bin > output.b85
basex --base91 < input.bin > output.b91

# Decoding
basex -d --base85 input.b85 > output.bin

# Options
-d, --decode        Decode data
-w, --wrap=COLS     Wrap encoded lines after COLS characters (default 76)
-i, --ignore-garbage  Ignore non-alphabet chars when decoding
--base85            Use Base85 encoding (RFC 1924)
--base91            Use Base91 encoding
--ascii85           Use ASCII85 variant (Adobe)
--z85               Use Z85 variant (ZeroMQ)
--cpu-info          Show detected CPU features and exit
--force-portable    Disable SIMD optimizations
```

### 4. Algorithmen-Details

#### Base85 (RFC 1924)
- 4 bytes → 5 ASCII chars
- Alphabet: 0-9, A-Z, a-z, !#$%&()*+-;<=>?@^_`{|}~
- ~25% overhead vs Base64's ~33%

#### Base91
- Variable encoding (1-14 bits per output char)
- Alphabet: A-Z, a-z, 0-9, !#$%&()*+,-./:;<=>?@[]^_`{|}~
- ~23% overhead (bessere Effizienz als Base85)

### 5. AVX2-Optimierungen

#### Base85 AVX2
- 32 Bytes parallel verarbeiten (256-bit Register)
- Lookup-Tables in YMM-Registern
- VPSHUFB für schnelle Bit-Permutationen

#### Base91 AVX2
- Vektorisierte Bit-Packing-Operationen
- BMI2 PDEP/PEXT für effiziente Bit-Extraktion

### 6. Debian-Packaging

#### Paket-Optionen

**Option A: Single Package mit Runtime Detection** (EMPFOHLEN)
```
Package: basex
Architecture: amd64
Depends: libc6 (>= 2.31)
Description: Multi-algorithm base encoding tool with CPU optimizations
 Fast base encoding/decoding tool supporting Base85, Base91 and more.
 Automatically detects CPU features (AVX2, BMI2) for optimal performance.
```

**Option B: Multiple Packages**
```
basex-generic    # Portable version
basex-avx2       # AVX2-optimized
basex            # Meta-package with auto-selection
```

## Performance-Ziele
- **Broadwell/Kaby Lake:** 5-8 GB/s encoding (AVX2)
- **Portable:** 1-2 GB/s encoding
- **Memory:** < 10 MB resident size

## Build-Prozess
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make test
cpack -G DEB  # Debian package erstellen
```

## Testing-Strategie
1. Unit-Tests für jede Encoding-Variante
2. Cross-validation: Encoding → Decoding → Original
3. Performance-Benchmarks auf beiden CPU-Typen
4. Compatibility-Tests mit standard base64 tools

## Nächste Schritte
1. ✅ Projektstruktur aufsetzen
2. ⏳ Core-Library implementieren
3. ⏳ CPU-Detection + SIMD-Optimierungen
4. ⏳ CLI-Tool entwickeln
5. ⏳ Manpage schreiben
6. ⏳ Debian-Package bauen
