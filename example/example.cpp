#include "cpcli/cpcli.hpp"

using namespace cpcli::attribute;
struct Build {
    DERIVE_SERDE(Build, .attributes(help, hook(&Build::execute))
                 [attributes(abbr{"H"}, desc{"hello test"}, arg_hint{"print str"}, visible{false})]
                 (&Self::hello, "hello")
                 [attributes(abbr{"l"}, desc{"list test"}, arg_hint{"path"})]
                 (&Self::list,  "list")
                 [attributes(abbr{"D"}, desc{"CMake Option"}, arg_hint{"cmake option"})]
                 (&Self::define, "Def"))
    std::string hello;
    std::vector<std::string> list;
    std::map<std::string, bool> define;

    static void execute(cpcli::Command& cmd) {
        auto map = serde::deserialize<Build>(cmd);
        for(auto& [key, data] : map.define) { fmt::print("{}:{}\n",key,data); }
        for(auto& data : map.list) { fmt::print("{}\n",data); }
        for(auto& arg : cmd.args) { fmt::print("{},",arg); };
    }
};

struct Test {
    DERIVE_SERDE(Test, .attributes(help, hook(&Test::execute))
                 [attributes(desc{"build command"})]
                 (&Self::build_, "build"))
    Build build_;
    std::map<std::string, std::string> m;

    static void execute(cpcli::Command& cmd) {
        auto t = serde::deserialize<Test>(cmd);
    }
};

int main(int argc, char *argv[])
{
    cpcli::parse<Test>("test").parse(argc,argv);
    
    return 0;
}
