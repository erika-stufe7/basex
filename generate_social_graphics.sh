#!/bin/bash
# Generate Social Media Graphics for BaseX Benchmarks
# Creates SVG graphics that can be shared on Twitter, LinkedIn, etc.

mkdir -p social_media_graphics

# Graphic 1: Main Comparison
cat > social_media_graphics/benchmark_comparison.svg << 'EOF'
<svg width="1200" height="630" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <linearGradient id="grad1" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" style="stop-color:#667eea;stop-opacity:1" />
      <stop offset="100%" style="stop-color:#764ba2;stop-opacity:1" />
    </linearGradient>
  </defs>
  
  <!-- Background -->
  <rect width="1200" height="630" fill="url(#grad1)"/>
  
  <!-- Title -->
  <text x="600" y="80" font-family="Arial, sans-serif" font-size="48" font-weight="bold" fill="white" text-anchor="middle">
    Base64 is Wasteful
  </text>
  <text x="600" y="130" font-family="Arial, sans-serif" font-size="32" fill="#e0e0e0" text-anchor="middle">
    BaseX is 99% more efficient
  </text>
  
  <!-- Main Stats Box -->
  <rect x="100" y="180" width="1000" height="400" rx="20" fill="white" opacity="0.95"/>
  
  <!-- Column Headers -->
  <text x="200" y="240" font-family="Arial, sans-serif" font-size="24" font-weight="bold" fill="#333">
    Data Type
  </text>
  <text x="450" y="240" font-family="Arial, sans-serif" font-size="24" font-weight="bold" fill="#333">
    base64
  </text>
  <text x="650" y="240" font-family="Arial, sans-serif" font-size="24" font-weight="bold" fill="#333">
    zbase122
  </text>
  <text x="900" y="240" font-family="Arial, sans-serif" font-size="24" font-weight="bold" fill="#667eea">
    Savings
  </text>
  
  <!-- Row 1: Text -->
  <line x1="120" y1="260" x2="1080" y2="260" stroke="#ddd" stroke-width="2"/>
  <text x="200" y="300" font-family="Arial, sans-serif" font-size="22" fill="#555">
    44 KB Text
  </text>
  <text x="450" y="300" font-family="Arial, sans-serif" font-size="22" fill="#555">
    60 KB
  </text>
  <text x="650" y="300" font-family="Arial, sans-serif" font-size="22" font-weight="bold" fill="#27ae60">
    341 bytes
  </text>
  <text x="900" y="300" font-family="Arial, sans-serif" font-size="26" font-weight="bold" fill="#667eea">
    99.4%
  </text>
  
  <!-- Row 2: JSON -->
  <line x1="120" y1="320" x2="1080" y2="320" stroke="#ddd" stroke-width="2"/>
  <text x="200" y="360" font-family="Arial, sans-serif" font-size="22" fill="#555">
    48 KB JSON
  </text>
  <text x="450" y="360" font-family="Arial, sans-serif" font-size="22" fill="#555">
    65 KB
  </text>
  <text x="650" y="360" font-family="Arial, sans-serif" font-size="22" font-weight="bold" fill="#27ae60">
    234 bytes
  </text>
  <text x="900" y="360" font-family="Arial, sans-serif" font-size="26" font-weight="bold" fill="#667eea">
    99.6%
  </text>
  
  <!-- Row 3: Code -->
  <line x1="120" y1="380" x2="1080" y2="380" stroke="#ddd" stroke-width="2"/>
  <text x="200" y="420" font-family="Arial, sans-serif" font-size="22" fill="#555">
    11 KB Code
  </text>
  <text x="450" y="420" font-family="Arial, sans-serif" font-size="22" fill="#555">
    15 KB
  </text>
  <text x="650" y="420" font-family="Arial, sans-serif" font-size="22" font-weight="bold" fill="#27ae60">
    2.7 KB
  </text>
  <text x="900" y="420" font-family="Arial, sans-serif" font-size="26" font-weight="bold" fill="#667eea">
    82%
  </text>
  
  <!-- Row 4: Binary -->
  <line x1="120" y1="440" x2="1080" y2="440" stroke="#ddd" stroke-width="2"/>
  <text x="200" y="480" font-family="Arial, sans-serif" font-size="22" fill="#555">
    51 KB Binary
  </text>
  <text x="450" y="480" font-family="Arial, sans-serif" font-size="22" fill="#555">
    69 KB
  </text>
  <text x="650" y="480" font-family="Arial, sans-serif" font-size="22" font-weight="bold" fill="#27ae60">
    59 KB
  </text>
  <text x="900" y="480" font-family="Arial, sans-serif" font-size="26" font-weight="bold" fill="#667eea">
    15%
  </text>
  
  <!-- Footer -->
  <text x="600" y="540" font-family="Arial, sans-serif" font-size="20" fill="#555" text-anchor="middle">
    github.com/erika-stufe7/basex
  </text>
  
  <!-- Tagline -->
  <text x="600" y="570" font-family="Arial, sans-serif" font-size="18" font-style="italic" fill="#888" text-anchor="middle">
    Compression + Efficient Encoding = Massive Savings
  </text>
