# XC8 Compiler Installation Fix

## Problem Description

The GitHub Actions CI workflow was failing during the XC8 v2.46 compiler installation step with exit code 8. This was caused by:

1. **Ubuntu 24.04 Incompatibility**: The workflow was using `ubuntu-latest` (currently Ubuntu 24.04), but XC8 v2.46 is only officially tested and supported up to Ubuntu 20.04
2. **File Verification Failure**: The XC8 installer runs a post-installation verification step (`verifyinst`) that fails on Ubuntu 24.04, causing the installer to remove all installed files and exit with code 8
3. **Missing 32-bit Libraries**: XC8 compiler requires 32-bit libraries even on 64-bit systems

## Solution Implemented

### 1. Changed Runner OS to Ubuntu 20.04
```yaml
runs-on: ubuntu-20.04  # Changed from ubuntu-latest
```

This ensures the workflow runs on a version of Ubuntu that is officially supported by Microchip's XC8 v2.46 installer.

### 2. Enabled 32-bit Architecture Support
```yaml
sudo dpkg --add-architecture i386
```

This enables the installation of 32-bit packages on the 64-bit Ubuntu system.

### 3. Added Required 32-bit Dependencies
```yaml
sudo apt-get install -y wget unzip make libc6:i386 libstdc++6:i386 libncurses5:i386
```

These 32-bit libraries are required by the XC8 compiler even though it's a 64-bit installer.

### 4. Enhanced Installation Script

The installation script now includes:
- **Download retry logic**: `--retry-connrefused --tries=5 --timeout=30`
- **File verification**: Checks if the downloaded file exists and is not empty
- **Installation logging**: Captures output to `install.log` for debugging
- **Binary verification**: Verifies installation by checking for the compiler binary
- **Better error messages**: Provides detailed diagnostics when installation fails

## How to Verify the Fix

### Option 1: Trigger Workflow Manually
1. Go to Actions tab in GitHub
2. Select "CI Build and Test" workflow
3. Click "Run workflow" on the `copilot/fix-xc8-compiler-issue` branch
4. Watch the "Build with XC8 Compiler" job complete successfully

### Option 2: Merge and Test
1. Merge this PR to main or develop branch
2. The workflow will trigger automatically
3. Check that all three jobs complete successfully:
   - Build with XC8 Compiler
   - Static Code Analysis
   - Run Unit Tests

## Expected Results

When the fix works correctly, you should see:
1. XC8 installer downloads successfully
2. Installation completes without errors
3. Compiler version is displayed: `MPLAB XC8 C Compiler v2.46`
4. Build steps compile `.c` files to `.p1` object files
5. Build artifacts are uploaded successfully

## Troubleshooting

If the installation still fails:

1. **Check the installation log**: The workflow captures output to help diagnose issues
2. **Verify download**: Ensure the installer file downloads completely
3. **Check disk space**: Ensure the runner has enough space for installation
4. **Review dependencies**: Verify all 32-bit libraries installed correctly

## Alternative Solutions (If Needed)

If this fix doesn't work, consider:

1. **Use XC8 v2.32 or earlier**: These versions may have better Ubuntu compatibility
2. **Use Docker container**: Create a controlled environment with Ubuntu 20.04
3. **Cache XC8 installation**: Use GitHub Actions cache to avoid repeated downloads
4. **Manual extraction**: Extract the compiler from the installer without running verification

## References

- [Microchip XC8 v2.46 Release Notes](https://www.microchip.com/content/dam/mchp/documents/DEV/ProductDocuments/ReleaseNotes/xc8-v2.46-full-install-release-notes-PIC.pdf)
- [Microchip Forum: XC8 Installation Issues on Ubuntu 24.04](https://forum.microchip.com/s/topic/a5CV40000002rEjMAI/t399583)
- [Installing MPLAB XC8 C Compiler - Developer Help](https://developerhelp.microchip.com/xwiki/bin/view/software-tools/compilers/xc8/install/)
