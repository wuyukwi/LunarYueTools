CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    GIT_TAG v1.16.0
    DOWNLOAD_ONLY YES
)

set(SPDLOG_BUILD_SHARED ON)
add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
