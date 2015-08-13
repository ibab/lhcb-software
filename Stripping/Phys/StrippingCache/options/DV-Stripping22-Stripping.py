"""
Options for building Stripping22. 
"""

#use CommonParticlesArchive
stripping='stripping22'
from CommonParticlesArchive import CommonParticlesArchiveConf
CommonParticlesArchiveConf().redirect(stripping)


from Gaudi.Configuration import *
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
#Fix for TrackEff lines
#
from Configurables import DecodeRawEvent
DecodeRawEvent().setProp("OverrideInputs",4.1)

#
# Build the streams and stripping object
#
from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingSettings.Utils import strippingConfiguration
from StrippingArchive.Utils import buildStream
from StrippingArchive import strippingArchive

#get the configuration dictionary from the database
config  = strippingConfiguration(stripping)
#get the line builders from the archive
archive = strippingArchive(stripping)

stream = buildStream(stripping = config, streamName='ALL', archive = archive)

stream.lines=[l for l in stream.lines if (("TrackEffDown" not in l.name()))] #fix for Z02mumu bug

for l in stream.lines:
        if l.RequiredRawEvents and l._prescale==0:
                l.RequiredRawEvents=None

from Configurables import ProcStatusCheck
filterBadEvents = ProcStatusCheck()

sc = StrippingConf( Streams = [stream],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents, 
                    TESPrefix = 'Strip' )

enablePacking = True

from DSTWriters.Configuration import (SelDSTWriter,
                                      stripDSTStreamConf,
                                      stripDSTElements
                                      )


## Configure PV refitter
from Configurables import LoKi__PVReFitter
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").CheckTracksByLHCbIDs = True

## Configure the VeloTrack unpacker
from Configurables import UnpackTrack
unpackIt = UnpackTrack("unpackIt")
unpackIt.InputName = "pRec/Track/FittedHLT1VeloTracks"
unpackIt.OutputName = "Rec/Track/FittedHLT1VeloTracks"

#
# DaVinci Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = -1 # Number of events
DaVinci().HistogramFile = "DVHistos.root"
DaVinci().appendToMainSequence( [unpackIt] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().ProductionType = "Stripping"
DaVinci().DataType = "2015"

