//
// Created by Luis on 4/19/2024.
//

#ifndef GRASSRENDERER_WIND_H
#define GRASSRENDERER_WIND_H

#include <ShlibVK/ShlibVK.h>

class Wind
{
private:
    float mTimer, mTimeAccumulation;
    float mDirectionAngle;
    Vec3 mDirection;
    float mStrength;

public:
    Wind(float rangeMin, float rangeMax);
    ~Wind() = default;

    void Update(float delta);
    Vec4 GetWindData();
};


#endif //GRASSRENDERER_WIND_H
