#!/usr/bin/env python3
"""
Generate web_pages.h from HTML files in webui/
Converts HTML/CSS/JS to PROGMEM strings for ESP32
"""

import os
import re
import sys
from pathlib import Path

def minify_html(content):
    """Basic HTML minification"""
    # Remove HTML comments
    content = re.sub(r'<!--.*?-->', '', content, flags=re.DOTALL)
    # Remove extra whitespace between tags
    content = re.sub(r'>\s+<', '><', content)
    # Remove leading/trailing whitespace on lines
    lines = [line.strip() for line in content.split('\n')]
    content = ' '.join(lines)
    # Collapse multiple spaces
    content = re.sub(r'\s{2,}', ' ', content)
    return content.strip()

def escape_for_cpp(content):
    """Escape content for C++ raw string literal"""
    # Using raw string literals R"(...)" so minimal escaping needed
    # Just need to avoid the delimiter sequence
    return content

def generate_header(pages):
    """Generate the web_pages.h header file"""
    header = '''#pragma once

// ============================================================================
// Auto-generated web pages - DO NOT EDIT DIRECTLY
// Edit files in webui/ and run scripts/generate_web_pages.py
// ============================================================================

#include <Arduino.h>

'''

    for name, content in pages.items():
        var_name = f"PAGE_{name.upper()}"
        # Use raw string literal for cleaner embedding
        header += f'const char {var_name}[] PROGMEM = R"rawliteral(\n'
        header += content
        header += '\n)rawliteral";\n\n'

    # Add helper function
    header += '''
// Page lengths for Content-Length header
'''
    for name in pages.keys():
        var_name = f"PAGE_{name.upper()}"
        header += f'#define {var_name}_LEN (sizeof({var_name}) - 1)\n'

    return header

def main():
    # Find project root
    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    webui_dir = project_root / 'webui'
    output_file = project_root / 'firmware' / 'motor_controller' / 'web' / 'web_pages.h'

    if not webui_dir.exists():
        print(f"Error: webui directory not found at {webui_dir}")
        sys.exit(1)

    # Create output directory if needed
    output_file.parent.mkdir(parents=True, exist_ok=True)

    # Process HTML files
    pages = {}
    html_files = list(webui_dir.glob('*.html'))

    if not html_files:
        print("Error: No HTML files found in webui/")
        sys.exit(1)

    print(f"Processing {len(html_files)} HTML files...")

    for html_file in html_files:
        name = html_file.stem  # filename without extension
        print(f"  - {html_file.name}")

        with open(html_file, 'r', encoding='utf-8') as f:
            content = f.read()

        # Minify if --minify flag is passed
        if '--minify' in sys.argv:
            original_size = len(content)
            content = minify_html(content)
            new_size = len(content)
            print(f"    Minified: {original_size} -> {new_size} bytes ({100*new_size//original_size}%)")

        pages[name] = content

    # Generate header
    header_content = generate_header(pages)

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(header_content)

    print(f"\nGenerated: {output_file}")
    print(f"Total pages: {len(pages)}")

    # Show sizes
    total_size = 0
    for name, content in pages.items():
        size = len(content)
        total_size += size
        print(f"  {name}: {size:,} bytes")
    print(f"  TOTAL: {total_size:,} bytes")

if __name__ == '__main__':
    main()
