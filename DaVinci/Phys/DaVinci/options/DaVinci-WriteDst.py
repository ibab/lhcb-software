########################################################################
#
# $Id: DaVinci-WriteDst.py,v 1.14 2009-06-02 14:28:23 pkoppenb Exp $
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
from Configurables import DaVinci, DaVinciWriteDst
##############################################################################
from Configurables import GaudiSequencer, FilterDesktop, DeterministicPrescaler, PrintDecayTree
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#
# Get a J/psi
#
seq = GaudiSequencer("Seq_Jpsi")
jpsi = FilterDesktop('MyJpsi')
jpsi.InputLocations = [ 'StdLooseJpsi2MuMu' ]
jpsi.Code = "ALL"
det = DeterministicPrescaler("Prescale_Jpsi")
det.AcceptFraction = 0.5                    # take 50%
printDec = PrintDecayTree("Print_Jpsi")
printDec.InputLocations = [ "MyJpsi" ]
seq.Members += [ det, jpsi, printDec ]
#
# This the bit that declares the sequence to the Dst writer
# seq will bee passed to DaVinci(). You do not need to pass it yourself.
# The second line allows to save particles (not yet 100% functional)
#
DaVinciWriteDst().DstFiles[ "Jpsi.dst" ] = seq
DaVinciWriteDst().Items += [ "/Event/Phys/MyJpsi#2" ] # will not save StdLooseMuos
##############################################################################
DaVinci().EvtMax = 100
DaVinci().PrintFreq = 1 
# DaVinci().SkipEvents = 0
DaVinci().DataType = "2008" # Default is "DC06"
# DaVinci().Simulation   = False
##############################################################################
