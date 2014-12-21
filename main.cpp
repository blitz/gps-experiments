#include <cstdio>
#include <cmath>

#include "coord.hpp"

void print_info(char const *m, Coord::Vector v)
{
  printf("%s: %.0fm bearing %f\n", m, v.distance, rad_to_deg(v.bearing));
}

int main()
{
  Coord dresden = Coord::from_degrees(51.0504, 13.7373);
  Coord berlin  = Coord::from_degrees(52.5192, 13.4061);

  print_info("Dresden<->Berlin", dresden.vector_to(berlin));
  print_info("Berlin<->Dresden", berlin.vector_to(dresden));

  Coord dresden2 { ECEF { 3902745, 954079, 4937071 } };

  printf("Dresden %f %f %f\n", rad_to_deg(dresden2.lat), rad_to_deg(dresden2.lon), dresden2.height);

  return 0;
}
