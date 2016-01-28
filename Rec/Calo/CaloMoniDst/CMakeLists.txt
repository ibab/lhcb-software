################################################################################
# Package: CaloMoniDst
################################################################################
gaudi_subdir(CaloMoniDst v5r21)

gaudi_depends_on_subdirs(Associators/MCAssociators
                         Calo/CaloInterfaces
                         Calo/CaloKernel
                         Calo/CaloUtils
                         Event/DAQEvent
                         Event/L0Event
                         Event/LinkerEvent
                         Event/PhysEvent
                         Event/TrackEvent
                         Kernel/Relations
                         Tr/TrackInterfaces)

find_package(AIDA)
find_package(Boost)
find_package(GSL)
find_package(ROOT)

gaudi_add_module(CaloMoniDst
                 src/*.cpp
                 INCLUDE_DIRS ROOT AIDA Boost GSL AIDA Associators/MCAssociators Calo/CaloKernel Tr/TrackInterfaces
                 LINK_LIBRARIES ROOT Boost GSL AIDA CaloInterfaces CaloUtils DAQEventLib L0Event LinkerEvent PhysEvent TrackEvent RelationsLib)

gaudi_install_python_modules()

gaudi_env(SET CALOMONIDSTOPTS \${CALOMONIDSTROOT}/options)

