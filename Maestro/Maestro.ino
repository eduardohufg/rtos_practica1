/*Controlador encargado de recibir las entradas del teclado matricial,
  así como los datos recolactados por el sensor de temperatura y 
  el sensor ultrasónico, para posteriormente enviarlos a un periférico*/
//-------------------------------------------------------------------------------------------------------------
// Librerías
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

bool transmissionSuccess = false; // Variable para almacenar el estado de la transmisión

void setup() {
  Wire.begin();          // Unirse al bus i2c como master
  Serial.begin(9600);   // Configurar monitor serie a 9600
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
}

void loop() {
  Teclado();

  unsigned long currentTime = millis();

  // Calcular la distancia cada 3 segundos
  if (currentTime - lastDistanceTime >= 4000) {
    Distancia();
    lastDistanceTime = currentTime;
  }

  // Calcular la temperatura cada 2 segundos
  if (currentTime - lastTempTime >= 3000) {
    Temperatura();
    lastTempTime = currentTime;
  }

  Transmitir();

  // Esperar un breve tiempo para permitir que el esclavo procese los datos
  delay(10);

}

void Teclado() {
  char key = myKeypad.getKey();
  if (key) {
    keypressed = key;
    Serial.print("Caracter: ");
    Serial.println(key);
  }
}

void Temperatura() {
  temp = dht11.readTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");
}

void Distancia() {
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

void Transmitir() {
  Serial.println("Iniciando transmisión I2C...");
  Wire.beginTransmission(23);
  Wire.write(keypressed);
  
  Wire.write((byte)(temp & 0xFF));
  Wire.write((byte)((temp >> 8) & 0xFF));
  
  Wire.write((byte)(distancia & 0xFF));
  Wire.write((byte)((distancia >> 8) & 0xFF));
  
  byte error = Wire.endTransmission();
  if (error) {
    Serial.print("Error en la transmisión: ");
    Serial.println(error);
  } else {
    Serial.println("Transmisión exitosa.");
  }
}
