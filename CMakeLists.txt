cmake_minimum_required(VERSION 3.8)
project(parser)
set(CMAKE_C_STANDARD 11)

MESSAGE( STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME} )

include(ExternalProject)

set(LIBS m )

find_library(REGEX NAMES "regex" "libregex")
find_library(REGEX_PCRE NAMES "libpcre-posix" "libpcreposix")
find_library(PCRE NAMES  "libpcre2_32" "libpcre8" "libpcre16" "libpcre32" )
find_file(REGEX_H regex.h)

if(REGEX)
    set(LIBS ${LIBS}    ${REGEX})
elseif(REGEX_PCRE)
    set(LIBS ${LIBS}    ${REGEX_PCRE}   ${PCRE})
elseif(REGEX_H)
    set(LIBS ${LIBS})
else()
    MESSAGE( STATUS "There might not be a REGEX header on your platform." )
endif(REGEX)
MESSAGE( STATUS "libs: " ${LIBS} )

set(GCC_COVERAGE_COMPILE_FLAGS "-gstabs")
set(SOURCE_FILES main.c parser.c tokeniser.c  ${LINK_LIBRARIES} )
add_executable(calculator ${SOURCE_FILES})
target_link_libraries(calculator ${LIBS})




