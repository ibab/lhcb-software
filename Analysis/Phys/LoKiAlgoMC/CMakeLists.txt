################################################################################
# Package: LoKiAlgoMC
################################################################################
gaudi_subdir(LoKiAlgoMC v9r7)

gaudi_depends_on_subdirs(GaudiPython
                         Kernel/MCInterfaces
                         Phys/LoKiAlgo
                         Phys/LoKiGenMC
                         Phys/LoKiPhysMC)

find_package(PythonLibs)

gaudi_add_library(LoKiAlgoMCLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  INCLUDE_DIRS PythonLibs Kernel/MCInterfaces
                  LINK_LIBRARIES PythonLibs LoKiAlgo LoKiGenMCLib LoKiPhysMCLib)

gaudi_add_module(LoKiAlgoMC
                 src/Components/*.cpp
                 INCLUDE_DIRS PythonLibs Kernel/MCInterfaces
                 LINK_LIBRARIES PythonLibs LoKiAlgo LoKiGenMCLib LoKiPhysMCLib LoKiAlgoMCLib)

gaudi_add_dictionary(LoKiAlgoMC
                     dict/LoKiAlgoMCDict.h
                     dict/LoKiAlgoMC.xml
                     INCLUDE_DIRS PythonLibs PythonLibs Kernel/MCInterfaces
                     LINK_LIBRARIES PythonLibs PythonLibs LoKiAlgo LoKiGenMCLib LoKiPhysMCLib GaudiPythonLib LoKiAlgoMCLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
