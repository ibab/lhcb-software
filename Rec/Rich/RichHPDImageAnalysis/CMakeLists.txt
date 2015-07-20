################################################################################
# Package: RichHPDImageAnalysis
################################################################################
gaudi_subdir(RichHPDImageAnalysis v1r17)

gaudi_depends_on_subdirs(GaudiUtils
                         Rich/RichRecBase)

find_package(AIDA)
find_package(Boost COMPONENTS serialization)
find_package(COOL COMPONENTS CoolKernel CoolApplication)
find_package(ROOT COMPONENTS Hist Graf Graf3d Gpad Rint Postscript Matrix Physics Minuit2)

gaudi_add_library(RichHPDImageAnalysisLib
                  src/lib/*.cpp
                  PUBLIC_HEADERS RichHPDImageAnalysis
                  INCLUDE_DIRS ROOT AIDA Boost COOL
                  LINK_LIBRARIES ROOT Boost COOL GaudiUtilsLib RichRecBase)

gaudi_add_module(RichHPDImageAnalysis
                 src/component/*.cpp
                 INCLUDE_DIRS ROOT AIDA Boost COOL
                 LINK_LIBRARIES ROOT Boost COOL GaudiUtilsLib RichRecBase RichHPDImageAnalysisLib)

gaudi_add_dictionary(RichHPDImageAnalysis
                     dict/RichHPDImageAnalysisDict.h
                     dict/RichHPDImageAnalysisDict.xml
                     INCLUDE_DIRS ROOT AIDA Boost COOL
                     LINK_LIBRARIES ROOT Boost COOL GaudiUtilsLib RichRecBase RichHPDImageAnalysisLib
                     OPTIONS "-U__MINGW32__")

