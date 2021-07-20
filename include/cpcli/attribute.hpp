#pragma once

#ifndef __CPCLI_SERDE_ATTRIBUTE_HPP__
#define __CPCLI_SERDE_ATTRIBUTE_HPP__

#include "cpcli/cpcli.hpp"

namespace serde::attribute {
    struct abbr{
        abbr(std::string_view abbr) :abbr_(abbr) {};
        std::string_view abbr_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {

            }
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr (std::is_same_v<T, cpcli::Option>) {
                data.abbr(std::string{abbr_});
                next_attr.into(ctx, data, key, remains...);
            }
            else if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                auto option = cpcli::Option(std::string{key}).abbr(std::string{abbr_}).arg<T>();
                next_attr.template into<cpcli::Option, serde_ctx>(ctx, option, key, remains...);
            }
            else {
                next_attr.into(ctx, data, key, remains...);
            }
        }

    };

    struct desc{
        desc(std::string_view desc) :desc_(desc) {};
        std::string_view desc_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {

            }
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr (std::is_same_v<T, cpcli::Option>) {
                data.desc(std::string{desc_});
                next_attr.into(ctx, data, key, remains...);
            }
            else if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                auto option = cpcli::Option(std::string{key}).desc(std::string{desc_}).arg<T>();
                next_attr.template into<cpcli::Option, serde_ctx>(ctx, option, key, remains...);
            }
            else {
                next_attr.into(ctx, data, key, remains...);
            }
        }
    };
    struct arg_hint{
        arg_hint(std::string_view arg_hint) :arg_hint_(arg_hint) {};
        std::string_view arg_hint_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {

            }
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr (std::is_same_v<T, cpcli::Option>) {
                data.arg_hint(std::string{arg_hint_});
                next_attr.into(ctx, data, key, remains...);
            }
            else if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                auto option = cpcli::Option(std::string{key}).arg_hint(std::string{arg_hint_}).arg<T>();
                next_attr.template into<cpcli::Option, serde_ctx>(ctx, option, key, remains...);
            }
            else {
                next_attr.into(ctx, data, key, remains...);
            }
        }
    };
}


#endif
