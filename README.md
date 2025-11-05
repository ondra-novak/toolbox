# Toolbox organization

```
src/ - main files
tests/ - tests
```

Under src there is folder which specifies language and version

```
src/
  cpp.17/
  cpp.20/
  cpp.23/
  cpp.26/
  ts.5/
  js.5/
  js.6/
  ...
```

Under each directory are stored library and tool files. Each standalone library must be independent and usable separately. However, it is allowed to use other libraries from the toolbox, but they must be included correctly.

To include file from same version, just use #include "name"
To include file from different version, use #include <cpp.XX/name>

The meaning of the version is as follows: Libraries can be compiled in this language version and in newer ones. So, for example, it is possible to include a C++20 library for code in C++23. But not vice versa.

## Include guards for C++ headers

Include guards are mandatory. Don't use #pragma once because the file can be included from different paths. It is recommended to generate UUID like guard 

## Modules

It is recommended to design libraries to be compatible with C++ modules. It is also possible to split library into .hpp and .ixx (in the same folder)

## Headers

Use hpp is mandatory

## Imlementation files

It is recommended to put implementation part into header file and enable it through a macro definition. Typically this header is included into CPP file 
where the implementation is placed. This is not issue when modules are used

## Testing

Use constexpr testing whenever is possible. All tests are put into tests folder with same structure as src

## Documentation

Use in-source doxygen style

