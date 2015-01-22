################################################################################
# Package: OnlineBrunelSys
################################################################################
gaudi_subdir(OnlineBrunelSys v0r1)

gaudi_depends_on_subdirs(BrunelSys
                         Online/Gaucho
                         Online/GaudiOnline
                         Online/Checkpointing
                         Online/GaudiSerialize
                         Online/GaudiCheckpointing
                         Online/GaudiCheckpointing)
