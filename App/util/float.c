#include <math.h>
#include <limits.h>
#include <stdint.h>

uint16_t Float_To_UInt16(float val)
{
    if (!isfinite(val)) {
        return 0;
    }

    float rounded_f = round(val);

    if (rounded_f < 0.0f) {
        return 0;
    } else if (rounded_f > (float)UINT16_MAX) {
        return UINT16_MAX;
    }

    return (uint16_t)rounded_f;
}