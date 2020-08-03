#include <Arduino.h>
#include <DueTimer.h>
#include "Modbus.h"
#include "ModbusSerial.h"
#include "MS5611.h"

/************************PINOUT CONFIGURATION*************************

Digital In: 22, 24, 26, 28, 30, 32, 34, 36,       (%IX100.0 - %IX100.7)
            38, 40, 42, 44, 46, 48, 50, 52,       (%IX101.0 - %IX101.7)
            14, 15, 16, 17, 18, 19, 20, 21        (%IX102.0 - %IX102.7)
			
Digital Out: 23, 25, 27, 29, 31, 33, 35, 37,      (%QX100.0 - %QX100.7)
             39, 41, 43, 45, 47, 49, 51, 53       (%QX101.0 - %QX101.7)

Analog In: A0, A1, A2, A3, A4, A5, A6, A7, A8,    (%IW100 - %IW107)
           A9, A10, A11, A12, A13, A14, A15       (%IW108 - %IW115)
		   
Analog Out: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,       (%QW100 - %QW107)
            12, 13                                (%QW108 - %QW111)
			
*********************************************************************/

// ModBus Port information
#define BAUD        115200
#define ID          0
#define TXPIN       -1
#define TIMER       3

#define PERIOD                      500000
#define PRESSURE_THRESHOLE_UPPER    102000
#define PRESSURE_THRESHOLE_LOWER    100250

#define NUM_DISCRETE_INPUT      24
#define NUM_INPUT_REGISTERS     16
#define NUM_COILS               16
#define NUM_HOLDING_REGISTERS   12

// Input Register
#define COUNTER_IR                      0
#define ERROR_CODE_IR                   1
#define PRESSURE_REG_HIGH               2
#define PRESSURE_REG_LOW                3
#define TEMP_IR                         4
#define DWT_CYCCNT_HIGH_IR              5
#define DWT_CYCCNT_LOW_IR               6
#define DWT_FOLDCNT_IR                  7
#define DWT_LSUCNT_IR                   8
#define DWT_SLEEPCNT_IR                 9
#define DWT_EXCCNT_IR                   10
#define DWT_CPICNT_IR                   11
#define PRESSURE_THRESHOLE_UPPER_IR     12
#define PRESSURE_THRESHOLE_LOWER_IR     13

// Holding Register
#define PRESSURE_THRESHOLE_UPPER_HR     0
#define PRESSURE_THRESHOLE_LOWER_HR     1

// Motor
#define MOTOR_PIN   23
#define MOTOR_COIL  0
#define MOTOR_ISTS  0


// register addresses
volatile uint32_t *DWT_CONTROL  = (uint32_t *)0xE0001000;   // set to allow read of registers
volatile uint32_t *DEMCR        = (uint32_t *)0xE000EDFC;   // initilize debug register to allow DWT
volatile uint32_t *DWT_CYCCNT   = (uint32_t *)0xE0001004;   // cycle count 
volatile uint32_t *DWT_FOLDCNT  = (uint32_t *)0xE0001018;   // fold count register    
volatile uint32_t *DWT_LSUCNT   = (uint32_t *)0xE0001014;   // load store u count register 
volatile uint32_t *DWT_SLEEPCNT = (uint32_t *)0xE0001010;   // sleep count register 
volatile uint32_t *DWT_EXCCNT   = (uint32_t *)0xE000100C;   // exception overhead count
volatile uint32_t *DWT_CPICNT   = (uint32_t *)0xE0001008;   // cpi count

typedef struct {
    int32_t pressure;
    int32_t temp;
    int32_t motor_state;
    MS5611_Error error_code;
} SensorData;


//Modbus Object
ModbusSerial modbus = ModbusSerial();

// MS5611 Object
MS5611 ms = MS5611();

// Timer Object
DueTimer timer = DueTimer(TIMER);

// used to indicate how long arduino runs well
uint16_t counter;
bool update_flag;
uint32_t pressure_threshold_upper = 100600;
uint32_t pressure_threshold_lower = 100400;


void setPressureThresholdUpper(uint32_t value) {
     value = value * 10;

     if (value <= PRESSURE_THRESHOLE_UPPER && value > pressure_threshold_lower) {
         pressure_threshold_upper = value;
     }
}

void setPressureThresholdLower(uint32_t value) {
     value = value * 10;

     if (value >= PRESSURE_THRESHOLE_LOWER && value < pressure_threshold_upper) {
         pressure_threshold_lower = value;
     }
}

void updateSensorData(SensorData *data) {
    // read sensor data
    data->error_code = ms.readData(&data->temp, &data->pressure);
    if (data->error_code) {
        // failed to read data
        // turn on the LED
        digitalWrite(LED_BUILTIN, HIGH);
        // clear counter
        counter = 0;

        ms.reset();
        
        return;
    }

    // read data successfully
    // turn the LED off
    digitalWrite(LED_BUILTIN, LOW);
}

