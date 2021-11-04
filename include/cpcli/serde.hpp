#pragma once

#ifndef __CPCLI_SERDE_HPP_HH
#define __CPCLI_SERDE_HPP_HH

#include "cpcli/cpcli.hpp"

namespace serde {
    using namespace cpcli;
    using cpcli_t = cpcli::Command;
    template<> struct serde_type_checker<cpcli_t> {
        using Format = cpcli_t;
        static bool is_integer(Format& format) { return true; }
        static bool is_sequence(Format& format){ return true; }
        static bool is_map(Format& format)     { return true; }
        static bool is_float(Format& format)   { return true; }
        static bool is_string(Format& format)  { return true; }
        static bool is_bool(Format& format)    { return true; }
        static bool is_null(Format &format)    { return true; }
        static bool is_struct(Format& format)  { return true; }
    };

    template<> struct serde_adaptor_helper<cpcli_t> : derive_serde_adaptor_helper<cpcli_t> {
        inline constexpr static bool is_null(cpcli_t& adaptor, std::string_view key) { return true; }
        inline constexpr static size_t size(cpcli_t& adaptor) { return 1; }
        inline constexpr static bool is_struct(cpcli_t& adaptor) { return true; }
    };

    template<typename T> struct serde_adaptor<cpcli_t, T, type::struct_t> {
        static void from(cpcli_t& s, std::string_view key, T& data) {
            deserialize_to(s.get_subcommand(key), data);
        }
        static void into(cpcli_t& s, std::string_view key, const T& data) {
            serialize_to(data, s.get_subcommand(key));
            s.get_subcommand(key).name(std::string{key});
        }
    };

    template<typename T, typename U> struct serde_adaptor<cpcli_t, T, U> {
        static void from(cpcli_t& s, std::string_view key, T& data) {
            if(!s.get_arg(key).is_require() && !s.get_arg(key).visit()) return;
            data = s.get_arg(key).get<T>();
        }
        static void into(cpcli_t& s, std::string_view key, const T& data) {
            s.arg_after_init(std::string{key});
        }
    };
}

#endif
