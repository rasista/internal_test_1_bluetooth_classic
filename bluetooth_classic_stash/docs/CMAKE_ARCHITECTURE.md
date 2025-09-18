# Bluetooth Classic CMake Build System Architecture

## Overview
This document outlines the CMake-based build system architecture for the Bluetooth Classic project. The build system is organized hierarchically, with multiple interdependent components and auto-generation capabilities.

## Project Structure

### Root Project: btc_sdk_extension
The main project is defined in the root CMakeLists.txt and coordinates the build of all subcomponents.

**Key Components:**
- ULC (Upper Layer Controller)
- Common Utilities
- Scheduler
- RAIL Interface
- LLC (Link Layer Controller)
- RISCV Sequence Interface

## Component Dependencies

### 1. btc_hal (RAIL Interface)
**Purpose:** Provides interface between Bluetooth Classic and RAIL library

**Dependencies:**
- External:
  - RAIL Library (rail_lib)
  - SLCC Components
  - Device-specific components (sixg300)
- Internal:
  - btc_common (utilities)
  - btc_ulc (HCI and host interface)
  - btc_llc (Link Layer Control)
  - RISCV Sequence Interface

**Generated Files:**
```
- sl_btc_seq_interface.h
- procedure_role_autogen.h
- sl_structs_btc_seq_interface.h
```

### 2. btc_llc (Link Layer Controller)
**Purpose:** Implements the Bluetooth Classic Link Layer Control functionality

**Components:**
- Device Management
  - BTC Device
  - Memory Management
  - RAIL Interface
- Link Management
  - Inquiry/Page
  - ACL (Central/Peripheral)
  - eSCO (Central/Peripheral)
  - Role Switch
- RISCV Sequence Interface Integration

**Dependencies:**
- RAIL Interface
- Common Utilities
- RISCV Sequence Interface

**Build Configurations:**
- CROSS: For cross-compilation
- LLC_LIB_FPGA: For FPGA builds
- TEST_MODE support

### 3. riscvseq_interface
**Purpose:** Manages RISC-V sequencer for radio control

**Key Features:**
- RISC-V binary generation
- Sequencer interface compilation
- Address map generation
- Hardware abstraction

**Dependencies:**
- btc_llc
- RAIL RISC-V Interface Library
- Silicon Labs GSDK

**Generated Artifacts:**
```
- riscvseq_${RAIL_CHIP_NAME}.s37 (Binary)
- riscvseq_${RAIL_CHIP_NAME}.map (Memory map)
- generic_seq_btc.c (Generated C code)
- btc_riscvseq_address_map.h
```

### 4. btc_ulc (Upper Layer Controller)
**Purpose:** Implements the upper layer controller functionality

**Components:**
- HCI Layer
  - Command handlers
  - Event processing
  - Interface definitions
- Event Loop System
- Host Interface
- LMP (Link Management Protocol)

**Generated Files:**
```
- hci_intf_autogen.py
- hci_event_callback_autogen.py
- hci_lut_autogen.c
- hci_handlers_autogen.h
- Various HCI-related autogen files
```

### 5. btc_common (Utilities)
**Purpose:** Provides common utilities used across the project

**Key Components:**
- Debug utilities
- Queue management
- PKB management
- Platform dependencies
- JSON support (jansson)

### 6. btc_usched (Scheduler)
**Purpose:** Handles scheduling and timing management

**Features:**
- Event scheduling
- Timing management
- Role-based scheduling

## Build Configuration

### Toolchain Support
```cmake
# Supported toolchains
- ARM GCC (ToolchainARMGCC.cmake)
- RISCV (ToolchainRISCV.cmake)
```

### Build Types
- Debug
- Release

### Target Architecture Support
- Cortex-M33
- Cortex-M55

## Auto-Generation System

### Tools
- Jinja2 templating
- Python-based generators
- YAML configuration files

### Generated Components
1. Interface Files
2. Handlers
3. Look-up Tables
4. State Machines

## Build Targets

### Main Targets
```cmake
add_custom_target(btc_sdk_extension ALL
    DEPENDS btc_ulc btc_common btc_usched btc_hal
)
```

### Component-specific Targets
Each major component has its own build targets:
- `btc_ulc`: Upper Layer Controller library
- `btc_common`: Common utilities library
- `btc_usched`: Scheduler library
- `btc_hal`: RAIL interface library

## Build Process

1. **Configuration Phase**
   - Toolchain selection (ARM GCC/RISCV)
   - Architecture configuration
   - Component configuration
   - RISC-V sequencer setup

2. **Generation Phase**
   - Interface generation
   - Handler generation
   - LUT generation
   - RISC-V binary generation

3. **Compilation Phase**
   - Component compilation
   - Library creation
   - RISC-V sequencer compilation
   - Linking

4. **Installation Phase**
   - Binary installation
   - Header installation
   - RISC-V binary installation
   - Documentation installation

## Build Flow Dependencies
```
btc_sdk_extension
├── btc_ulc
│   └── btc_usched
├── btc_common
├── btc_hal
│   ├── btc_llc
│   └── riscvseq_interface
└── btc_llc
    └── riscvseq_interface
```

## Project Variables

### Path Variables
```cmake
${ULC} - Upper Layer Controller path
${UTILS} - Utilities path
${SCHEDULER} - Scheduler path
${RAIL_LIBINTF} - RAIL interface path
${CMAKE_CURRENT_BINARY_DIR} - Build directory
```

### Configuration Variables
```cmake
${CHIP_NAME} - Target chip name
${CHIP_CONFIG} - Chip configuration
${CMAKE_TOOLCHAIN_FILE} - Current toolchain file
```

## Best Practices

1. **Dependency Management**
   - Use explicit dependencies
   - Avoid circular dependencies
   - Use PRIVATE/PUBLIC/INTERFACE appropriately

2. **Auto-Generation**
   - Keep templates in dedicated directories
   - Use consistent naming conventions
   - Validate generated files

3. **Installation**
   - Install only necessary files
   - Maintain proper directory structure
   - Include version information

## Testing Integration

- Unit test framework support
- Integration test capabilities
- SimPy framework integration
- Test automation support

## Notes

1. **Version Control**
   - CMake minimum version: 3.25
   - Policy settings for modern CMake

2. **Performance**
   - Optimization flags (-Os)
   - Debug symbols (-g)
   - Architecture-specific optimizations

3. **Extensibility**
   - Modular design
   - Clear separation of concerns
   - Pluggable components