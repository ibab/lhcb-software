################################################################################
# Package: LHCbKernel
################################################################################
gaudi_subdir(LHCbKernel v16r0)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiKernel
                         GaudiObjDesc
                         Kernel/LHCbMath)

find_package(Boost COMPONENTS system)
find_package(ROOT)
find_package(VDT)

include(GaudiObjDesc)

god_build_headers(xml/*.xml
                  DESTINATION Kernel)

gaudi_add_library(LHCbKernel
                  src/*.cpp
                  PUBLIC_HEADERS Kernel
		  INCLUDE_DIRS ROOT Boost VDT
		  LINK_LIBRARIES ROOT Boost VDT GaudiKernel GaudiObjDescLib LHCbMathLib GaudiAlgLib)

god_build_dictionary(xml/*.xml
                     HEADERS_DESTINATION Kernel
                     EXTEND dict/LHCbKernelDict.h
                     INCLUDE_DIRS ROOT Boost
                     LINK_LIBRARIES ROOT Boost GaudiKernel GaudiObjDescLib LHCbMathLib LHCbKernel)

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(RichSmartIDTest
                       src/Test/RichSmartIDTest/*.cpp
                       INCLUDE_DIRS ROOT Boost
                       LINK_LIBRARIES ROOT Boost GaudiKernel GaudiObjDescLib LHCbMathLib LHCbKernel)
endif()

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