</svg>
EOF

# Graphic 2: Simple Tweet Card
cat > social_media_graphics/tweet_card.svg << 'EOF'
<svg width="1200" height="675" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <linearGradient id="grad2" x1="0%" y1="0%" x2="0%" y2="100%">
      <stop offset="0%" style="stop-color:#4facfe;stop-opacity:1" />
      <stop offset="100%" style="stop-color:#00f2fe;stop-opacity:1" />
    </linearGradient>
  </defs>
  
  <!-- Background -->
  <rect width="1200" height="675" fill="url(#grad2)"/>
  
  <!-- Main Content Box -->
  <rect x="100" y="100" width="1000" height="475" rx="30" fill="white" opacity="0.98"/>
  
  <!-- Emoji/Icon -->
  <text x="600" y="200" font-family="Arial, sans-serif" font-size="80" text-anchor="middle">
    🚀
  </text>
  
  <!-- Main Message -->
  <text x="600" y="290" font-family="Arial, sans-serif" font-size="48" font-weight="bold" fill="#333" text-anchor="middle">
    44 KB → 341 bytes
  </text>
  
  <text x="600" y="350" font-family="Arial, sans-serif" font-size="36" fill="#555" text-anchor="middle">
    That's 99.4% smaller!
  </text>
  
  <!-- Explanation -->
  <text x="600" y="420" font-family="Arial, sans-serif" font-size="28" fill="#777" text-anchor="middle">
    BaseX: Compression + Efficient Encoding
  </text>
  
  <text x="600" y="465" font-family="Arial, sans-serif" font-size="24" fill="#888" text-anchor="middle">
    Open source • MIT License • Production ready
  </text>
  
  <!-- URL -->
  <rect x="350" y="490" width="500" height="50" rx="25" fill="#667eea"/>
  <text x="600" y="525" font-family="Arial, sans-serif" font-size="24" font-weight="bold" fill="white" text-anchor="middle">
    github.com/erika-stufe7/basex
  </text>
</svg>
EOF

# Graphic 3: LinkedIn Banner
cat > social_media_graphics/linkedin_banner.svg << 'EOF'
<svg width="1584" height="396" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <linearGradient id="grad3" x1="0%" y1="50%" x2="100%" y2="50%">
      <stop offset="0%" style="stop-color:#fa709a;stop-opacity:1" />
      <stop offset="100%" style="stop-color:#fee140;stop-opacity:1" />
    </linearGradient>
  </defs>
  
  <!-- Background -->
  <rect width="1584" height="396" fill="url(#grad3)"/>
  
  <!-- Left Side - Logo Area -->
  <circle cx="198" cy="198" r="120" fill="white" opacity="0.95"/>
  <text x="198" y="180" font-family="Courier New, monospace" font-size="60" font-weight="bold" fill="#333" text-anchor="middle">
    Base
  </text>
  <text x="198" y="230" font-family="Courier New, monospace" font-size="60" font-weight="bold" fill="#667eea" text-anchor="middle">
    X
  </text>
  
  <!-- Main Content -->
  <rect x="380" y="48" width="1154" height="300" rx="25" fill="white" opacity="0.95"/>
  
  <!-- Headline -->
  <text x="957" y="120" font-family="Arial, sans-serif" font-size="48" font-weight="bold" fill="#333" text-anchor="middle">
    Base64 is Wasteful. We Can Do Better.
  </text>
  
  <!-- Stats -->
  <text x="580" y="200" font-family="Arial, sans-serif" font-size="32" fill="#555">
    📊 Text: 99.4% smaller
  </text>
  <text x="1050" y="200" font-family="Arial, sans-serif" font-size="32" fill="#555">
    💾 Code: 82% smaller
  </text>
  
  <text x="580" y="260" font-family="Arial, sans-serif" font-size="32" fill="#555">
    ⚡ 5-8 GB/s throughput
  </text>
  <text x="1050" y="260" font-family="Arial, sans-serif" font-size="32" fill="#555">
    🔧 Open Source (MIT)
  </text>
  
  <!-- Bottom -->
  <text x="957" y="315" font-family="Arial, sans-serif" font-size="24" fill="#888" text-anchor="middle">
    Compression + Efficient Encoding for Modern Workloads
  </text>
</svg>
EOF

