#include <alaio/to_json.hpp>
#include <alaio/from_json.hpp>
#include <alaio/to_bin.hpp>
#include <alaio/from_bin.hpp>
#include <alaio/bytes.hpp>
#include <alaio/crypto.hpp>
#include <alaio/symbol.hpp>
#include <alaio/asset.hpp>
#include <alaio/time.hpp>
#include <alaio/fixed_bytes.hpp>
#include <alaio/float.hpp>
#include <alaio/varint.hpp>
#include <alaio/abi.hpp>

int error_count;

void report_error(const char* assertion, const char* file, int line) {
    if(error_count <= 20) {
       printf("%s:%d: failed %s\n", file, line, assertion);
    }
    ++error_count;
}

#define CHECK(...) do { if(__VA_ARGS__) {} else { report_error(#__VA_ARGS__, __FILE__, __LINE__); } } while(0)

// Verify that serialization is consistent for vector_stream/size_stream/fixed_buf_stream
// and returns the serialized data.
template<typename T, typename F>
std::vector<char> test_serialize(const T& value, F&& f) {
   std::vector<char> buf1;
   alaio::vector_stream vecstream(buf1);
   alaio::size_stream szstream;
   f(value, vecstream);
   f(value, szstream);
   CHECK(szstream.size == vecstream.data.size());
   std::vector<char> buf2(szstream.size);
   alaio::fixed_buf_stream fxstream(buf2.data(), buf2.size());
   f(value, fxstream);
   CHECK(buf1 == buf2);
   return buf1;
}

alaio::abi round_trip_abi(const alaio::abi& src) {
   alaio::abi_def def;
   convert(src, def);
   alaio::abi result;
   convert(def, result);
   return result;
}

template<typename T>
auto check_result(T&& t) {
   CHECK(t);
   if(t) return t.value();
   else return std::decay_t<decltype(t.value())>{};
}

// Verifies that all 6 conversions between native/bin/json round-trip
template<typename T>
void test(const T& value, alaio::abi& abi1, alaio::abi& abi2) {
   std::vector<char> bin = test_serialize(value, [](const T& v, auto& stream) { return to_bin(v, stream); });
   std::vector<char> json = test_serialize(value, [](const T& v, auto& stream) { return to_json(v, stream); });
   {
      T bin_value;
      alaio::input_stream bin_stream(bin);
      from_bin(bin_value, bin_stream);
      CHECK(bin_value == value);
      T json_value;
      std::string mutable_json(json.data(), json.size());
      alaio::json_token_stream json_stream(mutable_json.data());
      from_json(json_value, json_stream);
      CHECK(json_value == value);
   }

   for(alaio::abi* abi : {&abi1, &abi2})
   {
      // Get the ABI
      using alaio::get_type_name;
      const alaio::abi_type* type = abi->get_type(get_type_name((T*)nullptr));

      // bin_to_json
      auto bin2 = type->json_to_bin({json.data(), json.size()});
      CHECK(bin2 == bin);
      // json_to_bin
      alaio::input_stream bin_stream{bin};
      auto json2 = type->bin_to_json(bin_stream);
      CHECK(json2 == std::string(json.data(), json.size()));
   }
}

char empty_abi[] = R"({
    "version": "alaio::abi/1.0"
})";

template<typename T>
void test_int(alaio::abi& abi1, alaio::abi& abi2) {
   for(T i : {T(0), T(1), std::numeric_limits<T>::min(), std::numeric_limits<T>::max()}) {
      test(i, abi1, abi2);
   }
}

using int128 = __int128;
using uint128 = unsigned __int128;
using alaio::varint32;
using alaio::varuint32;
using alaio::float128;
using alaio::microseconds;
using alaio::time_point;
using alaio::time_point_sec;
using alaio::block_timestamp;
using alaio::bytes;
using alaio::checksum160;
using alaio::checksum256;
using alaio::checksum512;
using alaio::public_key;
using alaio::private_key;
using alaio::signature;
using alaio::symbol;
using alaio::symbol_code;
using alaio::asset;

using vec_type = std::vector<int>;
struct struct_type {
   std::vector<int> v;
   std::optional<int> o;
   std::variant<int, double> va;
};
ALAIO_REFLECT(struct_type, v, o, va);
ALAIO_COMPARE(struct_type);

