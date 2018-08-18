# wifi_output_control
Control de salidas via WiFi controlado con un modulo ESP8266.

## Caracteristicas Generales:

* Modo Station o Access Point automatico
* Soporte OTA
* Configuracion red WiFi almacenada en memoria flash.
* Soporta hasta 2 salidas en simultaneo (PCB para 1 o 2).

## Primer Uso

1.  Una vez conectado, alimentar el circuito y esperar que el LED azul quede encendido fijo.
2.	Ya en modo AP, conectarse a la red del tip ESPXXX (XXX representa los ultimos numeros MAC del dispositivo).
3.	Mediante el navegador, ingresar a la pagina de control. http://192.168.4.1
4.	Ir a la pestaña Configuracion y ingresar los datos de la red que se quiere utilizar.
5. 	Reiniciar dispositivo. EL led comenzara a titilar, en caso de que pasado un tiempo no pueda conectarse volvera a encenderse fijo. Repetir la Configuracion. Si queda apagado la conexion tuvo exito.

Nota: El dispositivo admite conexion Serial USB, **Baudrate: 115200**


### TO DO

* Añadir mas salidas (Rediseño de PCB).
* Almacenar mas de un ssid a la vez.
* Añadir cambio de usuario / pass al login
* Rediseño site ( sacar spanglish)
* SPIFFS OTA
* Terminar el readme
