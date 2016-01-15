################################################################################
# Package: AlignTrTools
################################################################################
gaudi_subdir(AlignTrTools v4r17)

gaudi_depends_on_subdirs(Alignment/AlignmentInterfaces
                         Det/MuonDet
                         Det/OTDet
                         Det/STDet
                         Det/VeloDet
			                   Det/RichDet
                         Event/HltEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/LHCbKernel
                         Kernel/LHCbMath
                         Kernel/Relations
                         Muon/MuonInterfaces
                         ST/STKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(GSL)
find_package(AIDA)

gaudi_add_module(AlignTrTools
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Alignment/AlignmentInterfaces Tr/TrackInterfaces
                 LINK_LIBRARIES GSL MuonDetLib RichDetLib OTDetLib STDetLib VeloDetLib HltEvent LinkerEvent MCEvent RecEvent TrackEvent GaudiAlgLib LHCbKernel LHCbMathLib RelationsLib MuonInterfacesLib STKernelLib TrackFitEvent TrackKernel)

gaudi_install_python_modules()
