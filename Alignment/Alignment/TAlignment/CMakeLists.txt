################################################################################
# Package: TAlignment
################################################################################
gaudi_subdir(TAlignment v6r0)

gaudi_depends_on_subdirs(Alignment/AlignEvent
                         Alignment/AlignKernel
                         Alignment/AlignSolvTools
                         Alignment/AlignTrTools
                         Alignment/AlignmentInterfaces
                         Det/MuonDet
                         Det/STDet
                         Event/LinkerEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         GaudiPython
                         Kernel/LHCbKernel
                         Kernel/LHCbMath
                         Phys/LoKiCore
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(CLHEP)
find_package(GSL)
find_package(AIDA)
find_package(PythonLibs)

gaudi_add_module(TAlignment
                 src/*.cpp
                 INCLUDE_DIRS CLHEP GSL AIDA Alignment/AlignmentInterfaces Tr/TrackInterfaces
                 LINK_LIBRARIES CLHEP GSL AlignEvent AlignKernel MuonDetLib STDetLib LinkerEvent PhysEvent RecEvent TrackEvent GaudiAlgLib GaudiKernel LHCbKernel LHCbMathLib LoKiCoreLib TrackFitEvent TrackKernel)

gaudi_add_dictionary(TAlignment
                     dict/TAlignmentDict.h
                     dict/TAlignmentDict.xml
                     INCLUDE_DIRS CLHEP GSL PythonLibs Alignment/AlignmentInterfaces Tr/TrackInterfaces
                     LINK_LIBRARIES CLHEP GSL PythonLibs GaudiPythonLib AlignEvent AlignKernel MuonDetLib STDetLib LinkerEvent PhysEvent RecEvent TrackEvent GaudiAlgLib GaudiKernel LHCbKernel LHCbMathLib LoKiCoreLib TrackFitEvent TrackKernel
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()

