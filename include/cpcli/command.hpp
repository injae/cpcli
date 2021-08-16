#pragma once

#ifndef __CPCLI_APP_HPP__
#define __CPCLI_APP_HPP__

#include <memory>
#include <string>
#include <map>
#include <deque>
#include "cpcli/converter.hpp"

namespace cpcli
{
    struct Option {
        Option() = default;
        Option(const std::string& name) : name_(name), data_(""), data_type(ARG_TYPE::NONE) {}
        Option(const Option& o);
        Option(Option&& o) noexcept;
        void parse(const std::string& str);
        inline Option& abbr(const std::string& abbreviation) { abbr_ = abbreviation; return *this; }
        inline Option& desc(const std::string& description) { desc_ = description; return *this; }
        inline Option& arg_hint(const std::string& hint) { arg_hint_ = hint; return *this; }
        inline Option& visible(bool is_visible) { is_visible_ = is_visible; return *this; }
        inline bool visit() { return !data_.empty(); } 
        template<typename T>
        constexpr inline Option& arg(bool require=false) {
            data_type = find_arg_type<T>();
            require_data_=require;
            return *this;
        }
        template<typename T> inline T get() { return convert<T>(data_type, data_); }
        std::string help_str();

        std::string abbr_;
        std::string name_;
        std::string desc_;
        std::string data_;
        std::string arg_hint_;
        ARG_TYPE data_type;
        bool require_data_ = false;
        bool is_visible_ = true;

        DERIVE_SERDE(Option,
                     (&Self::abbr_, "abbr")
                     (&Self::name_, "name")
                     (&Self::desc_, "desc")
                     (&Self::data_type, "arg-type")
                     (&Self::data_, "arg")
                     (&Self::arg_hint_, "arg-hint")
                     (&Self::require_data_, "requrie")
                     (&Self::is_visible_, "visible"))
    };

    class Command {
    public:
        using Hook = std::function<void(Command&)>;
        Command() = default;
        Command(const std::string& name) : name_(name) {}
        Command(const Command& other) noexcept = default;
        Command(Command&& other) noexcept = default;
        Command& parse(int argc, char* argv[]);
        void parse(const std::string& str, std::deque<std::string>& args);

        inline Command& desc(const std::string& description) { desc_ = description; return *this; }
        Command& add_option(Option& option);
        Command& update_option(const std::string& name);

        Command& add_command(const std::string& name);
        Command& add_command(Command& command);
        std::string pop_arg();
        Option& operator[](const std::string& option_name) { return options_[option_name]; }
        Option& operator[](std::string_view option_name) { return options_[std::string{option_name}]; }
        Option& operator[](const char* option_name) { return options_[std::string{option_name}]; }
        Command& sub_command(const std::string& command) { return commands_[command]; }
        inline Command& name(std::string_view name) { name_ = std::string{name}; return *this; }
        std::string get_name() { return name_; }
        inline Command& hook(Hook&& func) { hooks_.push_back(std::move(func)); return *this; }
        bool has_sub_command(const std::string& command) { return commands_.find(command) != commands_.end(); }
        bool has_option(const std::string& option) { return options_.find(option) != options_.end(); }
        std::string help_str();
        void print_help();

        private:
            std::string name_;
            std::string desc_;
            std::map<std::string, Option> options_;
            std::map<std::string, std::string> abbr_options_;
            std::map<std::string, std::string> full_options_;
            std::map<std::string, Command> commands_;
            std::vector<Hook> hooks_;
        public:
            std::deque<std::string> args;

        DERIVE_SERDE(Command,
                    (&Self::name_, "name")
                    (&Self::desc_, "desc")
                    (&Self::options_, "options")
                    (&Self::abbr_options_, "abbrs")
                    (&Self::full_options_, "fulls")
                    (&Self::commands_, "commands"))
    };

    template<typename T>
    Command parse(const std::string& cmd="") {
        return cmd.empty() ? serde::serialize<cpcli::Command>(T{})
                           : serde::serialize<cpcli::Command>(T{}).name(cmd);
    }
}

#endif
