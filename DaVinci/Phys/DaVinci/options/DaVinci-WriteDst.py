a########################################################################
#
# $Id: DaVinci-WriteDst.py,v 1.11 2009-05-22 15:24:20 pkoppenb Exp $
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
#
# Get a J/psi
#
seq = GaudiSequencer("Seq_Jpsi")
j = FilterDesktop('MyJpsi')
j.InputLocations = [ 'StdLooseJpsi2MuMu' ]
j.Code = "ALL"
d = DeterministicPrescaler("Prescale_Jpsi")
d.AcceptFraction = 0.5                    # take 50%
p = PrintDecayTree("Print_Jpsi")
p.InputLocations = [ "MyJpsi" ]
seq.Members += [ d, j, p ]
#
# This the bit that declares the sequence to the Dst writer
# seq will passed to DaVinci()
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
