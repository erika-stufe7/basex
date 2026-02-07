#!/bin/bash

test_file=$1
label=$2

original_size=$(wc -c < "$test_file")

echo "========================================="
echo "Testing: $label"
echo "Original size: $original_size bytes"
echo "========================================="

# System base64
base64 "$test_file" > /tmp/test.b64
b64_size=$(wc -c < /tmp/test.b64)
b64_pct=$(awk "BEGIN {printf \"%.1f\", ($b64_size/$original_size)*100}")
echo "base64 (system):      $b64_size bytes ($b64_pct%)"

# BaseX encodings (pure)
../build/base85 "$test_file" > /tmp/test.b85
b85_size=$(wc -c < /tmp/test.b85)
b85_pct=$(awk "BEGIN {printf \"%.1f\", ($b85_size/$original_size)*100}")
echo "base85 (BaseX):       $b85_size bytes ($b85_pct%)"

../build/base91 "$test_file" > /tmp/test.b91
b91_size=$(wc -c < /tmp/test.b91)
b91_pct=$(awk "BEGIN {printf \"%.1f\", ($b91_size/$original_size)*100}")
echo "base91 (BaseX):       $b91_size bytes ($b91_pct%)"

../build/base122 "$test_file" > /tmp/test.b122
b122_size=$(wc -c < /tmp/test.b122)
b122_pct=$(awk "BEGIN {printf \"%.1f\", ($b122_size/$original_size)*100}")
echo "base122 (BaseX):      $b122_size bytes ($b122_pct%)"

echo ""
echo "With zstd compression:"

# zstd + base64
zstd -q -9 "$test_file" -o /tmp/test.zst && base64 /tmp/test.zst > /tmp/test.zst.b64
zstd_b64_size=$(wc -c < /tmp/test.zst.b64)
zstd_b64_pct=$(awk "BEGIN {printf \"%.1f\", ($zstd_b64_size/$original_size)*100}")
echo "zstd+base64 (pipe):   $zstd_b64_size bytes ($zstd_b64_pct%)"

# BaseX with zstd
../build/zbase64 "$test_file" > /tmp/test.zb64
zb64_size=$(wc -c < /tmp/test.zb64)
zb64_pct=$(awk "BEGIN {printf \"%.1f\", ($zb64_size/$original_size)*100}")
echo "zbase64 (integrated): $zb64_size bytes ($zb64_pct%)"

../build/zbase85 "$test_file" > /tmp/test.zb85
zb85_size=$(wc -c < /tmp/test.zb85)
zb85_pct=$(awk "BEGIN {printf \"%.1f\", ($zb85_size/$original_size)*100}")
echo "zbase85:              $zb85_size bytes ($zb85_pct%)"

../build/zbase91 "$test_file" > /tmp/test.zb91
zb91_size=$(wc -c < /tmp/test.zb91)
zb91_pct=$(awk "BEGIN {printf \"%.1f\", ($zb91_size/$original_size)*100}")
echo "zbase91:              $zb91_size bytes ($zb91_pct%)"

../build/zbase122 "$test_file" > /tmp/test.zb122
zb122_size=$(wc -c < /tmp/test.zb122)
zb122_pct=$(awk "BEGIN {printf \"%.1f\", ($zb122_size/$original_size)*100}")
echo "zbase122:             $zb122_size bytes ($zb122_pct%)"

echo ""