int main() {
   alaio::json_token_stream stream(empty_abi);
   alaio::abi_def def = alaio::from_json<alaio::abi_def>(stream);
   alaio::abi abi;
   convert(def, abi);
   abi.add_type<struct_type>();
   alaio::abi new_abi(round_trip_abi(abi));
   test(true, abi, new_abi);
   test(false, abi, new_abi);
   for(int i = -128; i <= 127; ++i) {
      test(static_cast<std::int8_t>(i), abi, new_abi);
   }
   for(int i = 0; i <= 255; ++i) {
      test(static_cast<std::uint8_t>(i), abi, new_abi);
   }
   for(int i = -32768; i <= 32767; ++i) {
      test(static_cast<std::int16_t>(i), abi, new_abi);
   }
   for(int i = 0; i <= 65535; ++i) {
      test(static_cast<std::uint16_t>(i), abi, new_abi);
   }
   test_int<int32_t>(abi, new_abi);
   test_int<uint32_t>(abi, new_abi);
   test_int<int64_t>(abi, new_abi);
   test_int<uint64_t>(abi, new_abi);
   test(int128{}, abi, new_abi);
   test(int128{0x1}, abi, new_abi);
   test(int128{-1}, abi, new_abi);
   test(int128{std::numeric_limits<int128>::max()}, abi, new_abi);
   test(int128{std::numeric_limits<int128>::min()}, abi, new_abi);
   test(uint128{}, abi, new_abi);
   test(uint128{1}, abi, new_abi);
   test(uint128(-1), abi, new_abi);
   test(uint128(std::numeric_limits<int128>::max()), abi, new_abi);
   test(uint128(std::numeric_limits<int128>::min()), abi, new_abi);
   test(varuint32{0}, abi, new_abi);
   test(varuint32{1}, abi, new_abi);
   test(varuint32{0xFFFFFFFFu}, abi, new_abi);
   test(varint32{0}, abi, new_abi);
   test(varint32{1}, abi, new_abi);
   test(varint32{-1}, abi, new_abi);
   test(varint32{0x7FFFFFFF}, abi, new_abi);
   test(varint32{std::numeric_limits<int32_t>::min()}, abi, new_abi);
   test(0.0f, abi, new_abi);
   test(1.0f, abi, new_abi);
   test(-1.0f, abi, new_abi);
   test(std::numeric_limits<float>::min(), abi, new_abi);
   test(std::numeric_limits<float>::max(), abi, new_abi);
   test(std::numeric_limits<float>::infinity(), abi, new_abi);
   test(-std::numeric_limits<float>::infinity(), abi, new_abi);
   // nans are not equal
   // test(std::numeric_limits<float>::quiet_NaN(), abi, new_abi);
   test(0.0, abi, new_abi);
   test(1.0, abi, new_abi);
   test(-1.0, abi, new_abi);
   test(std::numeric_limits<double>::min(), abi, new_abi);
   test(std::numeric_limits<double>::max(), abi, new_abi);
   test(std::numeric_limits<double>::infinity(), abi, new_abi);
   test(-std::numeric_limits<double>::infinity(), abi, new_abi);
   test(float128{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, abi, new_abi);
   test(float128{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80}}, abi, new_abi);
   test(float128{{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}, abi, new_abi);
   for(uint64_t i = 0; i < 10000; ++i) {
     test(time_point{microseconds(i * 1000)}, abi, new_abi);
   }
   // This is the largest time that can be parsed by the current implementation,
   // because of the dependency on time_point_sec.
   test(time_point{microseconds(0xFFFFFFFFull * 1000000)}, abi, new_abi);
   for(uint32_t i = 0; i < 10000; ++i) {
      test(time_point_sec{i}, abi, new_abi);
   }
   test(time_point_sec{0xFFFFFFFFu}, abi, new_abi);
   for(uint32_t i = 0; i < 10000; ++i) {
      test(block_timestamp{i}, abi, new_abi);
   }
   test(block_timestamp{0xFFFFFFFFu}, abi, new_abi);
   test(alaio::name("alaio"), abi, new_abi);
   test(alaio::name(), abi, new_abi);
   test(bytes(), abi, new_abi);
   test(bytes{{0, 0, 0, 0}}, abi, new_abi);
   test(bytes{{'\xff', '\xff', '\xff', '\xff'}}, abi, new_abi);
   using namespace std::literals::string_literals;
   test(""s, abi, new_abi);
   test("\0"s, abi, new_abi);
   // test("\xff"s, abi, new_abi); // invalid utf8 doesn't round-trip
   test("abcdefghijklmnopqrstuvwxyz"s, abi, new_abi);
   test(checksum160{{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}, abi, new_abi);
   test(checksum256{{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}, abi, new_abi);
   test(checksum512{{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}, abi, new_abi);
   test(public_key{std::in_place_index<0>}, abi, new_abi);
   test(public_key{std::in_place_index<1>}, abi, new_abi);
   test(private_key{std::in_place_index<0>}, abi, new_abi);
   test(private_key{std::in_place_index<1>}, abi, new_abi);
   test(signature{std::in_place_index<0>}, abi, new_abi);
   test(signature{std::in_place_index<1>}, abi, new_abi);
   // avoid using multichars to improve portability and clean up warnings
   auto multichars_to_uint32 = [] ( char const v[5] ) constexpr -> uint32_t {
      return (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
   };
   test(symbol{multichars_to_uint32("ZYX\x08")}, abi, new_abi);
   test(symbol_code{multichars_to_uint32("ZYXW")}, abi, new_abi);
   test(asset{5, symbol{multichars_to_uint32("ZYX\x08")}}, abi, new_abi);
   test(struct_type{}, abi, new_abi);
   test(struct_type{{1},2,3}, abi, new_abi);
   test(struct_type{{1,2},3,4.0}, abi, new_abi);
   test(std::vector{1, 2}, abi, new_abi);
   test(std::optional{3}, abi, new_abi);
   test(std::variant<int, double>{4}, abi, new_abi);
   if(error_count) return 1;
}
