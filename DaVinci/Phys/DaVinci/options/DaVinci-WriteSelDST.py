########################################################################
#
# $Id: DaVinci-WriteSelDST.py,v 1.3 2009-08-18 10:24:37 jpalac Exp $
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
##############################################################################
from Gaudi.Configuration import *
from Configurables import DaVinci, SelDSTWriter
##############################################################################
from Configurables import GaudiSequencer, FilterDesktop, DeterministicPrescaler, PrintDecayTree
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#
# Get a J/psi
#
jpsi = FilterDesktop('MyJpsi')
jpsi.Code = "ALL"

from PhysSelPython.Wrappers import Selection, DataOnDemand, SelectionSequence

MyLooseJpsi = DataOnDemand('DODStdLooseJpsi2MuMu',
                           Location = 'StdLooseJpsi2MuMu')

SelJpsi = Selection('SelJpsi',
                    Algorithm = jpsi,
                    RequiredSelections = [MyLooseJpsi])

SeqJpsi = SelectionSequence('SeqJpsi', TopSelection = SelJpsi)

det = DeterministicPrescaler("Prescale_Jpsi")
det.AcceptFraction = 0.5                    # take 50%
printDec = PrintDecayTree("Print_Jpsi")
printDec.InputLocations = [ "MyJpsi" ]
#seq.Members += [ det, jpsi, printDec ]
#
# This the bit that declares the sequence to the Dst writer
# seq will bee passed to DaVinci(). You do not need to pass it yourself.
# The second line allows to save particles (not yet 100% functional)
#
dstWriter = SelDSTWriter("JpsiDST",
                         SelectionSequences = [SeqJpsi],
                         SaveCandidates = True,
                         CopyProtoParticles = False )
##############################################################################
DaVinci().EvtMax = 100
DaVinci().PrintFreq = 1
DaVinci().UserAlgorithms = [ dstWriter.sequence(), printDec  ]
# DaVinci().SkipEvents = 0
DaVinci().DataType = "MC09" # Default is "MC09"
# DaVinci().Simulation   = False
##############################################################################
