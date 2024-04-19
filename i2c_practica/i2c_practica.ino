#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

String message;
bool S = false;
String mes = "";

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();

    Wire.begin(23);                // unirse al bus i2c con la dirección #23
    Wire.onReceive(eventoRecepcion); // registrar evento de recepción de datos
}

void loop() {
    lcd.setCursor(0, 1);
    lcd.print(mes); // Mostrar el mensaje acumulado
    delay(100);
}

void eventoRecepcion(int howMany) {
    while(Wire.available()) { // Mientras haya datos para leer
        char c = Wire.read(); // Leer un byte
        mes += String(c);     // Acumular el carácter en la variable mes
        Serial.print(c);      // Opcional: para depuración por Serial
    }
}

// function that executes whenever data is requested by master
