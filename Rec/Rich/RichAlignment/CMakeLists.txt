################################################################################
# Package: RichAlignment
################################################################################
gaudi_subdir(RichAlignment v1r12)

gaudi_depends_on_subdirs(Event/MCEvent
                         Kernel/MCInterfaces
                         Rich/RichRecBase)

gaudi_add_module(RichAlignment
                 src/*.cpp
                 INCLUDE_DIRS AIDA Kernel/MCInterfaces
                 LINK_LIBRARIES MCEvent RichRecBase)

gaudi_install_python_modules()

