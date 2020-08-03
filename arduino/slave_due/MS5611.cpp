#include "MS5611.h"

MS5611::MS5611() {
}

void MS5611::write(int cmd) {
    // write command to the sensor through I2C
    Wire.beginTransmission(MS5611_ADDRESS);
    Wire.write(cmd);
    Wire.endTransmission();

    delay(MS5611_DELAY);
}

MS5611_Error MS5611::read(int number, uint8_t *data) {
    // read `number` byte(s) from the sensor through I2C
    Wire.requestFrom(MS5611_ADDRESS, number);

    if (number != Wire.available()) {
        return MS5611_ERROR_READ;
    }

    while (Wire.available()) {
        data[number - Wire.available()] = Wire.read();
    }

    return MS5611_ERROR_OK;
}

void MS5611::reset() {
    // reset the sensor
    this->write(MS5611_RESET);
    delay(MS5611_DELAY);
}

MS5611_Error MS5611::readPROM() {
  // read PROM of the sensor (12 bytes)
    for (int offset = 0; offset < MS5611_PROM_SIZE; offset++) {
        uint8_t data[2];

        this->write(offset * 2 + MS5611_PROM_START);
        MS5611_Error error = this->read(2, data);
        if (error == MS5611_ERROR_READ) {
        return MS5611_ERROR_PROM;
        }

        prom[offset] = ((uint16_t)data[0] << 8 | (uint16_t)data[1]);
    }
    return MS5611_ERROR_OK;
}

bool MS5611::begin() {
    // initialize the sensor
    Wire.begin();

    this->reset();
    MS5611_Error error = this->readPROM();
    if (error) {
        return false;
    }
    return true;
}

MS5611_Error MS5611::readRawTemp(uint32_t *temp) {
    // read raw temperature data
    byte row[3];

    *temp = 0;

    this->write(MS5611_TEMP_OSR_4096);
    delay(MS5611_DELAY);

    this->write(MS5611_DATA);
    MS5611_Error error = this->read(3, row);
    if (error == MS5611_ERROR_READ) {
        return MS5611_ERROR_RAW_TEMP;
    }

    for (int i = 0; i < 3; i++) {
        *temp <<= 8;
        *temp |= row[i];
    }

    return MS5611_ERROR_OK;
}

MS5611_Error MS5611::readRawPressure(uint32_t *presure) {
    byte row[3];

    *presure = 0;

    this->write(MS5611_PRESSURE_OSR_4096);
    delay(MS5611_DELAY);

    this->write(MS5611_DATA);
    MS5611_Error error = this->read(3, row);
    if (error == MS5611_ERROR_READ) {
        return MS5611_ERROR_RAW_PRESSURE;
    }

    for (int i = 0; i < 3; i++) {
        *presure <<= 8;
        *presure |= row[i];
    }

    return MS5611_ERROR_OK;
}

MS5611_Error MS5611::readData(int32_t * temperature, int32_t * pressure) {
    MS5611_Error error;
    uint32_t raw_pressure;
    uint32_t raw_temp;

    error = this->readRawTemp(&raw_temp);
    if (error) {
        return error;
    }

    error = this->readRawPressure(&raw_pressure);
    if (error) {
        return error;
    }

    // difference between actual and reference temp
    int64_t dT = raw_temp - (uint32_t)prom[4] * (1 << 8);
    // actual temp
    int64_t temp = 2000 + dT * prom[5] / (1 << 23);
    int64_t temp2 = 0;
    
    // temp compensated pressure
    // prom data needs to be converted to 64 bits for preserving precision
    int64_t off = (uint64_t)prom[1] * (1 << 16) + (uint64_t)prom[3] * dT / (1 << 7);
    int64_t sens = (uint64_t)prom[0] * (1 << 15) + (uint64_t)prom[2] * dT / (1 << 8);

    int64_t off2 = 0;
    int64_t sens2 = 0;

    // temp compensation
    if(temp < 2000) {
        temp2 = dT * dT / (1 << 31);
        off2 = 5 * ((temp - 2000) * (temp - 2000)) / 2;
        sens2 = 5 * ((temp - 2000) * (temp - 2000)) / 4;  
    }
    else if (temp < -1500) {
        // TODO: 
    }

    off -= off2;
    sens -= sens2;

    *temperature = (int32_t)(temp - temp2);
    *pressure = (int32_t)((raw_pressure * sens / (1 << 21) - off) / (1 << 15));

    if (*pressure < 1000 || *pressure > 120000) {
        return MS5611_ERROR_OUTOFRANGE;
    }

    return MS5611_ERROR_OK;
}
