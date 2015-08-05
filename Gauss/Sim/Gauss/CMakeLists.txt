################################################################################
# Package: Gauss
################################################################################
gaudi_subdir(Gauss v48r3)

find_package(HepMC)

gaudi_install_python_modules()
gaudi_install_scripts()

gaudi_env(SET AlwaysKillLeadingHadron 1
          SET GAUSSOPTS \${GAUSSROOT}/options)

gaudi_add_test(QMTest QMTEST)
