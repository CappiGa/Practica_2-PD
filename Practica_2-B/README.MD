# Práctica 2 - B: Interrupciones con temporizador

## 1. Descripción
En esta práctica se implementa el uso de interrupciones por temporizador en un ESP32-S3. Se utiliza uno de los temporizadores de hardware del microcontrolador para generar interrupciones periódicas que son manejadas de forma asíncrona. El objetivo es comprender el funcionamiento de los temporizadores y su configuración utilizando el framework de Arduino.

## 2. Objetivos
- Configurar un temporizador en el ESP32-S3 para generar interrupciones periódicas.
- Manejar correctamente una interrupción mediante una función ISR.
- Mostrar por salida serie el número total de interrupciones ocurridas.

## 3. Introducción Teórica
Un temporizador es un componente del microcontrolador que cuenta a una frecuencia específica. Cuando alcanza un valor predefinido, puede generar una interrupción, permitiendo ejecutar una función específica sin intervención directa del programa principal.

El ESP32 cuenta con dos grupos de temporizadores, cada uno con dos temporizadores de propósito general. Estos temporizadores tienen las siguientes características:

- Basados en contadores de 64 bits y preescaladores de 16 bits.
- Capaces de contar hacia arriba o hacia abajo.
- Permiten configuración de alarmas y recarga automática.
- Funcionan con una frecuencia base, típicamente de 80 MHz.

El prescalador divide esta frecuencia base, permitiendo modificar el periodo de la interrupción. Por ejemplo, con un prescaler de 80, cada tick equivale a 1 microsegundo. Así, para generar una interrupción cada segundo, se requiere una cuenta de 1,000,000 ticks.

Más información sobre este funcionamiento puede consultarse en el siguiente tutorial:  
[https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/](https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/)

## 4. Materiales
- **ESP32-S3**
- **Protoboard**
- **Cable USB tipo C**

## 5. Desarrollo
### 5.1 Preparación del Entorno
1. Configurar el archivo `platformio.ini` con el siguiente contenido:
   ```ini
   [env:esp32-s3-devkitm-1]
   platform = espressif32
   board = esp32-s3-devkitm-1
   framework = arduino
   monitor_speed=115200
2. Conectar el ESP32-S3 al ordenador mediante USB.
### 5.2 Programación del Microcontrolador
El código implementado en main.cpp es el siguiente:
```cpp
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
```
Explicación del código:

- Se define una función `onTimer()` como ISR, que se ejecuta cuando ocurre la interrupción del temporizador.
- Se usa `timerBegin()` para inicializar el temporizador 0 con un divisor de 80, haciendo que cada tick equivalga a 1 µs.
- `timerAlarmWrite()` configura el temporizador para que lance una interrupción cada 1,000,000 µs (1 segundo).
- En el `loop()`, se verifica si ocurrió una interrupción y se actualiza un contador total que se imprime por la salida serie.
### 5.3 Subida del Código al ESP32-S3
1. Conectar la placa ESP32-S3 y asegurarse de que esté en modo de programación.
2. Compilar y cargar el código usando PlatformIO (`Upload`).
3. Abrir el monitor serie (`Monitor`) para observar los mensajes del contador de interrupciones.


## 6. Resultados
- Al iniciar el programa, se comienza a recibir por la consola serial el mensaje:
```cpp
An interrupt has occurred. Total number: 1
An interrupt has occurred. Total number: 2
...
```
- Esto indica que cada segundo se está generando una interrupción correctamente.
- La estabilidad del conteo se mantiene con el paso del tiempo, y el código demuestra el uso eficiente de interrupciones por hardware con temporizadores.
