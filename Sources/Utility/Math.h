#pragma once


class Math
{
public:
    static constexpr float floatError = 0.0001f;


    static bool IsZero(float value)
    {
        return value < floatError && value > -floatError;
    }


private:
    Math() = delete;



};
