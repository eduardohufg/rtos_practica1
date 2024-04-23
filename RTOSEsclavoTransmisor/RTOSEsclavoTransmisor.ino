#include <Arduino_FreeRTOS.h>
#include <Keypad.h>
#include <DHT11.h>
#include <Wire.h>
#include <semphr.h>

const byte numRows = 4;
const byte numCols = 4;
char keymap[numRows][numCols] = {{'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}};
byte rowPins[numRows] = {22, 24, 26, 28};
byte colPins[numCols] = {30, 32, 34, 36};
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

DHT11 dht11(2); // Conexión digital del sensor con la placa Arduino
#define TriggerPin 13
#define EchoPin 12

SemaphoreHandle_t xSemaphore;
char keypressed;
int temp = 0;
int distancia = 0;
long tiempo;

void setup() {
  Serial.begin(9600);
  Wire.begin(23); // Dirección I2C del esclavo
  Wire.onRequest(I2C); // Registro del evento onRequest
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  xSemaphore = xSemaphoreCreateMutex();

  xTaskCreate(Teclado, "Teclado", 128, NULL, 1, NULL);
  xTaskCreate(Temperatura, "Temperatura", 128, NULL, 3, NULL);
  xTaskCreate(Distancia, "Distancia", 128, NULL, 2, NULL);

  vTaskStartScheduler();
  for (;;);
}

void loop() {}

void I2C() {
  Wire.write(keypressed);
  Wire.write((byte)(temp & 0xFF));
  Wire.write((byte)((temp >> 8) & 0xFF));
  Wire.write((byte)(distancia & 0xFF));
  Wire.write((byte)((distancia >> 8) & 0xFF));
}

void Teclado(void *pvParameters) {
  for (;;) {
    char key = myKeypad.getKey();
    if (key) {
      xSemaphoreTake(xSemaphore, portMAX_DELAY);
      keypressed = key;
      xSemaphoreGive(xSemaphore);
      Serial.print("Caracter: ");
      Serial.println(key);
    }
  }
}

void Temperatura(void *pvParameters) {
  for (;;) {
    int readTemp = dht11.readTemperature();
    if (readTemp != temp) {
      xSemaphoreTake(xSemaphore, portMAX_DELAY);
      temp = readTemp;
      xSemaphoreGive(xSemaphore);
    }
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");
    vTaskDelay(pdMS_TO_TICKS(2000));  // Asegurar un delay adecuado entre lecturas

  }
}

void Distancia(void *pvParameters) {
  for (;;) {
    digitalWrite(TriggerPin, LOW);
    delayMicroseconds(4);
    digitalWrite(TriggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TriggerPin, LOW);
    tiempo = pulseIn(EchoPin, HIGH);
    int readDistancia = tiempo / 2 / 29.1;
    if (readDistancia != distancia) {
      xSemaphoreTake(xSemaphore, portMAX_DELAY);
      distancia = readDistancia;
      xSemaphoreGive(xSemaphore);
    }
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
