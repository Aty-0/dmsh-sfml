file(GLOB_RECURSE SOURCES *.cpp)

add_executable(dmsh-sfml ${SOURCES})
target_include_directories(dmsh-sfml
    PUBLIC "${PROJECT_SOURCE_DIR}/src"
)

target_compile_features(dmsh-sfml PRIVATE cxx_std_20)
target_link_libraries(dmsh-sfml PRIVATE SFML::Graphics)
target_link_libraries(dmsh-sfml PRIVATE Pal::Sigslot)