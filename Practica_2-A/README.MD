# Práctica 2 - A: Interrupciones con un botón

## 1. Descripción
En esta práctica se explora el uso de interrupciones en el microcontrolador ESP32-S3, tanto por hardware (botón) como por temporizador. El objetivo es comprender cómo se comporta el sistema ante eventos que requieren una respuesta inmediata, permitiendo controlar de forma eficiente procesos como el parpadeo de LEDs o la detección de entradas externas.

## 2. Objetivos
- Comprender el funcionamiento de las interrupciones en microcontroladores.
- Configurar interrupciones por temporizador utilizando el ESP32-S3.
- Implementar interrupciones por hardware mediante un botón físico.
- Visualizar el resultado de las interrupciones usando la salida por el puerto serie.

## 3. Introducción teórica
Una interrupción es un mecanismo mediante el cual un microcontrolador puede detener momentáneamente su ejecución normal para atender un evento urgente. Esto permite una respuesta inmediata ante sucesos externos (como la pulsación de un botón) o internos (como la expiración de un temporizador).

Las interrupciones pueden clasificarse en:
- **Hardware**: generadas por una señal externa, como un cambio en un pin GPIO.
- **Temporizadas (Timers)**: generadas tras un tiempo específico.
- **Por software**: no están soportadas actualmente en Arduino, pero sí en otros entornos.

En lugar de consultar constantemente el estado de las entradas (polling), las interrupciones permiten ejecutar una función específica (ISR) solo cuando ocurre el evento. Esto mejora la eficiencia y reduce la latencia de respuesta.

En el ESP32, todas las GPIO pueden configurarse como fuente de interrupciones mediante la función `attachInterrupt(pin, ISR, mode)`, donde:
- `pin`: número del pin a monitorear.
- `ISR`: función que se ejecutará cuando se dispare la interrupción.
- `mode`: condición que activa la interrupción (LOW, HIGH, CHANGE, FALLING, RISING).

También se puede usar `detachInterrupt(pin)` para desactivar la interrupción.

Las rutinas de interrupción deben ser lo más breves posible y, en el caso del ESP32, es recomendable marcarlas con `IRAM_ATTR` para que se almacenen en memoria RAM y se ejecuten más rápido.

## 4. Materiales
- **ESP32-S3**
- **Botón**
- **Protoboard**
- **Cable tipo C**

## 5. Desarrollo

### 5.1 Preparación del Entorno
1. Configurar el archivo `platformio.ini` con el siguiente contenido:
   ```ini
   [env:esp32-s3-devkitm-1]
   platform = espressif32
   board = esp32-s3-devkitm-1
   framework = arduino
   monitor_speed=115200
2. Conectar la placa ESP32-S3 al ordenador mediante un cable USB tipo C.
### 5.2 Programación del Microcontrolador
El código implementado en `main.cpp`  configura un temporizador por hardware que genera una interrupción cada segundo (1,000,000 microsegundos). Cada interrupción incrementa un contador, y se imprime por el monitor serie:
```cpp
#include <Arduino.h>

volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);
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
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
  }
}
```
**Explicación del código:**
- Se inicializa un temporizador (`timerBegin`) con una resolución de 1 μs.
- La rutina `onTimer()` se ejecuta cada segundo, incrementando el contador `interruptCounter`.
- En `loop()`, cuando `interruptCounter` es mayor a cero, se imprime el total de interrupciones procesadas por el monitor serie.
### 5.3 Subida del Código al Esp32-S3
1. Conectar la placa ESP32-S3 y asegurarse de que esté en modo de programación.
2. Compilar y cargar el código usando PlatformIO (`Upload`).
3. Abrir el monitor serie (`Monitor`) para observar los resultados.
## 6. Resultados
- Cada segundo se ejecuta la interrupción y se muestra en el monitor serie un mensaje del tipo:
```cpp
An interrupt as occurred. Total number: 1
An interrupt as occurred. Total number: 2
...
```
- El sistema responde correctamente a las interrupciones por timer, confirmando que el uso de interrupciones es efectivo y eficiente.
- No se usaron funciones bloqueantes como `delay()`, lo cual permite un mejor control del flujo de ejecución.
- El sistema se mantiene estable durante largos periodos de funcionamiento.ç

