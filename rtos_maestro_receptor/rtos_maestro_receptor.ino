#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino_FreeRTOS.h>
#include<semphr.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

SemaphoreHandle_t xSemaphore;
char keypressed = 0;
int temp = 0;
int distancia = 0;


void setup() {
  Wire.begin(); // Iniciar I2C como maestro
  Serial.begin(9600);
  lcd.init(); // Inicializar el LCD
  lcd.backlight(); // Encender la luz de fondo
  pinMode(2, OUTPUT); // Configurar el pin D2 como salida

  xSemaphore = xSemaphoreCreateMutex();

  xTaskCreate(TaskReceiveData, "ReceiveData", 128, NULL, 3, NULL);
  xTaskCreate(TaskUpdateLCD, "UpdateLCD", 128, NULL, 2, NULL);
  xTaskCreate(TaskCheckDistance, "CheckDistance", 128, NULL, 1, NULL);
}

void loop() {}

void TaskReceiveData(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (Wire.requestFrom(23, 6) == 6) {
      if (xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE) {
        keypressed = Wire.read();
        temp = Wire.read();
        temp |= Wire.read() << 8;
        distancia = Wire.read();
        distancia |= Wire.read() << 8;
        xSemaphoreGive(xSemaphore);

        Serial.print("Key pressed: ");
        Serial.println(keypressed);
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.println(" C");
        Serial.print("Distance: ");
        Serial.print(distancia);
        Serial.println(" cm");
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void TaskUpdateLCD(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("T: ");
      lcd.print(temp);
      lcd.print(" C");
      lcd.setCursor(8, 0);
      lcd.print("D: ");
      lcd.print(distancia);
      lcd.print(" cm");
      lcd.setCursor(0, 1);
      lcd.print("Key: ");
      lcd.print(keypressed);
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS); // Actualizar menos frecuentemente
  }
}

void TaskCheckDistance(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE) {
      if (distancia > 20) {
        digitalWrite(2, HIGH);
      } else {
        digitalWrite(2, LOW);
      }
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(200 / portTICK_PERIOD_MS); // Comprobar cada 200 ms
  }
}
