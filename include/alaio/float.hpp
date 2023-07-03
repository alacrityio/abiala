#pragma once

#ifdef __alaio_cdt__

namespace alaio {

using float32  = float;
using float64  = double;
using float128 = long double;

} // namespace alaio

#else

#   include <alaio/fixed_bytes.hpp>
#   include <limits>

namespace alaio {

using float32  = float;
using float64  = double;
using float128 = fixed_bytes<16>;

static_assert(sizeof(float32) == 4 && std::numeric_limits<float32>::is_iec559 &&
                    std::numeric_limits<float32>::digits == 24,
              "Unexpected float representation");
static_assert(sizeof(float64) == 8 && std::numeric_limits<float64>::is_iec559 &&
                    std::numeric_limits<float64>::digits == 53,
              "Unexpected double representation");

ALAIO_REFLECT(float128, value);

} // namespace alaio

#endif
