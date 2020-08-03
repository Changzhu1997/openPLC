#include <Arduino.h>
#include <Wire.h>

#ifndef MS5611_H
#define MS5611_H

#define MS5611_ADDRESS (0x77)

#define MS5611_RESET (0x1E)

#define MS5611_DATA (0x00)

#define MS5611_PRESSURE_OSR_256 (0x40)
#define MS5611_PRESSURE_OSR_512 (0x42)
#define MS5611_PRESSURE_OSR_1024 (0x44)
#define MS5611_PRESSURE_OSR_2048 (0x46)
#define MS5611_PRESSURE_OSR_4096 (0x48)

#define MS5611_TEMP_OSR_256 (0x50)
#define MS5611_TEMP_OSR_512 (0x52)
#define MS5611_TEMP_OSR_1024 (0x54)
#define MS5611_TEMP_OSR_2048 (0x56)
#define MS5611_TEMP_OSR_4096 (0x58)

// 00 68 B4 DF BA F2 71 74 66 73 7D 2C 6C 50 A5 27
#define MS5611_PROM_START (0xA2)
#define MS5611_PROM_SIZE (6)

#define MS5611_DELAY (100)

typedef enum {
    MS5611_ERROR_OK             = 0x0000,
    MS5611_ERROR_READ           = 0x0001,
    MS5611_ERROR_PROM           = 0x0002,
    MS5611_ERROR_RAW_TEMP       = 0x0003,
    MS5611_ERROR_RAW_PRESSURE   = 0x0004,
    MS5611_ERROR_OUTOFRANGE     = 0x0005
} MS5611_Error;

class MS5611 {
    private:
        void write(int cmd);
        MS5611_Error read(int number, byte *data);
        MS5611_Error readPROM();

    public:
        MS5611();

        uint16_t prom[MS5611_PROM_SIZE];
        void reset();
        bool begin();
        MS5611_Error readRawTemp(uint32_t *temp);
        MS5611_Error readRawPressure(uint32_t *pressure);
        MS5611_Error readData(int32_t * temperature, int32_t * pressure);
};

#endif
