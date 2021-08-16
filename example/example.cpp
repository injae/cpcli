#include "cpcli/cpcli.hpp"

using namespace cpcli::attribute;
struct Build {
    DERIVE_SERDE(Build, .attributes(help, hook(&Build::execute))
                 [attributes(abbr{"H"}, desc{"hello test"}, arg{"print str"}, visible{false})]
                 (&Self::hello, "hello")
                 [attributes(abbr{"l"}, desc{"list test"}, arg{"path"})]
                 (&Self::list,  "list")
                 [attributes(abbr{"D"}, desc{"CMake Option"}, arg{"cmake option"})]
                 (&Self::define, "Def")
                 [attributes(abbr{"d"}, desc{"CMake Option"}, arg{"cmake option"})]
                 (&Self::DD, "def")
                 )
    std::string hello;
    std::vector<std::string> list;
    std::map<std::string, bool> define;
    std::map<std::string, std::string> DD;

    static void execute(cpcli::Command& cmd) {
        auto map = serde::deserialize<Build>(cmd);
        for(auto& [key, data] : map.define) { fmt::print("{}:{}\n",key,data); }
        for(auto& [key, data] : map.DD) { fmt::print("{}:{}\n",key,data); }
        for(auto& data : map.list) { fmt::print("{}\n",data); }
        for(auto& arg : cmd.args) { fmt::print("{},",arg); };
    }
};

struct __test {
    DERIVE_SERDE(__test, .attributes(help))
    static void execute(cpcli::Command& cmd) {
        fmt::print("hllo\n");
    }
};

struct Test {
    DERIVE_SERDE(Test, .attributes(help, hook(&Test::execute))
                 [attributes(desc{"build command"})]
                 (&Self::build_, "build")
                 [attributes(desc{"test command"},hook(&Test::ttt))]
                 (&Self::test, "test"))
    Build build_;
    std::map<std::string, std::string> m;
    __test test;

    static void execute(cpcli::Command& cmd) {
        auto t = serde::deserialize<Test>(cmd);
        fmt::print("hello\n");
    }

    static void ttt(cpcli::Command& cmd) {
        fmt::print("{}\n",cmd.args);
    }
};

int main(int argc, char *argv[])
{
    auto test = cpcli::parse<Test>("test");
    test.parse(argc,argv);
    
    return 0;
}
