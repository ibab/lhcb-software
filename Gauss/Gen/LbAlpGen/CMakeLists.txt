################################################################################
# Package: LbAlpGen
################################################################################
gaudi_subdir(LbAlpGen v6r1)

gaudi_depends_on_subdirs(Gen/LbPythia)

find_package(ALPGEN COMPONENTS alpgen alpsho atopyt)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(LbAlpGenLibRead
                  src/Lib/alpsho.F
                  src/Lib/Ahio.cpp
                  src/Lib/gahio.F
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS ALPGEN
                  LINK_LIBRARIES LbPythiaLib ALPGEN)

gaudi_add_module(LbAlpGenRead
                 src/component/ReadAlpGen.cpp
                 LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead)

gaudi_add_library(LbAlpGenLibzbb
                  src/Lib/*.cpp
                  src/Lib/*.F
                  ${ALPGEN_AUTHOR_DIR}/zqqlib/zqq.f
                  src/zqq/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead)

gaudi_add_module(LbAlpGenzbb
                 src/component/AlpGenProduction.cpp
                 src/component/AlpGenProductionzbb.cpp
                 LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead LbAlpGenLibzbb)

gaudi_add_library(LbAlpGenLib4q
                  src/Lib/*.cpp
                  src/Lib/*.F
                  ${ALPGEN_AUTHOR_DIR}/4Qlib/4Q.f
                  src/4Q/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead LbAlpGenLibzbb)

gaudi_add_module(LbAlpGen4q
                 src/component/AlpGenProduction.cpp
                 src/component/AlpGenProduction4q.cpp
                 LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead LbAlpGenLibzbb
                                LbAlpGenLib4q)

gaudi_add_library(LbAlpGenLibwqq
                  src/Lib/*.cpp
                  src/Lib/*.F
                  ${ALPGEN_AUTHOR_DIR}/wqqlib/wqq.f
                  src/wqq/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead LbAlpGenLibzbb LbAlpGenLib4q)

gaudi_add_module(LbAlpGenwqq
                 src/component/AlpGenProduction.cpp
                 src/component/AlpGenProductionwqq.cpp
                 LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead LbAlpGenLibzbb
                                LbAlpGenLib4q LbAlpGenLibwqq)

gaudi_add_library(LbAlpGenLibtop
                  src/Lib/*.cpp
                  src/Lib/*.F
                  ${ALPGEN_AUTHOR_DIR}/toplib/top.f
                  src/top/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead LbAlpGenLibzbb LbAlpGenLib4q LbAlpGenLibwqq)

gaudi_add_module(LbAlpGentop
                 src/component/AlpGenProduction.cpp
                 src/component/AlpGenProductiontop.cpp
                 LINK_LIBRARIES LbPythiaLib LbAlpGenLibRead LbAlpGenLibzbb
                                LbAlpGenLib4q LbAlpGenLibwqq LbAlpGenLibtop)

#when alpgen pdf files will be copied to $LHAPDFSETSROOT directory, use instead:
#gaudi_env(SET ALPGENPDFPATH \${LHAPDFSETSROOT}/alpgen)
gaudi_env(SET ALPGENPDFPATH \${ALPGEN_AUTHOR_DIR}/alplib/pdfdat
          SET LBALPGENOPTS \${LBALPGENROOT}/options)
