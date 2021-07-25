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

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

//*****************************************************************************
// Tasks

// Task: Perform something
void startTask1(void *parameter) {
  
  // Print string to Terminal
  while (1) {
    int a = 1;
    int b[100];
    
    for (int i = 0; i < 10; i++) {
      b[i] = a + 1;
    }
    Serial.println(b[0]);

    Serial.print("High water mark (words): ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));

    Serial.print("Heap before malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());
    int *ptr = (int*)pvPortMalloc(1024 * sizeof(int));

    if (ptr == NULL) {
      Serial.println("Not enough heap.");
    }
    else { 
      for (int i = 0; i < 1024; i++){
        ptr[i] = 3;
      }
    }

    Serial.print("Heap after malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    vPortFree(ptr);
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  

}

//*****************************************************************************
// Main

void setup() {

  // Configure Serial
  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--FreeRTOS Memory Demo--");


  // Start blink task
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
            startTask1,      // Function to be called
            "Task 1",   // Name of task
            1500,           // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,           // Parameter to pass
            1,              // Task priority
            NULL,           // Task handle
            app_cpu);       // Run on one core for demo purposes (ESP32 only)
            
  vTaskDelete(NULL);
}

void loop() {
  

}
