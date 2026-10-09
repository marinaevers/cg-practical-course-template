#pragma once

#include <cmath>


// Central animation clock shared by all animated scene objects.
//
// All rotation speeds of the ride are given in "revolutions per period". Because they are integers,
// the whole ride is in exactly the same state after every period. Every animated object therefore
// derives its rotation angle from the same phase value in [0, 1) instead of accumulating its own angle.
// This keeps all objects perfectly synchronized and makes it possible to evaluate the scene at any
// point in time (used for the global gondola paths).
class AnimationClock {
public:
    static constexpr double default_period_s = 12.0; // one period lasts 12 s at normal speed

    double period_s = default_period_s; // Duration of one period in seconds (at speed 1)
    float speed = 1.0f;                 // Animation speed factor, 0 stops the animation

    // Advances the animation by the real time dt (seconds since the last frame).
    // Scaling with dt makes the motion independent of the frame rate.
    void advance(double dt) {
        set_time(time_s + dt * speed);
    }

    // Jumps to an absolute animation time. Time is wrapped into [0, period) to avoid
    // losing floating point precision when the program runs for a long time.
    void set_time(double t) {
        time_s = std::fmod(t, period_s);
        if (time_s < 0.0)
            time_s += period_s;
    }

    double time() const { return time_s; }

    // Fraction of the current period in [0, 1).
    double phase() const { return time_s / period_s; }

private:
    double time_s = 0.0; // Animation time in seconds, always in [0, period)
};
