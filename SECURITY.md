# Security Policy

## Overview

This document outlines the security considerations and reporting procedures for the PetSafe Cat Flap alternative firmware project.

## Scope

This security policy covers:

- Firmware vulnerabilities that could affect device operation
- Issues that could compromise pet safety
- Hardware security concerns
- Data privacy considerations

## Security Considerations

### Physical Security

This firmware controls a physical pet access device. Security considerations include:

1. **Pet Safety**: The primary security concern is ensuring pets can safely enter and exit
2. **Access Control**: RFID authentication prevents unauthorized pet access
3. **Fail-Safe Operation**: Device behavior during power loss or malfunction

### Firmware Security

#### Current Security Features

- **RFID Authentication**: Only registered tags grant access
- **EEPROM Storage**: Pet tags stored persistently in microcontroller EEPROM
- **No Remote Access**: Device operates independently without network connectivity
- **Code Protection**: Can be enabled via configuration bits (currently disabled for development)

#### Known Limitations

⚠️ **Important Security Limitations**:

1. **No Encryption**: RFID tags are stored without encryption in EEPROM
2. **Physical Access**: Anyone with physical access can reprogram the device
3. **RFID Clone Risk**: Standard 134.2 kHz RFID tags can potentially be cloned
4. **No Authentication**: Serial interface has no authentication mechanism
5. **Code Protection Off**: Default configuration allows firmware read-back

### Data Privacy

**What data is stored**:
- RFID tag IDs (stored in EEPROM)
- Configuration parameters (light threshold, flap position settings)

**What data is NOT stored**:
- No personal information
- No network data (device is offline)
- No location tracking
- No usage logs beyond current session

### Hardware Security

#### Programming Interface Security

The **ICSP interface** provides direct access to:
- Firmware read/write
- EEPROM read/write (including stored pet tags)
- Configuration bits

**Mitigation**:
- Enable code protection after deployment (optional)
- Secure physical access to device
- Remove/disable ICSP header after programming (optional)

#### Serial Interface Security

The **UART serial interface** allows:
- Status monitoring
- Mode changes
- Configuration reading/writing

**Risks**:
- No authentication required
- Anyone with physical access can control device
- Could be used to disable security features

**Mitigation**:
- Disable serial interface in deployment (optional)
- Secure physical access to device
- Monitor for unauthorized modifications

## Threat Model

### Low-Risk Threats

1. **RFID Tag Loss**: Pet loses collar with RFID tag
   - **Impact**: Pet cannot enter through flap
   - **Mitigation**: Learn multiple backup tags

2. **Battery Drain**: Device loses power
   - **Impact**: Flap stops functioning
   - **Mitigation**: Provide alternative pet access, use reliable power

### Medium-Risk Threats

1. **RFID Tag Cloning**: Neighbor clones your pet's tag
   - **Impact**: Unauthorized pet access
   - **Mitigation**: Monitor for unknown pets, clear and re-learn tags periodically

2. **Unauthorized Reprogramming**: Someone reprograms your device
   - **Impact**: Changed behavior, potential pet lockout
   - **Mitigation**: Enable code protection, secure physical access

3. **Serial Interface Abuse**: Unauthorized mode changes via serial
   - **Impact**: Pet could be locked in/out
   - **Mitigation**: Disable serial in deployment, monitor device behavior

### High-Risk Threats

1. **Malicious Firmware**: Compromised firmware uploaded
   - **Impact**: Complete control loss, potential pet harm
   - **Mitigation**: Only use trusted firmware sources, verify code before programming

2. **Physical Tampering**: Device damaged or modified maliciously
   - **Impact**: Device failure, pet injury risk
   - **Mitigation**: Secure physical location, regular inspection

## Reporting a Vulnerability

### What to Report

Please report any security issues related to:

- **Pet Safety**: Issues that could harm pets
- **Firmware Vulnerabilities**: Bugs that could be exploited
- **Hardware Issues**: Physical security concerns
- **Documentation Errors**: Misleading safety information

### How to Report

**For security-sensitive issues**:

1. **DO NOT** open a public GitHub issue
2. **Contact the maintainer directly**:
   - Open a security advisory on GitHub (if available)
   - Or email the maintainer (if contact provided)
   - Include "SECURITY" in the subject line

