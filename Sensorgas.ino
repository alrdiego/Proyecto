// ---- Librerías ----
#include <Wire.h>                // Librería para comunicación I2C (pantalla OLED)
#include <Adafruit_GFX.h>        // Librería gráfica base de Adafruit
#include <Adafruit_SSD1306.h>    // Control específico para pantallas OLED SSD1306

// ---- Configuración de pantalla ----
#define ANCHO_PANTALLA 128       // Ancho de la OLED en píxeles
#define ALTO_PANTALLA 64         // Alto de la OLED en píxeles
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);  // Objeto pantalla (I2C, sin pin de reset)

// ---- Pines y variables ----
int sensorGas = A1;              // Pin analógico del sensor de gas (MQ-2) OJO: comentario original menciona A0.
int led = 10;                    // Pin del LED indicador
int pinZumbador = 9;             // Pin de un buzzer (activo) controlado con HIGH/LOW
int valorSensor = 0;             // Variable para guardar la lectura analógica del sensor
int umbral = 50;                 // Umbral que define alarma vs. nivel seguro (ajustar tras pruebas) (El umbral original es de 7000, 50 es solo para la prueba)
int pinZumbadorP = 7;            // Pin de otro zumbador (pasivo) al que se le aplica tono()

void setup() {
  pinMode(led, OUTPUT);          // LED como salida
  pinMode(pinZumbador, OUTPUT);  // Zumbador (pasivo) como salida
  pinMode(pinZumbadorP, OUTPUT); // Zumbador (activo) como salida
  pinMode(sensorGas, INPUT);     // Sensor de gas como entrada analógica
  Serial.begin(9600);            // Inicializa puerto serie a 9600 bps para depuración

  // ---- Inicialización de pantalla OLED ----
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Inicia OLED (dirección I2C típica 0x3C)
    Serial.println(F("Error: no se encontró la pantalla OLED!"));  // Mensaje de error por serie
    for (;;);  // Si falla la pantalla, queda el bucle infinito (detiene el programa)
  }

  display.clearDisplay();        // Limpia el buffer de la pantalla
  display.setTextSize(1);        // Tamaño de texto 1 (pequeño)
  display.setTextColor(SSD1306_WHITE);  // Texto en color blanco (1 bit)
  display.setCursor(0, 0);       // Coloca el cursor en (x=0, y=0)
  display.println("Sensor de GAS");     // Muestra texto de bienvenida
  display.display();             // Envía el buffer a la pantalla
  delay(2000);                   // Pausa 2 segundos para leer el mensaje
}

void loop() {
  valorSensor = analogRead(sensorGas);  // Lee valor analógico del sensor (0-1023)
  Serial.print("Valor de gas: ");       
  Serial.println(valorSensor);          // Imprime etiqueta y valor por el monitor serie

  // ---- Actualización de pantalla (limpiar y volver a escribir) ----
  display.clearDisplay();               // Limpia el buffer antes de dibujar nuevo cuadro
  display.setTextSize(1);               // Asegura tamaño de texto 1
  display.setTextColor(SSD1306_WHITE);  // Asegura color blanco

  // ---- Título ----
  display.setCursor(0, 0);              // Cursor arriba a la izquierda
  display.println("Medicion de Gas");   // Título de la sección

  // ---- Valor en tiempo real ----
  display.setCursor(0, 20);             // Cursor más abajo
  display.println("Valor:");            // Etiqueta
  display.println(valorSensor);         // Muestra número leído

  // ---- Lógica de alarma ----
  if (valorSensor > umbral) {           // Si supera el umbral, hay alerta
    digitalWrite(led, HIGH);            // Enciende LED
    digitalWrite(pinZumbador, HIGH);    // Activa buzzer (activo) con nivel alto
    tone(pinZumbadorP, 5500);           // Genera tono de 5.5 kHz en buzzer (pasivo)
    display.setCursor(0, 50);           // Cursor para mensaje de estado
    display.println("ALERTA! Gas detectado"); // Texto de alerta en la pantalla
  } else {                              // Si NO supera el umbral
    digitalWrite(led, LOW);             // Apaga LED
    digitalWrite(pinZumbador, LOW);     // Desactiva buzzer (activo)
    noTone(pinZumbadorP);               // Detiene el tono en buzzer (pasivo)
    display.setCursor(0, 50);           // Cursor para mensaje de estado
    display.println("Nivel Seguro");    // Texto de estado seguro
  }

  display.display();                    // Renderiza en la pantalla lo que está en el buffer
  delay(500);                           // Pausa corta para estabilidad/legibilidad (0.5 s)
}
