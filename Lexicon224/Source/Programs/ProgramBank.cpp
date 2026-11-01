#include "ProgramBank.h"
namespace lex224 {
const std::array<Program, 6> &programs() {
  static const std::array<Program, 6> bank = [] {
    std::array<Program, 6> result{
        {{"Small Concert Hall B",
          {3, 2, 540, 4400, 25, 24, 2, 5, 100, true, true, false, 0},
          24,
          152,
          1.0f,
          .55f,
          .5f,
          .32f,
          false,
          false},
         {"Vocal Plate",
          {2, 2, 1000, 9800, 15, 20, 2, 7, 100, true, true, false, 1},
          20,
          127,
          .78f,
          .72f,
          .8f,
          .08f,
          true,
          false},
         {"Large Concert Hall B",
          {3.4f, 2.6f, 540, 4000, 25, 24, 2, 5, 100, true, true, false, 2},
          24,
          152,
          1.5f,
          .32f,
          .6f,
          .4f,
          false,
          false},
         {"Acoustic Chamber",
          {3.4f, 2.6f, 540, 4000, 10, 25, 2, 5, 100, true, true, false, 3},
          25,
          255,
          1.12f,
          .6f,
          .45f,
          .28f,
          false,
          true},
         {"Percussion Plate A",
          {2, 2, 1000, 9800, 15, 0, 2, 7, 100, true, true, false, 4},
          0,
          107,
          .68f,
          .8f,
          .75f,
          .25f,
          true,
          false},
         {"Small Concert Hall A",
          {3, 2, 540, 4400, 25, 24, 2, 5, 100, true, true, false, 5},
          24,
          152,
          .96f,
          .57f,
          .68f,
          .3f,
          false,
          false}}};
    // Designed program-specific timing sets, in seconds. No claim of recovered
    // ROM taps.
    const std::array<std::array<double, 8>, 6> delays{
        {{{.0297, .0371, .0411, .0437, .0531, .0593, .0677, .0739}},
         {{.0217, .0269, .0323, .0389, .0451, .0499, .0563, .0617}},
         {{.0473, .0581, .0671, .0797, .0893, .1013, .1139, .1277}},
         {{.0311, .0397, .0479, .0523, .0613, .0719, .0821, .0971}},
         {{.0181, .0229, .0287, .0343, .0409, .0473, .0533, .0581}},
         {{.0271, .0349, .0401, .0473, .0527, .0611, .0691, .0767}}}};
    const std::array<std::array<double, 8>, 6> taps{
        {{{0, .007, .019, .037, .061, .089, .131, .179}},
         {{0, .0023, .0057, .0091, .0143, .0217, .0301, .0431}},
         {{0, .013, .031, .057, .089, .127, .163, .197}},
         {{0, .021, .043, .065, .087, .109, .131, .150}},
         {{0, .0011, .0029, .0047, .0073, .0113, .0179, .0263}},
         {{0, .005, .017, .029, .053, .083, .127, .173}}}};
    for (std::size_t i = 0; i < result.size(); ++i) {
      result[i].delayTimes = delays[i];
      result[i].tapTimes = taps[i];
      result[i].tapGains =
          result[i].chamber ? std::array<float, 8>{{.46f, .49f, .51f, .52f,
                                                    .52f, .51f, .49f, .46f}}
          : result[i].plate ? std::array<float, 8>{{.9f, .73f, .68f, .57f, .48f,
                                                    .39f, .28f, .2f}}
                            : std::array<float, 8>{{.7f, .31f, .58f, .23f, .47f,
                                                    .29f, .18f, .12f}};
    }
    return result;
  }();
  return bank;
}
} // namespace lex224
