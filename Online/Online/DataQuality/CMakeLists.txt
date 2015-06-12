################################################################################
# Package: DataQuality
################################################################################
gaudi_subdir(DataQuality v0r1)

find_package(pytools)
find_package(Oracle)
find_package(SQLite)

gaudi_depends_on_subdirs(BrunelSys
                         Online/Gaucho
                         Online/GaudiOnline
                         Online/Checkpointing
                         Online/GaudiSerialize
                         Online/GaudiCheckpointing
                         Online/GaudiCheckpointing)

gaudi_install_python_modules()
