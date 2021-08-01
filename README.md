# Cpcli (Beta)
serdepp base command line parser

## Example
```cpp
#include "cpcli/cpcli.hpp"

using namespace cpcli::attribute;
struct Build {
    DERIVE_SERDE(Build,
                 (&Self::help,  "help",  abbr{"h"}, desc{"help command"})
                 (&Self::hello, "hello", abbr{"H"}, desc{"hello test"},   arg_hint{"print str"}, visible{false})
                 (&Self::list,  "list" , abbr{"l"}, desc{"list test"},    arg_hint{"option list"})
                 (&Self::define, "Def" , abbr{"D"}, desc{"CMake Option"}, arg_hint{"cmake option"}))
    std::string hello;
    std::vector<std::string> list;
    std::map<std::string, bool> define;
    bool help;

    void execute(cpcli::Command& cmd) {
        auto t = serde::deserialize<Build>(cmd);
        if(t.help) { cmd.print_help(); exit(1); }
        for(auto& [key, data] : t.define) { fmt::print("{}:{}\n",key,data); }
        for(auto& data : t.list) { fmt::print("{}\n",data); }
        for(auto& arg : cmd.args) { fmt::print("{},",arg); } fmt::print("\n");
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
        auto t = serde::deserialize<Test>(cmd);
        if(t.help) { cmd.print_help(); }
    }
};

struct App {
    DERIVE_SERDE(App, (&Self::test, "test",  hook_mem(&Test::execute)))
    Test test;
};


int main(int argc, char *argv[])
{
    cpcli::parse<App>("test").parse(argc,argv);
    
    return 0;
}
```
## Result
```console
> ./test -h
Usage:
   test <command> [--verbose]

Option:
   --help [-h]                     : help command      

Command:
   build                           :build command      

❯ ./test build --help # sub command
Usage:
   build [--verbose]

Option:
   --Def [-D] MAP                  : CMake Option {cmake option}
   --help [-h]                     : help command      
   --list [-l] SEQUENCE            : list test {option list}


❯ ./test build -DTEST=ON -DTEST2=OFF # map type args std::map<std::string, bool>
TEST:true
TEST2:false

❯ ./test build -l=a -l=b  # list type args std::vector<std::string>
a
b

❯ ./test build a b c # unregisted args
a,b,c,
```

