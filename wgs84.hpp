#pragma once

class WGS84 {

public:  
  // Current values from wikipedia. For the math check:
  // https://microem.ru/files/2012/08/GPS.G1-X-00006.pdf

  static constexpr float A = 6378137.0      /* meter */;
  static constexpr float B = 6356752.314245 /* meter */;

  static constexpr float E       = 0.08181919084296430231;
  static constexpr float E_PRIME = 0.08209443795004196366;
  
  /// A good average for formulas that assume the earth to be a
  /// sphere.
  static constexpr float earth_radius = (2 * WGS84::A + WGS84::B) / 3 /* meter */;

};

// EOF
