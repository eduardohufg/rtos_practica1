/*Controlador encargado de recibir las entradas del teclado matricial,
  así como los datos recolactados por el sensor de temperatura y 
  el sensor ultrasónico, para posteriormente enviarlos a un periférico*/
//--------------------------------------------------------------------------------------------------
// Librerías
#include<Keypad.h>    // Controlar el teclado matricial
#include<DHT11.h>     // Controlar el DHT11
#include<Wire.h>      // Utilizar el protoclo I2C
//---------------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------
// Señal de temperatura del sensor DHT11
// Iniciar un objeto de tipo DHT11
DHT11 dht11(2); // Conexión digital del sensor con la placa Arduino
int temp;
//----------------------------------------------------------
// Señal de distancia mediante el sensor ultrasónico HC-SR04
#define TriggerPin 13
#define EchoPin 12
int distancia;
long tiempo;

volatile int state = LOW;

unsigned long previousMillis = 0;
const long interval = 1000;  // Intervalo de tiempo en milisegundos

bool keyPressedFlag = false;
char lastKeyPressed = '\0'; // Carácter previamente recibido

void setup() {
  Wire.begin();          // Unirse al bus i2c como master
  Serial.begin(9600);   // Configurar monitor serie a 9600
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
}

void loop() {
  char key = myKeypad.getKey();

  if (key) {
    char keypresse = key;
    Serial.println(key);
  }
  //Teclado();
  Temperatura();
  Distancia();
  Transmitir();
}

// Obtener y enviar las entradas del teclado matricial
void Teclado() {
  if (keypressed != NO_KEY) {
    Serial.print("Caracter: ");
    Serial.println(keypressed);
  }
}

// Obtener y enviar los datos del sensor de temperatura y humedad DHT11 
void Temperatura() {
  temp = dht11.readTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");
}

// Obtener y enviar los datos del sensor de distancia ultrasónico HC-SR04
void Distancia() {
  digitalWrite(TriggerPin, LOW);      // generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);     //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  tiempo = pulseIn(EchoPin, HIGH);  //medir el tiempo entre pulsos en microsegundos
  distancia = tiempo / 2 / 29.1; //convertir a distancia en cm
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
}

void Transmitir() {
  Wire.beginTransmission(23);
    Wire.write(keypressed);
    Wire.write(temp);
    Wire.write(distancia);
    Wire.endTransmission();
}
