project(svmtool)
cmake_minimum_required(VERSION 2.6)

include(CheckCXXCompilerFlag)
include(SystemSpecificInformations.cmake)


CHECK_CXX_COMPILER_FLAG(-std=c++0x HAVE_STDCPP0X)

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  message("Linux flags")

  if (HAVE_STDCPP0X)
    set(CMAKE_CXX_FLAGS "-std=c++0x -W -Wall -DBOOST_NO_HASH")
  else (HAVE_STDCPP0X)
    set(CMAKE_CXX_FLAGS "-W -Wall -DNO_STDCPP0X")
  endif (HAVE_STDCPP0X)
  
else ()

  message("Windows flags")
  add_definitions(-D WIN32)

  # By default, do not warn when built on machines using only VS Express:
  IF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  ENDIF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)

  set(CMAKE_C_FLAGS "/Zc:wchar_t- /EHsc /GR ${CMAKE_C_FLAGS} /IC:/msys64/usr/include")
  set(CMAKE_CXX_FLAGS "/Zc:wchar_t- /EHsc /GR /W4 /MP /FAu ${CMAKE_CXX_FLAGS}")

endif ()

add_subdirectory(src)
add_subdirectory(sample)

install(FILES
    config.svmt
DESTINATION etc)

install(FILES
    FindSVMTOOL++.cmake
DESTINATION share/cmake/Modules)


########### packaging ###############
INCLUDE(InstallRequiredSystemLibraries)

set(CPACK_MONOLITHIC_INSTALL 1)
SET(CPACK_PACKAGE_NAME "svmtool-cpp")
SET(CPACK_PACKAGE_CONTACT "Gael de Chalendar <Gael.de-Chalendar@cea.fr>")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The SVMTool PoS tagger C++ implementation")
SET(CPACK_PACKAGE_VENDOR "Jesus Gimenez")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LGPL.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "7")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET(CPACK_STRIP_FILES "")
SET(CPACK_SOURCE_STRIP_FILES "")
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
SET(CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_GENERATOR ${SPECIFIC_SYSTEM_PREFERED_CPACK_GENERATOR})

INCLUDE(CPack)
