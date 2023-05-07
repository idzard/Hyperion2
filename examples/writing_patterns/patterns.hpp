#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>

namespace ExamplePatterns
{

    //////////////////
    // Basics
    //////////////////

    class HelloWorld : public Pattern<RGBA>
    {
    // This example will paint all leds red. It will show you the minimal code you need in a pattern

    public:
        HelloWorld()
        {
            // The name is displayed in the controller
            this->name = "Hello world";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            // ControlHubInput will run this Calculate method on patterns it knows about. 
            // If the active parameter is false, this pattern should not be displayed.
            // That is why we immediately return here. In some examples below you will see
            // how we can use this to create fade-outs or wait for an animation to finish
            // before stopping the rendering. (eg FadeFinishPattern))
            if (!active)
                return;

            // loop trough all leds and make them red.
            for (int i = 0; i < width; i++)
                pixels[i] = RGBA(255,0,0,255);
        }
    };

    class Palette : public Pattern<RGBA>
    {
    public:
        Palette()
        {
            this->name = "Palette colours";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // You can access the colours in the selected palette though the params argument.
            for (int i = 0; i < width; i++){
                if (i < width /3)
                    pixels[i] = params->getPrimaryColour();
                else if (i < 2*width /3)
                    pixels[i] = params->getSecondaryColour();
                else
                    pixels[i] = params->getHighlightColour();
            }
        }
    };

    class PaletteGradient : public Pattern<RGBA>
    {
    public:
        PaletteGradient()
        {
            this->name = "Palette gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // You can also access the gradient. It needs a parameter in the range 0-255
            for (int i = 0; i < width; i++){
                pixels[i] = params->gradient->get(255 * i / width);
            }
        }
    };


    class Blending : public Pattern<RGBA>
    {
    public:
        Blending()
        {
            this->name = "Color blending";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            RGBA red = RGBA(255,0,0,255);
            RGBA blue = RGBA(0,0,255,255);

            for (int i = 0; i < width; i++){
                // Alpha blending is really easy. You can just multiply an RGBA color with a float to
                // make it partially transparent.
                // Here i use a base of red, with a blue layer on top of it that fades from transparent
                // to fully opaque.
                pixels[i] = red + blue * (float(i)/width);
            }
        }
    };


