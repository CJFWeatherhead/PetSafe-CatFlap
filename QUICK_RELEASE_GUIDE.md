# Quick Release Guide

This is a quick reference for creating releases. For detailed documentation, see [RELEASE.md](RELEASE.md).

## Creating a Release in 3 Steps

### 1. Update CHANGELOG.md

Add a new version section with your changes:

```markdown
## [1.0.1] - 2024-12-12

### Fixed
- Fixed RFID timeout bug
- Corrected light sensor calibration

### Added
- Improved error messages
```

Commit and push:
```bash
git add CHANGELOG.md
git commit -m "Prepare release v1.0.1"
git push origin main
```

### 2. Create and Push a Tag

```bash
# Create an annotated tag
git tag -a v1.0.1 -m "Release v1.0.1 - Bug fixes and improvements"

# Push the tag to GitHub
git push origin v1.0.1
```

### 3. Wait for Automatic Build

GitHub Actions will automatically:
- ✅ Build the firmware with Docker + XC8
- ✅ Create a GitHub Release
- ✅ Attach `.hex` and `.elf` files
- ✅ Generate SHA256 checksums
- ✅ Add release notes

**Check progress**: Go to [Actions tab](https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions) → "Create Release" workflow

## Version Numbering

Follow semantic versioning:

- `v1.0.0` → `v2.0.0` - **Major**: Breaking changes, incompatible hardware
- `v1.0.0` → `v1.1.0` - **Minor**: New features, backward-compatible
- `v1.0.0` → `v1.0.1` - **Patch**: Bug fixes only

## For Users: Downloading Releases

1. Go to [Releases page](https://github.com/CJFWeatherhead/PetSafe-CatFlap/releases)
2. Click on latest release
3. Download `PetSafe-CatFlap-v1.0.1.hex` from Assets
4. Verify checksum:
   ```bash
   sha256sum -c checksums.txt
   ```
5. Flash to PIC16F886 using MPLAB X IPE

## Troubleshooting

**Tag already exists?**
```bash
git tag -d v1.0.1                          # Delete local
git push origin :refs/tags/v1.0.1          # Delete remote
git tag -a v1.0.1 -m "Release v1.0.1"      # Recreate
git push origin v1.0.1                     # Push again
```

**Build failed?**
- Check [Actions tab](https://github.com/CJFWeatherhead/PetSafe-CatFlap/actions) logs
- Ensure all source files compile locally first
- Verify Docker and build.sh work correctly

**Release not created?**
- Verify tag format matches `v*.*.*` (e.g., v1.0.0)
- Check workflow permissions in repository settings
- Review workflow logs for errors

## Complete Documentation

For detailed information, see:
- [RELEASE.md](RELEASE.md) - Complete release process guide
- [DEPLOYMENT.md](DEPLOYMENT.md) - Firmware deployment instructions
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines
