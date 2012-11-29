CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Stripping v6r2p2
              USE Phys v16r2
                  Lbcom v13r2
              DATA TMVAWeights)
