################################################################################
# Package: CaloPIDs
################################################################################
gaudi_subdir(CaloPIDs v5r21)

gaudi_depends_on_subdirs(Calo/CaloInterfaces
                         Calo/CaloUtils
                         GaudiAlg
                         Kernel/LHCbKernel
                         Kernel/LHCbMath
                         Kernel/Relations
                         Tr/TrackInterfaces)

find_package(AIDA)
find_package(Boost)

gaudi_add_module(CaloPIDs
                 src/*.cpp
                 INCLUDE_DIRS AIDA Tr/TrackInterfaces
                 LINK_LIBRARIES CaloInterfaces CaloUtils GaudiAlgLib LHCbKernel LHCbMathLib RelationsLib)

gaudi_install_python_modules()

gaudi_env(SET CALOPIDSOPTS \${CALOPIDSROOT}/options)

