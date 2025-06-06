
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
// configUSE_PREEMPTION 1

TaskHandle_t NextTaskHandle = NULL;
TaskHandle_t LEDGTaskHandle,LEDOTaskHandle,LEDRTaskHandle,ButtonTaskHandle; // LED GREEN,ORANGE & RED Task Handle
int main(void)
{
    // system init
    BaseType_t Status;
    Status=xTaskCreate(LEDGTaskHandler,"LED-G",200,NULL,2,&LEDGTaskHandle);
    configASSERD(Status == pdPASS);
    NextTaskHandle = LEDGTaskHandle;
    Status=xTaskCreate(LEDOTaskHandler,"LED-O",200,NULL,2,&LEDOTaskHandle);
    configASSERD(Status == pdPASS);
    Status=xTaskCreate(LEDRTaskHandler,"LED-R",200,NULL,2,&LEDRTaskHandle);
    configASSERD(Status==pdPASS);
    
    Status=xTaskCreate(ButtonTaskHandler,"BUTT-T",100,NULL,3,&ButtonTaskHandle);
    configASSERD(Status==pdPASS);
    vTaskStartScheduler();
    while(1)
    {
        
    }
}

static void LEDGTaskHandler(const void* Parameter)
{
    BaseType_t Status;
    while(1)
    {
        LED_Toggle(PIN_G);
        Status = xTaskNotifyWait(0,0,NULL,pdMS_TO_TICKS(1000));
        if(Status == pdTRUE)
        {
            vTaskSuspendAll(); // temperarlly stop context switching
            NextTaskHandle = LEDOTaskHandle; // this is shared resource so need to protect it
            vTaskResumeAll();
            LED_PIN_Set(PIN_G);
            vTaskDelete(NULL); // for self delete function we use NULL
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void LEDOTaskHandler(const void* Parameter)
{
   BaseType_t Status;
   while(1)
   {
       LED_Toggle(PIN_O);
       Status = xTaskNotifyWait(0,0,NULL,pdMS_TO_TICKS(1000));
       if(Status == pdTRUE)
       {
           vTaskSuspendAll();
           NextTaskHandle = LEDRTaskHandle;
           vTaskResumeAll();
           LED_PIN_Set(PIN_O);
           vTaskDelete(NULL);
       }
       vTaskDelay(pdMS_TO_TICKS(800));
   }
}

static void LEDRTaskHandler(const void* Parameter)
{
    BaseType_t Status;
   while(1)
   {
     LED_Toggle(PIN_R);
     Status = xTaskNotifyWait(0,0,NULL,pdMS_TO_TICKS(1000));
       if(Status == pdTRUE)
       {
           NextTaskHandle = NULL;
           LED_PIN_Set(PIN_R);
           vTaskDelete(NULL);
           vTaskDelete(ButtonTaskHandle);
       }
     vTaskDelay(pdMS_TO_TICKS(400));

   }       
}

static void ButtonTaskHandler(const void* Parameter)
{
    BaseType_t  ReadStatus=0,PrVRead=0;
    while(1)
    {
        ReadStatus = GPIO_PIN_Read(PIN_BUTT);
        if( (ReadStatus) && (!PrVRead) )
        {
           xTaskNotify(NextTaskHandle,0,eNoAction);
        }
        PrVRead = ReadStatus;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
