#pragma once

#ifndef __CPCLI_SERDE_HPP_HH
#define __CPCLI_SERDE_HPP_HH

#include "cpcli/cpcli.hpp"

namespace serde {
    using namespace cpcli;
    using cpcli_t = cpcli::Command;

    template<> struct serde_adaptor_helper<cpcli_t> : derive_serde_adaptor_helper<cpcli_t> {
        inline constexpr static bool is_null(cpcli_t& adaptor, std::string_view key) { return false; }
        inline constexpr static size_t size(cpcli_t& adaptor) { return 1; }
        inline constexpr static bool is_struct(cpcli_t& adaptor) { return true; }
    };

    template<typename T> struct serde_adaptor<cpcli_t, T, type::struct_t> {
        static void from(cpcli_t& s, std::string_view key, T& data) {
            deserialize_to(s.sub_command(std::string{key}), data);
        }
        static void into(cpcli_t& s, std::string_view key, const T& data) {
            auto cmd = serialize<cpcli_t>(data);
            cmd.name(key);
            s.add_command(cmd);
        }
    };

    template<typename T, typename U> struct serde_adaptor<cpcli_t, T, U> {
        static void from(cpcli_t& s, std::string_view key, T& data) {
            data = s[key].get<T>();
        }
        static void into(cpcli_t& s, std::string_view key, const T& data) {
            cpcli::Option opt = cpcli::Option(std::string{key});
            s.add_option(opt);
        }
    };

}

#endif
