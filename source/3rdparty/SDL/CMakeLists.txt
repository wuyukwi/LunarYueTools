add_subdirectory(SDL3 EXCLUDE_FROM_ALL)
set_target_properties(SDL3-shared PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL3)
set_target_properties(SDL_uclibc PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL3)