/*
 * STM32 UART demo core logic.
 *
 * This file is intentionally hardware-light so it can be copied into a
 * CubeMX/HAL project. Replace the adapter functions at the bottom with real
 * HAL UART, ADC, GPIO and PWM calls for the selected STM32 board.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FRAME_BUFFER_SIZE 128
#define UART_LINE_SIZE 96

static uint32_t g_sequence = 0;
static uint32_t g_upload_period_ms = 20;
static uint32_t g_last_upload_ms = 0;

void board_init(void);
uint32_t millis(void);
float read_adc_voltage(void);
float read_temperature_celsius(void);
float read_potentiometer_percent(void);
void uart_send_text(const char* text);
void uart_start_receive_line(void);
bool uart_line_available(void);
void uart_read_line(char* line, size_t line_size);
void set_led(bool enabled);
void set_pwm_compare(int value);

static uint8_t protocol_xor_checksum(const char* payload)
{
    uint8_t checksum = 0;

    while (*payload != '\0') {
        checksum ^= (uint8_t)(*payload);
        ++payload;
    }

    return checksum;
}

static void build_data_frame(char* frame, size_t frame_size)
{
    char payload[FRAME_BUFFER_SIZE];
    const uint32_t now_ms = millis();
    const float adc_voltage = read_adc_voltage();
    const float temperature = read_temperature_celsius();
    const float potentiometer = read_potentiometer_percent();

    snprintf(
        payload,
        sizeof(payload),
        "DATA,%lu,%lu,%.2f,%.2f,%.2f",
        (unsigned long)g_sequence++,
        (unsigned long)now_ms,
        adc_voltage,
        temperature,
        potentiometer
    );

    snprintf(
        frame,
        frame_size,
        "$%s*%02X\r\n",
        payload,
        protocol_xor_checksum(payload)
    );
}

static bool parse_command_payload(const char* line, char* target, size_t target_size, int* value)
{
    if (line == NULL || target == NULL || value == NULL) {
        return false;
    }

    if (strncmp(line, "$CMD,", 5) != 0) {
        return false;
    }

    const char* checksum_separator = strchr(line, '*');
    if (checksum_separator == NULL) {
        return false;
    }

    const size_t payload_length = (size_t)(checksum_separator - (line + 1));
    if (payload_length == 0 || payload_length >= UART_LINE_SIZE) {
        return false;
    }

    char payload[UART_LINE_SIZE];
    memcpy(payload, line + 1, payload_length);
    payload[payload_length] = '\0';

    unsigned int expected_checksum = 0;
    if (sscanf(checksum_separator + 1, "%2x", &expected_checksum) != 1) {
        return false;
    }

    if (protocol_xor_checksum(payload) != (uint8_t)expected_checksum) {
        return false;
    }

    char command[8];
    char parsed_target[16];
    if (sscanf(payload, "%7[^,],%15[^,],%d", command, parsed_target, value) != 3) {
        return false;
    }

    if (strlen(parsed_target) + 1 > target_size) {
        return false;
    }

    strcpy(target, parsed_target);
    return strcmp(command, "CMD") == 0;
}

static void handle_command_line(const char* line)
{
    char target[16];
    int value = 0;

    if (!parse_command_payload(line, target, sizeof(target), &value)) {
        return;
    }

    if (strcmp(target, "LED") == 0) {
        set_led(value != 0);
    } else if (strcmp(target, "PWM") == 0) {
        set_pwm_compare(value);
    } else if (strcmp(target, "PERIOD") == 0 && value >= 5 && value <= 1000) {
        g_upload_period_ms = (uint32_t)value;
    }
}

int main(void)
{
    board_init();
    uart_start_receive_line();

    while (1) {
        const uint32_t now_ms = millis();

        if ((now_ms - g_last_upload_ms) >= g_upload_period_ms) {
            char frame[FRAME_BUFFER_SIZE];
            build_data_frame(frame, sizeof(frame));
            uart_send_text(frame);
            g_last_upload_ms = now_ms;
        }

        if (uart_line_available()) {
            char line[UART_LINE_SIZE];
            uart_read_line(line, sizeof(line));
            handle_command_line(line);
        }
    }
}

/* Hardware adapter functions.
 * Replace these stubs with real STM32 HAL calls in a CubeMX project.
 */

void board_init(void)
{
    /* HAL_Init(), SystemClock_Config(), MX_GPIO_Init(), MX_USARTx_UART_Init(), etc. */
}

uint32_t millis(void)
{
    /* return HAL_GetTick(); */
    return 0;
}

float read_adc_voltage(void)
{
    /* Convert ADC raw value to voltage. */
    return 3.30f;
}

float read_temperature_celsius(void)
{
    /* Read a sensor or simulate one during bench testing. */
    return 25.60f;
}

float read_potentiometer_percent(void)
{
    /* Convert ADC raw value to 0.0-100.0 percent. */
    return 60.20f;
}

void uart_send_text(const char* text)
{
    (void)text;
    /* HAL_UART_Transmit(&huartx, (uint8_t*)text, strlen(text), 100); */
}

void uart_start_receive_line(void)
{
    /* Start UART receive interrupt or DMA line buffer. */
}

bool uart_line_available(void)
{
    /* Return true after a complete CRLF-terminated command line is received. */
    return false;
}

void uart_read_line(char* line, size_t line_size)
{
    if (line_size > 0) {
        line[0] = '\0';
    }
}

void set_led(bool enabled)
{
    (void)enabled;
    /* HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET); */
}

void set_pwm_compare(int value)
{
    (void)value;
    /* __HAL_TIM_SET_COMPARE(&htimx, TIM_CHANNEL_1, value); */
}
