################################################################################
# Package: MisAligner
################################################################################
gaudi_subdir(MisAligner v2r1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Det/OTDet
                         Det/STDet
                         Det/VeloDet
                         GaudiAlg)

find_package(AIDA)

gaudi_add_module(MisAlignAlg
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES MuonDetLib OTDetLib STDetLib VeloDetLib)

gaudi_install_python_modules()

gaudi_env(SET Misaligner \${MISALIGNERROOT}/options)

