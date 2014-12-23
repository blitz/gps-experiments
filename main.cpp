#include <cstdio>
#include <cmath>

#include <fstream>

#include "coord.hpp"
#include "ubx.hpp"

class UbxFile final : public UbxParser {

  std::ifstream file;

public:

  void parse_packet(uint8_t klass, uint8_t id, uint16_t length,
                    std::unique_ptr<uint8_t[]> payload) override
  {
    printf("UBX class %02x id %02x length %04x\n", klass, id, length);

    switch (klass) {
    case UBX_CLASS_NAV:
      switch (id) {
      case UBX_NAV_POSLLH:

        if (length < sizeof(NAV_POSLLH))
          break;

        NAV_POSLLH const *llh = reinterpret_cast<NAV_POSLLH *>(&payload[0]);

        printf("NAV POSLLH %.4f %.4f +%.0fm (accuracy %.0fm)\n",
               float(llh->lat) / 10000000,
               float(llh->lon) / 10000000,
               float(llh->height) / 1000,
               float(llh->hAcc) / 1000);

        break;
      }
      break;
    };
  }

  void read_packet()
  {
    UbxParser::read_packet([this] () -> uint8_t {
        char b;
        file.read(&b, 1);
        return b;
      });
  }

  explicit UbxFile(char const *name)
    : file(name, std::ios_base::binary | std::ios_base::in)
  { }

};

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

  // UbxFile ubx { "examples/COM3_141221_112015.ubx" };

  // while (true)
  //   ubx.read_packet();

  return 0;
}
