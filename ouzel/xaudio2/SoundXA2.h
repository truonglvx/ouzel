// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <xaudio2.h>

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class AudioXA2;

        class SoundXA2: public Sound
        {
            friend AudioXA2;
        public:
            virtual ~SoundXA2();
            virtual void free() override;

            virtual bool init(const SoundDataPtr& newSoundData) override;

            virtual bool play(bool repeatSound = false) override;
            virtual bool stop(bool resetSound = false) override;
            virtual bool reset() override;

        protected:
            SoundXA2();

            IXAudio2SourceVoice* sourceVoice = nullptr;
        };
    } // namespace audio
} // namespace ouzel
