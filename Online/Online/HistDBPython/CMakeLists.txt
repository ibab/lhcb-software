################################################################################
# Package: HistDBPython
################################################################################
gaudi_subdir(HistDBPython v1r9)

gaudi_depends_on_subdirs(GaudiAlg
                         Online/OnlineHistDB)

gaudi_add_module(HistDBPython src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib OnlineHistDB)