**For non-sensitive issues**:
- Open a regular GitHub issue
- Tag with "security" label

### What to Include

When reporting, please provide:

```
## Description
Clear description of the vulnerability

## Impact
What could an attacker do?
What is the worst-case scenario?

## Affected Versions
Which firmware versions are affected?

## Steps to Reproduce
1. Step one
2. Step two
3. Expected result
4. Actual result

## Proposed Solution
If you have ideas for fixing it

## Additional Context
Hardware setup, screenshots, logs, etc.
```

### Response Timeline

- **Acknowledgment**: Within 5 business days
- **Initial Assessment**: Within 10 business days
- **Fix Timeline**: Depends on severity
  - Critical (pet safety): Immediate priority
  - High: Within 30 days
  - Medium: Within 90 days
  - Low: Next release cycle

## Security Best Practices

### For Users

When deploying this firmware:

1. **Test Thoroughly**:
   - Test all modes before deployment
   - Verify RFID reading reliability
   - Ensure latches operate correctly
   - Provide alternative pet access during testing

2. **Secure Physical Access**:
   - Install device in secure location
   - Monitor for unauthorized modifications
   - Regular visual inspections

3. **Enable Code Protection** (optional):
   - After programming, enable code protection in configuration bits
   - This prevents firmware read-back
   - Note: Makes debugging difficult

4. **Disable Unused Features**:
   - If not using serial interface, disconnect physically
   - Remove ICSP header after programming (if comfortable)

5. **Regular Maintenance**:
   - Check device operation weekly
   - Test emergency manual override
   - Keep backup tags

6. **Monitor Behavior**:
   - Watch for unexpected mode changes
   - Check for new/unknown pets gaining access
   - Verify logs if available

### For Developers

When contributing:

1. **Code Review**:
   - Review security implications of changes
   - Consider failure modes
   - Think about physical safety

2. **Input Validation**:
   - Validate all serial commands
   - Check bounds on array access
   - Verify sensor readings are in range

3. **Safe Defaults**:
   - Default to safe state on error
   - Ensure pets can exit in failure mode
   - Fail-open rather than fail-closed (when safe)

4. **Testing**:
   - Test edge cases
   - Test error handling
   - Test power loss scenarios
   - Test timing-critical sections

5. **Documentation**:
   - Document security implications
   - Warn about dangerous operations
   - Explain failure modes

## Disclosure Policy

### Responsible Disclosure

We follow responsible disclosure:

1. **Report received** → Acknowledged
2. **Vulnerability confirmed** → Working on fix
3. **Fix developed** → Tested
4. **Fix released** → Public disclosure
5. **Credit given** → Reporter acknowledged (if desired)

### Public Disclosure

After a fix is released:

- **Security advisory** published on GitHub
- **CHANGELOG** updated with security notice
- **README** updated if necessary
- **Users notified** via releases

## Supported Versions

Currently, we support:

| Version | Supported          |
| ------- | ------------------ |
| Latest commit | :white_check_mark: |
| Older commits | :x: |

**Recommendation**: Always use the latest version from the main branch.

## Security Checklist

Before deploying to production:

- [ ] Tested all modes thoroughly
- [ ] Verified RFID reading works reliably
- [ ] Ensured latches operate smoothly
- [ ] Tested emergency manual override
- [ ] Provided alternative pet access
- [ ] Secured physical access to device
- [ ] Considered enabling code protection
- [ ] Disabled unused features (serial, etc.)
- [ ] Documented any modifications made
- [ ] Informed household members of operation
- [ ] Prepared backup plan for device failure

## Additional Resources

- [DEPLOYMENT.md](DEPLOYMENT.md) - Deployment guide with safety information
- [README.md](README.md) - Project overview
- [CONTRIBUTING.md](CONTRIBUTING.md) - Development guidelines

## Legal Notice

This firmware is provided "AS IS" without warranty. See [LICENSE](LICENSE) for full disclaimer.

**You are responsible for**:
- Safe deployment
- Pet welfare
- Compliance with local regulations
- Any consequences of using this firmware

## Contact

For security concerns:
- Use GitHub Security Advisories (preferred)
- Open a GitHub issue (for non-sensitive matters)
- Check GitHub repository for maintainer contact information

---

**Last Updated**: December 2025

**Remember**: Pet safety is paramount. When in doubt, provide alternative access!
