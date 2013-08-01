################################################################################
# Package: MVADictTools
################################################################################
gaudi_subdir(MVADictTools v1r0)

gaudi_depends_on_subdirs(Phys/LoKiArrayFunctors)

find_package(ROOT COMPONENTS TMVA)

gaudi_add_module(MVADictTools
                 src/*.cpp
		 INCLUDE_DIRS ROOT
                 LINK_LIBRARIES ROOT LoKiArrayFunctorsLib)

# gaudi_install_python_modules()
# gaudi_add_test(QMTest QMTEST)
