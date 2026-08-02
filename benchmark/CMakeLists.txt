add_executable(benchmark
    main.cpp
)

target_link_libraries(benchmark PRIVATE core)

target_compile_definitions(benchmark PRIVATE
        BUILD_FLAGS="$<JOIN:$<TARGET_PROPERTY:benchmark,COMPILE_OPTIONS>,\ >"
)

set_property(TARGET benchmark PROPERTY CXX_STANDARD 23)