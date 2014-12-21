#include <cmath>

#pragma once

// More precision than representable in float...
static const float PI = 3.1415926535897932384626433832795;

static const float earth_radius = (2 * 6378137 + 6356752) / 3 /* meter */;

static inline float deg_to_rad(float deg) { return deg * PI / 180; }
static inline float rad_to_deg(float rad) { return rad * 180 / PI; }
static inline float square(float a) { return a * a; }

class Coord
{
  float lat; // phi
  float lon; // lambda

public:

  Coord(float lat, float lon)
    : lat(lat), lon(lon)
  { }

  static Coord from_degrees(float lat, float lon)
  {
    return Coord(deg_to_rad(lat), deg_to_rad(lon));
  }

  float distance_to(Coord dst) const
  {
    float delta_lat = dst.lat - lat;
    float delta_lon = dst.lon - lon;

    float a = square(sinf(delta_lat / 2))
      + cosf(dst.lat) * cosf(lat) * square(sinf(delta_lon / 2));

    return earth_radius * 2 * atan2f(sqrtf(a), sqrtf(1 - a));
  }

  float bearing_to(Coord dst) const
  {
    float delta_lon = dst.lon - lon;

    return atan2f(sinf(delta_lon) * cosf(dst.lat),
                  cosf(lat) * sinf(dst.lat) - sinf(lat) * cosf(dst.lat) * cosf(delta_lon));
  }

  struct Vector {
    float distance;
    float bearing;
  };

  Vector vector_to(Coord dst) const
  {
    return Vector { distance_to(dst), bearing_to(dst) };
  }
};

// EOF
