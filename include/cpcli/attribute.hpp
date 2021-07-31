#pragma once

#ifndef __CPCLI_SERDE_ATTRIBUTE_HPP__
#define __CPCLI_SERDE_ATTRIBUTE_HPP__

#include "cpcli/cpcli.hpp"

namespace cpcli::attribute {
    struct abbr{
        abbr(std::string_view abbr) :abbr_(abbr) {};
        std::string_view abbr_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                if(ctx.adaptor.has_option(std::string{key})) {
                    ctx.adaptor[std::string{key}].abbr(std::string{abbr_});
                    ctx.adaptor.update_option(std::string{key});
                    next_attr.into(ctx, data, key, remains...);
                } else {
                    next_attr.into(ctx, data, key, remains..., abbr{abbr_});
                }
            }
            else {
                next_attr.into(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx>
        inline void into(serde_ctx& ctx, T& data, std::string_view key) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor[std::string{key}].abbr(std::string{abbr_});
                ctx.adaptor.update_option(std::string{key});
            }
        }

    };

    struct desc{
        desc(std::string_view desc) :desc_(desc) {};
        std::string_view desc_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                if(ctx.adaptor.has_option(std::string{key})) {
                    ctx.adaptor[std::string{key}].desc(std::string{desc_});
                    next_attr.into(ctx, data, key, remains...);
                }
                else if (ctx.adaptor.has_sub_command(std::string{key})) {
                    ctx.adaptor.sub_command(std::string{key}).desc(std::string{desc_});
                    next_attr.into(ctx, data, key, remains...);
                }
                 else {
                    next_attr.into(ctx, data, key, remains..., desc{desc_});
                }
            }
            else {
                next_attr.into(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx>
        inline void into(serde_ctx& ctx, T& data, std::string_view key) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                if(ctx.adaptor.has_option(std::string{key})) {
                    ctx.adaptor[std::string{key}].desc(std::string{desc_});
                }
                else if (ctx.adaptor.has_sub_command(std::string{key})) {
                    ctx.adaptor.sub_command(std::string{key}).desc(std::string{desc_});
                }
            }
        }
    };
    struct arg_hint{
        arg_hint(std::string_view arg_hint) :arg_hint_(arg_hint) {};
        std::string_view arg_hint_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                if(ctx.adaptor.has_option(std::string{key})) {
                    ctx.adaptor[std::string{key}].arg_hint(std::string{arg_hint_}).template arg<T>();
                    next_attr.into(ctx, data, key, remains...);
                } else {
                    next_attr.into(ctx, data, key, remains..., arg_hint{arg_hint_});
                }
            }
            else {
                next_attr.into(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx>
        inline void into(serde_ctx& ctx, T& data, std::string_view key) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor[std::string{key}].arg_hint(std::string{arg_hint_}).template arg<T>();
            }
        }
    };

    struct hook{
        hook(Command::Hook&& hook) :hook_(std::move(hook)) {};
        Command::Hook&& hook_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                if(!ctx.adaptor.has_sub_command(std::string{key})) {
                    next_attr.into(ctx, data, key, remains..., hook{std::move(hook_)});
                } 
                else {
                    ctx.adaptor.sub_command(std::string{key}).hook(std::move(hook_));
                    next_attr.into(ctx, data, key, remains...);
                }
            }
            else {
                next_attr.into(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx>
        inline void into(serde_ctx& ctx, T& data, std::string_view key) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor.sub_command(std::string{key}).hook(std::move(hook_));
            }
        }
    };

    template<typename Hook>
    struct hook_mem{
        hook_mem(Hook&& hook) :hook_(std::move(hook)) {};
        Hook&& hook_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                if(!ctx.adaptor.has_sub_command(std::string{key})) {
                    next_attr.into(ctx, data, key, remains..., hook_mem{std::move(hook_)});
                } 
                else {
                    ctx.adaptor.sub_command(std::string{key}).hook(std::move([hook=hook_,&data](auto& cmd){
                        (data.*hook)(cmd);
                    }));
                    next_attr.into(ctx, data, key, remains...);
                }
            }
            else {
                next_attr.into(ctx, data, key, remains...);
            }
        }

        template<typename T, typename serde_ctx>
        inline void into(serde_ctx& ctx, T& data, std::string_view key) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor.sub_command(std::string{key}).hook(std::move([hook=hook_,&data](auto& cmd){
                    (data.*hook)(cmd);
                }));
            }
        }
    };
}


#endif
