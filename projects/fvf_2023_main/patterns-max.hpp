
#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "ledsterPatterns.hpp"
#include "ledsterShapes.hpp"
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Max
{

    class ChevronsPattern : public Pattern<RGBA>
    {
        PixelMap3d map;
        LFO<SawDown> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade1 = FadeDown(1400, WaitAtEnd);

    public:
        ChevronsPattern(PixelMap3d map)
        {
            this->map = map;
            this->lfo = LFO<SawDown>(1000);
            this->lfoColour = LFO<Square>(1000);
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float amount = params->getIntensity(0.25, 4);
            lfo.setPeriod(params->getVelocity(2000, 500));
            lfoColour.setPeriod(params->getVariant(2000, 500));

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                float dir = map[index].z > 0 ? 1:-1;
                float phase = (0.5 * abs(map[index].x) - map[index].z + map[index].y * dir) * amount;
                auto col = lfoColour.getValue(phase) ? params->getSecondaryColour() : params->getPrimaryColour();
                pixels[index] += col * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class ChevronsConePattern : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical map;
        LFO<SawDown> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade1 = FadeDown(1400, WaitAtEnd);

    public:
        ChevronsConePattern(PixelMap3d map)
        {
            this->map = map.toCylindricalRotate90();
            this->lfo = LFO<SawDown>(1000);
            this->lfoColour = LFO<Square>(1000);
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float amount = params->getIntensity(0.25, 4);
            lfo.setPeriod(params->getVelocity(2000, 500));
            lfoColour.setPeriod(params->getVariant(2000, 500));

            for (int index = 0; index < std::min(width, (int)map.size()); index++)
            {
                float conePos = 0.5 + (map[index].r - map[index].z) / 2;

                float phase = conePos * amount; //(0.5 * abs(map[index].y) + map[index].x) * amount;
                auto col = lfoColour.getValue(phase) ? params->getSecondaryColour() : params->getPrimaryColour();
                pixels[index] += col * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        Permute perm;

    public:
        RadialGlitterFadePattern(PixelMap3d map)
        {
            this->map = map.toCylindricalRotate90();
            this->perm = Permute(map.size());
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = params->getIntensity(500, 100);

            // timeline.FrameStart();
            // if (timeline.Happened(0))
            if (watcher.Triggered())
            {
                fade.reset();
                perm.permute();
            }

            float velocity = params->getVelocity(600, 100);
            // float trail = params->getIntensity(0,1) * density;
            // float trail = params->getIntensity(0, 200);

            for (int i = 0; i < map.size(); i++)
            {
                // fade.duration = perm.at[i] * trail; // + 100;
                // fade.duration = (perm.at[i] * trail) < map.size() / 20 ? ; // + 100;

                // fade.duration = 100;
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= 4;

                float density = 481. / width;
                fade.duration = 100; // trail + perm.at[i] / (density * map.size()/ 10);
                if (perm.at[i] < density * map.size() / 10)
                    fade.duration *= perm.at[i] * 4 / (density * map.size() / 10);

                // vanaf de knik beide kanten op
                // float conePos = 1-(map[i].r + map[i].z)/2;
                // vanaf midden
                float conePos = 0.5 + (map[i].r - map[i].z) / 2;

                float fadePosition = fade.getValue(conePos * velocity);
                RGBA color = params->gradient->get(fadePosition * 255);
                pixels[i] = color * fadePosition * (1.5 - map[i].r) * transition.getValue();
            }
        }
    };

    class AngularFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();

    public:
        AngularFadePattern(PixelMap3d map)
        {
            this->map = map.toCylindricalRotate90();
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
                // perm.permute();
            }

            // float density = 481./width;
            float velocity = params->getVelocity(200, 30);
            // float trail = params->getIntensity(0, 200);

            for (int i = 0; i < map.size(); i++)
            {
                fade.duration = 80; // trail + perm.at[i] / (density * map.size()/ 10);
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= perm.at[i] * 4 / (density * map.size()/ 10);

                float fadePosition = fade.getValue(abs(map[i].th) * velocity);
                RGBA color = params->gradient->get(255 - abs(map[i].th) / M_PI * 255);
                pixels[i] = color * fadePosition * (map[i].r * 1.5) * transition.getValue();
                ;
            }
        }
    };

    class GrowingStrobePattern : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical map;
        FadeDown fade = FadeDown(5000, WaitAtEnd);

    public:
        GrowingStrobePattern(PixelMap3d::Cylindrical map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
            {
                fade.reset();
                return;
            }

            RGBA col = Utils::millis() % 100 < 25 ? params->getPrimaryColour() : RGBA();
            int directionUp = params->getVariant() > 0.5;

            for (int i = 0; i < map.size(); i++)
            {
                float conePos = 0.20 + (map[i].r - map[i].z) / 2;
                if (directionUp && conePos < fade.getValue())
                    continue;
                if (!directionUp && 1. - conePos < fade.getValue())
                    continue;

                pixels[i] = col;
            }
        }
    };

    class RadialFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();

    public:
        RadialFadePattern(PixelMap3d::Cylindrical map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            fade.duration = params->getIntensity(500, 120);
            int velocity = params->getVelocity(500, 50);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
            }

            for (int i = 0; i < map.size(); i++)
            {
                float conePos = 0.20 + (map[i].r - map[i].z) / 2;
                pixels[i] += params->getPrimaryColour() * fade.getValue(conePos* velocity) * transition.getValue();
            }
        }
    };

    
};