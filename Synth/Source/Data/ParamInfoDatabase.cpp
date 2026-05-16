/*
  ==============================================================================

    ParamInfoDatabase.cpp

  ==============================================================================
*/

#include "ParamInfoDatabase.h"
#include <unordered_map>

namespace ParamInfoDatabase {

// Normaliza el ID: OSC1/OSC2 -> OSC, LFO1/LFO2 -> LFO. Asi una sola entrada
// cubre los dos osciladores y los dos LFOs.
static juce::String normalize(const juce::String& id)
{
    auto s = id;
    if (s.startsWith("OSC1")) s = "OSC" + s.substring(4);
    else if (s.startsWith("OSC2")) s = "OSC" + s.substring(4);
    if (s.startsWith("LFO1")) s = "LFO" + s.substring(4);
    else if (s.startsWith("LFO2")) s = "LFO" + s.substring(4);
    return s;
}

static const std::unordered_map<std::string, ParamInfo>& getMap()
{
    static const std::unordered_map<std::string, ParamInfo> m = {
        // ===================== OSCILLATOR =====================
        { "OSCWAVETYPE", {
            "Wave Type", "Oscillator",
            "Forma de onda que produce el oscilador. Es la fuente del timbre, antes de cualquier procesamiento.",
            "(Selector, no es continuo). Sine: solo la fundamental, sin armonicos. Triangle: armonicos impares decaen rapido, dulce. Saw: todos los armonicos, brillante. Square: solo armonicos impares, hueco. Pulse: como Square pero con ancho variable. Noise: sin tono, ruido blanco.",
            "",
            "Saw para leads y bajos clasicos. Sine para sub-bajos limpios. Pulse modulado por LFO da el sonido tipico de pads vintage."
        }},
        { "OSCOCTAVE", {
            "Octave", "Oscillator",
            "Desplaza el oscilador en octavas enteras. Cada +1 duplica la frecuencia, cada -1 la divide a la mitad.",
            "Sonido mas agudo, con mas brillo.",
            "Sonido mas grave, con mas cuerpo.",
            "Pon OSC2 una octava debajo de OSC1 para obtener un sonido mas grueso, o una arriba para anadir brillo."
        }},
        { "OSCSEMI", {
            "Semitone", "Oscillator",
            "Desplaza el oscilador en semitonos (la unidad musical). 12 semitonos = 1 octava.",
            "Sube la afinacion. Combinaciones musicales: +5 (cuarta), +7 (quinta justa), +12 (octava).",
            "Baja la afinacion. -5, -7 y -12 son las opciones musicalmente coherentes.",
            "OSC2 a +7 semitonos respecto a OSC1 produce un sonido tipo organo/quinta epica."
        }},
        { "OSCFINE", {
            "Fine Tune", "Oscillator",
            "Desafinacion fina en centesimas de semitono (cents). 100 cents = 1 semitono.",
            "Sube la afinacion suavemente. Si OSC1 y OSC2 difieren en pocos cents, aparecen batidos que dan grosor (efecto chorus natural).",
            "Baja la afinacion. Mismos efectos en negativo.",
            "5-10 cents entre OSC1 y OSC2 es el truco clasico para que un synth suene 'ancho' sin que parezca desafinado."
        }},
        { "OSCPW", {
            "Pulse Width", "Oscillator",
            "Ancho del pulso. Solo afecta cuando la forma de onda es Pulse: define que porcentaje del ciclo esta 'arriba'.",
            "El pulso es mas asimetrico (mas tiempo arriba). Cerca del 95%, el timbre se vuelve nasal y agudo.",
            "Pulso muy delgado. Bajo (cerca de 5%), suena tambien nasal pero mas brillante.",
            "0.5 = onda cuadrada perfecta. Modulalo con un LFO triangle lento para conseguir el efecto PWM clasico de pads."
        }},

        // ===================== MIXER =====================
        { "OSC_MIX", {
            "Osc Mix", "Mixer",
            "Balance entre OSC1 y OSC2 en modo Mix o Sync.",
            "Mas OSC2 (totalmente a la derecha: solo OSC2 suena).",
            "Mas OSC1 (totalmente a la izquierda: solo OSC1 suena).",
            "0.5 es a partes iguales. En Ring y FM este parametro no se aplica (esos modos tienen su propia logica)."
        }},
        { "OSC_MODE", {
            "Osc Mode", "Mixer",
            "Define como se combinan OSC1 y OSC2.",
            "(Selector). Mix: suma simple. Ring: OSC1 x OSC2, sonido metalico inarmonico. Sync: OSC1 resetea la fase de OSC2 cada ciclo, sonido 'lead chillon'. FM: OSC2 modula la fase de OSC1, espectros muy ricos tipo DX7.",
            "",
            "Empieza en Mix; cambia a Ring para sonidos tipo campana; Sync para leads con caracter; FM para timbres digitales."
        }},
        { "FM_AMOUNT", {
            "FM Amount", "Mixer",
            "Intensidad de la modulacion de frecuencia. Solo activa en modo FM.",
            "Mas FM. El espectro se llena de armonicos inarmonicos (sidebands). A maximo, sonido muy metalico y complejo.",
            "Menos FM. A 0, solo se oye OSC1 limpio.",
            "Asigna ModEnv -> FM Amt con depth positivo y un decay corto para conseguir el clasico 'FM bell' brillante que decae."
        }},
        { "SUB_LEVEL", {
            "Sub Level", "Mixer",
            "Nivel del sub-oscilador, que toca una octava por debajo de la nota.",
            "Mas peso en los graves. Tipico de bajos electronicos.",
            "Menos sub. A 0, el sub esta inactivo.",
            "Anade 0.3-0.7 de sub a un bajo para que se note en sistemas grandes. Para pads suele estar a 0."
        }},
        { "SUB_WAVE", {
            "Sub Wave", "Mixer",
            "Forma de onda del sub-oscilador.",
            "(Selector). Sine es la opcion mas limpia y profunda. Square anade mas armonicos al grave.",
            "",
            "Sine es lo habitual. Si quieres que el sub sea un poco mas brillante, prueba Triangle o Square."
        }},

        // ===================== FILTER =====================
        { "FILTER_TYPE", {
            "Filter Type", "Filter",
            "Tipo de filtro. Define que rango de frecuencias deja pasar y cual corta.",
            "(Selector). LP: deja pasar las bajas, recorta las altas (oscurece). HP: al reves, recorta las bajas. BP: solo una banda alrededor del cutoff. Notch: corta una banda estrecha (opuesto al BP).",
            "",
            "LP es el mas usado en synths analogicos. HP para clarificar pads. BP para sonidos 'telefonicos'. Notch + LFO da un efecto tipo phaser."
        }},
        { "FILTER_CUTOFF", {
            "Filter Cutoff", "Filter",
            "Frecuencia de corte del filtro. En LP es la frecuencia desde la que se empiezan a recortar las altas; en HP, las bajas.",
            "Mas armonicos pasan. El sonido se vuelve mas brillante.",
            "Menos armonicos pasan. El sonido se vuelve mas oscuro o velado.",
            "Modula esto con un envelope (ModEnv -> Cutoff) para que cada nota empiece brillante y se vaya cerrando: el 'filter envelope' clasico."
        }},
        { "FILTER_RES", {
            "Filter Resonance", "Filter",
            "Realce en la frecuencia de corte. Tecnicamente es el factor Q del filtro.",
            "Pico cada vez mas marcado alrededor del cutoff. Cerca del maximo, el filtro 'canta' (autooscila): genera un tono propio.",
            "Filtro suave, sin enfasis. Curva natural.",
            "Resonance media (2-4) anade caracter a leads y bajos. Maxima resonance es para efectos mas extremos."
        }},
        { "FILTER_DRIVE", {
            "Filter Drive", "Filter",
            "Saturacion tipo analogica antes del filtro. Anade armonicos y comprime la dinamica.",
            "Sonido mas grueso, agresivo y con mas presencia. A maximo, distorsion notable.",
            "Sin saturacion, sonido limpio.",
            "Activa drive ligero (2-3) en bajos para darles cuerpo. Combinado con resonance alta puede sonar muy agresivo."
        }},
        { "FILTER_KEYTRACK", {
            "Filter Key Track", "Filter",
            "Seguimiento de teclado. Hace que el cutoff suba/baje segun la nota tocada.",
            "Cada nota mas aguda abre el filtro proporcionalmente. A 1.0, el cutoff sigue perfectamente el pitch.",
            "Cutoff fijo independientemente de la nota.",
            "Activa key track (0.5-1.0) si quieres que las notas agudas no suenen apagadas. Util en leads."
        }},

        // ===================== AMP ADSR =====================
        { "ATTACK", {
            "Attack", "Amp Envelope",
            "Tiempo desde que pulsas la tecla hasta que el volumen llega al maximo.",
            "Attack mas lento. El sonido entra suavemente. Tipico de pads y strings (1-3 s).",
            "Attack mas rapido. A 0.001 s es instantaneo: sonidos percusivos tipo pluck.",
            "Pad: 1-3 s. Lead: 0.005-0.05 s. Pluck/percusion: 0.001-0.005 s."
        }},
        { "DECAY", {
            "Decay", "Amp Envelope",
            "Tiempo desde que el volumen llega al maximo (despues del attack) hasta que baja al nivel de sustain.",
            "Decay mas lento. La caida tras el attack es mas suave.",
            "Decay mas rapido. La caida es brusca. Si Sustain=0, define cuanto dura la nota.",
            "Si Sustain esta al maximo, este parametro no se nota. Para plucks: decay corto + sustain bajo."
        }},
        { "SUSTAIN", {
            "Sustain", "Amp Envelope",
            "Nivel al que se mantiene la nota mientras pulsas la tecla.",
            "Sonido mantenido a volumen alto. Tipico de organo, pads, sostenidos.",
            "Sonido se apaga tras el decay. A 0, perfecto para plucks/percusion (piano, marimba).",
            "Sustain a 1.0 = sonido mantenido. Sustain a 0 = sonido percusivo (puede ignorar Decay)."
        }},
        { "RELEASE", {
            "Release", "Amp Envelope",
            "Tiempo desde que sueltas la tecla hasta que el volumen llega a 0.",
            "El sonido tarda mas en apagarse al soltar. Crea una 'cola' que se evapora.",
            "El sonido se corta en seco al soltar.",
            "Release largo (1-3 s) para pads y leads atmosfericos. Release corto (0.1-0.3 s) para bajos y plucks."
        }},

        // ===================== MOD ENV =====================
        { "MENV_ATTACK",  { "Mod Attack",  "Mod Envelope", "Tiempo de ataque del envelope de modulacion.", "Crecimiento mas lento al pulsar tecla.", "Crecimiento instantaneo.", "Usar con dest=Cutoff para crear un 'filter sweep' que se abre al inicio de la nota." } },
        { "MENV_DECAY",   { "Mod Decay",   "Mod Envelope", "Tiempo desde el pico hasta sustain.", "Caida del envelope mas larga.", "Caida brusca.", "Para 'filter envelope' clasico: depth +, sustain 0, decay 0.2-0.5 s." } },
        { "MENV_SUSTAIN", { "Mod Sustain", "Mod Envelope", "Nivel sostenido del envelope mientras pulsas tecla.", "Mantiene la modulacion alta.", "Modulacion baja al sostener. A 0, el envelope decae a 0 y se queda ahi.", "Para envelope tipo 'pluck' del filtro: sustain a 0." } },
        { "MENV_RELEASE", { "Mod Release", "Mod Envelope", "Tiempo de release del envelope tras soltar.", "Modulacion residual mas larga.", "Modulacion se corta al soltar.", "Suele ser similar al release del amp para que el efecto coincida con la nota." } },
        { "MENV_DEPTH", {
            "Mod Depth", "Mod Envelope",
            "Intensidad del envelope. Puede ser negativo (modulacion invertida).",
            "Mayor influencia sobre el destino.",
            "Menor influencia. Con depth NEGATIVO, el envelope se invierte: util para crear pitch-drops (kick electronico).",
            "Depth + con dest=Cutoff: filter sweep abriendo. Depth - con dest=Pitch: pitch que cae (bombo electronico)."
        }},
        { "MENV_DEST", {
            "Mod Destination", "Mod Envelope",
            "Que parametro modifica el envelope.",
            "(Selector). Pitch, PW de los osciladores, Amp, Mix, FM Amount, Cutoff, Resonance.",
            "",
            "Cutoff es el destino mas comun (filter envelope). FM Amt con depth + crea bells brillantes. Pitch + depth - hace 'kicks' electronicos."
        }},
    };
    return m;
}

ParamInfo getInfo(const juce::String& paramID)
{
    const auto& m = getMap();

    // Primero: busca el ID exacto.
    auto it = m.find(paramID.toStdString());
    if (it != m.end()) return it->second;

    // Segundo: normaliza (OSC1/2 -> OSC, LFO1/2 -> LFO) y busca de nuevo.
    const auto norm = normalize(paramID);
    if (norm != paramID)
    {
        it = m.find(norm.toStdString());
        if (it != m.end()) return it->second;
    }

    return {};
}

} // namespace ParamInfoDatabase
