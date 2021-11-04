#pragma once

#ifndef __CPCLI_CONVERTER_HPP__
#define __CPCLI_CONVERTER_HPP__

#include <serdepp/serde.hpp>
#include <serdepp/adaptor/fmt.hpp>

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
        ENUM,
        VALUE,
    };
    namespace detail {
        struct cpcli_default_hint;
        template<class T, typename=void>
        struct cpcli_hint_helper{
            using value = T;
        };

        template<>
        struct cpcli_hint_helper<void>{
            using value = void;
        };

        template<class T>
        struct cpcli_hint_helper<T, std::enable_if_t<serde::is_mappable_v<T>>>{
            using value = typename T::mapped_type;
        };

        template<class T>
        struct cpcli_hint_helper<T, std::enable_if_t<serde::is_sequenceable_v<T> && !serde::is_str_v<T>>>{
            using value = typename T::value_type;
        };

        template<class T, typename=void>
        struct cpcli_enum_helper {
            using value = ARG_TYPE;
        };

        template<class T>
        struct cpcli_enum_helper<T, std::enable_if_t<serde::is_enumable_v<T>>> {
            using value = T;
        };

        template<typename T>
        auto cpcli_enum_hint_list() {
            using namespace fmt::literals;
            constexpr auto& enum_list = magic_enum::enum_names<typename detail::cpcli_enum_helper<T>::value>();
            return fmt::format("{}", fmt::join(enum_list, " | "));
        }
    }

    template<typename T>
    constexpr ARG_TYPE find_arg_type() {
        if constexpr(serde::meta::is_str_v<T>) return ARG_TYPE::STRING;
        else if constexpr (serde::meta::is_sequenceable_v<T>) return ARG_TYPE::SEQUENCE;
        else if constexpr (serde::meta::is_mappable_v<T>) return ARG_TYPE::MAP;
        else if constexpr(std::is_same_v<int, T>) return ARG_TYPE::INTEGER;
        else if constexpr(std::is_same_v<bool, T>) return ARG_TYPE::BOOL;
        else if constexpr(std::is_same_v<float, T>) return ARG_TYPE::FLOAT;
        else if constexpr(serde::meta::is_enumable_v<T>) return ARG_TYPE::ENUM;
        else if constexpr(std::is_same_v<detail::cpcli_default_hint, T>) return ARG_TYPE::VALUE;
        //else if constexpr(serde::meta::is_optional_v<T>) return find_arg_type<typename T::value_type>();
        else throw parse_error(fmt::format("unsupport type"));
    }

    template<typename T=ARG_TYPE>
    inline std::string make_hint(ARG_TYPE type) {
        using namespace fmt::literals;
        auto element_type = serde::to_str(find_arg_type<typename detail::cpcli_hint_helper<T>::value>());
        switch(type) {
        case ARG_TYPE::NONE: return "none";
        case ARG_TYPE::BOOL: return "false or true";
        case ARG_TYPE::MAP:  return "KEY={} ..."_format(element_type);
        case ARG_TYPE::SEQUENCE: return "={} ..."_format(element_type);
        case ARG_TYPE::ENUM: return detail::cpcli_enum_hint_list<T>();
        default: return serde::to_str(type);
        }
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
        else if constexpr(serde::meta::is_enumable_v<T>) {
            if(str.empty()) {
                return magic_enum::enum_value<T>(0);
            } else {
                return serde::type::enum_t::from_str<T>(str);
            }
        }
        else {
            throw parse_error(fmt::format("unsupport type"));
        }
    }

    template<> inline bool converter(const std::string& str) {
        using namespace fmt::literals;
        std::string lower="";
        std::transform(str.begin(), str.end(), std::back_inserter(lower), ::tolower);
        if(lower == "off" || lower == "false") return false;
        else if(lower == "on"| lower == "true") return true;
        else throw parse_error(fmt::format("str[{}] -> bool", lower));
    }

    template <> inline std::string converter(const std::string& str) { return str; }

    template <> inline int converter(const std::string& str) { return std::stoi(str); }
    template <> inline float converter(const std::string& str) { return std::stof(str); }

    template<typename T>
    inline T convert(const std::string& hint, const std::string& data_str) {
        if(hint == "NONE") { return converter<T>(data_str.empty() ? "false" : data_str); }
        else { return converter<T>(data_str); }
    }

}

#endif
