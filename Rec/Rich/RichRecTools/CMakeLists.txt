################################################################################
# Package: RichRecTools
################################################################################
gaudi_subdir(RichRecTools v4r29)

gaudi_depends_on_subdirs(Det/RichDet
                         GaudiKernel
                         Kernel/LHCbKernel
                         Rich/RichRecBase)

find_package(GSL)

gaudi_add_module(RichRecTools
                 src/*.cpp
                 INCLUDE_DIRS GSL
                 LINK_LIBRARIES GSL RichDetLib GaudiKernel LHCbKernel RichRecBase)

