# Release Process Guide

This document explains how to create releases for the PetSafe Cat Flap firmware.

## Overview

The project uses automated GitHub Actions workflows to build and release firmware binaries. When a version tag is pushed, the release workflow automatically:

1. Builds the firmware using Docker and XC8 compiler
2. Creates versioned .hex and .elf files
3. Generates SHA256 checksums
4. Creates a GitHub Release with all artifacts
5. Publishes release notes

## Creating a New Release

### Prerequisites

- Write access to the repository
- Git configured with your GitHub credentials
- All changes committed and pushed to main branch

### Step-by-Step Process

#### 1. Update Version Information

Before creating a release, update the version in relevant files:

**Update CHANGELOG.md:**
```markdown
## [1.0.1] - 2024-12-12

### Fixed
- Description of bug fixes

### Added
- New features (if any)

### Changed
- Changes to existing functionality
```

**Commit the changes:**
```bash
git add CHANGELOG.md
git commit -m "Prepare release v1.0.1"
git push origin main
```

#### 2. Create and Push a Version Tag

Tags should follow semantic versioning: `v<major>.<minor>.<patch>`

**Example versions:**
- `v1.0.0` - Major release
- `v1.0.1` - Bug fix release
- `v1.1.0` - Minor feature release
- `v2.0.0` - Breaking changes

**Create an annotated tag:**
```bash
# Create tag with message
git tag -a v1.0.1 -m "Release v1.0.1 - Bug fixes and improvements"

# Push the tag to GitHub
git push origin v1.0.1
```

**Alternative - Create tag with detailed release notes:**
```bash
# Open editor for detailed tag message
git tag -a v1.0.1

# In the editor, write:
# Release v1.0.1
# 
# Bug Fixes:
# - Fixed RFID timeout handling
# - Corrected EEPROM write timing
# 
# Improvements:
# - Enhanced error handling in serial communication

# Push the tag
git push origin v1.0.1
```

#### 3. Automated Build and Release

Once the tag is pushed, GitHub Actions automatically:

1. **Triggers the Release Workflow** (`.github/workflows/release.yml`)
2. **Builds the Firmware**:
   - Sets up Docker environment
   - Compiles all source files with XC8
   - Generates .hex and .elf files
3. **Prepares Release Artifacts**:
   - Renames files with version (e.g., `PetSafe-CatFlap-v1.0.1.hex`)
   - Generates SHA256 checksums
4. **Creates GitHub Release**:
   - Uses the tag name as release name
   - Includes detailed release notes
   - Attaches firmware files as assets

#### 4. Monitor the Release Workflow

**Check workflow status:**
1. Go to the [Actions tab](https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions)
2. Look for the "Create Release" workflow
3. Click to view build logs and progress

**If the workflow fails:**
- Check the logs for error messages
- Fix any issues in the code
- Delete the tag: `git tag -d v1.0.1 && git push origin :refs/tags/v1.0.1`
- Make fixes, commit, and create the tag again

#### 5. Verify the Release

