#include <stdio.h>
#include <string.h> /* Added for strcmp() */
#include "FreeRTOS.h"
#include "task.h"

/* --- PARAMETERS FOR LAB 1 (ID=2) --- */
#define TASK1_PERIOD_MS     15
#define TASK2_PERIOD_MS     23
#define TASK3_PERIOD_MS     36

#define TASK1_COMPUTE_MS    4
#define TASK2_COMPUTE_MS    3
#define TASK3_COMPUTE_MS    4

/* Priorities (τ1 > τ2 > τ3, since ID=2 is even) */
#define TASK1_PRIORITY      ( tskIDLE_PRIORITY + 2 )
#define TASK2_PRIORITY      ( tskIDLE_PRIORITY + 2 )
#define TASK3_PRIORITY      ( tskIDLE_PRIORITY + 1 )

#define TASK_NAME_PREFIX "SEIV_"

/* --- FUNCTIONS FOR LAB 1 --- */
static void busy_wait_ms(uint32_t millis)
{

    volatile uint32_t dummy_result = 0;
    uint32_t loop_count = millis * 400000; 

    for (uint32_t i = 0; i < loop_count; i++)
    {
        dummy_result += i % 7 * 10 / 10; 
    }
}

static void vPeriodicTask( void *pvParameters ) {
    uint32_t compute_ms = (uint32_t)pvParameters;
    char *taskName = pcTaskGetName(NULL);
    TickType_t xLastWakeTime, xStartTime, xFinishTime, xPeriod;


    if (strcmp(taskName, TASK_NAME_PREFIX "Task1") == 0) {
        xPeriod = pdMS_TO_TICKS(TASK1_PERIOD_MS);
    } else if (strcmp(taskName, TASK_NAME_PREFIX "Task2") == 0) {
        xPeriod = pdMS_TO_TICKS(TASK2_PERIOD_MS);
    } else if (strcmp(taskName, TASK_NAME_PREFIX "Task3") == 0) {
        xPeriod = pdMS_TO_TICKS(TASK3_PERIOD_MS);
    } else {
        xPeriod = pdMS_TO_TICKS(100); 
    }

    xLastWakeTime = xTaskGetTickCount();
    for( ;; ) {
        xStartTime = xTaskGetTickCount();
        printf("%s | Start: %lu\n", taskName, xStartTime);
        busy_wait_ms(compute_ms);
        xFinishTime = xTaskGetTickCount();
        printf("%s | Finish: %lu (Duration: %lu ticks)\n", taskName, xFinishTime, xFinishTime - xStartTime);
        vTaskDelayUntil( &xLastWakeTime, xPeriod );
    }
}

void vStartLab1Tasks( void )
{
    printf( "Starting Lab 1 tasks...\n" );

    /* --- CREATE TASKS FOR LAB 1 --- */
    xTaskCreate( vPeriodicTask, TASK_NAME_PREFIX "Task1", configMINIMAL_STACK_SIZE, (void *)TASK1_COMPUTE_MS, TASK1_PRIORITY, NULL );
    xTaskCreate( vPeriodicTask, TASK_NAME_PREFIX "Task2", configMINIMAL_STACK_SIZE, (void *)TASK2_COMPUTE_MS, TASK2_PRIORITY, NULL );
    xTaskCreate( vPeriodicTask, TASK_NAME_PREFIX "Task3", configMINIMAL_STACK_SIZE, (void *)TASK3_COMPUTE_MS, TASK3_PRIORITY, NULL );
}