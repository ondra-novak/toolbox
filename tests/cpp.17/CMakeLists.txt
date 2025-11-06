set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/cpp.17)

set(testFiles sha1.cpp)
set(CXX_STANDARD 17)

foreach (testFile ${testFiles})
    string(REGEX MATCH "([^\/]+$)" filename ${testFile})
    string(REGEX MATCH "[^.]*" executable_name test_cpp17_${filename})
    add_executable(${executable_name} ${testFile})
    target_include_directories(${executable_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
    add_test(NAME ${executable_name} COMMAND ${executable_name})
endforeach ()
