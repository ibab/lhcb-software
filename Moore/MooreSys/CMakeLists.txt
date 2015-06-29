################################################################################
# Package: MooreSys
################################################################################
gaudi_subdir(MooreSys v23r7p2)

gaudi_depends_on_subdirs(Hlt/IndependenceTests
                         Hlt/Moore
                         L0/L0App)

gaudi_add_test(QMTest QMTEST)

# copy the TCK manifest for the _current_ release to the install area...
message(WARNING "installation of TCK manifest to be implemented")
#install(FILES ${HLTTCKROOT}/manifest/MOORE_$(version)
