# RTOS Laboratory Work #3 - Variant 2

This project demonstrates inter-task communication in FreeRTOS using Queues.

## Student Information
* **Variant:** 2
* **Number of Tasks:** 3 (Producer, Consumer, Monitor)
* **Platform:** FreeRTOS Windows Simulator (MSVC)

## Task Description
1.  **Queue Configuration:**
    * Type: Structure `sensor_data_t` containing `float temp` and `uint32_t id`.
    * Length: 10 items.

2.  **Producer Task:**
    * Generates simulated sensor data.
    * Sends data to the queue with **0 timeout** (non-blocking).
    * If the queue is full, the data is dropped, and a warning is printed.

3.  **Consumer Task:**
    * Waits for data from the queue with a **500ms timeout**.
    * Prints received data or a timeout message if no data arrives.

4.  **Monitor Task (Additional Task):**
    * Periodically (every 1000ms) checks the status of the queue using `uxQueueMessagesWaiting()`.
    * Displays the number of messages currently waiting in the queue.

## How to Run
1.  Open `RTOSDemo.sln` in Visual Studio.
2.  Ensure `main.c` contains the Variant 2 code (Lab 3).
3.  Exclude other main files (like `main_blinky.c`, `main_full.c`, or Lab 2 files) from the build.
4.  Build and Run (F5).

## Expected Output
Console will show:
- Producer sending IDs and Temps.
- Consumer receiving and processing them.
- Monitor periodically reporting queue usage (e.g., "Messages in Queue: 0 / 10" or higher if Consumer is slow).
- "Queue FULL" messages if Producer is faster than Consumer (controlled by delays).