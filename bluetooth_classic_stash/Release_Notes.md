# Release Notes

## Release Version: 1.0  
## Release Date: 2025-07-21

---

### Scope & Supported Features

This release focuses exclusively on non-connected states in Bluetooth Classic, providing comprehensive support for device discovery and connection establishment procedures. The system automatically triggers poll/null exchanges over ACL after successful page/page scan procedures, enabling seamless transition to data exchange.

| Feature/Enhancement           | Description                                                                                      |
|------------------------------|--------------------------------------------------------------------------------------------------|
| Inquiry                      | Device can perform inquiry to discover other Bluetooth devices.                                  |
| Inquiry Scan                  | Device can listen for inquiry requests from other devices.                                       |
| Page                         | Device can initiate a page to connect to known devices.                                          |
| Page Scan                    | Device can listen for page requests from other devices.                                          |
| ACL with Poll/Null exchanges | Asynchronous Connection-Less (ACL) link with poll/null packet exchanges supported.               |

---

### Release Deliverables

#### Firmware Files
- **Bit File**: `\\silabs.com\design\work\mcu40nm\ip_mcu_lpw\ghost_lpwh_fpga_ota_stub_se\output_324_TRUNK_1479\fpga_top\fpga\synth\vivado\build\fpga_top_elf.bit` - FPGA configuration file
- **Hex File**: `btc_lite_controller_app.hex` - BTC firmware file

#### Source Code

##### Repository 1: Bluetooth Classic
- **Repository**: `ssh://git@stash.silabs.com/btdm_audio/bluetooth_classic.git`
- **Commit ID**: `b29f88d59bc`

##### Repository 2: GSDK
- **Repository**: `ssh://git@stash.silabs.com/gsdk/gsdk.git`
- **Commit ID**: `712edcb652e5`
- **Note**: After cloning, run `git submodule update --init --recursive` to initialize submodules
- **Final Step**: Run `./tools/apply_submodule_overrides.sh` from `platform/radio/rail_lib/` directory

---

### Known Issues / Limitations

| Issue ID / Title                | Description / Impact                                                                                                         | Workaround / Status           |
|---------------------------------|-----------------------------------------------------------------------------------------------------------------------------|-------------------------------|
| Firmware reload                 | Firmware must be re-loaded for every new start procedure command.                                                           | Manual reload required        |
| Stop procedure                  | Stop procedure functionality not supported in this release.                                                                  | Not available                 |
| Channel limitation              | Hopping not enabled; all transmissions/receptions occur only on channel 0.                                                   | Use channel 0 only            |
| RAIL_LIB-13906                  | Intermittent decoding error: POLL packet not consistently recognized, causing LT address mismatch. | Known issue, may encounter randomly |

---

## Release Version: 1.1  
## Release Date: 2025-07-30

---

### Scope & Supported Features

**Functional scope remains identical to v1.0** - all non-connected state procedures supported.

**New in v1.1:** Added lightweight ring-buffer debug logger that captures HSS/LPW event callbacks, packet types (FHS, DM1/DH1), BD_ADDR/LT_ADDR details, and connection establishment flow without affecting radio timing.

---

### Release Deliverables

#### Firmware Files
- **Bit File**: `\\silabs.com\design\work\mcu40nm\ip_mcu_lpw\ghost_lpwh_fpga_ota_stub_se\output_324_TRUNK_1479\fpga_top\fpga\synth\vivado\build\fpga_top_elf.bit` - FPGA configuration file
- **Hex File**: `btc_lite_controller_app.hex` - BTC firmware file

#### Source Code

##### Repository 1: Bluetooth Classic
- **Repository**: `ssh://git@stash.silabs.com/btdm_audio/bluetooth_classic.git`
- **Commit ID**: `606c916f3af`

##### Repository 2: GSDK
- **Repository**: `ssh://git@stash.silabs.com/gsdk/gsdk.git`
- **Commit ID**: `8885f506e4362390bd`
- **Note**: After cloning, run `git submodule update --init --recursive` to initialize submodules
- **Final Step**: Run `./tools/apply_submodule_overrides.sh` from `platform/radio/rail_lib/` directory

---

### Known Issues / Limitations

