# Chat Cliente-Servidor en C++

Este proyecto es una implementación de un chat cliente-servidor en C++ utilizando Meson, Ninja, y Google Test (gtest) para las pruebas unitarias.

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
        ./build/bin/servidor
    Despues de ejecutar el servidor, en otra terminal ejecutas el cliente:
        ./build/bin/cliente

5. **Pruebas unitarias el proyecto**
    ```bash
    ninja -C build
    meson test -C build

6. **Limpiar**:
    ninja -C build clean

7. **Eliminar configuracion**:
    rm -rf build