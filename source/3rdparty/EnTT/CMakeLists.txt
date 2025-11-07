CPMAddPackage(
  NAME EnTT
  VERSION 3.1.1
  GITHUB_REPOSITORY skypjack/entt
  DOWNLOAD_ONLY True
)

if (EnTT_ADDED)
  add_library(EnTT INTERFACE)
  target_include_directories(EnTT INTERFACE ${EnTT_SOURCE_DIR}/src)
endif()