/*
 * FreeRTOS Lab #4 - Variant 2
 * Binary Semaphore Synchronization
 * Tasks: SensorA (500ms), SensorB (900ms), Handler
 */

/* --- INCLUDES --- */
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

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" /* Required for Semaphores */
#include "trcRecorder.h"

/* --- HEAP CONFIGURATION --- */
#define mainREGION_1_SIZE     82010
#define mainREGION_2_SIZE     239050
#define mainREGION_3_SIZE     168070

/* --- CONFIGURATION --- */
#define SENSOR_A_DELAY_MS   500
#define SENSOR_B_DELAY_MS   900

/* --- GLOBALS --- */
/* Головний об'єкт синхронізації - Бінарний Семафор */
SemaphoreHandle_t xBinarySemaphore;

/* Спільна змінна для передачі ID (оскільки бінарний семафор не передає дані) */
int Global_SenderID = 0; 

/* --- FUNCTION PROTOTYPES --- */
void vSensorA(void *pvParameters);
void vSensorB(void *pvParameters);
void vHandler(void *pvParameters);

/* System Prototypes */
void vAssertCalled( unsigned long ulLine, const char * const pcFileName );
static void prvInitialiseHeap( void );
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char * pcTaskName );
void vApplicationTickHook( void );
void vApplicationDaemonTaskStartupHook( void );

/* --- TASKS --- */

/* Sensor A: Fires every 500ms */
void vSensorA(void *pvParameters) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(SENSOR_A_DELAY_MS));

        /* Встановлюємо ID перед тим, як дати сигнал */
        taskENTER_CRITICAL(); 
        Global_SenderID = 1; // ID для Sensor A
        taskEXIT_CRITICAL();

        printf("[Sensor A] Signal Generated (500ms)\n");

        /* "Give" - піднімаємо прапорець семафора */
        xSemaphoreGive(xBinarySemaphore);
    }
}

/* Sensor B: Fires every 900ms */
void vSensorB(void *pvParameters) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(SENSOR_B_DELAY_MS));

        /* Встановлюємо ID перед тим, як дати сигнал */
        taskENTER_CRITICAL(); 
        Global_SenderID = 2; // ID для Sensor B
        taskEXIT_CRITICAL();

        printf("[Sensor B] Signal Generated (900ms)\n");

        /* "Give" - піднімаємо прапорець семафора */
        xSemaphoreGive(xBinarySemaphore);
    }
}

/* Handler: Waits for ANY signal */
void vHandler(void *pvParameters) {
    for (;;) {
        /* "Take" - чекаємо, поки семафор стане доступним.
           portMAX_DELAY означає чекати вічно, поки не прийде сигнал. */
        if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE) {
            
            int currentId;
            
            /* Читаємо ID */
            taskENTER_CRITICAL();
            currentId = Global_SenderID;
            taskEXIT_CRITICAL();

            printf("   >>> [Handler] CAUGHT SIGNAL from Sensor ID: %d at Tick: %u\n", 
                   currentId, xTaskGetTickCount());
        }
    }
}

/* --- MAIN --- */
int main( void )
{
    prvInitialiseHeap();
    vTraceEnable(TRC_START);

    printf("\nStarting Lab 4 (Variant 2: Binary Semaphore)...\n");

    /* 1. Створюємо бінарний семафор */
    xBinarySemaphore = xSemaphoreCreateBinary();

    if (xBinarySemaphore != NULL) {
        /* 2. Створюємо задачі */
        /* Handler має вищий пріоритет (2), щоб обробити сигнал миттєво */
        xTaskCreate(vHandler, "Handler", 1024, NULL, 2, NULL);
        
        /* Сенсори мають звичайний пріоритет (1) */
        xTaskCreate(vSensorA, "SensorA", 1024, NULL, 1, NULL);
        xTaskCreate(vSensorB, "SensorB", 1024, NULL, 1, NULL);

        /* 3. Запуск */
        vTaskStartScheduler();
    } else {
        printf("Failed to create semaphore.\n");
    }

    return 0;
}

/* --- STANDARD HOOKS --- */
void vApplicationMallocFailedHook( void ) { vAssertCalled( __LINE__, __FILE__ ); }
void vApplicationIdleHook( void ) { }
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char * pcTaskName ) { (void)pxTask; (void)pcTaskName; vAssertCalled( __LINE__, __FILE__ ); }
void vApplicationTickHook( void ) { }
void vApplicationDaemonTaskStartupHook( void ) { }

/* Allocations Helpers */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer, StackType_t ** ppxIdleTaskStackBuffer, configSTACK_DEPTH_TYPE * puxIdleTaskStackSize ) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer, StackType_t ** ppxTimerTaskStackBuffer, uint32_t * pulTimerTaskStackSize ) {
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName ) {
    printf( "ASSERT! Line %ld, file %s\r\n", ulLine, pcFileName );
    __debugbreak();
    for( ;; );
}

static void prvInitialiseHeap( void ) {
    static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
    const HeapRegion_t xHeapRegions[] = {
        { ucHeap + 1, mainREGION_1_SIZE },
        { ucHeap + 15 + mainREGION_1_SIZE, mainREGION_2_SIZE },
        { ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE, mainREGION_3_SIZE },
        { NULL, 0 }
    };
    vPortDefineHeapRegions( xHeapRegions );
}

/* Trace Recorder Helpers */
static uint32_t ulEntryTime = 0;
void vTraceTimerReset( void ) { ulEntryTime = xTaskGetTickCount(); }
uint32_t uiTraceTimerGetFrequency( void ) { return configTICK_RATE_HZ; }
uint32_t uiTraceTimerGetValue( void ) { return( xTaskGetTickCount() - ulEntryTime ); }