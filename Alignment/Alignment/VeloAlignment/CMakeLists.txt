################################################################################
# Package: VeloAlignment
################################################################################
gaudi_subdir(VeloAlignment v4r13)

gaudi_depends_on_subdirs(Alignment/AlignTrTools
                         Alignment/AlignmentInterfaces
                         Det/VeloDet
                         Event/PhysEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         GaudiSvc)

find_package(ROOT COMPONENTS Hist Matrix Graf)

gaudi_add_module(VeloAlignment
                 src/*.cpp
                 INCLUDE_DIRS ROOT Alignment/AlignmentInterfaces
                 LINK_LIBRARIES ROOT VeloDetLib TrackEvent PhysEvent GaudiAlgLib GaudiKernel)

gaudi_install_python_modules()

gaudi_env(SET VELOALIGNMENTOPTS \${VELOALIGNMENTROOT}/options)

