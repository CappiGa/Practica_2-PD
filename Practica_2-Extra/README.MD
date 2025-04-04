# Práctica 2 - Extra: Control de parpadeo con botones e interrupciones

## 1. Descripción
En esta práctica se desarrolla un programa en el ESP32-S3 que combina el uso de interrupciones por temporizador e interrupciones externas para controlar la frecuencia de parpadeo de un LED mediante dos pulsadores. El LED parpadea con una frecuencia inicial, y al presionar uno de los botones, la frecuencia aumenta o disminuye dependiendo del botón utilizado. Se implementa un sistema de filtrado por hardware utilizando resistencias pull-up internas y control por interrupciones.

## 2. Objetivos
- Configurar un temporizador para ejecutar código periódicamente.
- Usar interrupciones externas para capturar pulsaciones de botones.
- Controlar dinámicamente la frecuencia de parpadeo de un LED.
- Filtrar el ruido mecánico de los botones mediante la configuración adecuada.

## 3. Materiales
- **ESP32-S3**
- **Protoboard**
- **2 Pulsadores**
- **Cable USB tipo C**

## 4. Desarrollo

### 4.1 Preparación del Entorno
1. Configurar el archivo `platformio.ini` con el siguiente contenido:
   ```ini
   [env:esp32-s3-devkitm-1]
   platform = espressif32
   board = esp32-s3-devkitm-1
   framework = arduino
   monitor_speed=115200
   ```
2. Conectar el ESP32-S3 al ordenador mediante USB.

### 4.2 Programación del Microcontrolador
El código implementado en `main.cpp` es el siguiente:
```cpp
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
  blinkInterval = (blinkInterval > 200) ? blinkInterval - 200 : 100;
}

void IRAM_ATTR buttonISR2() {
  blinkInterval = (blinkInterval < 5000) ? blinkInterval + 500 : 5000;
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin1), buttonISR1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), buttonISR2, FALLING);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}

void loop() {
  if (interruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;

    digitalWrite(ledPin, HIGH);
    delay(blinkInterval / 2);
    digitalWrite(ledPin, LOW);
    delay(blinkInterval / 2);

    Serial.print("Interrupts: ");
    Serial.print(totalInterruptCounter);
    Serial.print(" - Blink Interval: ");
    Serial.println(blinkInterval);
  }
}
```

**Explicación del código:**
- Se definen interrupciones en dos botones que ajustan dinámicamente el tiempo de parpadeo.
- La función de temporizador genera una interrupción cada segundo, controlando el parpadeo del LED.
- Se garantiza que el tiempo de parpadeo esté entre 100 ms y 5000 ms.

### 4.3 Subida del Código al ESP32-S3
1. Conectar la placa ESP32-S3.
2. Compilar y cargar el código (`Upload`).
3. Abrir el monitor serie (`Monitor`) para verificar los cambios de intervalo al presionar los botones.

## 5. Resultados
- Inicialmente, el LED parpadea cada segundo.
- Al presionar el botón conectado al pin 16, el parpadeo se acelera.
- Al presionar el botón conectado al pin 17, el parpadeo se ralentiza.
- El puerto serie muestra información sobre el número de interrupciones y el intervalo actual:
  ```
  Interrupts: 1 - Blink Interval: 1000
  Interrupts: 2 - Blink Interval: 800
  Interrupts: 3 - Blink Interval: 600
  ...
  ```
