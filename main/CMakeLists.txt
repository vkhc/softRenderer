add_executable(${MAIN} main.cpp) 

#target_include_directories(${MAIN} PUBLIC ${PROJECT_BINARY_DIR})

target_link_libraries(${MAIN} PUBLIC mylib)

target_compile_features(${MAIN} PUBLIC cxx_std_17)