void updateModbus(SensorData *data) {
    // write PLC registers
    modbus.Ists(MOTOR_ISTS, data->motor_state);
    modbus.Ireg(COUNTER_IR, counter);
    modbus.Ireg(ERROR_CODE_IR, (int16_t)data->error_code);
    modbus.Ireg(PRESSURE_REG_HIGH, (uint16_t)((data->pressure >> 16) & 0x0000FFFF));
    modbus.Ireg(PRESSURE_REG_LOW, (uint16_t)(data->pressure & 0x0000FFFF));
    modbus.Ireg(TEMP_IR, (int16_t)data->temp);

    modbus.Ireg(DWT_CYCCNT_HIGH_IR, (uint16_t)((*DWT_CYCCNT >> 16) & 0x0000FFFF));
    modbus.Ireg(DWT_CYCCNT_LOW_IR, (uint16_t)(*DWT_CYCCNT & 0x0000FFFF));
    modbus.Ireg(DWT_FOLDCNT_IR, (uint16_t)(*DWT_FOLDCNT & 0x0000FFFF));
    modbus.Ireg(DWT_LSUCNT_IR, (uint16_t)(*DWT_LSUCNT & 0x0000FFFF));
    modbus.Ireg(DWT_SLEEPCNT_IR, (uint16_t)(*DWT_SLEEPCNT & 0x0000FFFF));
    modbus.Ireg(DWT_EXCCNT_IR, (uint16_t)(*DWT_EXCCNT & 0x0000FFFF));
    modbus.Ireg(DWT_CPICNT_IR, (uint16_t)(*DWT_CPICNT & 0x0000FFFF));
    modbus.Ireg(PRESSURE_THRESHOLE_UPPER_IR, (uint16_t)(pressure_threshold_upper / 10));
    modbus.Ireg(PRESSURE_THRESHOLE_LOWER_IR, (uint16_t)(pressure_threshold_lower / 10));

    setPressureThresholdUpper(modbus.Hreg(PRESSURE_THRESHOLE_UPPER_HR));
    setPressureThresholdLower(modbus.Hreg(PRESSURE_THRESHOLE_LOWER_HR));
}

void timerHandler() {
    update_flag = true;

    // increase counter
    counter++;
}

void modbusSetup() {
    //Config Modbus Serial (port, speed, rs485 tx pin)
    modbus.config(&Serial, BAUD, TXPIN);
    
    //Set the Slave ID
    modbus.setSlaveId(ID);
    
    //Dummy modbus registers
    for (int i = 0; i < NUM_DISCRETE_INPUT; ++i) {
        modbus.addIsts(i);
    }
    for (int i = 0; i < NUM_INPUT_REGISTERS; ++i) {
        modbus.addIreg(i);
    }
    for (int i = 0; i < NUM_COILS; ++i) {
        modbus.addCoil(i);
    }
    for (int i = 0; i < NUM_HOLDING_REGISTERS; ++i) {
        modbus.addHreg(i);
    }
}

void timerSetup() {
    timer.attachInterrupt(timerHandler);
    timer.start(PERIOD);
}

void setup() {
    // start the sensor
    ms.begin();

    // setup modbus
    modbusSetup();
    
    // setup motor pin to output mode
    pinMode(MOTOR_PIN, OUTPUT);
    // make motor pin default LOW (pump off)
    digitalWrite(MOTOR_PIN, LOW);

    // setup builtin LED (using to indicate error on reading sensor data)
    pinMode(LED_BUILTIN, OUTPUT);

    // initialize registers to zero
    *DEMCR = *DEMCR | 0x01000000;
    *DWT_CYCCNT = 0;
    *DWT_FOLDCNT = 0;
    *DWT_LSUCNT = 0;
    *DWT_SLEEPCNT = 0;
    *DWT_EXCCNT = 0;
    *DWT_CPICNT = 0;

    // enable registers to be read
    *DWT_CONTROL = *DWT_CONTROL | 1;
    *DWT_CONTROL = *DWT_CONTROL | 0x00200000;
    *DWT_CONTROL = *DWT_CONTROL | 0x00100000;
    *DWT_CONTROL = *DWT_CONTROL | 0x00080000;
    *DWT_CONTROL = *DWT_CONTROL | 0x00040000;
    *DWT_CONTROL = *DWT_CONTROL | 0x00020000;

    // setup timer
    timerSetup();

    update_flag = false;
}

void loop() {
    static SensorData sensorData = {
        .pressure = 0,
        .temp = 0,
        .motor_state = LOW,
        .error_code = MS5611_ERROR_OK
    };

    // modbus task, need to run periodically
    modbus.task();

    if (update_flag) {
        updateSensorData(&sensorData);

        if (sensorData.error_code) {
            digitalWrite(MOTOR_PIN, LOW);
        }
        else {
            if (sensorData.pressure < pressure_threshold_lower) {
                digitalWrite(MOTOR_PIN, HIGH);
                sensorData.motor_state = HIGH;
            }
            else if (sensorData.pressure > pressure_threshold_upper) {
                digitalWrite(MOTOR_PIN, LOW);
                sensorData.motor_state = LOW;
            }
        }

        updateModbus(&sensorData);

        update_flag = false;
    }
}
