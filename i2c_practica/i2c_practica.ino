#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección de LCD

volatile bool i2cIsBusy = false; // Flag para controlar el acceso al bus I2C

char keypressed = 0;
int temp = 0;
int distancia = 0;

volatile int newTemp = 0;
volatile int newDistancia = 0;
volatile char newKeypressed = 0;
volatile bool newDataAvailable = false; // Flag para nuevos datos disponibles

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  Wire.begin(23); // Dirección I2C para este Arduino como esclavo
  Wire.onReceive(receiveEvent); // Manejador de recepción de datos I2C
  Serial.println("Sistema iniciado y listo.");

  pinMode(2, OUTPUT); // Configura el pin D2 como salida
}

void loop() {
  if (!i2cIsBusy && newDataAvailable) {
    // Copiar los nuevos datos a las variables principales
    temp = newTemp;
    distancia = newDistancia;
    keypressed = newKeypressed;
    newDataAvailable = false; // Resetear el flag

    // Actualizar la LCD solo si no se están recibiendo datos
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
  }
}

void receiveEvent(int howMany) {
  i2cIsBusy = true; // Establece el flag indicando que I2C está ocupado

  if (howMany >= 5) {
    newKeypressed = Wire.read();
    newTemp = Wire.read();
    newTemp |= Wire.read() << 8;
    newDistancia = Wire.read();
    newDistancia |= Wire.read() << 8;
    
    newDataAvailable = true; // Establece el flag de nuevos datos disponibles
    Serial.println("Datos recibidos correctamente.");
  } else {
    Serial.print("Error: Datos incompletos. Bytes recibidos: ");
    Serial.println(howMany);
    newDataAvailable = false; // Evita actualizar con datos incompletos
  }

  i2cIsBusy = false; // Libera el flag después de procesar los datos
}
