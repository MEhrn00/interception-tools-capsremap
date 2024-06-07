# This is adapted from Json Turner's "The Ultimate CMake / C++ Quick Start"
# https://www.youtube.com/watch?v=YbgH7yat-Jo

# Function for setting common compiler warnings on a target. This will work for
# GCC, Clang and MSVC.
function(set_compiler_warnings target_name)
  target_compile_options(
    ${target_name}
    PRIVATE -Wall
            -Wextra
            -Wpedantic
            -Wshadow # Variable shadowing
            -Wnon-virtual-dtor # Class with virtual functions has non-virtual
                               # destructor
            -Wcast-align # Alignment issues with casts
            -Wunused # Unused variables
            -Woverloaded-virtual # Overloaded virtual functions
            -Wconversion # Lossy type conversions
            -Wsign-conversion # Sign conversion
            -Wnull-dereference # null deref
            -Wdouble-promotion # float implicitly promoted to a double
            -Wformat=2 # Potential format string vulnerabilities
            -Wold-style-cast # Warn on C-style casts
  )
endfunction()
