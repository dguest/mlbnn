#
# Basic setup for dumping Tagging information
#

# Set the minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )
find_package( AnalysisBase 21.2 REQUIRED )

# # Set up a work directory project:
atlas_project( WorkDir 1.0 USE AnalysisBase ${AnalysisBase_VERSION} )

# Set up the runtime environment setup script(s):
# lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
# install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh
#    DESTINATION . )

# Set the name of the package:
atlas_subdir( dumpxAOD )

# Packages that this package depends on:
atlas_depends_on_subdirs(
  PRIVATE
  Control/xAODRootAccess
  # Event/xAOD/xAODEventInfo
  Event/xAOD/xAODJet
  # Event/xAOD/xAODTrack
  # Event/xAOD/xAODTruth
  # Event/xAOD/xAODCutFlow
  PhysicsAnalysis/AnalysisCommon/HDF5Utils
  # PhysicsAnalysis/JetTagging/FlavorTagDiscriminants
  # Control/AthToolSupport/AsgTools
  # Reconstruction/Jet/JetCalibTools
  # Reconstruction/Jet/JetSelectorTools
  # Reconstruction/Jet/BoostedJetTaggers
  # InnerDetector/InDetRecTools/InDetTrackSelectionTool
  # Reconstruction/Jet/JetMomentTools
)

# External(s) used by the package:
find_package(ROOT REQUIRED COMPONENTS RIO Hist Tree Net Core)
find_package(HDF5 1.10.1 REQUIRED COMPONENTS CXX C)

# common requirements
set(_common
  # src/JetWriter.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} src
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${HDF5_LIBRARIES}
  xAODRootAccess
  xAODJet
  HDF5Utils)

# Build the test executable:
atlas_add_executable( dump-xaod util/dump-xaod.cxx ${_common} )
