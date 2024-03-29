cmake_minimum_required(VERSION 3.12)

set(CPPM_VERSION 0.0.13)
include(cmake/cppm_loader.cmake)
cppm_project()

project(cpcli VERSION 0.0.2 LANGUAGES C CXX)
cppm_setting()
cppm_cxx_standard(17)
cppm_compiler_option(DEFAULT)

find_cppkg(fmt 8.0.1  MODULE fmt::fmt-header-only TYPE lib)
find_cppkg(serdepp 0.1.3  MODULE serdepp::serdepp TYPE lib)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
find_cppkg(nlohmann_json 3.9.1  MODULE nlohmann_json::nlohmann_json TYPE lib)
find_cppkg(Catch2 2.9.1  MODULE Catch2::Catch2 TYPE lib)
endif()


cppm_target_define(cpcli STATIC
SOURCES
    src/command.cpp
)

cppm_examples_area()
if(CPCLI_BUILD_EXAMPLES)

cppm_target_define(test BINARY
SOURCES
    example/example.cpp
)

cppm_target_define(tt BINARY
SOURCES
    example/test.cpp
)

cppm_target_define(parse BINARY
SOURCES
    example/parser.cpp
)

endif()


set(cpcli_global_deps PRIVATE fmt serdepp)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   list(APPEND cpcli_global_deps PRIVATE nlohmann_json Catch2)
endif()
cppm_target_dependencies(cpcli
   ${cpcli_global_deps})

cppm_target_dependencies(test
   ${cpcli_global_deps}
   cpcli)

cppm_target_dependencies(tt
   ${cpcli_global_deps}
   cpcli)

cppm_target_dependencies(parse
   ${cpcli_global_deps}
   cpcli)


cppm_target_install(cpcli)
cppm_target_install(test)
cppm_target_install(tt)
cppm_target_install(parse)

