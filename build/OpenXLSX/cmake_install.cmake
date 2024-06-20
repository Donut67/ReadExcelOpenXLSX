# Install script for directory: D:/Projects/ReadExcelOpenXLSX/OpenXLSX

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/ReadExcelOpenXLSX")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/ucrt64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenXLSX/headers" TYPE FILE FILES "D:/Projects/ReadExcelOpenXLSX/build/OpenXLSX/OpenXLSX-Exports.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenXLSX/headers" TYPE FILE FILES
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLCell.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLCellIterator.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLCellRange.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLCellReference.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLCellValue.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLColor.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLColumn.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLCommandQuery.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLConstants.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLContentTypes.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLDateTime.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLDocument.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLException.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLFormula.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLIterator.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLProperties.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLRelationships.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLRow.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLRowData.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLSharedStrings.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLSheet.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLWorkbook.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLXmlData.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLXmlFile.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLXmlParser.hpp"
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/headers/XLZipArchive.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenXLSX" TYPE FILE FILES "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/OpenXLSX.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "lib" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/ReadExcelOpenXLSX/build/output/libOpenXLSX.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX" TYPE FILE FILES
    "D:/Projects/ReadExcelOpenXLSX/OpenXLSX/OpenXLSXConfig.cmake"
    "D:/Projects/ReadExcelOpenXLSX/build/OpenXLSX/OpenXLSX/OpenXLSXConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX/OpenXLSXTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX/OpenXLSXTargets.cmake"
         "D:/Projects/ReadExcelOpenXLSX/build/OpenXLSX/CMakeFiles/Export/c72cc94553a1a0c9b05f75dae42fb1d7/OpenXLSXTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX/OpenXLSXTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX/OpenXLSXTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX" TYPE FILE FILES "D:/Projects/ReadExcelOpenXLSX/build/OpenXLSX/CMakeFiles/Export/c72cc94553a1a0c9b05f75dae42fb1d7/OpenXLSXTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX" TYPE FILE FILES "D:/Projects/ReadExcelOpenXLSX/build/OpenXLSX/CMakeFiles/Export/c72cc94553a1a0c9b05f75dae42fb1d7/OpenXLSXTargets-noconfig.cmake")
  endif()
endif()

