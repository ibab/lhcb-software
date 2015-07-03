################################################################################
# Package: MVADictTools
################################################################################
gaudi_subdir(MVADictTools v1r9p1)

gaudi_depends_on_subdirs(Phys/LoKiArrayFunctors
                         Phys/DaVinciInterfaces)

find_package(ROOT COMPONENTS TMVA)

find_package(Boost COMPONENTS system filesystem)

gaudi_add_library(MVADictToolsLib
                  src/lib/*.cpp
                  PUBLIC_HEADERS MVADictTools
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost ROOT LoKiArrayFunctorsLib)

gaudi_add_module(MVADictTools
                 src/component/*.cpp
		 INCLUDE_DIRS ROOT 
                 LINK_LIBRARIES ROOT MVADictToolsLib LoKiArrayFunctorsLib)

gaudi_add_unit_test(utesttmva src/applications/utesttmva.cxx
		    INCLUDE_DIRS ROOT 
                    LINK_LIBRARIES ROOT MVADictToolsLib LoKiArrayFunctorsLib
                    TYPE Boost)

gaudi_install_python_modules()
gaudi_add_test(QMTest QMTEST)
