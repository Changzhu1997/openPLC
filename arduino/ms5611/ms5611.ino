#include <Wire.h>

#define MS5611_ADDRESS (0x77)

#define RESET (0x1E)

#define ROW (0x00)

#define PRESSURE_OSR_256 (0x40)
#define PRESSURE_OSR_512 (0x42)
#define PRESSURE_OSR_1024 (0x44)
#define PRESSURE_OSR_2048 (0x46)
#define PRESSURE_OSR_4096 (0x48)

#define TEMP_OSR_256 (0x50)
#define TEMP_OSR_512 (0x52)
#define TEMP_OSR_1024 (0x54)
#define TEMP_OSR_2048 (0x56)
#define TEMP_OSR_4096 (0x58)

#define PROM_START (0xA0)
#define PROM_END (0xAE)

#define RELAY_PIN (3)

#define DELAY (100)


byte prom[(PROM_END - PROM_START) * 2];

void setup() {
  Wire.begin();
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  relaySetup();

  delay(1000);

  int error;

  ms5611Reset();

  delay(1000);

  error = ms5611ReadPROM();
  if (error != 0)
  {
    Serial.print("Error: ");
    Serial.println(error);
  }

  Serial.print("PROM: ");
  for (int cmd = PROM_START; cmd <= PROM_END; cmd += 2)
  {
    int offset = (cmd - PROM_START);
    Serial.print(prom[offset], HEX);
    Serial.print(" ");
    Serial.print(prom[offset + 1], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

void loop() {
  int error;
  int temp;
  int pressure;

  error = ms5611ReadTemp(&temp);
  if (error)
  {
    Serial.print("Error: ");
    Serial.println(error);
  }

  Serial.print("TEMP: ");
  Serial.print(temp, HEX);
  Serial.println("");

  error = ms5611ReadPresure(&pressure);
  if (error)
  {
    Serial.print("Error: ");
    Serial.println(error);
  }

  Serial.print("Pressure: ");
  Serial.print(pressure, HEX);
  Serial.println("");

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  delay(1000);
}

void relaySetup()
{
  pinMode(RELAY_PIN, OUTPUT);
}

void ms5611Write(int cmd)
{
  Wire.beginTransmission(MS5611_ADDRESS);
  Wire.write(cmd);
  Wire.endTransmission();

  delay(DELAY);
}

int ms5611Read(int number, byte *data)
{
  Wire.requestFrom(MS5611_ADDRESS, number);

  if (number != Wire.available())
  {
    Serial.print("Number = ");
    Serial.print(number);
    Serial.print(" Available = ");
    Serial.println(Wire.available());
    return -1;
  }

  while (int offset = Wire.available())
  {
    data[number - offset] = Wire.read();
  }

  return 0;
}

void ms5611Reset()
{
  ms5611Write(RESET);
}

int ms5611ReadPROM()
{
  int error;
  for (int cmd = PROM_START; cmd <= PROM_END; cmd += 2)
  {
    ms5611Write(cmd);
    error = ms5611Read(2, prom + cmd - PROM_START);
    if (error != 0)
    {
      return error;
    }
  }
  return 0;
}

int ms5611ReadTemp(int *temp)
{
  int error;
  byte row[3];

  *temp = 0;

  ms5611Write(TEMP_OSR_4096);
  delay(DELAY);

  ms5611Write(ROW);
  error = ms5611Read(3, row);
  //  Serial.println(row[0], HEX);
  //  Serial.println(row[1], HEX);
  //  Serial.println(row[2], HEX);
  if (error != 0)
  {
    return error;
  }

  for (int i = 0; i < 3; i++)
  {
    *temp <<= 8;
    *temp |= row[i];
  }

  return 0;
}

int ms5611ReadPresure(int *presure)
{
  int error;
  byte row[3];

  *presure = 0;

  ms5611Write(PRESSURE_OSR_4096);
  delay(DELAY);

  ms5611Write(ROW);
  error = ms5611Read(3, row);
  //  Serial.println(row[0], HEX);
  //  Serial.println(row[1], HEX);
  //  Serial.println(row[2], HEX);
  if (error != 0)
  {
    return error;
  }

  for (int i = 0; i < 3; i++)
  {
    *presure <<= 8;
    *presure |= row[i];
  }

  return 0;
}
