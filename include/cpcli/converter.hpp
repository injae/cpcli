#pragma once

#ifndef __CPCLI_CONVERTER_HPP__
#define __CPCLI_CONVERTER_HPP__

#include <serdepp/serde.hpp>

namespace cpcli {
    struct parse_error : std::exception {
        explicit parse_error(std::string what) : what_(fmt::format("{}\n", what)) {}
        virtual ~parse_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
        std::string what_;
    };

    enum class ARG_TYPE {
        NONE,
        BOOL,
        INTEGER,
        STRING,
        FLOAT,
        SEQUENCE,
        MAP,
    };

    template<typename T>
    constexpr ARG_TYPE find_arg_type() {
        if constexpr(serde::meta::is_str_v<T>) return ARG_TYPE::STRING;
        else if constexpr (serde::meta::is_sequenceable_v<T>) return ARG_TYPE::SEQUENCE;
        else if constexpr (serde::meta::is_mappable_v<T>) return ARG_TYPE::MAP;
        else if constexpr(std::is_same_v<int, T>) return ARG_TYPE::INTEGER;
        else if constexpr(std::is_same_v<bool, T>) return ARG_TYPE::BOOL;
        else if constexpr(std::is_same_v<float, T>) return ARG_TYPE::FLOAT;
        else throw parse_error(fmt::format("unsupport type"));
    }

    template<typename T, typename U=void> inline T converter(const std::string& str) {
        if constexpr(serde::meta::is_sequenceable_v<T> && !serde::meta::is_str_v<T>) {
            T seq;
            auto last_pos = str.find_first_not_of(',', 0);
            auto pos      = str.find_first_of(',', last_pos);
            while (std::string::npos != pos || std::string::npos != last_pos) {
                seq.push_back(converter<serde::type::seq_e<T>>(str.substr(last_pos, pos - last_pos)));
                last_pos = str.find_first_not_of(',', pos);
                pos = str.find_first_of(',', last_pos);
            }
            return seq;
        }
        else if constexpr(serde::meta::is_mappable_v<T>) {
            T map;
            auto last_pos = str.find_first_not_of(',', 0);
            auto pos      = str.find_first_of(',', last_pos);
            while (std::string::npos != pos || std::string::npos != last_pos) {
                auto element = str.substr(last_pos, pos - last_pos);
                auto data_pos = element.find_first_of('=');
                auto key = converter<serde::type::map_k<T>>(element.substr(0, data_pos));
                auto data = converter<serde::type::map_e<T>>(element.substr(data_pos+1, pos - data_pos+1));
                map.emplace(key,data);
                last_pos = str.find_first_not_of(',', pos);
                pos = str.find_first_of(',', last_pos);
            }
            return map;
        }
        else {
            throw parse_error(fmt::format("unsupport type"));
        }
    }

    template<> inline bool converter(const std::string& str) {
        using namespace fmt::literals;
        std::string lower;
        std::transform(str.begin(), str.end(), lower.begin(), ::tolower);
        if(str == "off" || str == "false") return false;
        else if(str == "on"| str == "true") return true;
        else throw parse_error(fmt::format("str[{}] -> bool", str));
    }

    template <> inline std::string converter(const std::string& str) { return str; }

    template <> inline int converter(const std::string& str) { return std::stoi(str); }
    template <> inline float converter(const std::string& str) { return std::stof(str); }

    template<typename T>
    inline T convert(const ARG_TYPE& type, const std::string& data_str) {
        switch(type) {
        case ARG_TYPE::NONE: return  converter<T>(data_str.empty() ? "false" : data_str);
        default: return converter<T>(data_str);
        }
    }

}

#endif
