#pragma once
#include "animation.hpp"
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>

class VideoPattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    int frame = 0;
    Animation *animation;

public:
    VideoPattern(Animation *animation)
    {
        this->name = "Video";
        this->animation = animation;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        frame++;
        if (frame > animation->frames)
            frame = 0;

        for (int i = 0; i < width; i++)
        {
            int animationIndex = (frame * animation->pixels + i) * animation->depth;
            pixels[i] = (RGBA)RGB(
                            animation->pixelData[animationIndex + 0],
                            animation->pixelData[animationIndex + 1],
                            animation->pixelData[animationIndex + 2]) *
                        transition.getValue();
        }
    }
};

class VideoPalettePattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    int frame = 0;
    Animation *animation;

public:
    VideoPalettePattern(Animation *animation)
    {
        this->name = "Video palette";
        this->animation = animation;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        frame++;
        if (frame > animation->frames)
            frame = 0;

        for (int i = 0; i < width; i++)
        {
            int animationIndex = (frame * animation->pixels + i) * animation->depth;

            pixels[i] += params->getPrimaryColour() * (float(animation->pixelData[animationIndex + 0]) / 255.);
            //pixels[i] += params->getSecondaryColour() * (float(animation->pixelData[animationIndex + 1]) / 255.);
            //pixels[i] += params->getHighlightColour() * (float(animation->pixelData[animationIndex + 2]) / 255.);

            pixels[i] = pixels[i] * transition.getValue();
        }
    }
};