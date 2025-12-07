#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_rom_sys.h"   // esp_rom_delay_us

// XIAO ESP32-C3 pin mapping:
// D0 -> GPIO2
// D1 -> GPIO3
// D2 -> GPIO4
// D3 -> GPIO5

#define LED_GPIO     GPIO_NUM_4   // D2  - LED
#define BUZZER_GPIO  GPIO_NUM_5   // D3  - Buzzer
#define BUTTON_GPIO  GPIO_NUM_3   // D1  - Push button
#define DHT_GPIO     GPIO_NUM_2   // D0  - DHT11 data

// ------------ THRESHOLDS ------------
#define TEMP_LIMIT_C      31   // temperature alarm threshold
#define HUMIDITY_LOW_PCT  40   // humidity low threshold

static const char *TAG = "ROOM_REMINDER";

// ------------ SIMPLE DHT11 DRIVER ------------

static esp_err_t dht_wait_for_level(int expected_level, int timeout_us)
{
    int t = 0;
    while (gpio_get_level(DHT_GPIO) != expected_level) {
        if (t++ > timeout_us) {
            return ESP_ERR_TIMEOUT;
        }
        esp_rom_delay_us(1);
    }
    return ESP_OK;
}

static esp_err_t dht11_read_raw(uint8_t data[5])
{
    memset(data, 0, 5);

    // Start signal: MCU pulls line low for at least 18ms
    gpio_set_direction(DHT_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(20));  // 20ms
    gpio_set_level(DHT_GPIO, 1);
    esp_rom_delay_us(40);           // 20–40µs

    // Switch to input to let sensor respond
    gpio_set_direction(DHT_GPIO, GPIO_MODE_INPUT);

    // Sensor pulls low 80µs, then high 80µs
    if (dht_wait_for_level(0, 100) != ESP_OK) return ESP_ERR_TIMEOUT;
    if (dht_wait_for_level(1, 100) != ESP_OK) return ESP_ERR_TIMEOUT;

    // Read 40 bits (5 bytes)
    for (int i = 0; i < 40; i++) {
        // each bit: 50µs low, then high for 26-28µs (0) or ~70µs (1)
        if (dht_wait_for_level(0, 70) != ESP_OK) {
            return ESP_ERR_TIMEOUT;
        }

        // wait for line to go high (start of high pulse)
        if (dht_wait_for_level(1, 70) != ESP_OK) {
            return ESP_ERR_TIMEOUT;
        }

        // measure length of the high pulse
        int high_time = 0;
        while (gpio_get_level(DHT_GPIO) == 1) {
            if (high_time++ > 120) {
                break; // safety
            }
            esp_rom_delay_us(1);
        }

        uint8_t bit = (high_time > 40) ? 1 : 0; // >40µs -> '1'
        int byte_index = i / 8;
        data[byte_index] <<= 1;
        data[byte_index] |= bit;
    }

    // checksum
    uint8_t sum = data[0] + data[1] + data[2] + data[3];
    if (sum != data[4]) {
        return ESP_ERR_INVALID_CRC;
    }

    return ESP_OK;
}

static esp_err_t dht11_read(int *temperature_c, int *humidity_pct)
{
    uint8_t raw[5];
    esp_err_t err = dht11_read_raw(raw);
    if (err != ESP_OK) {
        return err;
    }

    // DHT11 format: [0]=RH int, [1]=RH decimal, [2]=T int, [3]=T decimal
    *humidity_pct   = raw[0];
    *temperature_c  = raw[2];
    return ESP_OK;
}

// ------------ GPIO INITIALISATION ------------

static void io_init(void)
{
    // LED + Buzzer as outputs
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LED_GPIO) | (1ULL << BUZZER_GPIO),
        .pull_down_en = 0,
        .pull_up_en   = 0,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Button as input with internal pull-up (pressed = 0)
    gpio_config_t btn_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .pull_down_en = 0,
        .pull_up_en   = 1,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&btn_conf);

    // DHT pin starts as input (we’ll switch to output during start signal)
    gpio_reset_pin(DHT_GPIO);
    gpio_set_direction(DHT_GPIO, GPIO_MODE_INPUT);
}

// ------------ APPLICATION LOGIC ------------

void app_main(void)
{
    io_init();
    ESP_LOGI(TAG, "Smart Room Comfort Reminder System started");

    int last_temp = 0;
    int last_hum  = 0;
    bool alarm_active = false;
    bool last_button_state = 1; // because of pull-up (1 = not pressed)

    int tick = 0;

    while (1) {
        // 1) Every 2 seconds: read DHT11
        if (tick % 20 == 0) {   // 20 * 100ms = 2000ms
            int t, h;
            esp_err_t err = dht11_read(&t, &h);
            if (err == ESP_OK) {
                last_temp = t;
                last_hum  = h;
                ESP_LOGI(TAG, "DHT11 OK - Temp: %d C, Humidity: %d %%", t, h);

                if (t > TEMP_LIMIT_C || h < HUMIDITY_LOW_PCT) {
                    alarm_active = true;
                    ESP_LOGW(TAG, "Comfort limit exceeded! Alarm ON");
                }
            } else {
                ESP_LOGE(TAG, "DHT11 read failed: %s", esp_err_to_name(err));
            }
        }

        // 2) Button handling (pressed = 0)
        int btn = gpio_get_level(BUTTON_GPIO);
        if (last_button_state == 1 && btn == 0) {
            // falling edge = just pressed
            alarm_active = false;          // silence alarm
            ESP_LOGI(TAG, "Button pressed: Alarm cleared");
        }
        last_button_state = btn;

        // 3) Alarm behaviour
        if (alarm_active) {
            // Blink LED and sound buzzer
            bool led_on = ((tick / 2) % 2) == 0; // toggle every 200ms
            gpio_set_level(LED_GPIO, led_on ? 1 : 0);
            gpio_set_level(BUZZER_GPIO, 1);
        } else {
            // Normal state: LED off, buzzer off
            gpio_set_level(LED_GPIO, 0);
            gpio_set_level(BUZZER_GPIO, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // loop every 100ms
        tick++;
    }
}
