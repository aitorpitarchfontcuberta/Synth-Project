# Hoja de ruta de mejoras del sintetizador

Listado de extensiones posibles ordenadas por prioridad recomendada. Cada bloque incluye qué aporta, cómo se implementaría a grandes rasgos y una estimación de esfuerzo.

---

## Bloque 1 — Lo más fundamental que aún falta

### 1. Filtros (con visualizador de respuesta)

Es lo que pospusimos al principio del proyecto y ahora es el siguiente paso natural. Un sintetizador subtractivo se llama así precisamente porque los filtros *sustraen* armónicos de la señal generada por los osciladores. Sin filtros, no hay forma de moldear el timbre dinámicamente.

Qué añadir:

- Filtro multi-modo (LP / HP / BP / Notch) con cutoff y resonance.
- Drive opcional antes del filtro para saturación analógica.
- Filtro envelope dedicado (otro ADSR independiente que afecte al cutoff).
- Visualizador de la respuesta en frecuencia: el usuario gira el cutoff y ve la curva moverse en directo.
- Como bonus didáctico: solapar la curva del filtro sobre el spectrum del output. El usuario verá literalmente "esto es lo que genera el oscilador, y esto es lo que el filtro deja pasar".
- Añadir destinos nuevos a LFOs y ModEnv: `Filter Cutoff` y `Filter Resonance`.

Esfuerzo medio. Valor enorme. Cierra el círculo subtractivo.

### 2. Sección de efectos (FX)

Reverb, Delay, Chorus y Distortion básicos. Toda la cadena habitual de un synth comercial. JUCE ya trae `juce::dsp::Reverb`, `DelayLine`, `Chorus`, etc., así que no es código difícil.

Para que sea didáctico, cada efecto puede tener un visualizador "antes/después" que muestre el espectro o la forma de onda con y sin el efecto activo.

Esfuerzo medio. Valor alto. Sin reverb todo suena seco y plástico.

---

## Bloque 2 — Mejoras de uso y musicalidad

### 3. Sistema de presets

Salvar y cargar patches con nombre. Banco de fábrica con 10-15 sonidos representativos (bajos, leads, pads, bells FM, plucks, drones, secuencias). Browser de presets con categorías. Botones de "previous / next / save / save as".

Sin esto, cada vez que abres el plugin partes de cero. Es una mejora que se nota muchísimo en el día a día y convierte al plugin en algo que se puede *enseñar*: cada preset es una lección.

Esfuerzo medio.

### 4. MIDI Learn / CC mapping

Click-derecho sobre cualquier knob → "Learn MIDI CC" → mueves un controlador externo y queda asignado. Convierte el plugin en algo realmente tocable con hardware externo.

JUCE proporciona casi todo lo necesario a través del APVTS y `MidiKeyboardState`. Esfuerzo bajo.

### 5. Macro controls

Cuatro knobs grandes en la parte superior o en una pestaña dedicada. Cada macro puede modular varios parámetros internos a la vez con cantidades configurables (positivas o negativas).

Permite, por ejemplo, que un único knob "abra" el filtro, suba el unison detune y añada reverb simultáneamente. Es la mejor forma de tener controles de performance sin malabarismos con varios knobs.

Esfuerzo bajo-medio.

### 6. Arpegiador / Step sequencer

Un arpegiador simple (Up / Down / UpDown / Random) con rate (sincronizable al tempo del host) y range (1-4 octavas). Opcionalmente, un step sequencer corto (8-16 pasos) con pitch y gate por paso.

Es un añadido que de repente vuelve el plugin mucho más divertido de tocar.

Esfuerzo medio.

---

## Bloque 3 — Mejoras puramente didácticas

### 7. Tooltips informativos

Al pasar el ratón por cualquier knob, mostrar una mini-explicación del parámetro y su efecto. Reaprovecharía el contenido del documento `PARAMETROS.md` que ya tenemos.

Esfuerzo bajo.

### 8. Indicador de notas activas

Una mini ventana que muestra qué notas MIDI están sonando ahora mismo y a qué frecuencia exacta (con detune, glide y pitch wheel aplicados). El usuario ve, por ejemplo, "A4 = 440.0 Hz → 442.3 Hz con detune +9 ct".

Esfuerzo bajo.

### 9. Cursor sobre el spectrum y el waveform

Al pasar el ratón por el panel de output mostrar la frecuencia exacta (en el spectrum) o el tiempo (en el waveform) bajo el cursor. Convierte el panel en una herramienta de análisis real.

Esfuerzo bajo.

---

## Bloque 4 — Mejoras técnicas (más profesionales que didácticas)

### 10. Aftertouch y sustain pedal

Aftertouch como fuente de modulación (otro destino más en LFOs y ModEnv). Sustain pedal (CC64) capturado para que las notas no se apaguen mientras esté pulsado.

Esfuerzo bajo.

### 11. Oversampling para distorsiones futuras

Cuando se añada saturación o distortion, hacerlo a 2x o 4x la sample rate y filtrar el alias. Solo relevante si añadimos no-linealidades fuertes. JUCE proporciona `juce::dsp::Oversampling`.

Esfuerzo bajo.

### 12. UI redimensionable

Que el usuario pueda agrandar o reducir el plugin desde una esquina. Más cosmético que funcional pero útil. Esfuerzo medio.

### 13. Modulación matricial completa

Sistema más flexible donde cualquier fuente (LFO1, LFO2, ModEnv, Velocity, Aftertouch, ModWheel, Macro1-4) puede ir a cualquier destino (cualquier knob continuo del plugin) con cantidad ajustable.

Es la forma más potente de organizar la modulación. Reemplazaría el sistema actual de "un destino por fuente". Esfuerzo medio-alto.

### 14. MPE (MIDI Polyphonic Expression)

Per-note pitch bend, slide y pressure. Útil solo si se tiene un controlador MPE (Linnstrument, Seaboard, ROLI). Esfuerzo medio-alto.

### 15. Wavetable synthesis

Tablas de onda personalizables con morphing entre formas, posición dentro de la tabla modulable, posibilidad de cargar tablas desde archivos. Implica un sistema completamente nuevo de generación. Esfuerzo alto.

---

## Recomendación priorizada

Este sería el orden de implementación que tendría más sentido para el proyecto.

**Paso 1 — Filtros con visualizador de respuesta.** Cierra el círculo subtractivo. Sin esto el synth se siente "incompleto" técnicamente. El visualizador de respuesta del filtro solapado con el spectrum del output es probablemente la cosa más didáctica que se puede añadir al proyecto.

**Paso 2 — Sistema de presets + banco de fábrica.** Una vez haya filtros, podemos preparar 10-15 patches que muestren todo lo que el synth sabe hacer. De repente el plugin se vuelve algo que se puede *enseñar*.

**Paso 3 — Efectos (Reverb + Delay + Chorus mínimo).** Es lo que falta para que el sonido tenga "espacio". Sin reverb todo suena seco y plástico.

**Paso 4 — Macros + MIDI Learn.** Para tocabilidad real.

**Paso 5 — Tooltips + indicador de notas + cursor sobre el spectrum.** Toques didácticos finales.

A partir de aquí ya se entra en territorio de "extensiones" que dependen de los gustos del autor: arpegiador, wavetables, modulación matricial, MPE.
