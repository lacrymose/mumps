cmake_minimum_required(VERSION 3.12)
project(MUMPS 
  LANGUAGES C Fortran
  VERSION 5.1.2
  HOMEPAGE_URL http://mumps.enseeiht.fr/)

if(NOT DEFINED ARITH)
  set(ARITH d)
endif()

if(NOT DEFINED ORDERING)
  set(ORDERING PORD)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules)

set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

find_package(MPI REQUIRED COMPONENTS Fortran C)

find_package(SCALAPACK REQUIRED)


# -- mUMPS BUILD
include(${CMAKE_CURRENT_SOURCE_DIR}/sources.cmake)

add_library(ioc ${IOC_SRC})
target_include_directories(ioc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_library(mumps_common ${COMM_SRC})
target_link_libraries(mumps_common ioc ${SCALAPACK_LIBRARIES} MPI::MPI_Fortran MPI::MPI_C)
target_include_directories(mumps_common PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

if(ORDERING STREQUAL PORD)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../PORD ${CMAKE_CURRENT_BINARY_DIR}/lib)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../PORD/include)
  target_link_libraries(mumps_common pord)
  
  add_library(ordint ${PORDINT_SRC})
  target_compile_definitions(ordint PRIVATE "pord")
  target_include_directories(ordint PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
endif()

add_library(cint ${CINT_SRC})
target_compile_definitions(cint PRIVATE MUMPS_ARITH=MUMPS_ARITH_${ARITH} "Add_")
target_link_libraries(cint PRIVATE MPI::MPI_C)
target_include_directories(cint PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_library(${ARITH}mumps ${SRC})
target_include_directories(${ARITH}mumps PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(${ARITH}mumps 
#${ARITH}mumps_load ${ARITH}mumps_ooc ${ARITH}ana_aux_par
cint ${SCALAPACK_LIBRARIES} MPI::MPI_Fortran MPI::MPI_C)

add_dependencies(${ARITH}mumps mumps_common)

# --- install
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(UNIX)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/mumps-${PROJECT_VERSION}" CACHE PATH "..." FORCE)
  endif()
endif()

install(TARGETS pord mumps_common ${ARITH}mumps
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include  # no trailing slash!
  DESTINATION ${CMAKE_INSTALL_PREFIX})