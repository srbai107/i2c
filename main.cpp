#include "pico/stdlib.h"
#include "hardware/i2c.h"

// DS1307 I2C address
#define DS1307_ADDRESS 0x68

// Structure to hold time and date
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} rtc_time_t;

// Static variable to store I2C instance
static i2c_inst_t *ds1307_i2c;

// Convert decimal to BCD
static uint8_t dec_to_bcd(uint8_t val) {
    return ((val / 10 * 16) + (val % 10));
}

// Convert BCD to decimal
static uint8_t bcd_to_dec(uint8_t val) {
    return ((val / 16 * 10) + (val % 16));
}

// Initialize DS1307 RTC
void ds1307_init(i2c_inst_t *i2c, uint sda, uint scl) {
    ds1307_i2c = i2c;
    i2c_init(ds1307_i2c, 100 * 1000); // 100 kHz I2C clock

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);
}

// Set time to DS1307
void ds1307_set_time(const rtc_time_t *time) {
    uint8_t buf[8];
    buf[0] = 0x00; // Start register
    buf[1] = dec_to_bcd(time->second);
    buf[2] = dec_to_bcd(time->minute);
    buf[3] = dec_to_bcd(time->hour);
    buf[4] = dec_to_bcd(time->day);
    buf[5] = dec_to_bcd(time->date);
    buf[6] = dec_to_bcd(time->month);
    buf[7] = dec_to_bcd(time->year);

    i2c_write_blocking(ds1307_i2c, DS1307_ADDRESS, buf, 8, false);
}

// Get time from DS1307
void ds1307_get_time(rtc_time_t *time) {
    uint8_t reg = 0x00;
    uint8_t buf[7];

    i2c_write_blocking(ds1307_i2c, DS1307_ADDRESS, &reg, 1, true);
    i2c_read_blocking(ds1307_i2c, DS1307_ADDRESS, buf, 7, false);

    time->second = bcd_to_dec(buf[0] & 0x7F);
    time->minute = bcd_to_dec(buf[1]);
    time->hour   = bcd_to_dec(buf[2] & 0x3F);
    time->day    = bcd_to_dec(buf[3]);
    time->date   = bcd_to_dec(buf[4]);
    time->month  = bcd_to_dec(buf[5]);
    time->year   = bcd_to_dec(buf[6]);
}
ds1037
