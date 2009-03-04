##############################################################################
# File for running Brunel on real data with default 2008 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#   gaudirun.py Brunel-2008.py <someDataFiles>.py
##############################################################################

from Configurables import ( Escher, TrackSys )
importOptions('$STDOPTS/PreloadUnits.opts')

Escher().DataType     = "2008"
#-- DC06 database tags are backwards compatible by construction, use default
#Escher.DDDBtag = "DC06-repro0710"
#Escher.CondDBtag = "DC06-repro0710"
Escher().DDDBtag = "default"
Escher().CondDBtag = "default"
Escher().Kalman = True
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().Detectors = ["OT"]
Escher().SpecialData = [ "fieldOff", "cosmics"]
TrackSys().ExpertTracking += [ "noDrifttimes" ]
TrackSys().TrackPatRecAlgorithms = [ "PatSeed" ]


##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-TED_Data.py
##############################################################################
# Just instantiate the configurable...
#theApp = Escher()




