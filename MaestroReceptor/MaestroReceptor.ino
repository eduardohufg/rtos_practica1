// Maestro encargado de recibir datos de la entrada de un teclado matricial,
// temperatura y distancia, para mostrarlos en un display LCD y controlar un motor
// con base en la temperatura
//-------------------------------------------------------------------------------------------------------------
// Librerías
#include<Wire.h>                // Utilizar el protoclo I2C
#include <LiquidCrystal_I2C.h>  // Utilizar el LCD vía I2C

// Crear un objeto de tipo LiquidCrystal_I2C
LiquidCrystal_I2C lcd(0x27,16,2);

char keypressed = 0;
int temp = 0;
int distancia = 0;

void setup() {
  Wire.begin();       // Iniciar I2C como maestro
  Serial.begin(9600);
  lcd.init();         // Inicializar el LCD
  lcd.backlight();    // Encender la luz de fondo

  pinMode(2, OUTPUT); // Configura el pin D2 como salida
}

void loop() {
  // Solicitar datos al esclavo
  Wire.requestFrom(23, 6); // Solicitar 6 bytes al esclavo con dirección 23

  // Leer los datos recibidos del esclavo
  keypressed = Wire.read();
  temp = Wire.read();
  temp |= Wire.read() << 8;
  distancia = Wire.read();
  distancia |= Wire.read() << 8;

  // Actualizar la LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(temp);
  lcd.print(" C");

  lcd.setCursor(8, 0);
  lcd.print("D: ");
  lcd.print(distancia);
  lcd.print(" cm");

  // Mostrar el carácter recibido en la segunda fila de la LCD
  lcd.setCursor(0, 1);
  lcd.print("Key: ");
  lcd.print(keypressed);

  // Verifica si la distancia es mayor a 20 y ajusta el pin D2
  if (distancia > 20) {
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }

  delay(100); // Esperar un poco antes de la próxima solicitud de datos
}