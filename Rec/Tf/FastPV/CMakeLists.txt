################################################################################
# Package: FastPV
################################################################################
gaudi_subdir(FastPV v1r2p1)

gaudi_depends_on_subdirs(Det/DetCond
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         Tf/PatKernel)

gaudi_add_module(FastPV
                 src/*.cpp
                 INCLUDE_DIRS Tf/PatKernel
                 LINK_LIBRARIES DetCondLib MCEvent RecEvent TrackEvent GaudiAlgLib)

