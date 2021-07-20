#include "cpcli/cpcli.hpp"
#include "cpcli/attribute.hpp"
#include "cpcli/serde.hpp"
#include "serdepp/adaptor/nlohmann_json.hpp"

struct Build {
    DERIVE_SERDE(Build,
                 (&Self::hello, "hello", abbr{"h"}, desc{"hello test"}, arg_hint{"print str"})
                 (&Self::list, "list"  , abbr{"l"}, desc{"list test"}, arg_hint{"option list"}))
    std::string hello;
    std::vector<std::string> list;
};

struct Test {
    DERIVE_SERDE(Test, (&Self::build_, "build"))
    Build build_;
};


int main(int argc, char *argv[])
{
    using namespace cpcli;
    using namespace fmt::literals;

    auto cmd = cpcli::Command("test");
    serde::deserialize_from<cpcli::Command>(Test{}, cmd);
    //auto cmd = serde::deserialize<cpcli::Command>(t, "test");
    cmd.parse(argc, argv);
    fmt::print("{}\n",serde::deserialize<nlohmann::json>(cmd).dump(4));
    Test t = serde::serialize<Test>(cmd);
    fmt::print("{}\n",t);

    
    return 0;
}
