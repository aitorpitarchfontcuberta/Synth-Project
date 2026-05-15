# Guía de parámetros del sintetizador

Este documento explica qué hace cada parámetro del sintetizador y cómo su modificación afecta al sonido. Está organizado por pestaña/módulo.

---

## Pestaña 1 — Oscillators

Aquí se generan las ondas básicas que después se procesan. Es la **fuente** del sonido: cualquier cambio aquí cambia el timbre de raíz.

### OSC 1 / OSC 2 (idénticos)

**Wave** (forma de onda). Determina el **timbre puro** del oscilador antes de cualquier procesamiento:

- **Sine** — Tono puro, sin armónicos. Suena suave, redondo, "limpio". Útil para sub-bajos, leads etéreos y como portador en FM.
- **Triangle** — Pocos armónicos impares, decaen rápido. Suena dulce y nasal, parecido a una flauta o sub bien definido.
- **Saw** — Todos los armónicos (pares e impares). Es el sonido "clásico" de synth analógico: brillante, rico, ideal para leads, bajos, pads y supersaws.
- **Square** — Sólo armónicos impares. Suena hueco, "cuadrado", recuerda a un clarinete o a un videojuego de 8 bits.
- **Pulse** — Como Square pero con el ancho de pulso variable (ver *Pulse W*). Permite modular el timbre dinámicamente.
- **Noise** — Ruido blanco. No tiene tono — se usa para percusión, efectos, viento, atmósferas, o mezclado con una onda tonal para dar "aire".

**Octave** (`−4` a `+4`). Desplaza el oscilador en octavas enteras. Cada paso duplica (`+1`) o divide a la mitad (`−1`) la frecuencia. Útil para colocar OSC2 una octava abajo (sonido más gordo) o arriba (brillo).

**Semi** (semitonos, `−12` a `+12`). Desplaza en semitonos. Es como mover el oscilador por la escala cromática. Combinaciones musicales típicas:
- `+7` (quinta justa) → sonido "épico" tipo orquestal.
- `+5` (cuarta justa) → tensión, eclesiástico.
- `+12` (octava) → equivale a *Octave +1*.
- `+3` o `+4` → terceras (menor / mayor) para apilar acordes.

**Fine** (centésimas de semitono, `−100` a `+100`). Detune fino. Si pones OSC2 a `+7 ct` respecto a OSC1, los dos osciladores se desafinan ligeramente y aparece un **batido** lento que da grosor (efecto "chorus natural"). Es uno de los trucos clásicos para que un synth suene "ancho".

**Pulse W** (Pulse Width, `0.05`–`0.95`). Sólo afecta si la forma de onda es **Pulse**. Controla el porcentaje del ciclo en que la onda está "arriba":
- `0.5` → equivalente a una cuadrada perfecta.
- Valores extremos (cerca de 0.05 o 0.95) → tono más "nasal" y armónicos altos. Muy usado para sonidos tipo "ahora vienen los 80".
- Modularlo con un LFO produce el efecto **PWM**, característico de strings y pads vintage.

El visualizador encima de los knobs te muestra en tiempo real la forma exacta que se está generando.

### MIXER

**Mode** (Mix / Ring / Sync / FM). Define **cómo se combinan** OSC1 y OSC2:

- **Mix** — Suma ponderada simple. Es lo "normal": OSC1 y OSC2 suenan a la vez según el control *Osc Mix*.
- **Ring** — Modulación en anillo: `OSC1 × OSC2`. Genera frecuencias suma y diferencia, sonido metálico, "campanudo", a menudo inarmónico. Perfecto para sonidos tipo campana, robóticos, FX raros.
- **Sync** — Hard sync. OSC1 (master) resetea la fase de OSC2 (slave) cada ciclo. Si afinas OSC2 más alto que OSC1, obtienes ese sonido "lead chillón" tipo *The Cars*, *Van Halen* o leads de Moog.
- **FM** — OSC2 modula la fase de OSC1. Genera espectros muy ricos e inarmónicos. Suena a "synth digital de los 80" tipo DX7. La cantidad la controlas con *FM Amt*.

**Sub W / Sub Lvl**. El sub-oscilador es un oscilador extra **una octava por debajo** de la nota tocada. Su único parámetro tonal es su forma de onda; *Sub Level* controla cuánto se mezcla. Es la forma más rápida de añadir cuerpo y bajo a cualquier sonido. Muy típico en bajos electrónicos.

**Osc Mix** (`0`–`1`). En modos **Mix** y **Sync**, controla el balance: `0` = solo OSC1, `1` = solo OSC2, `0.5` = a partes iguales. En **Ring** y **FM** este parámetro no se aplica (ahí el modo define cómo se combinan internamente).

