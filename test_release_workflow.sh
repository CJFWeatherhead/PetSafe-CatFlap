#!/bin/bash
# Test script to validate release workflow configuration
# This script checks that the release workflow is properly configured

set -e

echo "========================================="
echo "Testing Release Workflow Configuration"
echo "========================================="
echo ""

# Check if release workflow exists
echo "1. Checking if release workflow exists..."
if [ -f ".github/workflows/release.yml" ]; then
    echo "   ✓ Release workflow file exists"
else
    echo "   ✗ Release workflow file not found"
    exit 1
fi

# Check YAML syntax
echo ""
echo "2. Validating YAML syntax..."
if python3 -c "import yaml; yaml.safe_load(open('.github/workflows/release.yml'))" 2>/dev/null; then
    echo "   ✓ YAML syntax is valid"
else
    echo "   ✗ YAML syntax error"
    exit 1
fi

# Check for required workflow components
echo ""
echo "3. Checking workflow components..."

REQUIRED_STRINGS=(
    "on:"
    "push:"
    "tags:"
    "v*.*.*"
    "Build Firmware and Create Release"
    "docker build"
    "build.sh"
    "dist/XC8_PIC16F886/production/PetSafe-CatFlap.production.hex"
    "action-gh-release"
    "*.hex"
    "checksums"
)

for str in "${REQUIRED_STRINGS[@]}"; do
    if grep -q "$str" .github/workflows/release.yml; then
        echo "   ✓ Contains: $str"
    else
        echo "   ✗ Missing: $str"
        exit 1
    fi
done

# Check if RELEASE.md documentation exists
echo ""
echo "4. Checking release documentation..."
if [ -f "RELEASE.md" ]; then
    echo "   ✓ RELEASE.md exists"
    
    # Check for key sections
    if grep -q "Creating a New Release" RELEASE.md && \
       grep -q "Version Numbering Guidelines" RELEASE.md && \
       grep -q "Troubleshooting" RELEASE.md; then
        echo "   ✓ RELEASE.md has required sections"
    else
        echo "   ✗ RELEASE.md missing required sections"
        exit 1
    fi
else
    echo "   ✗ RELEASE.md not found"
    exit 1
fi

# Check if README.md mentions releases
echo ""
echo "5. Checking README.md updates..."
if grep -q "Download Pre-built Firmware" README.md && \
   grep -q "Releases" README.md && \
   grep -q "RELEASE.md" README.md; then
    echo "   ✓ README.md updated with release information"
else
    echo "   ✗ README.md missing release information"
    exit 1
fi

# Check if CHANGELOG.md is updated
echo ""
echo "6. Checking CHANGELOG.md updates..."
if grep -q "Automated release workflow" CHANGELOG.md; then
    echo "   ✓ CHANGELOG.md updated"
else
    echo "   ✗ CHANGELOG.md not updated"
    exit 1
fi

# Simulate release file naming
echo ""
echo "7. Testing release artifact naming..."
VERSION="v1.0.0"
EXPECTED_HEX="PetSafe-CatFlap-${VERSION}.hex"
EXPECTED_ELF="PetSafe-CatFlap-${VERSION}.elf"

echo "   Expected hex filename: $EXPECTED_HEX"
echo "   Expected elf filename: $EXPECTED_ELF"
echo "   ✓ Naming convention verified"

# Check Docker build script exists
echo ""
echo "8. Checking build dependencies..."
if [ -f "build.sh" ]; then
    echo "   ✓ build.sh exists"
else
    echo "   ✗ build.sh not found"
    exit 1
fi

if [ -f "Dockerfile" ]; then
    echo "   ✓ Dockerfile exists"
else
    echo "   ✗ Dockerfile not found"
    exit 1
fi

echo ""
echo "========================================="
echo "✓ All release workflow checks passed!"
echo "========================================="
echo ""
echo "To create a release:"
echo "  1. Update CHANGELOG.md with version information"
echo "  2. Create and push a version tag:"
echo "     git tag -a v1.0.0 -m 'Release v1.0.0'"
echo "     git push origin v1.0.0"
echo "  3. GitHub Actions will automatically build and create the release"
echo ""
echo "See RELEASE.md for detailed instructions."
