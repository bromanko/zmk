#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/keymap.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#if DT_NODE_HAS_STATUS(LED1_NODE, okay)
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
#else
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

#if DT_NODE_HAS_STATUS(LED2_NODE, okay)
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
#else
#error "Unsupported board: led2 devicetree alias is not defined"
#endif

#if DT_NODE_HAS_STATUS(LED3_NODE, okay)
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
#else
#error "Unsupported board: led3 devicetree alias is not defined"
#endif

static void led_pin_init(const struct gpio_dt_spec *gpio) {
    if (!device_is_ready(gpio->port)) {
        LOG_ERR("LED GPIO device not ready");
        return;
    }
    gpio_pin_configure_dt(gpio, GPIO_OUTPUT_ACTIVE | gpio->dt_flags);
    gpio_pin_set_dt(gpio, 0);
}

static void led_pin_set(const struct gpio_dt_spec *gpio, int value) {
    gpio_pin_set_dt(gpio, value);
}

static int led_event_handler(const zmk_event_t *eh) {
    const uint8_t layer_active = zmk_keymap_highest_layer_active();
    led_pin_set(&led3, layer_active == 3 /* Nav */);
    led_pin_set(&led2, layer_active == 7 /* Symbol */);
    led_pin_set(&led1, layer_active == 6 /* Num */);

    return 0;
}

static int led_init(const struct device *port) {
    led_pin_init(&led1);
    led_pin_init(&led2);
    led_pin_init(&led3);

    return 0;
}

ZMK_LISTENER(led, led_event_handler);

#ifdef CONFIG_ZMK_SPLIT_BLE_ROLE_CENTRAL
ZMK_SUBSCRIPTION(led, zmk_layer_state_changed);
#endif

SYS_INIT(led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
