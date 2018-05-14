##################################################################
# Package level things
##################################################################
#

# Set the name of the package:
atlas_subdir( dumpxAOD )

# Packages that this package depends on
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
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${HDF5_LIBRARIES}
  xAODRootAccess
  xAODJet
  HDF5Utils)

# Build the test executable:
atlas_add_executable( dump-xaod util/dump-xaod.cxx ${_common} )
