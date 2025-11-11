Lab 1: Introduction to FreeRTOS

## Description

This lab covers the basics of FreeRTOS by creating, running, and monitoring three periodic tasks. The primary goals were:
1.  To create three periodic tasks ($\tau_1, \tau_2, \tau_3$) based on a calculated variant.
2.  To implement a "busy-wait" loop to simulate computational work ($C_i$).
3.  To log start and finish timestamps (in ticks) for each task.
4.  To observe and confirm priority-based preemption.

## Variant Details (ID = 2)

Based on the student ID `2`, the following parameters were calculated:

* **Prefix:** `VOME_`
* **Periods:**
    * T1 = 15 ms
    * T2 = 23 ms
    * T3 = 36 ms
* **Compute Times:**
    * C1 = 4 ms
    * C2 = 3 ms
    * C3 = 4 ms
* **Priorities:**
    * Rule: $\tau_1 > \tau_2 > \tau_3$ (since ID is even)
    * `SEIV_Task1`: Priority 3 (Highest)
    * `SEIV_Task2`: Priority 2
    * `SEIV_Task3`: Priority 1 (Lowest)

---

## Results

The program was successfully compiled and executed. The console log shows all three tasks running periodically according to their specified parameters.

The key result is the clear demonstration of **priority-based preemption**. The log shows that the higher-priority task (`SEIV_Task1`) correctly interrupts (preempts) the lower-priority task (`SEIV_Task2`) as soon as it becomes ready.

### Log Snippet: Preemption Example

This snippet shows the scheduler preempting `Task2` in favor of `Task1`.