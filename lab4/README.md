# RTOS Laboratory Work #4 - Variant 2

This project demonstrates synchronization between tasks using a **Binary Semaphore** in FreeRTOS.

## Student Information
* **Variant:** 2
* **Mechanism:** Binary Semaphore
* **Tasks:** 3 (Sensor A, Sensor B, Handler)
* **Platform:** FreeRTOS Windows Simulator (MSVC)

## Task Description

The goal is to simulate two sensors sending signals to a single handler. Since a Binary Semaphore can only hold a signal (taken/given) but not data, a shared variable is used to identify the sender.

1.  **Binary Semaphore (`xBinarySemaphore`):**
    * Created using `xSemaphoreCreateBinary()`.
    * Acts as a signaling flag.

2.  **Task: Sensor A:**
    * Runs every **500 ms**.
    * Sets global `SenderID = 1`.
    * Calls `xSemaphoreGive()` to signal an event.

3.  **Task: Sensor B:**
    * Runs every **900 ms**.
    * Sets global `SenderID = 2`.
    * Calls `xSemaphoreGive()` to signal an event.

4.  **Task: Handler:**
    * Waiting in Blocked state calling `xSemaphoreTake(..., portMAX_DELAY)`.
    * Unblocks immediately when any sensor gives the semaphore.
    * Reads the ID and prints which sensor triggered the alarm.

## How to Run
1.  Open `RTOSDemo.sln` in Visual Studio.
2.  Ensure `main.c` contains the Lab 4 (Variant 2) code.
3.  Exclude conflicting main files from the build.
4.  Build and Run (F5).

## Expected Output
The console will show mixed signals from Sensor A and Sensor B, processed by the Handler immediately after generation.

```text
[Sensor A] Signal Generated (500ms)
   >>> [Handler] CAUGHT SIGNAL from Sensor ID: 1 at Tick: 500
[Sensor B] Signal Generated (900ms)
   >>> [Handler] CAUGHT SIGNAL from Sensor ID: 2 at Tick: 900
[Sensor A] Signal Generated (500ms)
   >>> [Handler] CAUGHT SIGNAL from Sensor ID: 1 at Tick: 1000
...