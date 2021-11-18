#include "cpcli/cpcli.hpp"
#include <serdepp/adaptor/nlohmann_json.hpp>

int main(int argc, char* argv[]) {
    using namespace cpcli;
    cli_parser parser(argc, argv);
    fmt::print("{}\n", serde::serialize<nlohmann::json>(parser).dump(4));
    auto cmd = cpcli::Command("tt")
        .about("Cpcli example cli program")
        .arg(Arg("help").abbr("h").full("help"))
        .arg(Arg("path").index(1).require(true))
        .arg(Arg("make options").index(3).value("make args..."))
        .arg(Arg("define").abbr("D").desc("Define Variable").value(ARG_TYPE::MAP))
        .arg(Arg("test").index(2).possible_values({"install", "uninstall"}))
        .subcommand(Command("s")
                    .about("s command")
                    .arg(Arg("help").abbr("h").full("help"))
                    .arg(Arg("prefix").index(1)));

    fmt::print("{}\n", serde::serialize<nlohmann::json>(cmd).dump(4));
    cmd.build();
    fmt::print("{}\n", serde::serialize<nlohmann::json>(cmd).dump(4));
    auto& match = cmd.parse(parser);
    fmt::print("{}\n", serde::serialize<nlohmann::json>(match).dump(4));
    if(match["help"].visit()) { match.print_help(); exit(1); }
    if(auto error = match.has_error(); error) {
        fmt::print("{}\n",*error);
        match.print_help();
        exit(1);
    }
    fmt::print("list:{}, {}\n", match["3"].visit(), match["3"].get<std::string>());
    


    return 0;
}
