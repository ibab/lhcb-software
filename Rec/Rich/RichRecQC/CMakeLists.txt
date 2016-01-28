################################################################################
# Package: RichRecQC
################################################################################
gaudi_subdir(RichRecQC v2r73)

gaudi_depends_on_subdirs(Event/MCEvent
                         Kernel/MCInterfaces
                         Rich/RichAlignment
                         Rich/RichRecBase
                         Rich/RichRecSys)

gaudi_add_module(RichRecQC
                 src/*.cpp
                 INCLUDE_DIRS AIDA Kernel/MCInterfaces
                 LINK_LIBRARIES MCEvent RichRecBase)

gaudi_install_python_modules()

