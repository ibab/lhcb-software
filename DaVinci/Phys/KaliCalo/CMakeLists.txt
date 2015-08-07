################################################################################
# Package: KaliCalo
################################################################################
gaudi_subdir(KaliCalo v4r18)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         GaudiPython
                         Phys/AnalysisPython
                         Phys/LoKiAlgo
                         Phys/LoKiArrayFunctors
                         Phys/LoKiPhys
                         Phys/DaVinci)

find_package(pytools)

gaudi_add_module(KaliCalo
                 src/*.cpp
                 INCLUDE_DIRS pytools
                 LINK_LIBRARIES pytools CaloUtils GaudiPythonLib AnalysisPythonLib LoKiAlgo LoKiArrayFunctorsLib LoKiPhysLib)

gaudi_add_dictionary(KaliCalo
                     dict/KaliCaloDict.h
                     dict/KaliCalo.xml
                     INCLUDE_DIRS pytools
                     LINK_LIBRARIES pytools CaloUtils GaudiPythonLib AnalysisPythonLib LoKiAlgo LoKiArrayFunctorsLib LoKiPhysLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()