This section remains same as version 1.0

---

## Release Version: 1.2  
## Release Date: 2025-08-05

---

### Scope & Supported Features

**Functional scope remains identical to v1.0** - all non-connected state procedures supported.

**New in v1.2:** Added the fix for inquiry scan faulty arguments in non-default callings

---

### Release Deliverables

#### Firmware Files
- **Bit File**: `\\silabs.com\design\work\mcu40nm\ip_mcu_lpw\ghost_lpwh_fpga_ota_stub_se\output_324_TRUNK_1479\fpga_top\fpga\synth\vivado\build\fpga_top_elf.bit` - FPGA configuration file
- **Hex File**: `btc_lite_controller_app.hex` - BTC firmware file

#### Source Code

##### Repository 1: Bluetooth Classic
- **Repository**: `ssh://git@stash.silabs.com/btdm_audio/bluetooth_classic.git`
- **Commit ID**: `000324703a7`

##### Repository 2: GSDK
- **Repository**: `ssh://git@stash.silabs.com/gsdk/gsdk.git`
- **Commit ID**: `8885f506e4362390bd`
- **Note**: After cloning, run `git submodule update --init --recursive` to initialize submodules
- **Final Step**: Run `./tools/apply_submodule_overrides.sh` from `platform/radio/rail_lib/` directory

---

### Known Issues / Limitations

This section remains same as version 1.0

---

## Release Version: 1.3  
## Release Date: 2025-08-13

---

### Scope & Supported Features

**Functional scope remains identical to v1.0** - all non-connected state procedures supported.

**New in v1.3:** Added RTT logger for LPW to HSS events. These events help in understanding the behaviour of each test/scenario.

---

### Release Deliverables

#### Firmware Files
- **Bit File**: `\\silabs.com\design\work\mcu40nm\ip_mcu_lpw\ghost_lpwh_fpga_ota_stub_se\output_324_TRUNK_1479\fpga_top\fpga\synth\vivado\build\fpga_top_elf.bit` - FPGA configuration file
- **Hex File**: `btc_lite_controller_app.hex` - BTC firmware file

#### Source Code

##### Repository 1: Bluetooth Classic
- **Repository**: `ssh://git@stash.silabs.com/btdm_audio/bluetooth_classic.git`
- **Commit ID**: `585b88f0b69`

##### Repository 2: GSDK
- **Repository**: `ssh://git@stash.silabs.com/gsdk/gsdk.git`
- **Commit ID**: `8885f506e4362390bd`
- **Note**: After cloning, run `git submodule update --init --recursive` to initialize submodules
- **Final Step**: Run `./tools/apply_submodule_overrides.sh` from `platform/radio/rail_lib/` directory

---

### Known Issues / Limitations

| Issue ID / Title                | Description / Impact                                                                                                         | Workaround / Status           |
|---------------------------------|-----------------------------------------------------------------------------------------------------------------------------|-------------------------------|
| Firmware reload                 | Firmware must be re-loaded for every new start procedure command.                                                           | Manual reload required        |
| Stop procedure                  | Stop procedure functionality not supported in this release.                                                                  | Not available                 |
| Channel limitation              | Hopping not enabled; all transmissions/receptions occur only on channel 0.                                                   | Use channel 0 only            |
| Page/Page Scan failures         | Sometimes randomly page and page scan are failing. Dev team is working on fixing the same.                                   | Under investigation           |

---

## Release Version: 1.4  
## Release Date: 2025-08-21

---

### Scope & Supported Features

**New in v1.4:** 
1. **Fixed BTC Channel Support**: Added `set_fixed_btc_channel` CLI command to set any BTC channel for all communications, allowing users to fix a specific channel instead of being limited to channel 0.
2. **Enhanced ACL Traffic Generation**: Added capability to create any kind of ACL traffic with different ACL packets up to 100 bytes of payload. This enables more comprehensive testing scenarios with various packet types and sizes.

---

### Release Deliverables

#### Firmware Files
- **Bit File**: `\\silabs.com\design\work\mcu40nm\ip_mcu_lpw\ghost_lpwh_fpga_ota_stub_se\output_324_TRUNK_1479\fpga_top\fpga\synth\vivado\build\fpga_top_elf.bit` - FPGA configuration file
- **Hex File**: `btc_lite_controller_app.hex` - BTC firmware file

