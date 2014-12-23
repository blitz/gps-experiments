#pragma once

#include <cstdint>
#include <memory>

class UbxParser {
  static constexpr uint8_t SYNC_1 = 0xB5;
  static constexpr uint8_t SYNC_2 = 0x62;

protected:

  /// UBX class codes
  enum {
    UBX_CLASS_NAV = 0x01,

  };

  /// UBX ID codes
  enum {
    UBX_NAV_POSLLH = 0x02,
    UBX_NAV_SOL    = 0x06,
  };

  /// Navigation solution with angular coordinates.
  struct NAV_POSLLH {
    uint32_t iTOW;              // ms
    int32_t  lon;               // 1e-7 deg
    int32_t  lat;               // 1e-7 deg
    int32_t  height;            // mm
    int32_t  hMSL;              // mm
    int32_t  hAcc;              // mm
    int32_t  vAcc;              // mm
  } __attribute__((packed));

  struct NAV_SOL {
    uint32_t iTOW;              // ms
    int32_t  fTOW;              // ns
    int16_t  week;
    uint8_t  gpsFix;
    uint8_t  flags;
    int32_t  ecef_x;            // cm
    int32_t  ecef_y;            // cm
    int32_t  ecef_z;            // cm
    uint32_t pAcc;              // cm
    int32_t  ecef_vel_x;        // cm
    int32_t  ecef_vel_y;        // cm
    int32_t  ecef_vel_z;        // cm
    uint32_t sAcc;              // cm
    uint16_t pDOP;
    uint8_t  _reserved1;
    uint8_t  numSV;
    uint32_t _reserved2;
  } __attribute__((packed));

public:

  virtual void parse_packet(uint8_t klass, uint8_t id, uint16_t length,
                            std::unique_ptr<uint8_t[]> payload) = 0;

  template <typename CLOSURE>
  void read_packet(CLOSURE read_byte)
  {
    while (read_byte() != SYNC_1);
    while (read_byte() != SYNC_2);

    uint8_t ck_a = 0;
    uint8_t ck_b = 0;

    /// Checksum a single byte.
    auto checksum = [&] (uint8_t d) -> uint8_t {
      ck_a += d;
      ck_b += ck_a;

      return d;
    };

    /// Read a byte, checksum it and return it.
    auto read_check = [&] () -> uint8_t {
      return checksum(read_byte());
    };

    uint8_t  klass  = read_check();
    uint8_t  id     = read_check();
    uint16_t length = read_check();

    length |= read_check() << 8;

    std::unique_ptr<uint8_t[]> payload { new uint8_t[length] };

    for (size_t i = 0; i < length; i++) {
      payload[i] = read_check();
    }

    if ((ck_a != read_byte()) or
        (ck_b != read_byte()))
      // Checksum error
      return;

    parse_packet(klass, id, length, std::move(payload));
  }
};

// EOF
