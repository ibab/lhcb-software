################################################################################
# Package: LoKiAlgo
################################################################################
gaudi_subdir(LoKiAlgo v9r10)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiPython
                         Phys/DaVinciKernel
                         Phys/LoKiPhys)

find_package(PythonLibs)

gaudi_add_library(LoKiAlgo
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  INCLUDE_DIRS PythonLibs
                  LINK_LIBRARIES PythonLibs DaVinciKernelLib LoKiPhysLib GaudiAlgLib)

gaudi_add_dictionary(LoKiAlgo
                     dict/LoKiAlgoDict.h
                     dict/LoKiAlgo.xml
                     INCLUDE_DIRS PythonLibs PythonLibs
                     LINK_LIBRARIES PythonLibs PythonLibs DaVinciKernelLib LoKiPhysLib GaudiPythonLib LoKiAlgo
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
