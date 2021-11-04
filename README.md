# Cpcli (Beta)
serdepp base command line parser

## Example
```cpp
#include "cpcli/cpcli.hpp"
#include <serdepp/adaptor/nlohmann_json.hpp>
#include <serdepp/adaptor/fmt.hpp>

enum class O {
    one, two, three
};

using namespace cpcli::attribute;
struct Build {
    DERIVE_SERDE(Build, .attributes(help, hook(&Self::execute), about{"build command"})
                 [attributes(abbr{"H"}, full{}, desc{"hello test"}, visible{false}, _value{})]
                 (&Self::hello, "hello")
                 [attributes(abbr{"l"}, full{}, desc{"list test"}, _value{})]
                 (&Self::list,  "list")
                 [attributes(abbr{"i"}, full{}, desc{"list test"}, _value{})]
                 (&Self::ilist,  "ilist")
                 [attributes(abbr{"D"}, full{"Def"}, desc{"CMake Option"}, _value{})]
                 (&Self::define, "define")
                 [attributes(abbr{"d"}, full{"def"}, desc{"CMake Option"}, _value{})]
                 (&Self::DD, "DD")
                 [attributes(abbr{"o"}, full{}, desc("or not"), _value{})]
                 (&Self::oo, "oo")
                 )
    std::string hello;
    std::vector<std::string> list;
    std::vector<int> ilist;
    std::map<std::string, bool> define;
    std::map<std::string, std::string> DD;
    O oo;

    static void execute(cpcli::Command& cmd) {
        auto map = serde::deserialize<Build>(cmd);
    }
};

struct __test {
    DERIVE_SERDE(__test, .attributes(help, hook(&Self::execute), about{"test command"}))
    static void execute(cpcli::Command& cmd) {
        auto map = serde::deserialize<__test>(cmd);
        fmt::print("{}\n", map);
    }
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

```
## Result
```console
❯ ./test -h    
cpcli test command

Usage:
   test [OPTIONS]
   test [OPTIONS] [SUBCOMMAND]

Option:
   -h, --help                       Print usage information.

Command:
   build                            build command      
   test                             test command       

./test build -h
build command

Usage:
   build [OPTIONS]

Option:
   -d, --def <KEY=STRING ...>       CMake Option        
   -D, --Def <KEY=BOOL ...>         CMake Option        
   -h, --help                       Print usage information.
   -i, --ilist <=INTEGER ...>       list test           
   -l, --list <=STRING ...>         list test           
   -o, --oo <one | two | three>     or not              
```

