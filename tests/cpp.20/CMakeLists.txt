set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/cpp.20)

set(testFiles FunctionView.cpp OpenHashMap.cpp TypeName.cpp)
set(CXX_STANDARD 20)

foreach (testFile ${testFiles})
    string(REGEX MATCH "([^\/]+$)" filename ${testFile})
    string(REGEX MATCH "[^.]*" executable_name test_cpp20_${filename})
    add_executable(${executable_name} ${testFile})
    target_compile_features(${executable_name} PRIVATE cxx_std_20)
    target_include_directories(${executable_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)
    add_test(NAME ${executable_name} COMMAND ${executable_name})
endforeach ()
