################################################################################
# Package: GaudiConf
################################################################################
gaudi_subdir(GaudiConf v19r14)

gaudi_depends_on_subdirs(Kernel/LHCbKernel
                         L0/L0DU
			 DAQ/RawEventCompat)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES GaudiConf.SimConf GaudiConf.DigiConf GaudiConf.CaloPackingConf GaudiConf.DstConf)
gaudi_install_python_modules()

gaudi_env(SET STDOPTS \${GAUDICONFROOT}/options)


gaudi_add_test(QMTest QMTEST)
