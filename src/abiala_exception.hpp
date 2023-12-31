// copyright defined in abiala/LICENSE.txt

#pragma once

#include "abiala.hpp"

#include <exception>

namespace abiala {

struct error : std::exception {
    std::string message;

    error(std::string message) : message(std::move(message)) {}
    virtual const char* what() const noexcept { return message.c_str(); }
};

inline std::string public_key_to_string(const public_key& key) {
    std::string dest, error;
    if (!public_key_to_string(dest, error, key))
        throw abiala::error(error);
    return dest;
}

inline std::string signature_to_string(const signature& key) {
    std::string dest, error;
    if (!signature_to_string(dest, error, key))
        throw abiala::error(error);
    return dest;
}

inline time_point_sec string_to_time_point_sec(const char* s) {
    time_point_sec result;
    std::string error;
    if (!string_to_time_point_sec(result, error, s, s + strlen(s)))
        throw abiala::error(error);
    return result;
}

inline time_point string_to_time_point(const std::string& s) {
    time_point result;
    std::string error;
    if (!string_to_time_point(result, error, s))
        throw abiala::error(error);
    return result;
}

inline uint64_t string_to_symbol_code(const char* s) {
    uint64_t result;
    if (!alaio::string_to_symbol_code(result, s, s + strlen(s)))
        throw abiala::error("invalid symbol_code");
    return result;
}

inline uint64_t string_to_symbol(const char* s) {
    uint64_t result;
    if (!alaio::string_to_symbol(result, s, s + strlen(s)))
        throw abiala::error("invalid symbol");
    return result;
}

inline asset string_to_asset(const char* s) {
    asset result;
    if (!alaio::string_to_asset(result.amount, result.sym.value, s, s + strlen(s)))
        throw abiala::error("invalid asset");
    return result;
}

inline void check_abi_version(const std::string& s) {
    std::string error;
    if (!check_abi_version(s, error))
        throw abiala::error(error);
}

inline contract create_contract(const abi_def& abi) {
    std::string error;
    contract c;
    if (!fill_contract(c, error, abi))
        throw abiala::error(error);
    return c;
}

inline void json_to_bin(std::vector<char>& bin, const abi_type* type, const jvalue& value) {
    std::string error;
    if (!json_to_bin(bin, error, type, value))
        throw abiala::error(error);
}

} // namespace abiala