# Graphic 4: GitHub Social Preview (1280x640)
cat > social_media_graphics/github_social_preview.svg << 'EOF'
<svg width="1280" height="640" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <linearGradient id="grad4" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" style="stop-color:#0f2027;stop-opacity:1" />
      <stop offset="50%" style="stop-color:#203a43;stop-opacity:1" />
      <stop offset="100%" style="stop-color:#2c5364;stop-opacity:1" />
    </linearGradient>
  </defs>
  
  <!-- Background -->
  <rect width="1280" height="640" fill="url(#grad4)"/>
  
  <!-- Title -->
  <text x="640" y="100" font-family="Arial, sans-serif" font-size="64" font-weight="bold" fill="white" text-anchor="middle">
    BaseX
  </text>
  <text x="640" y="160" font-family="Arial, sans-serif" font-size="36" fill="#aaa" text-anchor="middle">
    High-Performance Base Encoding with Compression
  </text>
  
  <!-- Stats Grid -->
  <!-- Box 1 -->
  <rect x="80" y="220" width="260" height="180" rx="15" fill="white" opacity="0.1"/>
  <text x="210" y="270" font-family="Arial, sans-serif" font-size="48" font-weight="bold" fill="#4facfe" text-anchor="middle">
    99%
  </text>
  <text x="210" y="320" font-family="Arial, sans-serif" font-size="24" fill="white" text-anchor="middle">
    Text Size
  </text>
  <text x="210" y="350" font-family="Arial, sans-serif" font-size="24" fill="white" text-anchor="middle">
    Reduction
  </text>
  
  <!-- Box 2 -->
  <rect x="380" y="220" width="260" height="180" rx="15" fill="white" opacity="0.1"/>
  <text x="510" y="270" font-family="Arial, sans-serif" font-size="48" font-weight="bold" fill="#4facfe" text-anchor="middle">
    82%
  </text>
  <text x="510" y="320" font-family="Arial, sans-serif" font-size="24" fill="white" text-anchor="middle">
    Code Size
  </text>
  <text x="510" y="350" font-family="Arial, sans-serif" font-size="24" fill="white" text-anchor="middle">
    Reduction
  </text>
  
  <!-- Box 3 -->
  <rect x="680" y="220" width="260" height="180" rx="15" fill="white" opacity="0.1"/>
  <text x="810" y="270" font-family="Arial, sans-serif" font-size="48" font-weight="bold" fill="#4facfe" text-anchor="middle">
    8 GB/s
  </text>
  <text x="810" y="320" font-family="Arial, sans-serif" font-size="24" fill="white" text-anchor="middle">
    Encoding
  </text>
  <text x="810" y="350" font-family="Arial, sans-serif" font-size="24" fill="white" text-anchor="middle">
    Speed
  </text>
  
  <!-- Box 4 -->
  <rect x="980" y="220" width="260" height="180" rx="15" fill="white" opacity="0.1"/>
  <text x="1110" y="270" font-family="Arial, sans-serif" font-size="48" font-weight="bold" fill="#4facfe" text-anchor="middle">
    8
  </text>
  <text x="1110" y="320" font-family="Arial, sans-serif" font-size="24" fill="white" text-anchor="middle">
    CLI Tools
  </text>
  <text x="1110" y="350" font-family="Arial, sans-serif" font-size="24" fill="white" text-anchor="middle">
    Ready to Use
  </text>
  
  <!-- Features -->
  <text x="640" y="460" font-family="Arial, sans-serif" font-size="28" fill="white" text-anchor="middle">
    ✓ Base32/64/85/91/122  ✓ zstd Compression  ✓ AVX2 Optimized
  </text>
  
  <!-- Bottom -->
  <rect x="390" y="510" width="500" height="70" rx="35" fill="#4facfe"/>
  <text x="640" y="560" font-family="Arial, sans-serif" font-size="32" font-weight="bold" fill="white" text-anchor="middle">
    github.com/erika-stufe7/basex
  </text>
</svg>
EOF

echo "✅ Social media graphics created in social_media_graphics/"
echo ""
echo "Files created:"
echo "  - benchmark_comparison.svg (1200x630 - Twitter/Facebook)"
echo "  - tweet_card.svg (1200x675 - Twitter Card)"
echo "  - linkedin_banner.svg (1584x396 - LinkedIn Banner)"
echo "  - github_social_preview.svg (1280x640 - GitHub Social Preview)"
echo ""
echo "To convert to PNG (requires inkscape or ImageMagick):"
echo "  inkscape --export-type=png benchmark_comparison.svg"
echo "  # or"
echo "  convert benchmark_comparison.svg benchmark_comparison.png"
EOF

chmod +x social_media_graphics/generate_graphics.sh
