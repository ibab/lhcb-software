################################################################################
# Package: MBMDump
################################################################################
gaudi_subdir(MBMDump v1r18)

gaudi_depends_on_subdirs(DAQ/MDF
                         Online/OnlineKernel
                         Online/UPI)

gaudi_add_library(MBMDump src/*.cpp
                  PUBLIC_HEADERS MBMDump
                  LINK_LIBRARIES OnlineKernel UPI MDFLib)

gaudi_add_executable(mbm_dump main/*.cpp
                     LINK_LIBRARIES MBMDump)
