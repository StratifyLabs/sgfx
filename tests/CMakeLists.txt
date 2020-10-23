

set(DEPENDENCIES TestAPI FsAPI JsonAPI)

stratify_api_add_test_executable(${CMAKE_PROJECT_NAME} 32768 "${DEPENDENCIES}")



