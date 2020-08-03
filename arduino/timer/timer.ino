#include <Arduino.h>
#include <DueTimer.h>


#define TIMER   0
#define PERIOD  500000

DueTimer timer = DueTimer(TIMER);

void timerHandler() {
    Serial.println("Handler.");
}

void timerSetup() {
    timer.attachInterrupt(timerHandler);
    timer.start(500000);
}

void setup() {

    Serial.begin(115200);

    timerSetup();
}

void loop() {
    Serial.println("Loop.");
    delay(500);
}
