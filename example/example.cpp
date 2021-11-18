#include "cpcli/cpcli.hpp"
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/fmt.hpp>

enum class O {
    one, two, three
};

using namespace cpcli::attribute;
struct Build {
    DERIVE_SERDE(Build, .attributes(help, hook(&Self::execute), about{"build command"})
                 [attributes(flags{"H/hello"}, desc{"hello test"}, visible{false}, _value{})]
                 (&Self::hello, "hello")
                 [attributes(flags{"l/list"}, desc{"list test"}, _value{})]
                 (&Self::list,  "list")
                 [attributes(flags{"i/ilist"}, desc{"int list test"}, _value{})]
                 (&Self::ilist,  "ilist")
                 [attributes(flags{"d"}, desc{"CMake Option"}, _value{})]
                 (&Self::define, "define")
                 [attributes(flags{"D/Def"}, desc{"CMake Option"}, _value{})]
                 (&Self::DD, "DD")
                 [attributes(flags{"oo"}, desc("or not"), _value{})]
                 (&Self::oo, "oo")
                 [attributes(_index(1), desc("install commands"), possible_values{"install","uninstall"})]
                 (&Self::make_opt, "install_util")
                 [attributes(_index(2), desc("config"),  _value{})]
                 (&Self::make_opt2, "install_util_config")
                 )
    std::string hello;
    std::vector<std::string> list;
    std::vector<int> ilist;
    std::map<std::string, bool> define;
    std::map<std::string, std::string> DD;
    std::string make_opt;
    std::vector<std::string> make_opt2;
    O oo;

    static void execute(cpcli::Command& cmd) {
        auto map = serde::deserialize<Build>(cmd);
        fmt::print("{}\n",map);
    }
};

struct __test {
    DERIVE_SERDE(__test, .attributes(help, hook(&Self::execute), about{"test command"})
                 [attributes(_index(1), desc("remain"), _value{})]
                 (&Self::remain, "remain")
                 )
    static void execute(cpcli::Command& cmd) {
        auto map = serde::deserialize<__test>(cmd);
        fmt::print("{}\n", map.remain);
    }
    std::vector<std::string> remain;
};

struct Test {
    DERIVE_SERDE(Test, .attributes(help, hook(&Self::execute), about{"cpcli test command"})
                 (&Self::build_, "build")
                 (&Self::test, "test"))
    Build build_;
    std::map<std::string, std::string> m;
    __test test;

    static void execute(cpcli::Command &cmd) {
        auto map = serde::deserialize<Test>(cmd);
        fmt::print("{}\n",map);
    }
};

int main(int argc, char *argv[])
{
    cpcli::parse_with_exec<Test>(argc, argv);
    
    return 0;
}
