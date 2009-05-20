########################################################################
#
# $Id: DaVinci-WriteDst.py,v 1.10 2009-05-20 14:18:08 pkoppenb Exp $
#
# Options for a DaVinci job creating DSTs
#
# It looks for a J/psi and then saves the events in DSTs
# according to the number of PVs.
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
jpsi = FilterDesktop('MyJpsi')
jpsi.InputLocations = [ 'StdLooseJpsi2MuMu' ]
jpsi.Code = "ALL"
# @todo This does not work properly
# jpsi.CloneTree = True # Write the muons as well
#
# Loop over three overlapping mass zones
#
MassRanges = [ [ 2600, 3200 ], [ 3050, 3150 ],  [ 3000, 3500 ] ]

for i in MassRanges :
    lm = i[0]
    hm = i[1]
    ln = str(lm)
    hn = str(hm)
    name = ln+"_"+hn
    seq = GaudiSequencer("Seq_"+name)
    d = DeterministicPrescaler("Prescale_"+name)
    d.AcceptFraction = 100./(hm-lm)  # same background level in each
    j = jpsi.clone("Jpsi_"+name)
    j.Code = "(MM>"+ln+") & (MM<"+hn+")"
    p = PrintDecayTree("Print_"+name)
    p.InputLocations = [ "Jpsi_"+name ]
    seq.Members += [ d, j, p ]
    #
    # This the bit that declares the sequence to the Dst writer
    # The second line allows to sdae particles (not yet 100% functional)
    #
    DaVinciWriteDst().DstFiles[ "Jpsi_"+name+".dst" ] = seq
    DaVinciWriteDst().Items += [ "/Event/Phys/Jpsi_"+name+"#2" ] # will not save StdLooseMuos
##############################################################################
# get write out sequence. Pass it to DaVinci()
#
seq =  DaVinciWriteDst().dstSequence()
DaVinci().UserAlgorithms = [ jpsi, seq ]
#
##############################################################################
DaVinci().EvtMax = 1000
DaVinci().PrintFreq = 1 
# DaVinci().SkipEvents = 0
DaVinci().DataType = "2008" # Default is "DC06"
# DaVinci().Simulation   = False
##############################################################################
