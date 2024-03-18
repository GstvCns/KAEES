#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <esp_check.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

const char TAG[] = "main";

enum Command
{
    ADD = 0,
    SUBTRACT,
    CLEAR,
    UPDATE
};
typedef enum Command Command;

struct Message
{
    uint8_t preamble;
    uint8_t target;
    Command cmd;
    int value;
};
typedef struct Message Message;

void uart_event_task(void *pvParameters);
bool validate_packet(Message packet);
void execute_command(Command cmd, int value);

uint8_t current_id = 1;
int current_value = 0;

QueueHandle_t uart2_queue;
char serial_buffer[128];

void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_driver_install(UART_NUM_2, 1024, 1024, 20, &uart2_queue, 0);
    uart_param_config(UART_NUM_2, &uart_config);

    uart_set_pin(UART_NUM_2, GPIO_NUM_4, GPIO_NUM_5, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    xTaskCreatePinnedToCore(&uart_event_task, "uart_event_task", 2048, (void *)UART_NUM_2, 20, NULL, APP_CPU_NUM);
    while (1)
    {
        Message packet_out = {
            .preamble = 0xAA,
            .cmd = rand() % 4,
            .target = rand() % 4,
            .value = (rand() % 21 - 10)};
        ESP_LOGI(TAG, "TARGET_OUT: %d", packet_out.target);
        ESP_LOGI(TAG, "COMMAND_OUT: %d", packet_out.cmd);
        ESP_LOGI(TAG, "VALUE_OUT: %d", packet_out.value);
        ESP_LOGI(TAG, "current value: %d", current_value);

        uart_write_bytes(UART_NUM_2, &packet_out, sizeof(packet_out));
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    uart_port_t uartx = (uart_port_t)pvParameters;
    ESP_LOGI(TAG, "uart_event_task init");
    for (;;)
    {
        if (xQueueReceive(uart2_queue, (void *)&event, portMAX_DELAY))
        {
            switch (event.type)
            {
            case UART_DATA:
                Message packet_in;
                uart_read_bytes(uartx, &packet_in, event.size, portMAX_DELAY);
                if (validate_packet(packet_in))
                {
                    execute_command(packet_in.cmd, packet_in.value);
                    ESP_LOGI(TAG, "new current value: %d", current_value);
                }
                break;

            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                uart_flush_input(uartx);
                xQueueReset(uart2_queue);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                uart_flush_input(uartx);
                xQueueReset(uart2_queue);
                break;

            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

bool validate_packet(Message packet)
{
    if (packet.preamble != 0xAA)
    {
        ESP_LOGE(TAG, "PREAMBLE_IN: 0x%x", packet.preamble);
        return false;
    }
    if (packet.target != current_id)
    {
        ESP_LOGW(TAG, "TARGET_IN: %d", packet.target);
        return false;
    }
    return true;
}

void execute_command(Command cmd, int value)
{

    switch (cmd)
    {
    case ADD:
        ESP_LOGI(TAG, "adding: %d", value);
        current_value += value;
        break;

    case SUBTRACT:
        ESP_LOGI(TAG, "subtracting: %d", value);
        current_value -= value;
        break;

    case CLEAR:
        ESP_LOGI(TAG, "clearing");
        current_value = 0;
        break;

    case UPDATE:
        ESP_LOGI(TAG, "updating: %d", value);
        current_value = value;
        break;

    default:
        ESP_LOGE(TAG, "UNKNOWN COMMAND");
        break;
    }
}