# RTOS Laboratory Work #2: Thread Local Storage (TLS)

This project demonstrates the use of **Thread Local Storage (TLS)** in FreeRTOS to create thread-safe tasks without using global variables. Each task maintains its own unique state and configuration using TLS pointers.

## Description

The goal of this laboratory work is to:
1.  Learn how to use TLS slots (`vTaskSetThreadLocalStoragePointer`, `xTaskGetThreadLocalStoragePointer`).
2.  Implement thread-safe modules where each task has its own context and execution profile.
3.  Avoid race conditions by eliminating shared global variables for task state.
4.  Implement a "Burst" logging mechanism to reduce console I/O overhead.

## Variant Details (Variant 2)

* **Student Variant:** 2
* **Total Tasks:** 5
* **Execution Mode:** `Delay`
* **Burst Size:** 2 (Logs are printed in batches of 2 lines)

### Task Configuration

| Parameter | Value | Description |
| :--- | :--- | :--- |
| **TLS Index 0** | `task_context_t` | Stores dynamic state (counters, checksums, buffers). |
| **TLS Index 1** | `profile_t` | Stores static configuration (mode, delays). |
| **Base Delay** | 110 ms | Starting delay for Task 0. |
| **Step** | 7 ms | Increment of delay for each subsequent task. |
| **Iterations** | 52 | Calculated as $N_i = 10 \times N_{tasks} + IDX$ ($10 \times 5 + 2$). |

## Implementation Details

### 1. Data Structures (TLS)
Instead of global variables, the following structures are allocated dynamically for each task:
* **Context (V1):** Contains the task name, current iteration `iter`, CRC8 `checksum`, and a string buffer `line1` for logging.
* **Profile (V2):** Contains execution settings like `baseDelayMs` and `burstMode`.

### 2. Logic Flow
1.  **Initialization:** The task allocates memory for Context and Profile, initializes them, and saves pointers to TLS slots.
2.  **Loop:**
    * Increments iteration counter.
    * Updates CRC8 Checksum based on iteration and a unique seed.
    * Writes a log message to the internal buffer.
    * **Burst Check:** If the buffer has 2 lines, it flushes (prints) them to the console.
    * **Delay:** Waits for `110 + (7 * TaskID)` ms.
3.  **Cleanup:** Frees allocated memory and deletes the task upon completion.

## Results

The application successfully creates 5 independent tasks that run in parallel.
* **Thread Safety:** Each task calculates its own unique checksum and maintains its own iteration counter correctly, proving that TLS isolates data effectively.
* **Timing:** Tasks wake up at different intervals due to the calculated delay (`110ms`, `117ms`, `124ms`, etc.).

### Expected Log Output
```text
[Task T02_0] Tick: 110 Iter: 1 Sum: 0x3B
[Task T02_0] Tick: 220 Iter: 2 Sum: 0xA2
[Task T02_1] Tick: 117 Iter: 1 Sum: 0xCC
...
Task finished 0
Task finished 1