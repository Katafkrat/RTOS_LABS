/*
 * FreeRTOS Lab #3 - Variant 2 (Final Fixed Version)
 * Tasks: Producer, Consumer, Monitor
 * Queue Data: Struct {float temp, uint32_t id}
 */

 /* --- 1. INCLUDES --- */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <stdint.h> 

/* Visual studio intrinsics */
#include <intrin.h>

#ifdef WIN32_LEAN_AND_MEAN
#include "winsock2.h"
#else
#include <winsock.h>
#endif

/* FreeRTOS kernel includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "trcRecorder.h"

/* --- 2. DEFINES & CONSTANTS (CRITICAL FOR HEAP) --- */
/* Ці рядки виправляють помилку 'undeclared identifier' */
#define mainREGION_1_SIZE     82010
#define mainREGION_2_SIZE     239050
#define mainREGION_3_SIZE     168070

/* Lab 3 Settings */
#define QUEUE_LEN       10      
#define PRODUCER_DELAY  200     
#define CONSUMER_DELAY  100     
#define MONITOR_DELAY   1000    

/* --- 3. DATA STRUCTURES --- */
typedef struct {
    float temp;
    uint32_t id;
} sensor_data_t;

/* --- 4. GLOBAL VARIABLES --- */
QueueHandle_t xQueueData;

/* --- 5. FUNCTION PROTOTYPES --- */
void vProducer(void* arg);
void vConsumer(void* arg);
void vMonitor(void* arg);

/* System Prototypes */
void vAssertCalled(unsigned long ulLine, const char* const pcFileName);
static void prvInitialiseHeap(void);
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName);
void vApplicationTickHook(void);
void vApplicationDaemonTaskStartupHook(void);

/* --- 6. TASKS IMPLEMENTATION --- */

/* Producer: Sends data (Non-blocking) */
void vProducer(void* arg) {
    sensor_data_t data;
    data.id = 0;
    data.temp = 20.0f;

    for (;;) {
        data.id++;
        data.temp += 0.5f;
        if (data.temp > 50.0f) data.temp = 20.0f;

        /* Timeout = 0 (Non-blocking) */
        if (xQueueSend(xQueueData, &data, 0) == pdPASS) {
            taskENTER_CRITICAL();
            printf("[Producer] Sent ID: %u, Temp: %.2f\n", data.id, data.temp);
            taskEXIT_CRITICAL();
        }
        else {
            taskENTER_CRITICAL();
            printf("[Producer] Queue FULL! Data dropped (ID: %u)\n", data.id);
            taskEXIT_CRITICAL();
        }

        vTaskDelay(pdMS_TO_TICKS(PRODUCER_DELAY));
    }
}

/* Consumer: Receives data (Timeout 500ms) */
void vConsumer(void* arg) {
    sensor_data_t receivedData;

    for (;;) {
        /* Timeout = 500ms */
        if (xQueueReceive(xQueueData, &receivedData, pdMS_TO_TICKS(500)) == pdPASS) {
            taskENTER_CRITICAL();
            printf("   [Consumer] Processed ID: %u, Temp: %.2f (Tick: %u)\n",
                receivedData.id, receivedData.temp, xTaskGetTickCount());
            taskEXIT_CRITICAL();
        }
        else {
            taskENTER_CRITICAL();
            printf("   [Consumer] Timeout waiting for data...\n");
            taskEXIT_CRITICAL();
        }

        vTaskDelay(pdMS_TO_TICKS(CONSUMER_DELAY));
    }
}

/* Monitor: Checks queue status */
void vMonitor(void* arg) {
    UBaseType_t messagesWaiting;

    for (;;) {
        messagesWaiting = uxQueueMessagesWaiting(xQueueData);

        taskENTER_CRITICAL();
        printf("====== [Monitor] Messages in Queue: %lu / %d ======\n",
            messagesWaiting, QUEUE_LEN);
        taskEXIT_CRITICAL();

        vTaskDelay(pdMS_TO_TICKS(MONITOR_DELAY));
    }
}

/* --- 7. MAIN --- */
int main(void)
{
    /* Initialize Heap first */
    prvInitialiseHeap();

    /* Initialize Trace Recorder (Optional) */
    vTraceEnable(TRC_START);

    /* Create Queue */
    xQueueData = xQueueCreate(QUEUE_LEN, sizeof(sensor_data_t));

    if (xQueueData != NULL) {
        printf("\nStarting Lab 3 (Variant 2)...\n");

        /* Create Tasks */
        xTaskCreate(vProducer, "Producer", 1024, NULL, 1, NULL);
        xTaskCreate(vConsumer, "Consumer", 1024, NULL, 1, NULL);
        xTaskCreate(vMonitor, "Monitor", 1024, NULL, 2, NULL);

        /* Start Scheduler */
        vTaskStartScheduler();
    }
    else {
        printf("Failed to create queue.\n");
    }

    return 0;
}

/* --- 8. HOOKS & HELPERS --- */

void vApplicationMallocFailedHook(void) { vAssertCalled(__LINE__, __FILE__); }
void vApplicationIdleHook(void) {}
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName) { (void)pxTask; (void)pcTaskName; vAssertCalled(__LINE__, __FILE__); }
void vApplicationTickHook(void) {}
void vApplicationDaemonTaskStartupHook(void) {}

/* Static Allocation Helpers */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, configSTACK_DEPTH_TYPE* puxIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize) {
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vAssertCalled(unsigned long ulLine, const char* const pcFileName) {
    printf("ASSERT! Line %ld, file %s\r\n", ulLine, pcFileName);
    __debugbreak();
    for (;; );
}

/* Heap Initialization (Uses defines from Section 2) */
static void prvInitialiseHeap(void) {
    static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
    const HeapRegion_t xHeapRegions[] = {
        { ucHeap + 1, mainREGION_1_SIZE },
        { ucHeap + 15 + mainREGION_1_SIZE, mainREGION_2_SIZE },
        { ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE, mainREGION_3_SIZE },
        { NULL, 0 }
    };
    vPortDefineHeapRegions(xHeapRegions);
}

/* Trace Recorder Timers */
static uint32_t ulEntryTime = 0;
void vTraceTimerReset(void) { ulEntryTime = xTaskGetTickCount(); }
uint32_t uiTraceTimerGetFrequency(void) { return configTICK_RATE_HZ; }
uint32_t uiTraceTimerGetValue(void) { return(xTaskGetTickCount() - ulEntryTime); }