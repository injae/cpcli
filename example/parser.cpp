#include "cpcli/cpcli.hpp"
#include <serdepp/adaptor/nlohmann_json.hpp>

struct compile_command {
    DERIVE_SERDE(compile_command,
                 (&Self::directory, "directory")
                 (&Self::command, "command")
                 (&Self::file, "file"))
    std::string directory;
    std::string command;
    std::string file;
};

std::deque<std::string> tokenize(std::string const &str, const char delim) 
{ 
    std::deque<std::string> out;
    // construct a stream from the string 
    std::stringstream ss(str); 
 
    std::string s; 
    while (std::getline(ss, s, delim)) { 
        out.push_back(s); 
    } 

    return out;
} 

std::string replace_all(const std::string &message, const std::string &pattern, const std::string &replace) {
    std::string result = message;
    std::string::size_type pos = 0;
    std::string::size_type offset = 0;
    while ((pos = result.find(pattern, offset)) != std::string::npos)
    {
    result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
    offset = pos + replace.size();
    }
    return result;
}


class clang_cli_parser : public cpcli::cli_parser_interface {
public:
    using cpcli::cli_parser_interface::cli_parser_interface;
    using cli_parser_interface::serde;
    void parse() {
        using namespace cpcli;
        name_ = std::filesystem::path(unparsed_args.front()).filename().string();
        unparsed_args.pop_front();
        while(!unparsed_args.empty()) {
            auto arg = unparsed_args.front(); 
            auto name_idx = 1;
            if(auto idx = arg.find('='); idx != std::string::npos) { // has value
                auto key= arg.substr(name_idx, idx-name_idx);
                opts_[key] += contains(opts_, key) ? "," + arg.substr(idx+1) : arg.substr(idx+1);
            } else {
                auto key = arg.substr(1);
                if(unparsed_args.size() > 2 && unparsed_args[1][0] != '-') {
                    opts_[key] += contains(opts_, key) ? "," + unparsed_args[1] : unparsed_args[1];
                    unparsed_args.pop_front();
                }
                else {
                    opts_[key] = "true";
                }
            }
            unparsed_args.pop_front();
        }
    }
};

int main(int argc, char *argv[])
{
    std::ifstream fstream("../compile_commands.json");
    std::stringstream buffer;
    buffer << fstream.rdbuf();
    auto json = nlohmann::json::parse(buffer.str());
    auto compile_commands = serde::deserialize<std::vector<compile_command>>(json);
    
    for(auto& ccmd : compile_commands) {
        ccmd.command = replace_all(ccmd.command, "-I", "-I ");
        clang_cli_parser parser(tokenize(ccmd.command, ' '));
        parser.parse();
        fmt::print("file: {}\n", ccmd.file);
        fmt::print("local include: {}\n", cpcli::converter<std::vector<std::string>>(parser.opts_["I"]));
        for(auto& dep_path : cpcli::converter<std::vector<std::string>>(parser.opts_["isystem"])) {
            std::filesystem::path path(dep_path);
            auto dep = path.parent_path().filename().string();
            for(auto const& dir_entry: std::filesystem::recursive_directory_iterator{path}) {
                fmt::print("-include: {}{}\n",dir_entry.path().string());
            }
            fmt::print("- depends: {}\n", dep);
        }
        
    }



    
    return 0;
}
