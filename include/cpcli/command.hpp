#pragma once

#ifndef __CPCLI_CHUNK_HPP__
#define __CPCLI_CHUNK_HPP__

#include <serdepp/serde.hpp>
#include <serdepp/adaptor/fmt.hpp>
#include <deque>
#include <vector>
#include <map>
#include <filesystem>
#include "cpcli/converter.hpp"
#include <serdepp/adaptor/nlohmann_json.hpp>

namespace cpcli {
    template<class Container, typename T>
    bool contains(Container& con, T key) { return con.find(key) != con.end(); }

    class cli_parser {
    public:
        DERIVE_SERDE(cli_parser, (&Self::name_, "name")(&Self::opts_, "opts")(&Self::unparsed_args, "unparsed"))
        cli_parser() = default;
        cli_parser(const cli_parser& o) = default;
        cli_parser(const std::deque<std::string>& args) : unparsed_args(args) {}
        cli_parser(int argc, char* argv[])  {
            for(int i = 0; i < argc; ++i) { unparsed_args.emplace_back(argv[i]); }  
        }
        cli_parser& clear();
        void parse();
        std::deque<std::string>& remains() { return unparsed_args; }
        std::deque<std::string> unparsed_args;
        std::string name_;
        std::map<std::string, std::string> opts_;
    };

    class Command;
    class Arg {
    public:
        friend Command;
        Arg() = default;
        Arg(const Arg& o) = default;
        Arg(Arg&& o) noexcept = default;
        Arg(const std::string& name) : name_(name), data_(""), arg_hint_("NONE") {}
        void parse(const std::string& str);

        inline Arg&& abbr(const std::string& abbreviation) { abbr_ = abbreviation; return std::move(*this); }
        inline Arg&& full(const std::string& full) { full_ = full; return std::move(*this); }
        inline Arg&& desc(const std::string& description) { desc_ = description; return std::move(*this); }
        inline Arg&& arg_hint(const std::string& hint) { arg_hint_ = hint; return std::move(*this); }
        inline Arg&& visible(bool is_visible) { is_visible_ = is_visible; return std::move(*this); }
        inline Arg&& require(bool is_require) { require_ = is_require; return std::move(*this); }
        inline Arg&& index(size_t index) { index_ = index; return std::move(*this); }
        template<class T>
        inline Arg&& value() {
            if constexpr(serde::is_enumable_v<T>) {
                data_ = std::string{serde::type::enum_t::to_str(magic_enum::enum_value<T>(0))};
            }
            arg_hint_ = make_hint<T>(find_arg_type<T>());
            return std::move(*this);
        }
        inline Arg&& value(std::string_view hint) {
            arg_hint_ = hint;
            return std::move(*this);
        }

        inline Arg&& value(ARG_TYPE type) {
            arg_hint_ = make_hint(type);
            return std::move(*this);
        }

        inline bool visit() { return !data_.empty(); } 
        template<typename T> inline T get() { return convert<T>(arg_hint_, data_); }
        std::string help_str();
        bool is_require() { return require_; }
    private:
        std::optional<std::string> set(const std::string& value);

    private:
        std::string name_;
        std::string abbr_;
        std::string full_;
        std::string desc_;
        std::string data_;
        std::string arg_hint_="NONE";
        size_t index_ = 0;
        bool require_ = false;
        bool is_visible_ = true;

    public:
        DERIVE_SERDE(Arg,
                     (&Self::name_, "name")
                     [attributes(make_optional)]
                     (&Self::abbr_, "abbr")
                     [attributes(make_optional)]
                     (&Self::full_, "full")
                     [attributes(make_optional)]
                     (&Self::desc_, "desc")
                     (&Self::data_, "arg")
                     (&Self::index_, "index")
                     [attributes(make_optional)]
                     (&Self::arg_hint_, "arg-hint")
                     (&Self::require_, "requrie")
                     (&Self::is_visible_, "visible"))
    };

    class Command {
    public:
        using Hook = std::function<void(Command&)>;
        Command() = default;
        Command(Command&& o) noexcept=default;
        Command(const Command& o)=default;
        Command(const std::string& name) : name_(name) {}

        Command& parse(cli_parser& parser);
        Command&& arg(Arg&& arg);
        Command&& name(const std::string& name) { name_ = name; return std::move(*this); }
        Command&& about(const std::string& desc) { desc_ = desc;  return std::move(*this); }
        Command&& hook(Hook&& func) { hooks_.push_back(std::move(func));  return std::move(*this); }

        std::string help_str();
        void print_help();
        Command&& subcommand(Command&& command) {
            commands_.emplace(command.name_, std::move(command));
            return std::move(*this);
        }

        Arg& operator[](std::string_view name) { return args_.at(arg_mapper.at(std::string{name})); }
        Command& get_subcommand(std::string_view name) { return commands_[std::string{name}]; }
        Arg& get_arg(std::string_view name) { return args_[std::string{name}]; }
        bool has_sub_command(std::string_view name) { return contains(commands_, std::string{name}); }
        bool has_arg(std::string_view name) {return contains(args_, std::string{name});}
        bool is_visit() { return visit_;}

        Command& arg_after_init(const std::string& key);
        const std::optional<std::string>& has_error() { return error_message; }
        inline void call_hook() { for(auto& hook : hooks_) { hook(*this); } }

     private:
        std::string name_;
        std::string version_;
        std::string desc_;
        std::map<std::string, Arg> args_;
        std::map<std::string, std::string> arg_mapper;
        std::map<std::string, Command> commands_;
        std::optional<std::string> error_message;
        int require_count=0;
        size_t positional_arg_size=0;
        bool visit_=false;
        std::vector<Hook> hooks_;
    public:
        DERIVE_SERDE(Command,
                     (&Self::name_, "name")
                     (&Self::version_, "version")
                     (&Self::desc_, "desc")
                     (&Self::args_, "args")
                     (&Self::visit_, "visit")
                     (&Self::arg_mapper, "mapper")
                     (&Self::commands_, "commands")
                     (&Self::positional_arg_size, "pargs_size"))
    };

    template<typename T>
    Command parse(const std::string& cmd="") {
        return cmd.empty() ? serde::serialize<cpcli::Command>(T{})
                           : serde::serialize<cpcli::Command>(T{}).name(cmd);
    }

    template<typename T>
    Command parse_with_exec(int argc, char *argv[]) {
        cpcli::cli_parser parser(argc, argv);
        auto cmd = parse<T>(std::filesystem::path(argv[0]).filename().string());
        cmd.parse(parser).call_hook();
        return cmd;
    }
}

#endif
