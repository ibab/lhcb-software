################################################################################
# Package: L0DU
################################################################################
gaudi_subdir(L0DU v10r33)

gaudi_depends_on_subdirs(Calo/CaloUtils
			 Calo/CaloDAQ
                         Det/CaloDet
                         Event/DAQEvent
                         Event/L0Event
                         L0/L0Base
                         DAQ/DAQKernel
                         L0/L0Interfaces)

find_package(AIDA)

gaudi_add_module(L0DU
                 src/*.cpp
                 INCLUDE_DIRS AIDA L0/L0Interfaces Calo/CaloDAQ
                 LINK_LIBRARIES CaloUtils CaloDetLib DAQKernelLib DAQEventLib L0Event L0Base)

gaudi_install_python_modules()

gaudi_env(SET L0DUOPTS \${L0DUROOT}/options)

