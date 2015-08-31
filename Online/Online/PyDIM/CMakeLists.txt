################################################################################
# Package: PyDIM
################################################################################
gaudi_subdir(PyDIM v2r16)

gaudi_depends_on_subdirs(Online/DIM)

#use DIM           v* Online
#use Python        v* LCG_Interfaces

#apply_pattern pyd_module module=dimc   files=dimmodule.cpp
#apply_pattern pyd_module module=dimcpp files=dimcppmodule.cpp
gaudi_add_python_module(dimc dimmodule.cpp
                        LINK_LIBRARIES dim)
gaudi_add_python_module(dimcpp dimcppmodule.cpp
                        LINK_LIBRARIES dim)

# Disable generation of ConfUserDB (must be done before gaudi_install_python_modules)
set_directory_properties(PROPERTIES CONFIGURABLE_USER_MODULES None)

gaudi_install_python_modules()
