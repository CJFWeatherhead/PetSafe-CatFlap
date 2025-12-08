#!/bin/bash
#
# Static Analysis Script for PetSafe Cat Flap Firmware
# Runs cppcheck on all C source files
#

set -e

echo "============================================="
echo "PetSafe Cat Flap - Static Code Analysis"
echo "============================================="
echo ""

# Check if cppcheck is installed
if ! command -v cppcheck &> /dev/null; then
    echo "Error: cppcheck is not installed"
    echo ""
    echo "Install with:"
    echo "  Ubuntu/Debian: sudo apt-get install cppcheck"
    echo "  macOS: brew install cppcheck"
    echo "  Windows: Download from http://cppcheck.sourceforge.net/"
    exit 1
fi

echo "Running cppcheck analysis..."
echo ""

# Run cppcheck with comprehensive options
cppcheck \
    --enable=all \
    --inconclusive \
    --std=c99 \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --template='{file}:{line}:{column}: {severity}: {message} [{id}]' \
    --quiet \
    *.c 2>&1 | tee cppcheck-report.txt

echo ""
echo "============================================="

# Check if any issues were found
if [ -s cppcheck-report.txt ]; then
    ISSUE_COUNT=$(wc -l < cppcheck-report.txt)
    echo "Analysis complete: $ISSUE_COUNT issue(s) found"
    echo "Full report saved to: cppcheck-report.txt"
    echo ""
    echo "Note: Some warnings may be false positives"
    echo "Review each issue to determine if action is needed"
    exit 0
else
    echo "Analysis complete: No issues found!"
    rm -f cppcheck-report.txt
    exit 0
fi
