# Chat Cliente-Servidor en C++

Este proyecto es una implementación de un sistema de chat cliente-servidor en C++ que permite a los usuarios conectarse a un servidor de chat, enviar mensajes públicos y privados, crear salas y ver listas de usuarios en tiempo real. Está construido utilizando las herramientas Meson y Ninja para la configuración y construcción, y Google Test (gtest) para las pruebas unitarias.

## Requisitos Previos

Antes de configurar y compilar el proyecto, asegúrate de tener las siguientes herramientas instaladas en tu sistema:

- **Meson**: Sistema de construcción.
- **Ninja**: Backend de construcción utilizado por Meson.
- **Git**: Para clonar y gestionar dependencias como gtest.
- **Compilador C++**: Como `g++`.

## Configuración del Proyecto

1. **Clonar el repositorio**:
   ```bash
   git clone <URL-del-repositorio>
   cd <nombre-del-repositorio>

2. **Configurar el proyecto**
    ```bash
    meson setup build

3. **Compilar el proyecto**
    ```bash
    ninja -C build

4. **Ejecutar el proyecto**
    ```bash
    Servidor: 
        ./build/bin/mainServer <numero de puerto>
    Despues de ejecutar el servidor, en otra terminal ejecutas el cliente:
        ./build/bin/mainClient <ip del servidor> <puerto del servidor>

5. **Borrar los binarios**:
    ninja -C build clean

6. **Eliminar configuracion**:
    rm -rf build

LISTA DE COMANDOS:
Identificarse: id <username>
Establece el nombre de usuario con el que se identifica el cliente en el servidor.

Actualizar estado: sts <nuevo_estado>
Cambia el estado actual del usuario.

Listar usuarios: users
Solicita al servidor una lista de los usuarios conectados y su estado.

Desconectarse: exit
Cierra la conexión con el servidor y termina el cliente.

Enviar mensaje público: pb <mensaje>
Envía un mensaje público a todos los usuarios conectados al chat.

Enviar mensaje privado: txt <username> <mensaje>
Envía un mensaje privado a un usuario específico identificado por <username>.

Crear una nueva sala: mkRoom <nombre_de_la_sala>
Crea una nueva sala de chat con el nombre especificado.

Invitar a usuarios a una sala: invt <nombre_de_la_sala> <usuario1,usuario2,...>
Invita a uno o más usuarios a unirse a una sala de chat existente.


NOTAS:
No documente el servidor porque se me paso el tiempo
No implemente todo el protocolo, faltaron las partes de: JOIN_ROOM, ROOM_USERS, ROOM_TEXT, LEAVE_ROOM y LEFT_ROOM de DISCONNECTED
Olvide eliminar todo lo relacionado a las pruebas unitarias
Olvide eliminar las impresiones de depuracion del cliente