**FM Amt** (`0`–`1`). Solo activo en modo **FM**. A `0` no hay FM (suena solo OSC1). A medida que sube, el espectro se vuelve cada vez más complejo e inarmónico. Modularlo con un envelope (ver *Mod Env*) es lo que da los famosos *FM bells*: claro al inicio, oscuro después.

---

## Pestaña 2 — Modulation

Las modulaciones son lo que hace que un sonido **se mueva en el tiempo**: vibrato, tremolo, barridos, PWM, etc. Sin modulación, todos los sonidos serían estáticos.

### LFO 1 y LFO 2 (idénticos)

Un LFO (Low Frequency Oscillator) es un oscilador lento — produce una señal cíclica que se usa para mover otros parámetros, no para oírlo directamente.

**Wave**. Igual que en los osciladores pero más limitado: Sine, Triangle, Saw, Square y S&H (Sample & Hold = valores aleatorios escalonados). La forma define el tipo de movimiento:
- **Sine / Triangle** — Modulación suave, oscilante. Vibrato, tremolo orgánico.
- **Saw** — Subida progresiva y caída instantánea. Crea barridos repetitivos.
- **Square** — Salto entre dos valores. Trémolos bruscos, efectos "robotizados".
- **S&H** — Movimiento aleatorio. Sonidos tipo "computer", "bubbles", random pitches.

**Rate** (`0.01`–`30 Hz`). Velocidad del LFO. Por debajo de 1 Hz son barridos muy lentos (cinco segundos por ciclo). Cerca de 5–8 Hz suena a vibrato natural. Por encima de 15 Hz entra en zona de "modulación audible" cuando se aplica a pitch (suena casi como FM lenta).

**Depth** (`0`–`1`). Cantidad de modulación. A 0 no modula nada (aunque el LFO siga corriendo internamente).

**Dest**. A dónde se aplica la modulación:
- **None** — Desactivado.
- **Pitch** — Modula la afinación de ambos osciladores. Con Sine + rate bajo → vibrato. Con S&H → pitch aleatorios.
- **Osc1 PW / Osc2 PW** — Modula el pulse width de cada oscilador. Aplicado al modo *Pulse* con LFO triangle a 0.3 Hz → el clásico PWM de pads ricos.
- **Amp** — Modula el volumen → tremolo.
- **Mix** — Modula la mezcla OSC1↔OSC2 → cambios cíclicos de timbre.
- **FM Amt** — Solo si el modo es FM → modulación de la intensidad de la FM, espectros que respiran.

### MOD ENV

Un segundo envelope ADSR (igual que el de amplitud) pero con destino libre — no afecta al volumen por defecto. Sirve para que un parámetro **siga una forma** cuando tocas cada nota.

**A / D / S / R**. Igual que en el envelope de amplitud (ver pestaña Envelope).

**Depth** (`−1` a `+1`). Cuánto afecta este envelope al destino. Puede ser **negativo** (envelope invertido). Ejemplo: depth negativo con dest=Pitch → cada nota empieza más aguda y "cae" al tono real (efecto pitch-drop, muy usado en sonidos de bombo de techno).

**Dest**. Mismos destinos que los LFOs. Usos típicos:
- Dest = **FM Amt** → bells brillantes que se vuelven oscuras (FM clásica).
- Dest = **Pitch** + depth negativo → kick drum sintético.
- Dest = **Osc1/2 PW** → barrido de timbre.

El visualizador del envelope se actualiza en tiempo real al girar los knobs.

---

## Pestaña 3 — Envelope (Amp ADSR)

El envelope de amplitud define **cómo cambia el volumen** desde que pulsas una tecla hasta que la sueltas. Tiene cuatro fases:

**Attack** (`0.001`–`5 s`). Tiempo desde que pulsas la tecla hasta que el volumen llega al máximo.
- `0.001 s` → Ataque instantáneo (pluck, percusión).
- `0.01–0.1 s` → Sonidos "rápidos" tipo piano eléctrico, organ, lead.
- `1–3 s` → Pads y strings que entran lentamente.

**Decay** (`0.001`–`5 s`). Tiempo desde el pico de attack hasta llegar al nivel de sustain. Si sustain está al máximo, este parámetro no se nota porque no hay nada que decaer.

**Sustain** (`0`–`1`). Nivel al que se mantiene la nota mientras la tecla está pulsada (después de attack y decay).
- `0` → La nota se apaga totalmente tras decay (sonido percusivo: piano, marimba, bajo plucked).
- `1` → La nota se mantiene a volumen pleno mientras la pulses (órgano, pad sostenido).
- Valores intermedios → comportamiento mixto.

