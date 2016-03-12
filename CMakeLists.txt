# FC4 - A personal file compare utility
# CMakeLists.txt, generated gencmake.pl, on 2016/03/12 19:31:40
cmake_minimum_required( VERSION 2.8.6 )

# CMakeScripts or use the ones that come by default with CMake.
# set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})

project( fc4 )

# The version number.
set( fc4_MAJOR 4 )
set( fc4_MINOR 0 )
set( fc4_POINT 0 )

# Allow developer to select is Dynamic or static library built
#set( LIB_TYPE STATIC )  # set default static
#option( BUILD_SHARED_LIB "Set ON to build shared Library (DLL)" OFF )

if(CMAKE_COMPILER_IS_GNUCXX)
    set( WARNING_FLAGS -Wall )
endif(CMAKE_COMPILER_IS_GNUCXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
   set( WARNING_FLAGS "-Wall -Wno-overloaded-virtual" )
endif() 

if(WIN32 AND MSVC)
    # turn off various warnings - none needed in this compile
    set(WARNING_FLAGS "${WARNING_FLAGS} /wd4996")
    # foreach(warning 4244 4251 4267 4275 4290 4786 4305)
    #     set(WARNING_FLAGS "${WARNING_FLAGS} /wd${warning}")
    # endforeach(warning)
    set( MSVC_FLAGS "-DNOMINMAX -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D__CRT_NONSTDC_NO_WARNINGS" )
    # if (${MSVC_VERSION} EQUAL 1600)
    #    set( MSVC_LD_FLAGS "/FORCE:MULTIPLE" )
    # endif (${MSVC_VERSION} EQUAL 1600)
    #set( NOMINMAX 1 )
    # to distinguish between debug and release libs
    set( CMAKE_DEBUG_POSTFIX "d" )
else()
    # items for unix
endif()

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT" )
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}" )

# configuration file, if needed
# configure_file( ${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h )
# add_definitions( -DHAVE_CONFIG_H )
# include_directories( ${CMAKE_BINARY_DIR} )

#if(BUILD_SHARED_LIB)
#   set(LIB_TYPE SHARED)
#   message(STATUS "*** Building DLL library ${LIB_TYPE}")
#else()
#   message(STATUS "*** Option BUILD_SHARED_LIB is OFF ${LIB_TYPE}")
#endif()

# Total libs 0, exes 1
#################################################
##### LIBRARIES 0 #####
#################################################
#list(APPEND add_LIBS ${name})
# deal with install, if any...
#install( TARGETS ${name}
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib )
#install(FILES ${${name}_HDRS} DESTINATION include)
#################################################
##### EXECUTABLE 1 #####
#################################################

# src EXECUTABLE from [src],
# have 6 C/C++ sources, 5 headers
set(name fc4)
set(dir src)
include_directories( $dir )
set(${name}_SRCS
    ${dir}/Fc4.c
    ${dir}/Fc4Comp.c
    ${dir}/Fc4Help.c
    ${dir}/Fc4List.c
    ${dir}/Fc4Main.c
    ${dir}/Fc4Utils.c
    )
set(${name}_HDRS
    ${dir}/Fc4.h
    ${dir}/Fc4Help.h
    ${dir}/Fc4List.h
    ${dir}/Fc4Utils.h
    ${dir}/Fc4Vers.h
    )
add_executable( ${name} ${${name}_SRCS} ${${name}_HDRS} )
if (add_LIBS)
    target_link_libraries( ${name} ${add_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

# eof
