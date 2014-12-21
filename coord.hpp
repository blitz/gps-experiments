#pragma once

#include <cmath>
#include "math.hpp"
#include "wgs84.hpp"

/// Earth-centered, earth-fixed coordinates (cartesian).
struct ECEF
{
  float x, y, z;
};

/// Angular coordinates.
class Coord
{
public:

  float lat; // phi
  float lon; // lambda

  /// Height of this coordinate over the ellipsoid. This is ignored
  /// for distance and bearing (for now).
  float height;

  Coord(float lat, float lon, float height = 0)
    : lat(lat), lon(lon), height(0)
  { }

  Coord(ECEF ecef)
  {
    float p     = sqrtf(square(ecef.x) + square(ecef.y));
    float theta = atanf((ecef.z * WGS84::A) / (p * WGS84::B));

    lon = atan2f(ecef.y, ecef.x);
    lat = atan2f(ecef.z + square(WGS84::E_PRIME) * WGS84::B * cube(sinf(theta)),
                 (p - square(WGS84::E) * WGS84::A * cube(cosf(theta))));

    float n = WGS84::A / sqrtf(1 - square(WGS84::E) * square(sinf(lat)));
    height = p / cosf(lat) - n;
  }
  
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

    return WGS84::earth_radius * 2 * atan2f(sqrtf(a), sqrtf(1 - a));
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
