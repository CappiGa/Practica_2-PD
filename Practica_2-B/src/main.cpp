#include <Arduino.h>


volatile int interruptCounter = 0;
int totalInterruptCounter = 0;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


// Función de interrupción del temporizador
void IRAM_ATTR onTimer() {
 portENTER_CRITICAL_ISR(&timerMux);
 interruptCounter++;
 portEXIT_CRITICAL_ISR(&timerMux);
}


void setup() {
 Serial.begin(115200);
  // Configurar el temporizador en el ESP32
 timer = timerBegin(0, 80, true); // Timer 0, divisor 80 (1 tick = 1µs)
 timerAttachInterrupt(timer, &onTimer, true);
 timerAlarmWrite(timer, 1000000, true); // Interrupción cada 1 segundo
 timerAlarmEnable(timer); // Habilita la alarma
}


void loop() {
 if (interruptCounter > 0) {
   portENTER_CRITICAL(&timerMux);
   interruptCounter--;
   portEXIT_CRITICAL(&timerMux);


   totalInterruptCounter++;
   Serial.print("An interrupt has occurred. Total number: ");
   Serial.println(totalInterruptCounter);
 }
}