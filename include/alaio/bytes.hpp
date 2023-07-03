#pragma once

#include "from_json.hpp"
#include "to_json.hpp"
#include "operators.hpp"
#include <vector>

namespace alaio {

struct bytes {
   std::vector<char> data;
};

ALAIO_REFLECT(bytes, data);
ALAIO_COMPARE(bytes);

template <typename S>
void from_json(bytes& obj, S& stream) {
   return alaio::from_json_hex(obj.data, stream);
}

template <typename S>
void to_json(const bytes& obj, S& stream) {
   return alaio::to_json_hex(obj.data.data(), obj.data.size(), stream);
}

} // namespace alaio
