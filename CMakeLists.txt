cmake_minimum_required(VERSION 3.6)

include(cmake/cppm_tool.cmake)
cppm_project()
project(cpcli VERSION 0.0.1 LANGUAGES C CXX)
cppm_setting()

cppm_cxx_standard(17)
cppm_compiler_option(DEFAULT)



cppm_target_define(cpcli SHARED 
SOURCES 
	src/value.cpp)

cppm_target_define(cpcli-bin BINARY 
SOURCES 
	test/main.cpp
)


cppm_target_dependencies(cpcli 
)


cppm_target_dependencies(cpcli-bin 
)

cppm_target_install(cpcli)

cppm_target_install(cpcli-bin)

