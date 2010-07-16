########################################################################
#
#
# Options for a DaVinci job creating DSTs
#
# It looks for a J/psi and then saves the events in a DST
#
# For a more complicated example with several DSTs
# corresponding to various mass ranges look in
# DaVinciSys/tests/options
#
# @author Patrick Koppenburg
# @date 2009-02-03
#
# @added FSR writing: Jaap Panman
#
##############################################################################
from Gaudi.Configuration import *
from Configurables import DaVinci, SelDSTWriter, DumpFSR
##############################################################################
from Configurables import GaudiSequencer, FilterDesktop
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Get a J/psi
#
jpsi = FilterDesktop('MyJpsi')
jpsi.Code = "ALL"

from PhysSelPython.Wrappers import Selection, DataOnDemand, SelectionSequence

MyLooseJpsi = DataOnDemand(Location = 'Phys/StdLooseJpsi2MuMu')

SelJpsi = Selection('SelJpsi',
                    Algorithm = jpsi,
                    RequiredSelections = [MyLooseJpsi])

SeqJpsi = SelectionSequence('SeqJpsi', TopSelection = SelJpsi)

#from MicroDSTExample.Selections import SeqBs2Jpsi2MuMuPhi2KK
#SeqBs = SeqBs2Jpsi2MuMuPhi2KK.SeqBs2Jpsi2MuMuPhi2KK

#
# This the bit that declares the sequence to the Dst writer
# seq will be passed to DaVinci(). You do not need to pass it yourself.
# The second line allows to save particles (not yet 100% functional)
#
dstWriter = SelDSTWriter("JpsiDSTWriter",
                         SelectionSequences = [SeqJpsi],
                         SaveCandidates = True,
                         CopyMCTruth = False,
                         WriteFSR = True,
                         OutputFileSuffix = "Test")
##############################################################################
DaVinci().EvtMax = 500
DaVinci().PrintFreq = 100
DaVinci().UserAlgorithms = [ dstWriter.sequence(),
                             DumpFSR("dump") ]
# DaVinci().SkipEvents = 0
DaVinci().DataType = "2009" 
DaVinci().InputType = "DST"
DaVinci().Simulation   = False
DaVinci().WriteFSR = False   ##############TODO: not yet connected

DaVinci().Input =  [
    #"DATAFILE='PFN:LumiData_062910_0000000001_fsr.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:nolumi_span_063815_0000000001.dst' TYP='POOL_ROOTTREE' OPT='READ'",
  ]

##############################################################################
