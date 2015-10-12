################################################################################
# Package: VeloDet
################################################################################
gaudi_subdir(VeloDet v13r21p1)

gaudi_depends_on_subdirs(Det/DetDesc
                         Det/DetDescCnv
                         GaudiAlg
                         Kernel/LHCbKernel
                         Kernel/LHCbMath)

find_package(Boost COMPONENTS regex)
find_package(VDT)

gaudi_add_library(VeloDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS VeloDet
		  INCLUDE_DIRS VDT Boost
		  LINK_LIBRARIES VDT Boost DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib)

gaudi_add_module(VeloDet
                 src/component/*.cpp
		 INCLUDE_DIRS VDT Boost
		 LINK_LIBRARIES VDT Boost DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib VeloDetLib)

gaudi_add_dictionary(VeloDet
                     dict/VeloDetDict.h
                     dict/VeloDetDict.xml
		     INCLUDE_DIRS VDT Boost
		     LINK_LIBRARIES VDT Boost DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib VeloDetLib
                     OPTIONS "-U__MINGW32__")


gaudi_add_test(QMTest QMTEST)
