#pragma once

// More precision than representable in float...
static const float PI = 3.1415926535897932384626433832795;

static inline float deg_to_rad(float deg) { return deg * PI / 180; }
static inline float rad_to_deg(float rad) { return rad * 180 / PI; }
static inline float square(float a) { return a * a; }
static inline float cube(float a)   { return a * a * a; }

// EOF
