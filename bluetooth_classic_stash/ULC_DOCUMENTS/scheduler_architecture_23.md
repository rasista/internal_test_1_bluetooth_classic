# Bluetooth Classic Scheduler Architecture

## Overview

The Bluetooth Classic Scheduler (`src/lc/ulc/scheduler/`) is a sophisticated task scheduling system that manages the execution of various Bluetooth Classic procedures including ACL connections, Page/Inquiry operations, and scan procedures. It provides priority-based scheduling, resource management, and timing coordination for the Upper Layer Controller (ULC).

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Core Components](#core-components)
3. [Task Management](#task-management)
4. [Role-Based Scheduling](#role-based-scheduling)
5. [Priority System](#priority-system)
6. [Timing and Synchronization](#timing-and-synchronization)
7. [Event System](#event-system)
8. [API Interface](#api-interface)
9. [Configuration](#configuration)
10. [Performance Metrics](#performance-metrics)

## Architecture Overview

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    ULC Scheduler                            │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │   ACL       │  │   Page/     │  │   Scan      │         │
│  │ Scheduler   │  │  Inquiry    │  │ Scheduler   │         │
│  │             │  │ Scheduler   │  │             │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              Core Scheduler Engine                      │ │
│  │  • Task Queue Management                                │ │
│  │  • Priority Resolution                                  │ │
│  │  • Timing Coordination                                  │ │
│  │  • Event Handling                                       │ │
│  └─────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              Porting Layer                              │ │
│  │  • Hardware Abstraction                                 │ │
│  │  • Time Management                                      │ │
│  │  • Resource Management                                  │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Key Design Principles

- **Priority-Based Scheduling**: Tasks are scheduled based on priority levels
- **Time-Sensitive Operations**: Precise timing for Bluetooth slot operations (625μs)
- **Resource Management**: Efficient allocation of radio resources
- **Event-Driven Architecture**: Asynchronous event handling
- **Modular Design**: Separate schedulers for different Bluetooth procedures

## Core Components

### 1. Main Scheduler Engine (`usched.c/usched.h`)

**Purpose**: Core scheduling engine that manages task queues and execution.

**Key Structures**:
```c
typedef struct usch_Task {
    struct usch_Task *next;           // Linked list pointer
    uint32_t start;                   // Absolute start time
    exec_timing_t initTime;           // Initialization time
    uint32_t minRuntime;              // Minimum runtime
    uint32_t maxRuntime;              // Maximum runtime
    uint32_t flags;                   // Task configuration flags
    uint32_t priority;                // Task priority
    uint8_t task_id;                  // Unique task identifier
    uint32_t delay;                   // Task delay
    uint8_t status_flags;             // Status flags
    void (*handler)(uint8_t event, struct usch_Task *); // Event handler
    void *user_data;                  // User data pointer
} usch_Task_t;

struct usch_s {
    uint32_t flags;                   // Scheduler flags
    usch_Task_t *head;                // Task queue head
    usch_Task_t *waiting_queue_head;  // Waiting queue head
    usch_Task_t *scheduled;           // Currently scheduled task
    uint32_t endTime;                 // Current task end time
    struct {
        uint16_t tasks;               // Task counter
        uint16_t slips;               // Timing slip counter
        uint16_t failures;            // Failure counter
        uint16_t overdues;            // Overdue task counter
        uint16_t overlappings;        // Overlap counter
        uint16_t late_recovered;      // Late recovery counter
        uint16_t late_ends;           // Late end counter
    } counters;
};
```

**Key Functions**:
- `usch_AddTask()`: Add task to scheduler
- `usch_RemoveTask()`: Remove task from scheduler
- `usch_ScheduleProcess()`: Main scheduling process
- `find_white_spaces()`: Find available time slots

### 2. ACL Scheduler (`usched_acl.c/usched_acl.h`)

**Purpose**: Manages ACL (Asynchronous Connection-Less) connection scheduling.

**Key Features**:
- **Multi-Connection Support**: Up to 3 concurrent ACL connections
- **Priority Levels**: High priority (A2DP) and Low priority connections
- **Dynamic Runtime**: Adjustable runtime based on pending packets
- **Connection Types**: Central and Peripheral role support

**Key Structures**:
```c
typedef struct usched_acl_params_s {
    uint32_t start_priority;          // Starting priority
    uint32_t end_priority;            // Ending priority
    uint32_t priority_step_size;      // Priority step size
    uint32_t role;                    // ACL_CENTRAL or ACL_PERIPHERAL
    uint32_t min_runtime;             // Minimum runtime
    uint32_t max_runtime;             // Maximum runtime
    uint32_t duration;                // Connection duration
    uint32_t acl_priority;            // ACL_LOW_PRIORITY or ACL_HIGH_PRIORITY
    uint32_t conn_info_index;         // Connection info index
    uint32_t poll_interval;           // Poll interval
    uint32_t is_tx_pending;           // TX pending flag
    uint32_t is_rx_pending;           // RX pending flag
} usched_acl_params_t;
```

**Key Functions**:
- `acl_role_initialization()`: Initialize ACL role
- `acl_central_reschedule()`: Reschedule central ACL task
- `acl_peripheral_reschedule()`: Reschedule peripheral ACL task
- `get_max_runtime_based_on_pending_packets()`: Calculate runtime

### 3. Page/Inquiry Scheduler (`usched_pi.c/usched_pi.h`)

**Purpose**: Manages Page and Inquiry procedure scheduling.

**Key Features**:
- **Page Procedures**: Device discovery and connection establishment
- **Inquiry Procedures**: Device discovery without connection
- **Priority Management**: Dynamic priority adjustment
- **Timing Control**: Precise timing for Bluetooth procedures

**Key Functions**:
- `pi_role_initialization()`: Initialize Page/Inquiry role
- `pi_reschedule()`: Reschedule Page/Inquiry task
- `usched_pi_parameters_init()`: Initialize parameters

### 4. Scan Scheduler (`usched_pi_scan.c/usched_pi_scan.h`)

**Purpose**: Manages Page Scan and Inquiry Scan procedures.

**Key Features**:
- **Page Scan**: Listen for page requests
- **Inquiry Scan**: Listen for inquiry requests
- **Scan Windows**: Configurable scan windows and intervals
- **Power Management**: Efficient scan scheduling

### 5. Common Role Management (`usched_common_role.c/usched_common_role.h`)

**Purpose**: Provides common functionality for all role types.

**Key Structures**:
```c
typedef struct role_cb_s {
    uint32_t end_tsf;                 // End timestamp
    int16_t start_priority;           // Starting priority
    int16_t end_priority;             // Ending priority
    usch_Task_t task;                 // Associated task
    uint32_t priority_step_size;      // Priority step size
    usched_private_data_t usched_private_data; // Role-specific data
    uint32_t occupied;                // Occupation status
    void (*callback_function)(usch_Task_t *task, enum usch_taskEvent); // Callback
} role_cb_t;

typedef struct uscheduler_info_s {
    role_cb_t inquiry_role_cb;        // Inquiry role control block
    role_cb_t inquiry_scan_role_cb;   // Inquiry scan role control block
    role_cb_t page_role_cb;           // Page role control block
    role_cb_t page_scan_role_cb;      // Page scan role control block
    role_cb_t connection_role_cb[MAX_ACL_CONNECTIONS_SUPPORTED]; // ACL connections
} uscheduler_info_t;
```

## Task Management

### Task Lifecycle

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Created   │───▶│  Scheduled  │───▶│  Running    │───▶│  Completed  │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │                   │
       ▼                   ▼                   ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Pending   │    │   Waiting   │    │   Active    │    │   Finished  │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
```

### Task States

1. **Created**: Task is created but not yet scheduled
2. **Scheduled**: Task is added to the scheduler queue
3. **Waiting**: Task is waiting for its start time
4. **Running**: Task is currently executing
5. **Completed**: Task has finished execution

### Task Events

```c
enum usch_taskEvent {
    usch_taskEvent_Init,        // Task initialization
    usch_taskEvent_Stop,        // Task stop request
    usch_taskEvent_ScheduleFail // Scheduling failure
};
```

## Role-Based Scheduling

### Role Types

1. **Inquiry Role** (Priority: 245-200)
   - Device discovery without connection
   - High priority for initial discovery

2. **Page Role** (Priority: 205-170)
   - Connection establishment
   - Medium-high priority

3. **Inquiry Scan Role** (Priority: 240-210)
   - Listen for inquiry requests
   - High priority for responsiveness

4. **Page Scan Role** (Priority: 180-160)
   - Listen for page requests
   - Medium priority

5. **ACL Connection Roles** (Variable Priority)
   - Central/Peripheral connections
   - Dynamic priority based on connection type

### Role Management

```c
#define INQUIRY_ROLE 0
#define PAGE_ROLE 1
#define INQUIRY_SCAN_ROLE 2
#define PAGE_SCAN_ROLE 3
#define NON_ACL_ROLES_COUNT 4
#define MAX_ACL_CONNECTIONS_SUPPORTED 3
```

## Priority System

### Priority Levels

| Role Type | Start Priority | End Priority | Step Size |
|-----------|----------------|--------------|-----------|
| Inquiry | 245 | 200 | 5 |
| Inquiry Scan | 240 | 210 | 5 |
| Page | 205 | 170 | 5 |
| Page Scan | 180 | 160 | 5 |
| ACL (High) | Variable | Variable | Variable |
| ACL (Low) | Variable | Variable | Variable |

### Priority Adjustment

```c
uint32_t usched_increase_priority(uint32_t curr_priority, uint32_t end_priority, uint32_t step_size)
{
    curr_priority = curr_priority - step_size;
    if (curr_priority < end_priority) {
        curr_priority = end_priority;
    }
    return curr_priority;
}
```

### ACL Priority Types

```c
#define ACL_LOW_PRIORITY 0      // Standard data connections
#define ACL_HIGH_PRIORITY 1     // A2DP audio connections
```

## Timing and Synchronization

### Bluetooth Slot Timing

- **Slot Width**: 625μs (Bluetooth standard)
- **Multi-Slot Packets**: 1, 3, or 5 slots
- **Timing Precision**: Microsecond-level accuracy

### Runtime Configuration

```c
#define CONNECTED_NORMAL_PRIORITY_SLOTS (4 * 625)   // 4 slots
#define CONNECTED_HIGH_PRIORITY_SLOTS (10 * 625)    // 10 slots
#define INQUIRY_MAX_RUN_TIME (16 * 625)             // 16 slots
#define PAGE_MAX_RUN_TIME (16 * 625)                // 16 slots
```

### Time Management

```c
uint32_t find_white_spaces(uint32_t best_possible_start,
                           uint32_t worst_possible_end,
                           uint32_t min_duration,
                           uint32_t max_duration,
                           uint32_t overhead,
                           usch_Task_t *task);
```

## Event System

### Event Types

1. **Task Events**:
   - `usch_taskEvent_Init`: Task initialization
   - `usch_taskEvent_Stop`: Task stop
   - `usch_taskEvent_ScheduleFail`: Scheduling failure

2. **Scheduler Events**:
   - Task addition/removal
   - Priority changes
   - Timing updates

### Event Handling

```c
void usch_SendEvent(uint8_t event);
void task_fail(usch_Task_t *task, uint8_t cause);
```

## API Interface

### Configuration API (`usched_api.yaml`)

The scheduler provides a comprehensive API for:

1. **ACL Management**:
   - Start/stop ACL connections
   - Priority configuration
   - Runtime management

2. **Page/Inquiry Management**:
   - Start/stop procedures
   - Parameter configuration
   - Timing control

3. **Scan Management**:
   - Start/stop scans
   - Window configuration
   - Interval management

### API Structures

```c
// ACL Start Parameters
typedef struct {
    uint32_t start_priority;
    uint32_t priority_step_size;
    uint32_t end_priority;
    uint32_t role;                    // Central/Peripheral
    uint32_t min_runtime;
    uint32_t max_runtime;
    uint32_t duration;
    uint32_t acl_priority;            // High/Low priority
    uint32_t conn_info_index;
    uint32_t poll_interval;
    uint32_t is_tx_pending;
    uint32_t is_rx_pending;
} usched_api_start_acl_t;

// Page/Inquiry Start Parameters
typedef struct {
    uint32_t start_priority;
    uint32_t priority_step_size;
    uint32_t end_priority;
    uint32_t role;
    uint32_t min_runtime;
    uint32_t max_runtime;
    uint32_t pi_total_duration;
    uint32_t curr_priority;
    uint32_t end_tsf;
    uint32_t delay;
    uint8_t address[6];
    uint8_t mode;
    uint16_t clock_offset;
    uint8_t current_device_index;
} usched_api_start_pi_t;
```

## Configuration

### Build Configuration

- **CMakeLists.txt**: Build system configuration
- **configure.sh**: Configuration script
- **autogen/**: Auto-generated configuration files

### Runtime Configuration

- **Priority Levels**: Configurable per role type
- **Runtime Limits**: Adjustable based on requirements
- **Connection Limits**: Maximum concurrent connections
- **Timing Parameters**: Slot timing and intervals

## Performance Metrics

### Scheduler Counters

```c
struct {
    uint16_t tasks;           // Total tasks processed
    uint16_t slips;           // Timing slips
    uint16_t failures;        // Scheduling failures
    uint16_t overdues;        // Overdue tasks
    uint16_t overlappings;    // Task overlaps
    uint16_t late_recovered;  // Late recoveries
    uint16_t late_ends;       // Late task endings
} counters;
```

### Performance Monitoring

- **Task Execution Time**: Monitor task runtime
- **Scheduling Efficiency**: Track scheduling success rate
- **Resource Utilization**: Monitor resource usage
- **Timing Accuracy**: Track timing precision

## Key Features

### 1. Priority-Based Scheduling
- Dynamic priority adjustment
- Role-specific priority ranges
- Conflict resolution

### 2. Multi-Connection Support
- Up to 3 concurrent ACL connections
- Independent connection management
- Resource sharing

### 3. Timing Precision
- Microsecond-level timing accuracy
- Bluetooth slot alignment
- Multi-slot packet support

### 4. Event-Driven Architecture
- Asynchronous event handling
- Callback-based notifications
- State machine integration

### 5. Resource Management
- Efficient resource allocation
- Conflict detection and resolution
- Dynamic runtime adjustment

### 6. Fault Tolerance
- Error recovery mechanisms
- Graceful degradation
- Failure handling

## Integration Points

### 1. Lower Layer Controller (LLC)
- ACL peripheral/central integration
- Radio resource management
- Timing coordination

### 2. Host Interface
- HCI command processing
- Event generation
- Parameter configuration

### 3. Device Management
- Multi-device support
- Connection state management
- Resource allocation

## Future Enhancements

### 1. Enhanced Priority Management
- More granular priority levels
- Dynamic priority adjustment
- Quality of Service (QoS) support

### 2. Improved Resource Management
- Better resource utilization
- Advanced conflict resolution
- Power management integration

### 3. Performance Optimization
- Reduced scheduling overhead
- Improved timing accuracy
- Enhanced throughput

### 4. Extended Functionality
- Additional Bluetooth procedures
- Enhanced scan capabilities
- Advanced connection management

## Conclusion

The Bluetooth Classic Scheduler provides a robust, efficient, and scalable solution for managing Bluetooth Classic procedures. Its modular design, priority-based scheduling, and precise timing control make it well-suited for real-time Bluetooth applications requiring high performance and reliability.

The architecture supports multiple concurrent connections, provides comprehensive error handling, and offers extensive configuration options for various use cases. The event-driven design ensures responsive operation while maintaining efficient resource utilization.
