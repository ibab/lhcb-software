################################################################################
# Package: Hlt1Lines
################################################################################
gaudi_subdir(Hlt1Lines v7r20)

gaudi_depends_on_subdirs(Hlt/HltLine
                         Hlt/HltTracking
                         Tr/TrackMonitors)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