**Release** (`0.001`–`5 s`). Tiempo desde que sueltas la tecla hasta que el volumen llega a 0. Releases largos hacen que las notas se "evaporen". Cortos las cortan en seco.

El **visualizador** dibuja la curva A-D-S-R en tiempo real y los puntos clave (peak, sustain, release). Es la mejor forma de entender qué estás haciendo.

---

## Pestaña 4 — Master / Voice

Aquí están los controles globales que afectan a todo el sintetizador a alto nivel.

**Gain** (`0`–`1`). Volumen general de salida.

**Vel** (Velocity Sensitivity, `0`–`1`). Cuánto afecta la fuerza con la que pulsas la tecla al volumen.
- `0` → Velocity ignorada, todas las notas suenan igual de fuerte (típico de bajos, leads "fijos").
- `1` → Velocity totalmente activa: notas suaves suenan en silencio, notas fuertes a máximo (expresividad tipo piano).
- Intermedios → comportamiento natural.

**Glide** (Portamento, `0`–`2 s`). Tiempo de "deslizamiento" entre notas. A 0 no hay glide. A 0.2 s las notas se interpolan suavemente entre sí — muy usado en bajos de reggae, leads de TB-303 o sintetizadores expresivos.

**Voice** (Voice Mode). Modo de polifonía:
- **Poly** — Polifónico. Cada nota usa una voz independiente. Puedes tocar acordes.
- **Mono** — Una sola voz a la vez. Cada nota retriguerea las envolventes.
- **Legato** — Una sola voz, pero **no** retriguerea las envolventes si la nota anterior aún suena. Permite frasear suavemente (típico en solos de synth).

### Unison

Apila varias voces del mismo oscilador con detune para crear sonido "ancho".

**Unison** (`1`–`7`). Número de voces apiladas por nota.
- `1` → Sin unison (una sola voz por nota, como siempre).
- `3` → Sonido más grueso, ligero detune.
- `7` → "Supersaw" estilo trance.

**Detune** (`0`–`50 ct`). Cuánto se desafinan entre sí las voces de unison. A 0 cents todas suenan iguales y no aporta nada. A 10–20 cents da grosor sin que suene desafinado. A 50 cents puede sonar deliberadamente roto y agresivo.

**Spread** (`0`–`1`). Reservado para spread estéreo (separación L/R de las voces de unison). El visualizador muestra cómo se distribuyen las voces en el espacio.

El **visualizador** muestra cada voz como un círculo, desplazado horizontalmente según el detune y verticalmente según el spread. Cuantos más círculos, más voces; cuanto más separados, más "ancho" suena.

---

## Cómo se enrutan las modulaciones internamente

El motor lee, **por cada muestra**:

1. LFO1, LFO2 y ModEnv generan un valor en `[−1, +1]` (los LFOs) o `[0, 1]` (envelope, escalado por `Depth` que puede ser negativo).
2. Cada uno aporta su valor al **destino seleccionado** (Pitch, PW, Amp, Mix, FM Amt). Si varios apuntan al mismo destino, **se suman**.
3. Los osciladores leen los valores ya modulados y generan la muestra.
4. Se aplica el envelope de amplitud, velocity y master gain.

Esto significa que puedes hacer combinaciones potentes: por ejemplo LFO1 a Pitch con depth bajo (vibrato sutil), LFO2 a PW con rate medio (PWM), y ModEnv a FM Amt con depth alto (FM que decae). Todo a la vez.

---

## Recetas rápidas

**Bajo simple "subby"**
- OSC1: Sine, Octave −1
- OSC2: Sine, Octave −1, Fine +5
- Mode: Mix, Mix 0.5
- Sub: Sine, Level 0.5
- Amp ADSR: A 0.002, D 0.3, S 0.2, R 0.1

**Pad rico con PWM**
- OSC1: Pulse, PW 0.4
- OSC2: Pulse, PW 0.6, Fine +7
- LFO1: Triangle, Rate 0.25 Hz, Depth 0.5, Dest Osc1 PW
- LFO2: Triangle, Rate 0.3 Hz, Depth 0.5, Dest Osc2 PW
- Amp ADSR: A 1.5, D 0.5, S 0.8, R 2.0

**Lead con FM brillante que decae**
- OSC1: Sine, OSC2: Sine, OSC2 Octave +1
- Mode: FM, FM Amt 0.0
- Mod Env: A 0.001, D 0.8, S 0.0, R 0.3, Depth +1, Dest FM Amt
- Amp ADSR: A 0.005, D 0.2, S 0.7, R 0.3

**Kick electrónico**
- OSC1: Sine
- Sub: 0
- Mod Env: A 0.001, D 0.06, S 0, R 0.05, Depth −1, Dest Pitch
- Amp ADSR: A 0.001, D 0.3, S 0, R 0.1
