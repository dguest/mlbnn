#
# Basic setup for dumping Tagging information
#

# Set the minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# For some reason this is required (otherwise it crashes)
set(CMAKE_BUILD_TYPE Debug)

# get analysis base
find_package( AnalysisBase 21.2 REQUIRED )

# # Set up a work directory project:
atlas_project( WorkDir 1.0 USE AnalysisBase ${AnalysisBase_VERSION} )
