#include <cstdio>
#include <cmath>

// More precision than representable in float...
static const float PI = 3.1415926535897932384626433832795;

static const float earth_radius = (2 * 6378137 + 6356752) / 3 /* meter */;

float deg_to_rad(float deg)
{
  return deg * PI / 180;
}

float rad_to_deg(float rad)
{
  return rad * 180 / PI;
}

float square(float a)
{
  return a * a;
}

class GeoCoord
{
  float lat; // phi
  float lon; // lambda

public:

  GeoCoord(float lat, float lon)
    : lat(lat), lon(lon)
  { }

  static GeoCoord from_degrees(float lat, float lon)
  {
    return GeoCoord(deg_to_rad(lat), deg_to_rad(lon));
  }

  float distance_to(GeoCoord dst) const
  {
    float delta_lat = dst.lat - lat;
    float delta_lon = dst.lon - lon;

    float a = square(sinf(delta_lat / 2))
      + cosf(dst.lat) * cosf(lat) * square(sinf(delta_lon / 2));

    return earth_radius * 2 * atan2f(sqrtf(a), sqrtf(1 - a));
  }

  float bearing_to(GeoCoord dst) const
  {
    float delta_lon = dst.lon - lon;

    return atan2f(sinf(delta_lon) * cosf(dst.lat),
                  cosf(lat) * sinf(dst.lat) - sinf(lat) * cosf(dst.lat) * cosf(delta_lon));
  }

  struct Vector {
    float distance;
    float bearing;
  };

  Vector vector_to(GeoCoord dst) const
  {
    return Vector { distance_to(dst), bearing_to(dst) };
  }
};

void print_info(char const *m, GeoCoord::Vector v)
{
  printf("%s: %.0fm bearing %f\n", m, v.distance, rad_to_deg(v.bearing));
}

int main()
{
  GeoCoord dresden = GeoCoord::from_degrees(51.0504, 13.7373);
  GeoCoord berlin  = GeoCoord::from_degrees(52.5192, 13.4061);

  print_info("Dresden<->Berlin", dresden.vector_to(berlin));
  print_info("Berlin<->Dresden", berlin.vector_to(dresden));

  return 0;
}