#### Source Code

##### Repository 1: Bluetooth Classic
- **Repository**: `ssh://git@stash.silabs.com/btdm_audio/bluetooth_classic.git`
- **Commit ID**: `3d36281da32`

##### Repository 2: GSDK
- **Repository**: `ssh://git@stash.silabs.com/gsdk/gsdk.git`
- **Commit ID**: `8885f506e4362390bd`
- **Note**: After cloning, run `git submodule update --init --recursive` to initialize submodules
- **Final Step**: Run `./tools/apply_submodule_overrides.sh` from `platform/radio/rail_lib/` directory

---

### Known Issues / Limitations

| Issue ID / Title                | Description / Impact                                                                                                         | Workaround / Status           |
|---------------------------------|-----------------------------------------------------------------------------------------------------------------------------|-------------------------------|
| Firmware reload                 | Firmware must be re-loaded for every new start procedure command.                                                           | Manual reload required        |
| Stop procedure                  | Stop procedure functionality not supported in this release.                                                                  | Not available                 |
| ACL payload limitation          | ACL packets with payload size greater than 100 bytes are not supported due to known implementation limitation.                | Limit payload to 100 bytes   |
| Page/Page Scan failures         | Sometimes randomly page and page scan are failing. Dev team is working on fixing the same.                                   | Under investigation           |
| RSCDEV-39657                    | Support and test large data packets with RTT logging                                                                         | Under investigation           |
| RAIL_LIB-14194                  | Clarify and Optimize BTC Wrap-Around Handling Approach in RAIL_SEQ_BTC_ConvertBtcDate                                       | Under investigation           |
| RAIL_LIB-14223                  | Timing Deviation (~25-30us) in TX/RX Timing After Large 3DH5 Packet Exchange                                                | Under investigation           |

---

## Release Version: 2.0  
## Release Date: 2025-12-09

---

### Scope & Supported Features

**New in v2.0:**
1. **PRS Signals for Radio Activity Validation**: PRS signals are enabled to check the radio activity for page scan and inquiry scan validations as per the feature request from SQA team.
2. **Slow Frequency Hopping**: Enabled frequency hopping with slow BTC clock where 1 BTC slot is 1250 us instead of 625 us, implementing slow hopping functionality.

---

### Release Deliverables

#### Firmware Files
- **Bit File**: `\\silabs.com\design\work\mcu40nm\ip_mcu_lpw\ghost_lpwh_fpga_ota_stub_se\output_381_TRUNK_1798\fpga_top\fpga\synth\vivado\build\fpga_top_elf.bit` - FPGA configuration file
- **Hex File**: `btc_lite_controller_app.hex` - BTC firmware file

#### Source Code

##### Repository 1: Bluetooth Classic
- **Repository**: `ssh://git@stash.silabs.com/btdm_audio/bluetooth_classic.git`
- **Commit ID**: `beb32a41ef1`

##### Repository 2: GSDK
- **Repository**: `ssh://git@stash.silabs.com/gsdk/gsdk.git`
- **Commit ID**: `338830df0db438a04a6ce8e874d10c165ffb59de`
- **Note**: After cloning, run `git submodule update --init --recursive` to initialize submodules
- **Final Step**: Run `./tools/apply_submodule_overrides.sh` from `platform/radio/rail_lib/` directory

---

### Known Issues / Limitations

| Issue ID / Title                | Description / Impact                                                                                                         | Workaround / Status           |
|---------------------------------|-----------------------------------------------------------------------------------------------------------------------------|-------------------------------|
| Firmware reload                 | Firmware must be re-loaded for every new start procedure command.                                                           | Manual reload required        |
| Stop procedure                  | Stop procedure functionality not supported in this release.                                                                  | Not available                 |
| ACL payload limitation          | ACL packets with payload size greater than 500 bytes are not supported due to known implementation limitation.                | Limit payload to 500 bytes   |
| RAIL_LIB-14223                  | Timing Deviation (~25-30us) in TX/RX Timing After Large 3DH5 Packet Exchange                                                | Under investigation           |

---