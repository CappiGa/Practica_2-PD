#include <Arduino.h>


// Definimos los pines de los pulsadores y el LED
const int ledPin = 4;
const int buttonPin1 = 16;  // Primer pulsador (incrementa la frecuencia)
const int buttonPin2 = 17;  // Segundo pulsador (disminuye la frecuencia)


// Variables de control
volatile int interruptCounter = 0;
int totalInterruptCounter = 0;
unsigned long blinkInterval = 1000; // Intervalo de parpadeo en ms (1 segundo)


// Variables de temporizador
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


// Configuración del temporizador
void IRAM_ATTR onTimer() {
 portENTER_CRITICAL_ISR(&timerMux);
 interruptCounter++;
 portEXIT_CRITICAL_ISR(&timerMux);
}


// ISR de los botones para cambiar la frecuencia
void IRAM_ATTR buttonISR1() {
 // Reducimos el intervalo (más rápido), pero garantizamos que no sea menor a 100ms
 blinkInterval = (blinkInterval > 200) ? blinkInterval - 200 : 100; // Reducimos el intervalo
}


void IRAM_ATTR buttonISR2() {
 // Aumentamos el intervalo (más lento), pero garantizamos que no sea mayor a 5000ms
 blinkInterval = (blinkInterval < 5000) ? blinkInterval + 500 : 5000; // Aumentamos el intervalo
}


void setup() {
 // Inicializamos el puerto serie
 Serial.begin(115200);


 // Configuramos el pin para el LED
 pinMode(ledPin, OUTPUT);


 // Configuramos los pines de los botones como entradas con pullup
 pinMode(buttonPin1, INPUT_PULLUP);
 pinMode(buttonPin2, INPUT_PULLUP);


 // Configuramos las interrupciones de los botones
 attachInterrupt(digitalPinToInterrupt(buttonPin1), buttonISR1, FALLING);
 attachInterrupt(digitalPinToInterrupt(buttonPin2), buttonISR2, FALLING);


 // Configuración del temporizador
 timer = timerBegin(0, 80, true); // 80 prescaler
 timerAttachInterrupt(timer, &onTimer, true); // Llamamos a la función onTimer en cada interrupción
 timerAlarmWrite(timer, 1000000, true); // Alarma cada 1 segundo
 timerAlarmEnable(timer); // Activamos la alarma
}


void loop() {
 if (interruptCounter > 0) {
   portENTER_CRITICAL(&timerMux);
   interruptCounter--; // Restamos para no sobrecargar
   portEXIT_CRITICAL(&timerMux);


   totalInterruptCounter++;


   // Parpadeo del LED
   digitalWrite(ledPin, HIGH);  // Encendemos el LED
   delay(blinkInterval / 2);    // Esperamos medio intervalo
   digitalWrite(ledPin, LOW);   // Apagamos el LED
   delay(blinkInterval / 2);    // Esperamos medio intervalo


   // Imprimimos el número de interrupciones y la frecuencia del parpadeo
   Serial.print("Interrupts: ");
   Serial.print(totalInterruptCounter);
   Serial.print(" - Blink Interval: ");
   Serial.println(blinkInterval);
 }
}