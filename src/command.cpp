#include "cpcli/command.hpp"

namespace cpcli
{
    Option::Option(const Option& o)  {
        data_type = o.data_type;
        abbr_ = o.abbr_;
        name_ = o.name_;
        desc_ = o.desc_;
        data_ = o.data_;
        arg_hint_ = o.arg_hint_;
        require_data_ = o.require_data_;
        is_visible_ = o.is_visible_;
    }

    Option::Option(Option&& o) noexcept {
        abbr_ = std::move(o.abbr_);
        name_ = std::move(o.name_);
        desc_ = std::move(o.desc_);
        data_ = std::move(o.data_);
        data_type  = std::move(o.data_type);
        require_data_ = std::move(o.require_data_);
        is_visible_ = std::move(o.is_visible_);
        arg_hint_ = std::move(o.arg_hint_);
    }

    std::string Option::help_str() {
        using namespace fmt::literals;
        if(!is_visible_)  return "";
        auto arg_type = data_type != ARG_TYPE::NONE ? serde::to_string(data_type) : "";
        auto front = abbr_.empty() ? "{:<3}--{} {}"_format("",name_, arg_type)
                                    :"{:<3}--{} [-{}] {}"_format("",name_, abbr_, arg_type);
        auto end = arg_hint_.empty() ? ": {0}"_format(desc_)
                                        : ": {0} {{{1}}}"_format(desc_, arg_hint_);
        return "{:<35}{:<20}\n"_format(front, end);
    }

    std::string Command::help_str() {
        using namespace fmt::literals;
        auto front = "{:<3}{}"_format("", name_);
        auto end = ":{0}"_format(desc_);
        return "{:<35}{:<20}\n"_format(front, end);
    }

    void Command::print_help() {
        fmt::print("Usage:\n");
        auto has_cmd = commands_.empty() ? "" : " <command>";
        auto has_opt = options_.empty() ? "" : " [--verbose]";
        if(name_ != "") fmt::print("   {}{}{}\n\n",name_, has_cmd, has_opt);
        if(!options_.empty()) {
            fmt::print("Option:\n");
            for(auto& [key, opt] : options_) { fmt::print("{}", opt.help_str()); }
            fmt::print("\n");
        }
        if(!commands_.empty()) {
            fmt::print("Command:\n");
            for(auto& [key, cmd] : commands_) { fmt::print("{}", cmd.help_str()); }
            fmt::print("\n");
        }
    }

    void Option::parse(const std::string& str) {
        auto data_idx = str.find("=");
        bool has_data = data_idx != std::string::npos;
        if(has_data) {
            if(data_type == ARG_TYPE::SEQUENCE) { data_ += "," + str.substr(data_idx+1); }
            else if(data_type == ARG_TYPE::MAP) { data_ += "," + str.substr(2); }
            else data_ = str.substr(data_idx+1);
        } else {
            if(data_type == ARG_TYPE::NONE) data_ = "true";
            else if(require_data_) { fmt::print(stderr, "{} require data:", name_); exit(1); }
        }
    }

    void Command::parse(const std::string& str, std::deque<std::string>& args) {
        if(str != name_) { fmt::print(stderr, "wrong command name"); exit(1); }
        bool is_subcommand = false;
        while(!args.empty()) {
            auto parsed_str = args.front(); args.pop_front();
            auto key_str = parsed_str[0] == '-' && parsed_str[1] != '-' ?  2 : parsed_str.find("=");
            auto key = key_str != std::string::npos ? parsed_str.substr(0, key_str) : parsed_str;
            if(full_options_.find(key) != full_options_.end()) {
                options_[full_options_[key]].parse(parsed_str);
            }
            else if(abbr_options_.find(key) != abbr_options_.end()) {
                options_[abbr_options_[key]].parse(parsed_str);
            }
            else if(commands_.find(key) != commands_.end()) {
                is_subcommand=true;
                commands_[key].parse(parsed_str, args);
            } else {
                if(parsed_str[0] == '-') { fmt::print(stderr, "unknown option: {}\n",parsed_str); exit(1);}
                if(hooks_.empty() && !this->args.empty()) {fmt::print(stderr, "unparsed data\n"); exit(1);}
                args.push_front(parsed_str);
                break;
            }
        }
        if(!is_subcommand) {
            this->args = args;
            for (auto &hook : hooks_) { hook(*this);}
            //if(!this->args.empty()) {fmt::print(stderr, "{} unparsed data\n", name_); exit(1);}
        }
    }


    Command& Command::parse(int argc, char* argv[]) {
        for(int i = 1; i < argc; ++i) { args.push_back(std::string{argv[i]}); }  
        parse(name_, args);
        return *this;
    }

    Command& Command::add_option(Option& option) {
        using namespace fmt::literals;
        auto name = option.name_;
        full_options_["--{}"_format(name)] = name;
        if(!option.abbr_.empty()) abbr_options_["-{}"_format(option.abbr_)] = name;
        options_.emplace(name, option);
        return *this;
    }

    Command& Command::update_option(const std::string& name) {
        using namespace fmt::literals;
        auto& option = options_[name];
        if(!option.abbr_.empty()) abbr_options_["-{}"_format(option.abbr_)] = name;
        return *this;
    }

    Command& Command::add_command(const std::string& name) {
        commands_.emplace(name, Command{name});
        return commands_[name];
    }

    Command& Command::add_command(Command& command) {
        commands_.emplace(command.name_, command);
        return *this;
    }

    std::string Command::pop_arg() {
        if(args.empty()) throw parse_error("no remain args");
        auto arg = args.front(); args.pop_front();
        return arg;
    };
}
