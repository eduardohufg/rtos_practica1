#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino_FreeRTOS.h>

// Crear un objeto de tipo LiquidCrystal_I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

char keypressed = 0;
int temp = 0;
int distancia = 0;

void setup() {
  Wire.begin(); // Iniciar I2C como maestro
  Serial.begin(9600);
  lcd.init(); // Inicializar el LCD
  lcd.backlight(); // Encender la luz de fondo
  pinMode(2, OUTPUT); // Configurar el pin D2 como salida

  // Crear tareas
  xTaskCreate(TaskReceiveData, "ReceiveData", 128, NULL, 3, NULL); // Mayor prioridad
  xTaskCreate(TaskUpdateLCD, "UpdateLCD", 128, NULL, 2, NULL);
  xTaskCreate(TaskCheckDistance, "CheckDistance", 128, NULL, 1, NULL);
}

void loop() {
  // En FreeRTOS, loop() usualmente queda vacío.
}

// Tarea para recibir datos del esclavo I2C
void TaskReceiveData(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    Wire.requestFrom(23, 6);
    keypressed = Wire.read();
    temp = Wire.read();
    temp |= Wire.read() << 8;
    distancia = Wire.read();
    distancia |= Wire.read() << 8;

    Serial.print("Key pressed: ");
    Serial.println(keypressed);
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" C");
    Serial.print("Distance: ");
    Serial.print(distancia);
    Serial.println(" cm");
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Tarea para actualizar la pantalla LCD
void TaskUpdateLCD(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
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
    
    vTaskDelay(500 / portTICK_PERIOD_MS); // Actualizar menos frecuentemente
  }
}

// Tarea para verificar la distancia y activar un pin
void TaskCheckDistance(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    if (distancia > 20) {
      digitalWrite(2, HIGH);
    } else {
      digitalWrite(2, LOW);
    }
    
    vTaskDelay(200 / portTICK_PERIOD_MS); // Comprobar cada 200 ms
  }
}
