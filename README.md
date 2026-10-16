# Monitor de nivel mediante ADC - Raspberry Pi Pico

* Juan Pablo Ruiz Gil
* Marco Uriel Castaneda Avila

## Descripción

Este proyecto implementa un sistema de monitoreo de nivel utilizando el convertidor analógico-digital (ADC) de una Raspberry Pi Pico.

El sistema obtiene periódicamente una lectura de un sensor analógico conectado al pin GP26, aplica un filtro de promedio móvil de 8 muestras y convierte el resultado a voltaje y porcentaje.

Dependiendo del porcentaje obtenido, el sistema clasifica el nivel como:

* **NORMAL:** menor al 50 %
* **WARNING:** entre 50 % y 74.9 %
* **ALARMA:** igual o mayor al 75 %

El estado se representa mediante tres LEDs:

* LED verde: estado NORMAL.
* LED amarillo: estado WARNING.
* LED rojo: estado ALARMA.

## Hardware

| Componente       | GPIO        |
| ---------------- | ----------- |
| Sensor analógico | GP26 / ADC0 |
| LED verde        | GP13        |
| LED amarillo     | GP14        |
| LED rojo         | GP15        |

El sensor debe utilizar una referencia de alimentación compatible con el ADC de la Raspberry Pi Pico, normalmente 3.3 V.

## Funcionamiento

El programa realiza continuamente las siguientes operaciones:

1. Lee el valor del ADC.
2. Agrega la lectura a una ventana de 8 muestras.
3. Calcula el promedio de las muestras disponibles.
4. Convierte el resultado a voltaje.
5. Convierte el resultado a porcentaje.
6. Clasifica el porcentaje.
7. Actualiza el LED correspondiente.
8. Muestra la información mediante el puerto serial.
9. Espera 250 ms.
10. Repite el proceso.

## Promedio móvil

Para reducir las variaciones producidas por el ruido del sensor se utiliza un promedio móvil.

La ventana contiene un máximo de 8 lecturas. Cuando se obtiene una nueva lectura y la ventana ya está llena, se reemplaza la lectura más antigua.

Esto permite obtener una señal más estable para realizar la clasificación.

## ADC

El ADC del RP2040 utilizado mediante Pico SDK tiene una resolución de 12 bits.

Por lo tanto, las lecturas se encuentran entre:

```text
0 y 4095
```

El porcentaje se obtiene mediante:

```text
porcentaje = lectura_filtrada × 100 / 4095
```

El voltaje se obtiene mediante:

```text
voltaje = lectura_filtrada × 3.3 / 4095
```

## Clasificación

Los límites utilizados son:

```text
WARNING = 50 %
ALARMA  = 75 %
```

La lógica es:

```text
Si porcentaje >= 75 %
    ALARMA

Si no, si porcentaje >= 50 %
    WARNING

Si no
    NORMAL
```

## Estructura del proyecto

```text
ADC_Project/
│
├── CMakeLists.txt
├── README.md
│
├── include/
│   ├── moving_average.h
│   ├── classifier.h
│   └── outputs.h
│
└── src/
    ├── main.c
    ├── moving_average.c
    ├── classifier.c
    └── outputs.c
```

## Compilación

El proyecto requiere:

* Raspberry Pi Pico SDK
* CMake
* compilador GCC para ARM
* Ninja o Make
* Raspberry Pi Pico / Pico W

Después de configurar el Pico SDK, se genera el directorio de compilación:

```bash
mkdir build
cd build
cmake ..
```

Posteriormente se compila:

```bash
cmake --build .
```

El proceso genera los archivos necesarios para cargar el programa en la Raspberry Pi Pico.

## Salida serial

El programa muestra información similar a:

```text
Raw = 2450
Voltage = 1.97 V
Filtered = 59.8 %
State = WARNING
--------------------
```

La lectura `Raw` corresponde al valor obtenido directamente del ADC.

`Voltage` representa el voltaje calculado.

`Filtered` corresponde al porcentaje después de aplicar el promedio móvil.

`State` representa el estado actual del sistema.

## Objetivo

El objetivo del proyecto es implementar un sistema embebido capaz de adquirir una señal analógica, filtrarla y tomar una decisión basada en diferentes niveles de referencia, utilizando la Raspberry Pi Pico y el ADC del RP2040.
