# LAB2 - Blackjack Antártico

## Integrantes:
- **Benjamin Navarrete** - 20824875-8  
- **Carolina Cartagena** - 19391725-9

---

## Descripción

Este proyecto es una implementación del clásico juego **Blackjack** adaptado al contexto del laboratorio 2 de Sistemas Operativos. El juego se ejecuta en la terminal y simula una partida entre un **crupier humano** y **cuatro bots**, cada uno ejecutado como un proceso hijo mediante `fork()`.

---

## Compilación

Para compilar el programa, asegúrate de tener `g++` instalado y usa:

```bash
make
```

---

## Ejecución

Para iniciar el juego:

```bash
./LAB2_TuApellido_SocioApellido
```

Se te pedirá ingresar la cantidad de rondas. El crupier (usuario humano) debe interactuar con el juego a través de la terminal, mientras que los bots jugarán automáticamente.

---

## Reglas implementadas

- Juego de Blackjack con 1 mazo de 52 cartas (sin comodines).
- El crupier juega como humano (controlado desde consola).
- Cada bot es ejecutado como un proceso hijo usando `fork()`.
- Se reparten 2 cartas al inicio a cada jugador.
- Bots deciden si pedir carta o plantarse según su puntaje:
  - [0-11]: siempre piden.
  - [12-18]: 50% de probabilidad de pedir.
  - [19-21]: no piden.
- Crupier decide manualmente si pedir o plantarse.
- Cada jugador que vence al crupier gana +1 punto.
- El crupier gana +1 si vence a la mayoría, +2 si gana a todos.

---

## Archivos incluidos

- `LAB2_TuApellido_SocioApellido.cpp`: implementación principal del juego.
- `Makefile`: sistema de compilación.
- `README.md`: este archivo.

---

## Supuestos y consideraciones

- El mazo se reinicia y mezcla al inicio de cada ronda.
- Los bots no se comunican entre sí ni con el crupier, solo imprimen su jugada.
- No se implementa apuesta, solo puntaje por ronda.
- Los empates se consideran como victorias para el jugador pero no dan puntos.

---

## Notas

- Se usaron estructuras de POO (Clases: `Carta`, `Mazo`, `Jugador`, `JugadorBot`, `Crupier`, `Juego`).
- Uso de `fork()` para simular independencia de decisiones en los bots.
- El código fue cuidadosamente estructurado en un solo archivo `.cpp` como lo indica el enunciado.
- Cumple con todos los requisitos y formato exigido por el profesor Viktor Tapia.

---

## Créditos

Desarrollado para el curso de Sistemas Operativos - 2025.

