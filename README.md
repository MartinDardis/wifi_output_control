# Control de salidas - ESP8266 WiFi

## Caracteristicas Generales:

* Modo Station o Access Point automatico
* Soporte ARDUINO OTA y WEB OTA (update con firm.bin)
* Configuracion red WiFi y Login almacenada en memoria flash.
* Soporta hasta 2 salidas en simultaneo (PCB para 1 o 2).

## Diagrama de Cableado

![PCB](https://github.com/MartinDardis/wifi_output_control/raw/master/pcb_points.png)

Los pines **1,2,3** pertenecen al relay 1, donde 2 (NEGRO) es la entrada, 1 (AMARILLO) es la salida normal cerrada y 3 (ROJO) la normal abierta.

Los pines **4,5,6** pertenecen al relay 2, donde 5 (NEGRO) es la entrada, 4 (AMARILLO) es la salida normal cerrada y 6 (ROJO) la normal abierta.

Los pines de **VCC** (AZUL) y **GND** (NEGRO), pueden recibir entre 5 y 20 V. No deben ser utilizados si se alimenta el modulo mediante el puerto USB.

## Primer Uso

Una vez realizado el cableado se procede a la configuracion inicial.

1.  Una vez conectado, alimentar el circuito y esperar que el LED azul quede encendido fijo.
2.	Ya en modo AP, conectarse a la red del tip ESPXXX (XXX representa los ultimos numeros MAC del dispositivo).
3.	Mediante el navegador, ingresar a la pagina de control. http://192.168.4.1
4.	Ir a la pestaña Configuracion y ingresar los datos de la red que se quiere utilizar.
5. 	Reiniciar dispositivo. EL led comenzara a titilar, en caso de que pasado un tiempo no pueda conectarse volvera a encenderse fijo. Repetir la Configuracion. Si queda apagado la conexion tuvo exito.

Nota: El dispositivo admite conexion Serial USB, **Baudrate: 115200**


### TO DO

* Añadir mas salidas (Rediseño de PCB).
* Almacenar mas de un ssid a la vez.
* SPIFFS OTA
* Terminar el readme
