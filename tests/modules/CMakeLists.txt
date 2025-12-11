set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/modules)

set(testFiles 
    type_name.cpp 
    utf8.cpp
    json.cpp)
set(CXX_STANDARD 20)

foreach (testFile ${testFiles})
    string(REGEX MATCH "([^\/]+$)" filename ${testFile})
    string(REGEX MATCH "[^.]*" executable_name test_modules_${filename})
    add_custom_target(${executable_name} ALL 
        COMMAND cairn -j0 -B${CMAKE_BINARY_DIR} -c${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${executable_name}=${CMAKE_CURRENT_LIST_DIR}/${testFile} 
        clang++ -g3 -D_DEBUG -std=c++20        
    )
    add_test(NAME ${executable_name} COMMAND ${executable_name})
endforeach ()
