//
// Created by Luis on 4/19/2024.
//

#include "Wind.h"

Wind::Wind(float rangeMin, float rangeMax)
    : mTimer(0), mTimeAccumulation(0)
{
    mDirection = {1, 0, 0};
    mStrength = 0;
}

void Wind::Update(float delta)
{
    mTimer += delta;
    mTimeAccumulation += delta;

    mStrength = (sinf(mTimeAccumulation) + 1);
}

Vec4 Wind::GetWindData()
{
    Vec4 data = { };

    data.x = mDirection.x;
    data.y = mDirection.y;
    data.z = mDirection.z;
    data.w = mStrength;

    return data;
}
