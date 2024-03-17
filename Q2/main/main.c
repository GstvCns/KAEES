#include <stdio.h>
#include <stdlib.h>
#include <esp_log.h>
#include "esp_intr_alloc.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define BUTTON GPIO_NUM_0
#define DEBUG

typedef enum states
{
    ON = 0,
    OFF,
    PROTECTED
} states_e;

states_e current_state = OFF;

const char TAG[] = "main";

uint64_t button_pressed_at;
uint64_t button_pressed_for_ms;

SemaphoreHandle_t button_semaphore = NULL;

void IRAM_ATTR button_isr_handler(void *args)
{
    if (current_state == PROTECTED)
        return;

    if (!gpio_get_level(BUTTON))
    {
        button_pressed_at = esp_timer_get_time();
        current_state = ON;
        xSemaphoreGiveFromISR(button_semaphore, NULL);
    }
    else
    {
        button_pressed_at = 0;
        current_state = PROTECTED;
    }
}

void app_main(void)
{
#ifdef DEBUG
    esp_log_level_set("*", ESP_LOG_INFO);
#endif

    button_semaphore = xSemaphoreCreateBinary();

    esp_rom_gpio_pad_select_gpio(BUTTON);
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_direction(BUTTON, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_set_intr_type(BUTTON, GPIO_INTR_ANYEDGE));
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_isr_handler_add(BUTTON, button_isr_handler, NULL));

    while (1)
    {
        switch (current_state)
        {
        case ON:
            button_pressed_for_ms = (esp_timer_get_time() - button_pressed_at) / 1000;
            ESP_LOGI(TAG, "current_state: ON (%lld ms)", button_pressed_for_ms);
            break;

        case OFF:
            ESP_LOGI(TAG, "current_state: OFF");
            xSemaphoreTake(button_semaphore, portMAX_DELAY);
            break;

        case PROTECTED:
            ESP_LOGI(TAG, "current_state: PROTECTED");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            current_state = OFF;
            break;

        default:
            ESP_LOGE(TAG, "INVALID STATUS");
            break;
        }
#ifdef DEBUG
        vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
    }
}
