cmake_minimum_required(VERSION 3.14)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "default build type")
endif()

project(MUMPS
LANGUAGES C Fortran
VERSION 5.3.3
DESCRIPTION "Sparse direct parallel solver"
HOMEPAGE_URL "http://mumps-solver.org/")

enable_testing()
include(CTest)
include(FeatureSummary)

# --- user options
option(autobuild "auto-build Lapack and/or Scalapack if missing or broken" OFF)
option(parallel "parallel or sequential (non-MPI, non-Scalapack)" ON)
option(intsize64 "use 64-bit integers in C and Fortran" OFF)
option(metis "use METIS" OFF)
option(scotch "use Scotch" OFF)
option(openmp "use OpenMP" OFF)
option(matlab "build optional Matlab interface" OFF)
option(ClangTest "run test for C-language" ON)

if(matlab)
  set(BUILD_SHARED_LIBS true)
endif()

if(NOT DEFINED arith)
  set(arith "s;d")
endif()

if(intsize64)
  add_compile_definitions(INTSIZE64)
endif()

message(STATUS "CMake ${CMAKE_VERSION}")

# --- compiler setup
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/compilers.cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# --- dependencies
find_package(Threads)

# -- Lapack
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/lapack.cmake)

# -- Scalapack / MPI

if(parallel)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/mpi.cmake)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/scalapack.cmake)

  set(NUMERIC_INCDIRS)
  set(NUMERIC_LIBS SCALAPACK::SCALAPACK MPI::MPI_Fortran MPI::MPI_C)
  if(openmp)
    find_package(OpenMP COMPONENTS C Fortran REQUIRED)
    list(APPEND NUMERIC_LIBS OpenMP::OpenMP_Fortran OpenMP::OpenMP_C)
  endif()
else(parallel)
  add_subdirectory(libseq)
  set(NUMERIC_INCDIRS ${CMAKE_CURRENT_SOURCE_DIR}/libseq)
  set(NUMERIC_LIBS mumps::mpiseq)
endif(parallel)
list(APPEND NUMERIC_LIBS LAPACK::LAPACK ${CMAKE_THREAD_LIBS_INIT})

# --- ordering libs

set(ORDERING_LIBS)
set(ORDERING_FLAGS -Dpord)
if(scotch)
  find_package(Scotch COMPONENTS ESMUMPS REQUIRED)
  list(APPEND ORDERING_FLAGS -Dscotch)
  list(APPEND ORDERING_LIBS Scotch::Scotch)
endif()
if(metis)
  find_package(METIS REQUIRED)
  list(APPEND ORDERING_FLAGS -Dmetis)
  list(APPEND ORDERING_LIBS METIS::METIS)
endif()
list(APPEND ORDERING_LIBS MUMPS::PORD)

# --- build MUMPS

add_subdirectory(PORD)
add_subdirectory(src)

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  # being used via FetchContent
  return()
endif()

# --- matlab (optional)
if(matlab)
  add_subdirectory(MATLAB)
endif()

# --- examples (optional)

string(REGEX MATCH "^Visual Studio" vs_backend ${CMAKE_GENERATOR})
if(vs_backend)
  message(STATUS "Visual Studio programs linking Fortran and C code require special configuration, see:")
  message(STATUS "https://software.intel.com/en-us/articles/configuring-visual-studio-for-mixed-language-applications")
  message(STATUS "Consider using Ninja backend instead of VS backend if possible for your project.")
  return()
endif()

add_subdirectory(tests)

add_subdirectory(examples)

# feature summary
add_feature_info(MumpsParallel parallel "parallel MUMPS (using MPI and Scalapack)")
add_feature_info(64-bit-integer intsize64 "use 64-bit integers in C and Fortran")
add_feature_info(Metis metis "METIS graph partitioning http://glaros.dtc.umn.edu/gkhome/metis/metis/overview")
add_feature_info(Scotch scotch "Scotch graph partitioning https://www.labri.fr/perso/pelegrin/scotch/")
add_feature_info(Openmp openmp "OpenMP API https://www.openmp.org/")
add_feature_info(MumpsMatlab matlab "MUMPS Matlab interface")
add_feature_info(AutoScalapack scalapack_external "auto-build Scalapack")
add_feature_info(AutoLapack lapack_external "auto-build Lapack")
feature_summary(WHAT ALL)
