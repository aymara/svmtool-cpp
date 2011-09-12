# - Find the native svmtool++ includes and library

# created from FindPNG.cmake (Copyright 2002-2009 Kitware, Inc.)
# by Gael de Chalendar (CEA LIST)

# - Find the native SVMTOOL++ includes and library
#
# This module defines
#  SVMTOOL++CPP_INCLUDE_DIR, where to find soci.h, etc.
#  SVMTOOL++CPP_LIBRARY_DIRS, the libraries to link against to use SVMTOOL++.
#  SVMTOOL++_DEFINITIONS - You should add_definitons(${SVMTOOL++_DEFINITIONS}) before compiling code that includes png library files.
#  SVMTOOL++_FOUND, If false, do not try to use SVMTOOL++.
# also defined, but not for general use are
#  SVMTOOL++CPP_LIBRARY_DIRS, where to find the SVMTOOL++ library.
# None of the above will be defined unles zlib can be found.
# SVMTOOL++ depends on Zlib

#=============================================================================
# Copyright 2002-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

find_path(SVMTOOL++CPP_INCLUDE_DIR svmtool/tagger.h)

set(SVMTOOL++_NAMES ${SVMTOOL++_NAMES} svmtool)
find_library(SVMTOOL++CPP_LIBRARY_DIRS NAMES ${SVMTOOL++_NAMES})

if (SVMTOOL++CPP_LIBRARY_DIRS AND SVMTOOL++CPP_INCLUDE_DIR)
    if (CYGWIN)
      if(BUILD_SHARED_LIBS)
          # No need to define SVMTOOL++_USE_DLL here, because it's default for Cygwin.
      else(BUILD_SHARED_LIBS)
        SET (SVMTOOL++_DEFINITIONS -DSVMTOOL++_STATIC)
      endif(BUILD_SHARED_LIBS)
    endif (CYGWIN)
  set(SVMTOOL++_LIBRARIES ${SVMTOOL++_NAMES})
endif (SVMTOOL++CPP_LIBRARY_DIRS AND SVMTOOL++CPP_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set SVMTOOL++_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SVMTOOL++  DEFAULT_MSG  SVMTOOL++CPP_LIBRARY_DIRS SVMTOOL++CPP_INCLUDE_DIR)

mark_as_advanced(SVMTOOL++CPP_INCLUDE_DIR SVMTOOL++CPP_LIBRARY_DIRS )
