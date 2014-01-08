################################################################################
# Package: Hlt1Lines
################################################################################
gaudi_subdir(Hlt1Lines v7r1)

gaudi_depends_on_subdirs(Hlt/HltLine
                         Hlt/HltTracking)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
