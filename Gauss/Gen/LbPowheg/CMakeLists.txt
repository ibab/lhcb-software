################################################################################
# Package: LbPowheg
################################################################################
gaudi_subdir(LbPowheg v1r0p1)

gaudi_depends_on_subdirs(Gen/Generators
                         Gen/LbPythia)

find_package(Boost COMPONENTS filesystem system)
find_package(POWHEG-BOX)

gaudi_add_module(LbPowheg
                 src/component/*.cpp
                 LINK_LIBRARIES GeneratorsLib LbPythiaLib)

gaudi_env(SET CTEQPDF \${LHAPATH}
          SET LBPOWHEGOPTS \${LBPOWHEGROOT}/options
          SET POWHEGEXE ${POWHEG-BOX_BINARY_PATH})

