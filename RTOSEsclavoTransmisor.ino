// Librerías
#include<Arduino_FreeRTOS.h>
#include<Keypad.h>    // Controlar el teclado matricial
#include<DHT11.h>     // Controlar el DHT11
#include<Wire.h>      // Utilizar el protoclo I2C
//-------------------------------------------------------------------------------------------------------------
// Configurar el teclado
const byte numRows = 4; // Número de filas en el teclado
const byte numCols = 4; // Número de columnas en el teclado

// Keymap permite mapear los botones conforme a la fila y columana que tienen en el teclado
char keymap[numRows][numCols] =
  { {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'} };

// Conexiones (digitales) del teclado a la placa Arduino
byte rowPins[numRows] = {22, 24, 26, 28};     // Filas 1 a 4
byte colPins[numCols] = {30, 32, 34, 36};  // Columnas 1 a 4

// Iniciar un objeto de tipo Keypad
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);
char keypressed;
//-------------------------------------------------------------------------------------------------------------
// Señal de temperatura del sensor DHT11
// Iniciar un objeto de tipo DHT11
DHT11 dht11(2); // Conexión digital del sensor con la placa Arduino
int temp;
//-------------------------------------------------------------------------------------------------------------
// Señal de distancia mediante el sensor ultrasónico HC-SR04
#define TriggerPin 13
#define EchoPin 12
int distancia;
long tiempo;
//-------------------------------------------------------------------------------------------------------------

unsigned long lastDistanceTime = 0;
unsigned long lastTempTime = 0;
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(9600);           // Configurar monitor serie a 9600
  Wire.begin(23);               // Dirección I2C para este Arduino como esclavo
  Wire.onRequest(Enviar); // Configurar la función de callback para solicitudes de datos
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  // TAREAS
  xTaskCreate(Enviar, "Tarea para enviar los datos vía I2C", 128, NULL, 3, NULL);
  xTaskCreate(Teclado, "Tarea para obtener la entrada del teclado", 128, NULL, 2, NULL);
  xTaskCreate(Temperatura, "Tarea para obtener la temperatura", 128, NULL, 1, NULL);
  xTaskCreate(Distancia, "Tarea para obtener la distancia", 128, NULL, 1, NULL);
  
  vTaskStartScheduler();

  for (;;);
}

void loop() {}

void Enviar(void *Parameter) {
  for(;;) {
    Wire.write(keypressed); // Enviar el caracter presionado
    Wire.write((byte)(temp & 0xFF)); // Enviar la temperatura (byte bajo)
    Wire.write((byte)((temp >> 8) & 0xFF)); // Enviar la temperatura (byte alto)
    Wire.write((byte)(distancia & 0xFF)); // Enviar la distancia (byte bajo)
    Wire.write((byte)((distancia >> 8) & 0xFF)); // Enviar la distancia (byte alto)
  }
  vTaskDelete(NULL);
}

void Teclado(void *Parameter) {
  for(;;) {
    char key = myKeypad.getKey();
    if (key) {
      keypressed = key;
      Serial.print("Caracter: ");
      Serial.println(key);
    }
  }
  vTaskDelete(NULL);
}

void Temperatura(void *Parameter) {
  for(;;) {
    temp = dht11.readTemperature();
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.println(" °C");
  }
  vTaskDelete(NULL);
}

void Distancia(void *Parameter) {
  for(;;) {
    digitalWrite(TriggerPin, LOW);
    delayMicroseconds(4);
    digitalWrite(TriggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TriggerPin, LOW);
    tiempo = pulseIn(EchoPin, HIGH);
    distancia = tiempo / 2 / 29.1;
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }
  vTaskDelete(NULL);
}
