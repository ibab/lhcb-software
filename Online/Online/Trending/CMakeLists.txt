################################################################################
# Package: Trending
################################################################################
gaudi_subdir(Trending v2r5)

gaudi_depends_on_subdirs(GaudiAlg
                         Online/OnlineKernel)

gaudi_install_headers(Trending)

gaudi_add_module(Trending
                   src/TrendingTool.cpp
                   src/SimpleTrendWriter.cpp
                   src/TestTrend.cpp
                 LINK_LIBRARIES GaudiAlgLib OnlineKernel)

gaudi_add_executable(ExtractFromTrendingFile ExtractFromTrendingFile.cpp
                     LINK_LIBRARIES GaudiKernel)
gaudi_add_executable(DumpTrendingFile DumpTrendingFile.cpp
                     LINK_LIBRARIES GaudiKernel)

gaudi_install_python_modules()
