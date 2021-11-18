#pragma once

#ifndef __CPCLI_SERDE_ATTRIBUTE_HPP__
#define __CPCLI_SERDE_ATTRIBUTE_HPP__

#include "cpcli/cpcli.hpp"

namespace cpcli::attribute {
    struct flags {
        flags(std::string_view query) :flags_(query) {};
        std::string_view flags_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                if(auto sep = flags_.find('/'); sep != std::string_view::npos) {
                    auto _abbr = flags_.substr(0, sep);
                    auto _full = flags_.substr(sep+1);
                    ctx.adaptor.get_arg(std::string{key}).abbr(std::string{_abbr});
                    ctx.adaptor.get_arg(std::string{key}).full(std::string{_full});
                } else {
                    if(flags_.size() >= 2) {
                        ctx.adaptor.get_arg(std::string{key}).full(std::string{flags_});
                    } else {
                        ctx.adaptor.get_arg(std::string{key}).abbr(std::string{flags_});
                    }
                }
            }
            next_attr.into(ctx, data, key, remains...);
        }

    };


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
                ctx.adaptor.get_arg(std::string{key}).abbr(std::string{abbr_});
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct full{
        full(std::string_view full="") :full_(full) {};
        std::string_view full_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor.get_arg(std::string{key}).full(std::string{full_.empty() ? key : full_});
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct _index{
        _index(size_t idx) :index_(idx) {};
        size_t index_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor.get_arg(std::string{key}).index(index_);
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct possible_values{
        possible_values(std::initializer_list<std::string> values) : values_(values) {};
        possible_values(std::set<std::string> values) : values_(std::move(values)) {};
        std::set<std::string> values_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor.get_arg(std::string{key}).possible_values(std::move(values_));
            }
            next_attr.into(ctx, data, key, remains...);
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
                ctx.adaptor.get_arg(std::string{key}).desc(std::string{desc_});
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct about{
        about(std::string_view about) :about_(about) {};
        std::string_view about_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                if(key.empty()) {
                    ctx.adaptor.about(std::string{about_});
                } else {
                    ctx.adaptor.get_subcommand(std::string{key}).about(std::string{about_});
                }
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct _value{
        _value(std::string_view value="") :value_(value) {};
        std::string_view value_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor.get_arg(std::string{key}).template value<T>();
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct visible{
        visible(bool is_visible) :is_visible_(is_visible) {};
        bool is_visible_;

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,

                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                ctx.adaptor.get_arg(std::string{key}).visible(is_visible_);
            }
            next_attr.into(ctx, data, key, remains...);
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
                 ctx.adaptor.hook(std::move(hook_));
             }
             next_attr.into(ctx, data, key, remains...);
         }
    };

    namespace detail {
        struct help{
            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                        Next&& next_attr, Attributes&&... remains) const {
                next_attr.from(ctx, data, key, remains...);
            }

            template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
            inline void into(serde_ctx& ctx, T& data, std::string_view key,
                             Next&& next_attr, Attributes&&... remains) const {
                if constexpr(std::is_same_v<typename serde_ctx::Adaptor, cpcli::Command>) {
                    ctx.adaptor.arg(cpcli::Arg("help")
                                    .abbr("h")
                                    .full("help")
                                    .desc("Print usage information."));
                    ctx.adaptor.hook([](auto& cmd){
                        if(cmd["help"].visit()) {
                            cmd.print_help();
                            exit(1);
                        }
                        if(auto err = cmd.has_error(); err) {
                            fmt::print(stderr, "err: {}\n", *err);
                            cmd.print_help();
                            exit(1);
                        }
                    });
                }
                next_attr.into(ctx, data, key, remains...);
            }
        };
    }
    constexpr static auto help = detail::help{};
}


#endif
