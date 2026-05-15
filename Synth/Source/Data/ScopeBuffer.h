/*
  ==============================================================================

    ScopeBuffer.h
    Author: Aitor

    Ring buffer thread-safe (single producer / single consumer) usado para
    pasar muestras desde el audio thread al GUI thread (osciloscopio).

  ==============================================================================
*/

#pragma once

#include <array>
#include <atomic>

struct ScopeBuffer
{
    // Potencia de 2 para enmascarar con (Size - 1) y evitar modulos
    static constexpr int Size = 4096;
    static constexpr int Mask = Size - 1;

    std::array<float, Size> data {};
    std::atomic<int>        writePos { 0 };
    std::atomic<double>     sampleRate { 48000.0 };

    // Llamado desde el audio thread
    inline void push(float s) noexcept
    {
        const int w = writePos.load(std::memory_order_relaxed);
        data[(size_t)w] = s;
        writePos.store((w + 1) & Mask, std::memory_order_release);
    }

    // Llamado desde el GUI thread. Copia las ultimas N muestras a 'dest'.
    inline void copyLastN(float* dest, int N) const noexcept
    {
        const int w = writePos.load(std::memory_order_acquire);
        for (int i = 0; i < N; ++i)
            dest[i] = data[(size_t)((w - N + i) & Mask)];
    }
};
