/**
 * FreeRTOS LED Demo
 * 
 * One task flashes an LED at a rate specified by a value set in another task.
 * 
 * Date: December 4, 2020
 * Author: Shawn Hymel
 * License: 0BSD
 */

// Needed for atoi()
#include <stdlib.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Some string to print
const char msg[] = "Hello my name is Marta this is a longer line of text";

// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

//*****************************************************************************
// Tasks

// Task: Print to Serial Terminal with lower priority
void startTask1(void *parameter) {

  int msg_len = strlen(msg);

  // Print string to Terminal
  while (1) {
    Serial.println();
    for (int i = 0; i < msg_len; i++) {
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

  }

}

// Task: print to Serial Terminal with higher priority
void startTask2(void *parameter) {

  // Loop forever
  while (1) {
    Serial.print('*');
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//*****************************************************************************
// Main

void setup() {

  // Configure Serial
  Serial.begin(300);

  Serial.println();
  Serial.println("---FREERTOS Task Demo---");
  Serial.print("Setup and loop task running on core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority ");
  Serial.println(uxTaskPriorityGet(NULL));
  
  // Start blink task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
            startTask1,      // Function to be called
            "Task 1",   // Name of task
            1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,           // Parameter to pass
            1,              // Task priority
            &task_1,           // Task handle
            app_cpu);       // Run on one core for demo purposes (ESP32 only)
            
  // Start serial read task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
            startTask2,     // Function to be called
            "Task 2",  // Name of task
            1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,           // Parameter to pass
            2,              // Task priority (must be same to prevent lockup)
            &task_2,           // Task handle
            app_cpu);       // Run on one core for demo purposes (ESP32 only)

}

void loop() {
  for (int i = 0; i < 3; i++) {
    vTaskSuspend(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  if ( task_1 != NULL) {
    vTaskDelete(task_1);
    task_1 = NULL;
  }

}
