#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/bootrom.h"

#define ERR_NO_ID      2
#define ERR_BAD_SIZE   3
#define OK_CLEANED     4
#define ERR_VERIFY     5

void blink_led(int times) {
#ifdef PICO_DEFAULT_LED_PIN
    for (int i = 0; i < times; ++i) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(150);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(150);
    }
#endif
}

static void finish(int code) {
    blink_led(code);
    sleep_ms(500);
    reset_usb_boot(0, 0);
}

int main() {
#ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

    uint8_t txbuf[4] = {0x9f};
    uint8_t rxbuf[4] = {0};
    uint32_t ints = save_and_disable_interrupts();
    flash_do_cmd(txbuf, rxbuf, 4);
    restore_interrupts(ints);

    uint8_t cap = rxbuf[3];
    bool id_empty = (rxbuf[0] == 0x00 || rxbuf[0] == 0xFF);
    if (id_empty || cap < 20 || cap > 31) {
        finish(ERR_NO_ID);
        return 0;
    }

    uint32_t flash_size_bytes = 1u << cap;
    if (flash_size_bytes < PICO_FLASH_SIZE_BYTES) {
        finish(ERR_BAD_SIZE);
        return 0;
    }
    if (flash_size_bytes > PICO_FLASH_SIZE_BYTES) {
        flash_size_bytes = PICO_FLASH_SIZE_BYTES;
    }

    static uint8_t eyecatcher[FLASH_PAGE_SIZE] = "RASPBERRY CLEANED";

    ints = save_and_disable_interrupts();
    flash_range_erase(0, flash_size_bytes);
    flash_range_program(0, eyecatcher, FLASH_PAGE_SIZE);
    restore_interrupts(ints);

    const uint8_t *flash = (const uint8_t *)XIP_BASE;
    bool ok = (memcmp(flash, eyecatcher, FLASH_PAGE_SIZE) == 0);
    for (uint32_t i = FLASH_PAGE_SIZE; ok && i < flash_size_bytes; ++i) {
        if (flash[i] != 0xFF) ok = false;
    }

    finish(ok ? OK_CLEANED : ERR_VERIFY);
    return 0;
}