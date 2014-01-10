################################################################################
# Package: AlignmentTools
################################################################################
gaudi_subdir(AlignmentTools v3r0p1)

gaudi_depends_on_subdirs(Alignment/AlignmentInterfaces
                         GaudiAlg
                         GaudiSvc)

find_package(GSL)

gaudi_add_module(AlignmentTools
                 src/*.cpp
                 INCLUDE_DIRS GSL Alignment/AlignmentInterfaces
                 LINK_LIBRARIES GSL GaudiAlgLib)

gaudi_install_python_modules()

gaudi_env(SET ALIGNMENTTOOLSOPTS \${ALIGNMENTTOOLSROOT}/options)

