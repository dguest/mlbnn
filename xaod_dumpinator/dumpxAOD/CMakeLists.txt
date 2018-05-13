#
# Basic setup for dumping Tagging information
#

# Set the minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# we want a debug build, so we know where things crashed
# set(CMAKE_BUILD_TYPE Debug)

# get analysis base
find_package( AnalysisBase 21.2 REQUIRED )

# # Set up a work directory project:
atlas_project( WorkDir 1.0 USE AnalysisBase ${AnalysisBase_VERSION} )

# Set the name of the package:
atlas_subdir( dumpxAOD )

# Packages that this package depends on. Not very much!
atlas_depends_on_subdirs(
  PRIVATE
  Control/xAODRootAccess
  Event/xAOD/xAODJet
  PhysicsAnalysis/AnalysisCommon/HDF5Utils)

# External(s) used by the package:
find_package(ROOT REQUIRED COMPONENTS RIO Hist Tree Net Core)
find_package(HDF5 1.10.1 REQUIRED COMPONENTS CXX C)

# common requirements
set(_common
  Root/JetWriter.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} Root
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${HDF5_LIBRARIES}
  xAODRootAccess
  xAODJet
  HDF5Utils)

# Build the test executable:
atlas_add_executable( dump-xaod util/dump-xaod.cxx ${_common} )
