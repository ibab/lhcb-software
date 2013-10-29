################################################################################
# Package: MVADictTools
################################################################################
gaudi_subdir(MVADictTools v1r2)

gaudi_depends_on_subdirs(Phys/LoKiArrayFunctors)

find_package(ROOT COMPONENTS TMVA)

include_directories(src)

gaudi_add_library(MVADictToolsLib
                  src/lib/*.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES ROOT LoKiArrayFunctorsLib)

gaudi_add_module(MVADictTools
                 src/component/*.cpp
		 INCLUDE_DIRS ROOT
                 LINK_LIBRARIES ROOT MVADictToolsLib LoKiArrayFunctorsLib)

gaudi_add_unit_test(utesttmva src/applications/utesttmva.cxx
		    INCLUDE_DIRS ROOT
                    LINK_LIBRARIES ROOT MVADictToolsLib LoKiArrayFunctorsLib
                    TYPE Boost)

# gaudi_install_python_modules()
gaudi_add_test(QMTest QMTEST)
