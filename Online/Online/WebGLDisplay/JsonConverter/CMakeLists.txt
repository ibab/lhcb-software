################################################################################
# Package: JsonConverter
################################################################################
gaudi_subdir(JsonConverter v0r1)
find_package(GSL)

gaudi_depends_on_subdirs(LHCbSys
			 RecSys
                         DaVinciSys)

gaudi_add_module(JsonConverter src/*.cpp
	         INCLUDE_DIRS GSL
                 LINK_LIBRARIES GaudiKernel GaudiAlgLib LHCbKernel DaVinciKernelLib VeloDetLib OTDetLib STDetLib MuonDetLib gsl )
