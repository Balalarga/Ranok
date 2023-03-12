include(ModularBuild/BuildSystem)

DefineFolderAppWithDeps(
    Config
    Language
    Executor
    Graphics
    Localization
    Log
    Model
    Utils
    WindowSystem
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin"
)

target_compile_definitions(${PROJECT_NAME} PUBLIC "DEBUG_MODE")
target_compile_definitions(${PROJECT_NAME} PUBLIC "PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC "OVERRIDE_DEFAULT_CONFIG=1")
target_compile_definitions(${PROJECT_NAME} PUBLIC "RANOK_CODE_EXTENSION=\".rcode\"")
