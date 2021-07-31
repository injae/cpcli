#include "cpcli/cpcli.hpp"
#include "cpcli/attribute.hpp"
#include "cpcli/serde.hpp"
#include "serdepp/adaptor/nlohmann_json.hpp"

using namespace cpcli::attribute;
struct Build {
    DERIVE_SERDE(Build,
                 (&Self::help,  "help",  abbr{"h"}, desc{"help command"})
                 (&Self::hello, "hello", abbr{"H"}, desc{"hello test"},   arg_hint{"print str"})
                 (&Self::list,  "list" , abbr{"l"}, desc{"list test"},    arg_hint{"option list"})
                 (&Self::define, "Def" , abbr{"D"}, desc{"CMake Option"}, arg_hint{"cmake option"}))
    std::string hello;
    std::vector<std::string> list;
    std::map<std::string, bool> define;
    bool help;

    void execute(cpcli::Command& cmd) {
        auto t = serde::serialize<Build>(cmd);
        if(t.help) { cmd.print_help(); exit(1); }
        for(auto& [key, data] : t.define) { fmt::print("{}:{}\n",key,data); }
    }
};

struct Test {
    DERIVE_SERDE(Test,
                 (&Self::build_, "build", desc{"build command"}, hook_mem(&Build::execute))
                 (&Self::help,   "help",  abbr{"h"}, desc{"help command"})
                 )
    Build build_;
    bool help;
    std::map<std::string, std::string> m;
    void execute(cpcli::Command& cmd) {
        auto t = serde::serialize<Test>(cmd);
        fmt::print("{}\n", serde::deserialize<nlohmann::json>(cmd).dump(4));
        fmt::print("{}\n", serde::deserialize<nlohmann::json>(t).dump());
        if(cmd["help"].visit()) { cmd.print_help(); }
    }
};

struct App {
    DERIVE_SERDE(App, (&Self::test, "test",  hook_mem(&Test::execute)))
    Test test;
};


int main(int argc, char *argv[])
{
    using namespace cpcli;
    using namespace fmt::literals;

    auto app = serde::deserialize<cpcli::Command>(App{});
    app.sub_command("test").parse(argc, argv);
    
    return 0;
}
