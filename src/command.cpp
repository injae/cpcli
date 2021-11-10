#include "cpcli/command.hpp"

#include<tuple>
#include<map>

namespace cpcli {
    cli_parser& cli_parser::clear() {
        name_.clear();
        opts_.clear();
        return *this;
    }

    void cli_parser::parse() {
        name_ = std::filesystem::path(unparsed_args.front()).filename().string();
        unparsed_args.pop_front();
        while(!unparsed_args.empty()) {
            auto arg = unparsed_args.front(); 
            bool is_short = arg[0] == '-' ? true : false;
            bool is_long  = is_short && arg[1] == '-' ? true: false;
            if(is_short && !is_long) {
                if(arg.size() > 2) {
                    auto key= arg.substr(1, 1);
                    auto value = arg[3] == '=' ? arg.substr(2) : arg.substr(3);
                    opts_[key] += contains(opts_, key) ? "," + value : value;
                } else {
                    opts_[arg.substr(1, 1)] = "true";
                }
                unparsed_args.pop_front();
            }
            else if(is_long) {
                auto name_idx = 2;
                if(auto idx = arg.find('='); idx != std::string::npos) { // has value
                    auto key= arg.substr(name_idx, idx-name_idx);
                    opts_[key] += contains(opts_, key) ? "," + arg.substr(idx+1) : arg.substr(idx+1);
                } else {
                    opts_[arg.substr(name_idx)] = "true";
                }
                unparsed_args.pop_front();
            } else {
                break; 
            }
        }
    }

    std::optional<std::string> Arg::set(const std::string& value) {
        if(!value.empty()) {
            data_ = value;
        } else {
            if(arg_hint_ != "NONE" && require_) { return fmt::format("{} require data:", name_); } 
        }
        return std::nullopt;
    }

    std::string Arg::help_str() {
        using namespace fmt::literals;
        if(!is_visible_)  return "";
        if(index_ != 0) { return ""; }
        auto arg_type = arg_hint_ == "NONE" ? "" : "<{}>"_format(arg_hint_);
        auto front = abbr_.empty()
            ? "{:<3}--{} {}"_format("",full_, arg_type)
            : full_.empty()
            ? "{:<3}-{} {}"_format("",abbr_, arg_type)
            : "{:<3}-{}, --{} {}"_format("",abbr_, full_, arg_type);
        return "{:<35} {:<20}\n"_format(front, desc_);
    }

    std::string Command::help_str() {
        using namespace fmt::literals;
        auto front = "{:<3}{}"_format("", name_);
        auto end = " {0}"_format(desc_);
        return "{:<35}{:<20}\n"_format(front, end);
    }

    void Command::print_help() {
        if(!desc_.empty()) { fmt::print("{}\n\n",desc_); }
        fmt::print("Usage:\n");
        std::string has_cmd = commands_.empty() ? "" : " [SUBCOMMAND]";
        std::string has_opt = args_.empty() ? "" : " [OPTIONS]";
        std::string has_pos_arg = "";
        for(int i = 1; i <= positional_arg_size; ++i) {
            auto& pos_arg = args_[arg_mapper[std::to_string(i)]];
            has_pos_arg += " <" + (pos_arg.arg_hint_ == "NONE" ? pos_arg.name_ : pos_arg.arg_hint_) + ">";
        }
        if(!has_cmd.empty() && !has_pos_arg.empty() ) {
            fmt::print("   {0}{2}{1}\n   {0}{1}{3}\n\n",name_, has_opt, has_cmd, has_pos_arg);
        }
        else if(!has_cmd.empty()) {
            fmt::print("   {0}{1}\n   {0}{1}{2}\n\n",name_, has_opt, has_cmd);
        }
        else if(!has_pos_arg.empty()) {
            fmt::print("   {0}{1}{2}\n\n",name_, has_opt, has_pos_arg);
        }
        else {
            fmt::print("   {0}{1}\n\n",name_, has_opt);
        }

        if(!args_.empty()) {
            fmt::print("Option:\n");
            for(auto& [key, opt] : args_) { fmt::print("{}", opt.help_str()); }
            fmt::print("\n");
        }
        if(!commands_.empty()) {
            fmt::print("Command:\n");
            for(auto& [key, cmd] : commands_) { fmt::print("{}", cmd.help_str()); }
            fmt::print("\n");
        }
        
    }

    Command& Command::arg_after_init(const std::string& key) {
        auto& arg = args_[key];
        arg.name_ = key;
        if(!arg.full_.empty()) arg_mapper.emplace(arg.full_, arg.name_);
        if(!arg.abbr_.empty()) arg_mapper.emplace(arg.abbr_, arg.name_);
        if(arg.index_ != 0) {
            positional_arg_size = std::max(positional_arg_size, arg.index_);
            arg_mapper.emplace(std::to_string(arg.index_), arg.name_);
        }
        if(arg.require_) require_count++;
        return *this;
    }

    Command& Command::parse(cli_parser& parser) {
        visit_ = true;
        parser.parse();
        //fmt::print("{}\n",parser);
        size_t check_require=0;
        if(parser.name_ != name_) { error_message = fmt::format("wrong program name\n"); return *this; }
        size_t index = 0;
        for(auto& arg : parser.unparsed_args){
            auto sidx = std::to_string(++index);
            if(contains(commands_, arg)) {
                auto& cmd = commands_[arg];
                return cmd.parse(parser.clear());
            }
            else if(contains(arg_mapper, sidx)) {
                auto& parg = args_[arg_mapper[sidx]];
                if(parg.require_) check_require++;
                if(auto err = parg.set(arg); err) { error_message = err; return *this; }
            }
            else {
                error_message = fmt::format("unregisted command: {}\n",arg);
                return *this;
            }
        }

        for(auto& [key, value] : parser.opts_) {
            if(!contains(arg_mapper, key)) {
                error_message = fmt::format("unregisted options: {}\n", key);
                return *this;
            }
            auto& arg = args_[arg_mapper[key]];
            if(arg.require_) check_require++;
            if(auto err = arg.set(value); err) {
                error_message = err;   
                return *this;
            }
        }

        if(require_count != check_require) {
            error_message = fmt::format("need more arguments\n"); 
        }

        return *this;
    }

    Command&& Command::arg(Arg&& arg) {
        auto key = arg.name_;
        args_.emplace(arg.name_, std::move(arg));
        arg_after_init(key);
        return std::move(*this);
    }
}
