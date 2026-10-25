#include "com_tool.h"

int16_t com_limit(int16_t value, int16_t min, int16_t max) {
    if (value < min) {
        value = min;
    }
    else if (value > max) {
        value = max;
    }
    return value;
}