    class PatternLayering : public Pattern<RGBA>
    {
    public:
        PatternLayering()
        {
            this->name = "Pattern layering";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++){
                // Alpha blending is also useful to stack multiple patterns. 
                // This example applies a similar alpha gradient as the example above, 
                // but this time without a background color. It will blend with the
                // pattern underneath it.
                pixels[i] = params->getPrimaryColour() * (float(i)/width);

                // The pixels[] array is filled with RGBA(0,0,0,0) (i.e.: all pixels transparent)
                // before this Calculate function is called. That is why you can return early if
                // active is false. It also means that you only need to paint the pixels that 
                // your pattern covers. 
            }
        }
    };

    //////////////////
    // Helpers
    //////////////////



    // Now you know the basics, it is up to you how you want to fill the array of pixel data. 
    // Everything is allowed. You can make your own code a simple or complex as you want.
    // There are a few helper classes that you can use if you like. They save you from writing 
    // the same logic over and over again.
    // Below are a few examples of the most used helpers

    class LFOPattern : public Pattern<RGBA>
    {
    public:

        // Here the lfo object is created.
        // Different shapes are available. Common ones are:
        // Sin:             Speaks for itself
        // SinFast:         Since calculating sin() is relatively slow, this shape uses a lookup table with 255 precalculated values.  
        // NegativeCosFast: Similar to sin, but has its starting point at the bottom instead of the center. Also uses lookup table.
        // SawDown:         Goes from 1 to 0, linearly.
        LFO<SinFast> lfo;

        LFOPattern()
        {
            this->name = "LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++){
                // lfo.getValue() will give you the value in the range 0-1. 
                // This is convenient when you want to do alpha blending,
                pixels[i] = params->getPrimaryColour() * lfo.getValue();
            }
        }
    };


    class LFOChase : public Pattern<RGBA>
    {
    public:

        // LFO is suited to create chases. 
        // In this example i use SawDown, but you can play around, and try different shapes
        LFO<SawDown> lfo;

        LFOChase()
        {
            this->name = "LFO chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++){
                // In order to create a chase you need to tap into the lfo at different points.
                // You can do this by providing the phase argument. This argument shifts the phase
                // of the lfo. 
                // 0 = no shift
                // 1 = shift one full cycle
                // So it makes sense to have a value between 0-1 as phase, but other values will work as well.
                // Eg: if you change lfo.getValue(phase) to lfo.getValue(2 * phase), it will pass a value
                // between 0-2 as argument, and therefore fit 2 cycles on the circle. try it.
                float phase = float(i)/width;
                pixels[i] = params->getPrimaryColour() * lfo.getValue(phase);
            }
        }
    };


    class LFOChaseGradient : public Pattern<RGBA>
    {
    public:

        // Example with lfo gradient
        LFO<SawDown> lfo;

        LFOChaseGradient()
        {
            this->name = "LFO chase gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++){
                float phase = float(i)/width;
                float lfoVal = lfo.getValue(phase);
                // Here i use lfoVal to lookup the gradient value, but also to apply transparency, 
                // so the chase nicely fades to transparent
                pixels[i] = params->gradient->get(255 * lfoVal) * lfoVal; 
            }
        }
    };

    class FadePattern : public Pattern<RGBA>
    {
    public:
        // This example shows a Fade and BeatWatcher. These two helpers are typically used together.
        FadeDown fade;
        BeatWatcher watcher;

        FadePattern()
        {
            this->name = "Fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // Watcher.Triggered() will return true in the first frame after a beat occurred.
            // This is used to reset the fade
            if (watcher.Triggered())
                fade.reset();

            for (int i = 0; i < width; i++){
                pixels[i] = params->getPrimaryColour() * fade.getValue();
            }
        }
    };

    class FadeChasePattern : public Pattern<RGBA>
    {
    public:
        FadeDown fade;
        BeatWatcher watcher;

        FadeChasePattern()
        {
            this->name = "Fade Chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            if (watcher.Triggered())
                fade.reset();

            for (int i = 0; i < width; i++){
                // It is also easy to change this fade into a chase by providing an argument to getValue. 
                // This time the argument is the number of milliseconds to delay the fade.
                // (NB: i am considering to change this to a faction of the duration, so you pass a number in the range 0-1, to get a delay of 0-duration. That way is it closer to how lfo is working)
                float phase = ((float)i)/width;
                int maxDelay = 500;
                pixels[i] = params->getPrimaryColour() * fade.getValue(maxDelay * phase);
            }
        }
    };

    class FadeFinishPattern : public Pattern<RGBA>
    {
    public:
        FadeDown fade;
        BeatWatcher watcher;

        FadeFinishPattern()
        {
            this->name = "Fade finish";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            int maxDelay = 500;

            // In this example, we wait for the fade animation to finished before stopping the pattern.
            // This makes the transition a little bit nicer.

            //only trigger a new fade if the pattern is still active
            if (active && watcher.Triggered())
                fade.reset();

            //if the fade is finished we can stop rendering
            if (fade.isFinished(maxDelay))
                return;

            for (int i = 0; i < width; i++){
                float phase = ((float)i)/width;
                pixels[i] = params->getPrimaryColour() * fade.getValue(maxDelay * phase);
            }
        }
    };

    class TransitionPattern : public Pattern<RGBA>
    {
        // Transitions are a nice way to fade-in and fade-out patterns.
        // Here i add a transition to the pattern. It has a fade in time of 500 and fade out time of 1000
        Transition transition = Transition(500,1000);

    public:
        TransitionPattern()
        {
            this->name = "Transition";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            // transition.Calculate must be called each time. It will do some internal calculations to
            // calculate the fade-in/out position. It will return a boolean that indicates if the 
            // fade is finished and the pattern should render or not. 
            // This logic replaces the if(!active) return; logic that we used before.
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                //multiply your pixels colors with transition.getValue() to apply the fade in and out:
                pixels[i] = params->getSecondaryColour() * transition.getValue();
            }
        }
    };

    class PermutePattern : public Pattern<RGBA>
    {
    public:
        // You can use permute to create patterns that apply to only some of the lights.
        // Permute will create a list of numbers in random order. 
        // eg Permute with a size of 5 could give you [3,4,1,0,2]
        FadeDown fade;
        BeatWatcher watcher;
        Permute permute;

        PermutePattern()
        {
            this->name = "Permute";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // We need to tell permute how many leds we are working with
            permute.setSize(width);

            if (watcher.Triggered()){
                // Reorder the permutations on each beat
                permute.permute();
                fade.reset();
            }

            // Notice that we only loop to width / 4, so we only paint 25% of the pixels
            for (int i = 0; i < width / 4; i++) {
                // lookup the number of the pixel we are going to paint:
                int randomizedPixelIndex = permute.at[i];
                pixels[randomizedPixelIndex] = params->getPrimaryColour() * fade.getValue();
            }
        }
    };

    //////////////////
    // Params
    //////////////////

    class ParamsPattern : public Pattern<RGBA>
    {
    public:
        LFO<PWM> lfo;

        ParamsPattern()
        {
            this->name = "Params";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // You can use params in you patterns to give the user freedom to further tweak some value.
            // I use a predetermined set of params (Velocity, amount, size, etc).
            // You can get their values from the params argument. By default they will be in the range 0-1,
            // but you can rescale it by providing a min and max argument.

            lfo.setPeriod(params->getVelocity(5000,500));
            lfo.setPulseWidth(params->getSize(0.1,0.9));
            int amount = params->getAmount(1,4);

            for (int i = 0; i < width; i++){
                float phase = float(i)/width;
                pixels[i] = params->getPrimaryColour() * lfo.getValue(amount * phase);
            }
        }
    };

    //////////////////
    // Mapping
    //////////////////

    class MappedPattern : public Pattern<RGBA>
    {
    public:
        LFO<SinFast> lfo;
        PixelMap map;

        // For mapped patterns, the constructor needs to receive the map and store it in a property
        MappedPattern(PixelMap map)
        {
            this->name = "Mapped";
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++){
                // In your pattern you can access the coordinates with map[i].x and map[i].y
                // You should generate your maps in range [-1,1].
                // In this example i need a phase in range [0,1], so i need to rescale the value.
                // For here on you can use it the same way you did before
                float phase = Utils::rescale(map[i].y, 0,1,-1,1);
                pixels[i] = params->getPrimaryColour() * lfo.getValue(phase);
            }
        }
    };

};