**After workflow completes:**
1. Go to the [Releases page](https://github.com/CJFWeatherhead/PetSafe-CatFlap/releases)
2. Verify the new release appears
3. Check that all assets are attached:
   - `PetSafe-CatFlap-v1.0.1.hex`
   - `PetSafe-CatFlap-v1.0.1.elf`
   - `checksums.txt`
4. Review the release notes

**Test the release artifacts:**
```bash
# Download the .hex file
wget https://github.com/CJFWeatherhead/PetSafe-CatFlap/releases/download/v1.0.1/PetSafe-CatFlap-v1.0.1.hex

# Download checksums
wget https://github.com/CJFWeatherhead/PetSafe-CatFlap/releases/download/v1.0.1/checksums.txt

# Verify integrity
sha256sum -c checksums.txt
```

## Version Numbering Guidelines

Follow [Semantic Versioning 2.0.0](https://semver.org/):

### Major Version (X.0.0)
Increment when making **incompatible changes**:
- Hardware incompatibilities
- Breaking changes to EEPROM format
- Changes that require hardware modifications
- Incompatible serial protocol changes

### Minor Version (0.X.0)
Increment when adding **backward-compatible functionality**:
- New operating modes
- New features
- Enhanced functionality
- Non-breaking serial protocol additions

### Patch Version (0.0.X)
Increment for **backward-compatible bug fixes**:
- Bug fixes
- Performance improvements
- Code refactoring without behavior changes
- Documentation updates (optional)

## Release Types

### Stable Releases
- Tags: `v1.0.0`, `v1.1.0`, `v2.0.0`
- Fully tested on hardware
- Documented in CHANGELOG.md
- Recommended for production use

### Pre-releases (Future)
- Tags: `v1.1.0-beta.1`, `v2.0.0-rc.1`
- Mark as "pre-release" in GitHub
- For testing new features
- Not recommended for production

## Manual Release Testing

You can trigger the release workflow manually without creating a tag:

**Trigger manual build:**
1. Go to [Actions tab](https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions)
2. Select "Create Release" workflow
3. Click "Run workflow"
4. Select branch and click "Run workflow"

This will create a **pre-release** with a timestamp-based version (e.g., `manual-20241213-151030`) instead of a semantic version tag. Manual releases are marked as pre-releases to distinguish them from official tagged releases.

**Important:** Manual workflow runs will create a lightweight git tag with the timestamp-based version. These tags are separate from semantic version tags (e.g., `v1.0.1`) used for official releases. You can filter them in the releases page by looking for the "Pre-release" badge.

**Note:** Manual releases are useful for:
- Testing the release workflow
- Creating development builds
- Sharing firmware with testers before an official release

For production releases, always use proper version tags (e.g., `v1.0.1`).

## Troubleshooting

### Release Workflow Fails

**Build errors:**
- Check Docker build logs
- Verify all source files are committed
- Ensure XC8 compiler can find all dependencies

**Permission errors:**
- Verify repository has "Read and write permissions" for workflows
- Settings → Actions → General → Workflow permissions

**Tag already exists:**
```bash
# Delete local tag
git tag -d v1.0.1

# Delete remote tag
git push origin :refs/tags/v1.0.1

# Recreate tag with fixes
git tag -a v1.0.1 -m "Release v1.0.1"
git push origin v1.0.1
```

### Missing Release Assets

If artifacts don't appear in the release:
- Check workflow logs for upload errors
- Verify files exist in `dist/XC8_PIC16F886/production/`
- Check file paths in release workflow

## Best Practices

### Before Releasing

1. **Test thoroughly:**
   - Run all unit tests: `ceedling test:all`
   - Run static analysis: `./lint.sh`
   - Build successfully on CI
   - Test on hardware if possible

2. **Update documentation:**
   - Update CHANGELOG.md
   - Update version references in README.md (if applicable)
   - Review and update DEPLOYMENT.md if needed

3. **Code review:**
   - Have changes reviewed by another developer
   - Address all review comments
   - Ensure CI passes on all branches

### Release Checklist

- [ ] All changes committed and pushed
- [ ] CHANGELOG.md updated with version and changes
- [ ] All tests passing on CI
- [ ] Static analysis clean (or issues justified)
- [ ] Documentation updated
- [ ] Version tag follows semantic versioning
- [ ] Tag annotation includes meaningful release notes

### After Releasing

1. **Announce the release:**
   - Post in discussions/announcements
   - Update any external documentation
   - Notify users via relevant channels

2. **Monitor for issues:**
   - Watch for bug reports
   - Be ready to create patch release if needed
   - Update documentation based on user feedback

## Release Artifacts

Each release includes:

### Firmware Binary (.hex)
- **File**: `PetSafe-CatFlap-v1.0.1.hex`
- **Format**: Intel HEX
- **Use**: Flash directly to PIC16F886
- **Size**: ~8-10 KB

### Debug Symbols (.elf)
- **File**: `PetSafe-CatFlap-v1.0.1.elf`
- **Format**: Executable and Linkable Format
- **Use**: Debugging with MPLAB X IDE
- **Contains**: Symbol table, debug info

### Checksums
- **File**: `checksums.txt`
- **Format**: SHA256 hashes
- **Use**: Verify file integrity
- **Example**:
  ```
  a1b2c3... PetSafe-CatFlap-v1.0.1.hex
  d4e5f6... PetSafe-CatFlap-v1.0.1.elf
  ```

## For Users (Downloading Releases)

### Finding Releases

1. Go to [Releases page](https://github.com/CJFWeatherhead/PetSafe-CatFlap/releases)
2. Latest release is shown at the top
3. Click on a release to see details and download assets

### Downloading Firmware

**Via Web Browser:**
1. Go to desired release
2. Scroll to "Assets" section
3. Click on the `.hex` file to download

**Via Command Line:**
```bash
# Replace VERSION with actual version (e.g., v1.0.1)
VERSION="v1.0.1"
wget https://github.com/CJFWeatherhead/PetSafe-CatFlap/releases/download/${VERSION}/PetSafe-CatFlap-${VERSION}.hex
```

### Verifying Downloads

Always verify checksums before flashing:

```bash
# Download checksum file
wget https://github.com/CJFWeatherhead/PetSafe-CatFlap/releases/download/v1.0.1/checksums.txt

# Verify (should output "OK")
sha256sum -c checksums.txt
```

## Release History

See [CHANGELOG.md](CHANGELOG.md) for complete version history and changes.

## Related Documentation

- [DEPLOYMENT.md](DEPLOYMENT.md) - Flashing firmware to hardware
- [DOCKER_BUILD.md](DOCKER_BUILD.md) - Docker build system
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contributing guidelines
- [TESTING.md](TESTING.md) - Testing procedures

---

**Questions or Issues?**

If you encounter problems with the release process:
1. Check workflow logs in the Actions tab
2. Review this documentation
3. Open an issue with details about the problem
4. Include workflow logs and error messages
