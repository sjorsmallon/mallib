#include "math.ih"

const int SMALLEST_NON_DENORMAL = 1 << IEEE_FLT_MANTISSA_BITS;

const float Math::PI = 3.14159265358979323846f;
const float Math::INF = 1e30f;
const float Math::DEG2RAD = PI / 180.0f;
const float Math::RAD2DEG = 180.f / PI;
const float Math::SEC2MS = 1000.0f;
const float Math::MS2SEC = 0.001f;
const float Math::FLT_SMALLEST_NON_DENORMAL =  reinterpret_cast< const float * >( & SMALLEST_NON_DENORMAL );


void Math::Init()
{
    //??
}