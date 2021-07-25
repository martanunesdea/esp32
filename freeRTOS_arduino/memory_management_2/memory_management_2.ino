/**
 * FreeRTOS LED Demo
 * 
 * Generate a stack overflow on purpose
 * 
 * Date: July 2021
 * Author: Marta Nunes de Abreu
 * Source: digikey
 * License: 0BSD
 */
#include <string>
// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;
static String my_string = "";

//*****************************************************************************
// Tasks

// Task: Perform something
void startTask1(void *parameter) {
  
  // Print string to Terminal

  Serial.print("High water mark (words): ");
  Serial.println(uxTaskGetStackHighWaterMark(NULL));

  Serial.print("Heap before malloc (bytes): ");
  Serial.println(xPortGetFreeHeapSize());
  char *ptr = (char*)pvPortMalloc(1024 * sizeof(char));

  if (ptr != NULL) {
    while(true) // remain here until told to break
    {
        Serial.println("Task 1: Enter some text:");
        my_string = Serial.readString();
        //Serial.println(my_string);
        if ( my_string.length() != 0)
        {
          break;
        }
      }
 
    }
    //Serial.println(*ptr);
    //my_string = ptr;

  Serial.print("Heap after malloc (bytes): ");
  Serial.println(xPortGetFreeHeapSize());

  vPortFree(ptr);
    
  vTaskDelay(100 / portTICK_PERIOD_MS);

}



// Task: print to Serial Terminal with higher priority
void startTask2(void *parameter) {

  // Loop forever
  while (1) {
    Serial.println("Task 2:");
    Serial.println(my_string);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//*****************************************************************************
// Main

void setup() {

  // Configure Serial
  Serial.begin(300);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--FreeRTOS Memory Demo--");


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
            5000,           // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,           // Parameter to pass
            2,              // Task priority
            &task_1,           // Task handle
            app_cpu);       // Run on one core for demo purposes (ESP32 only)
            
  // Start serial read task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
            startTask2,     // Function to be called
            "Task 2",  // Name of task
            1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,           // Parameter to pass
            1,              // Task priority (must be same to prevent lockup)
            &task_2,           // Task handle
            app_cpu);       // Run on one core for demo purposes (ESP32 only)

  vTaskDelete(NULL);
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
