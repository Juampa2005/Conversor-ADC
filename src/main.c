#include "pico/stdlib.h"
#include "hardware/adc.h"

// ============================================================
// CONFIGURACIÓN
// ============================================================

#define ADC_PIN 26
#define ADC_INPUT 0

#define GREEN_LED 13
#define YELLOW_LED 14
#define RED_LED 15

#define VREF 3.3f
#define ADC_MAX 4095.0f

#define WINDOW_SIZE 8

#define WARNING_LEVEL 50.0f
#define ALARM_LEVEL 75.0f

#define SAMPLE_TIME_MS 250

// ============================================================
// PROMEDIO MÓVIL
// ============================================================

uint16_t window[WINDOW_SIZE];

int window_index = 0;
int window_count = 0;

float moving_average(uint16_t new_value)
{
    window[window_index] = new_value;

    window_index++;

    if (window_index >= WINDOW_SIZE)
    {
        window_index = 0;
    }

    if (window_count < WINDOW_SIZE)
    {
        window_count++;
    }

    uint32_t sum = 0;

    for (int i = 0; i < window_count; i++)
    {
        sum += window[i];
    }

    return (float)sum / window_count;
}

// ============================================================
// CLASIFICACIÓN
// ============================================================

int classify_level(float percent)
{
    if (percent >= ALARM_LEVEL)
    {
        return 2;
    }
    else if (percent >= WARNING_LEVEL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// ============================================================
// CONTROL DE LEDs
// ============================================================

void update_leds(int state)
{
    gpio_put(GREEN_LED, 0);
    gpio_put(YELLOW_LED, 0);
    gpio_put(RED_LED, 0);

    if (state == 0)
    {
        // NORMAL
        gpio_put(GREEN_LED, 1);
    }
    else if (state == 1)
    {
        // WARNING
        gpio_put(YELLOW_LED, 1);
    }
    else
    {
        // ALARMA
        gpio_put(RED_LED, 1);
    }
}

// ============================================================
// PROGRAMA PRINCIPAL
// ============================================================

int main()
{
    // ========================================================
    // ADC
    // ========================================================

    adc_init();

    // GP26 = ADC0
    adc_gpio_init(ADC_PIN);

    // Seleccionar ADC0
    adc_select_input(ADC_INPUT);

    // ========================================================
    // LEDs
    // ========================================================

    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);

    gpio_init(YELLOW_LED);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);

    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);

    // Apagar LEDs inicialmente
    gpio_put(GREEN_LED, 0);
    gpio_put(YELLOW_LED, 0);
    gpio_put(RED_LED, 0);

    // ========================================================
    // Inicializar promedio móvil
    // ========================================================

    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        window[i] = 0;
    }

    // ========================================================
    // BUCLE PRINCIPAL
    // ========================================================

    while (true)
    {
        // Leer ADC
        uint16_t raw = adc_read();

        // Aplicar promedio móvil
        float filtered = moving_average(raw);

        // Convertir a voltaje
        float voltage = filtered * VREF / ADC_MAX;

        // Convertir a porcentaje
        float percent = filtered * 100.0f / ADC_MAX;

        // Clasificar
        int state = classify_level(percent);

        // Actualizar LEDs
        update_leds(state);

        // Evitar warnings del compilador
        (void)raw;
        (void)filtered;
        (void)voltage;
        (void)percent;

        // Esperar 250 ms
        sleep_ms(SAMPLE_TIME_MS);
    }

    return 0;
}