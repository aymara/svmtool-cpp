project(svmtool)
cmake_minimum_required(VERSION 2.6)

include(CheckCXXCompilerFlag)
include(SystemSpecificInformations.cmake)

option(WITH_ARCH "Enable architecture optimizations" OFF)
message("WITH_ARCH=${WITH_ARCH}")

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    if ("x${CMAKE_BUILD_TYPE}" STREQUAL "xRelease")
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=generic -march=x86-64")
    endif()
endif()


CHECK_CXX_COMPILER_FLAG(-std=c++0x HAVE_STDCPP0X)

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
  message("Linux flags")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-omit-frame-pointer -g")

    if (WITH_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic -msse4.2")
    endif()

    if (HAVE_STDCPP11)
      message("C++11 supported")
      set(CMAKE_CXX_FLAGS "-std=c++11 -DBOOST_NO_HASH ${CMAKE_CXX_FLAGS}")
      CHECK_CXX_COMPILER_FLAG(-Wsuggest-override HAVE_SUGGEST_OVERRIDE)
      if (HAVE_SUGGEST_OVERRIDE)
#       set(CMAKE_CXX_FLAGS "-Wsuggest-override -Werror=suggest-override ${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "-Wsuggest-override ${CMAKE_CXX_FLAGS}")
      endif()
    elseif (HAVE_STDCPP0X)
      message("C++0x supported")
      set(CMAKE_CXX_FLAGS "-std=c++0x -DBOOST_NO_HASH ${CMAKE_CXX_FLAGS}")
    else ()
      message("C++0x NOT supported")
      set(CMAKE_CXX_FLAGS "-DNO_STDCPP0X ${CMAKE_CXX_FLAGS}")
    endif ()
    set(CMAKE_CXX_FLAGS "-W -Wall ${CMAKE_CXX_FLAGS}")
  
else ()

  message("Windows flags")
  add_definitions(-D WIN32)

  # By default, do not warn when built on machines using only VS Express:
  IF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  ENDIF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)

  set(CMAKE_C_FLAGS "/Zc:wchar_t- /EHsc /GR ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "/Zc:wchar_t- /EHsc /GR /W4 /MP /FAu ${CMAKE_CXX_FLAGS}")

  set(LIB_INSTALL_DIR "bin")
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
