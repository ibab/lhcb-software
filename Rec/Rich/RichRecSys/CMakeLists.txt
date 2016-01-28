################################################################################
# Package: RichRecSys
################################################################################
gaudi_subdir(RichRecSys v5r9)

gaudi_depends_on_subdirs(Rich/RichENNRingFinder
                         Rich/RichGlobalPID
                         Rich/RichPIDMerge
                         Rich/RichParticleSearch
                         Rich/RichRecAlgorithms
                         Rich/RichRecBase
                         Rich/RichRecMCTools
                         Rich/RichRecMonitors
                         Rich/RichRecPhotonTools
                         Rich/RichRecStereoTools
                         Rich/RichRecTemplateRings
                         Rich/RichRecTools
                         Rich/RichRecTrackTools
			 DAQ/DAQSys)

gaudi_install_python_modules()

gaudi_env(SET RICHRECSYSOPTS \${RICHRECSYSROOT}/options)

