//===-------------------- pinin.c --------------*- C //-*-===//
// This is the platform interface between different HAL and the rest of the
// SW.
//
// This is NOT really a component in the sense that does not publish or
// subscribe any signal. For some obscure reasons to myself I decided to not
// consider this as a component.
//
// The functions herein defined can be called by periodically scheduled
// components or by ISR callback functions.
//
//
//===----------------------------------------------------------------------===//

#include "pinin.h"
#include "FreeRTOS.h"
#include "application_setup.h"
#include "semphr.h"
#include "serial_port/serial_port.h"
#include "usart.h"

// ADC PV complete
extern ADC_HandleTypeDef hadc1;
extern SemaphoreHandle_t xSemaphoreADC_PV;
uint8_t ADC_RESOLUTION_BITS;
float PV_PIN_VOLTAGE;
float ANALOG_IN_RESOLUTION;

void pinin_init() {
    switch (hadc1.Init.Resolution) {
        case ADC_RESOLUTION_6B:
            ADC_RESOLUTION_BITS = 6;
            break;
        case ADC_RESOLUTION_8B:
            ADC_RESOLUTION_BITS = 8;
            break;
        case ADC_RESOLUTION_10B:
            ADC_RESOLUTION_BITS = 10;
            break;
        case ADC_RESOLUTION_12B:
            ADC_RESOLUTION_BITS = 12;
            break;
    }
    /* ADC PV */
    xSemaphoreADC_PV = xSemaphoreCreateBinary();
    PV_PIN_VOLTAGE = 3300.0F; // [mV]
    ANALOG_IN_RESOLUTION =
        PV_PIN_VOLTAGE / (float)(1 << ADC_RESOLUTION_BITS); // [mV]
}

void pinin_pv(float *pPinVoltage, enum HAL_Function_Mode mode) {
    /* Return the pin voltage in the interval [0, PIN_VOLTAGE] [V]*/
    size_t analog_read = 0; // integer value

    /* size_t analog_read = readAnalog(PV); */
    switch (mode) {
        case POLLING:
            HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1, portMAX_DELAY);
            HAL_ADC_Stop(&hadc1);
            analog_read = HAL_ADC_GetValue(&hadc1);
            break;
        case INTERRUPT:
            break;
        case DMA:
            // The last number is the number of conversions.
            // If analog_read[3], then the last number shall be 3, i.e. 3
            // readings.
            HAL_ADC_Start_DMA(&hadc1, &analog_read, 1);
            xSemaphoreTake(xSemaphoreADC_PV, portMAX_DELAY); // Wait for ADC
                                                             // conversion end
            HAL_ADC_Stop_DMA(&hadc1);
            break;
    }
    *pPinVoltage = ANALOG_IN_RESOLUTION * (float)analog_read / 1000.0F;
}

void pinin_usart(uint8_t *pMessage, size_t len, enum HAL_Function_Mode mode) {
    HAL_StatusTypeDef status = HAL_OK;
    switch (mode) {
        case POLLING:
            // AT 9600 Baud we have 1 bytes/ms transfer rate. We add 5ms
            // spare.
            status = HAL_UART_Receive(&huart2, pMessage, len,
                    pdMS_TO_TICKS(MSG_LENGTH_MAX + 5));
            break;
        case INTERRUPT:
            status = HAL_UART_Receive_IT(&huart2, pMessage, len);
            break;
        case DMA:
            break;
    }
    if (status != HAL_OK) {
        // If something goes wrong something
    }
}
