#pragma once
#include "ship_protocol.hpp"

namespace chain_types {
using namespace alaio::ship_protocol;

struct block_info {
   uint32_t               block_num = {};
   alaio::checksum256     block_id  = {};
   alaio::block_timestamp timestamp;
};

ALAIO_REFLECT(block_info, block_num, block_id, timestamp);
}; // namespace chain_types