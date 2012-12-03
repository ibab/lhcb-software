########################################################################
#
# $Id: DVTestWriteDst.py,v 1.11 2010/05/25 11:04:28 jpalac Exp $
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
from Configurables import DaVinci
##############################################################################
from Configurables import GaudiSequencer, FilterDesktop, DeterministicPrescaler, PrintDecayTree

#
# get DoD muons
#
from PhysSelPython.Wrappers import Selection, DataOnDemand, SelectionSequence
#from Configurables import SelDSTWriter
from DSTWriters.Configuration import SelDSTWriter
from DSTWriters.microdstelements import CloneParticleMCInfo

MyLooseJpsi = DataOnDemand(Location = 'Phys/StdLooseJpsi2MuMu/Particles')

#
# Loop over three overlapping mass zones
#
MassRanges = [ [ 3000, 3087 ], [ 3088, 3100 ],  [ 3100, 4000 ] ]

selections = []

for i in MassRanges :
    lm = i[0]
    hm = i[1]
    ln = str(lm)
    hn = str(hm)
    name = ln+"_"+hn

    # define the prescale to have a constant (assumed flat) level of background
    prescale = DeterministicPrescaler("Prescale_"+name,
                                      AcceptFraction = 100./(hm-lm))
    filter   = FilterDesktop("Jpsi_"+name,
                             Code = "(MM>"+ln+") & (MM<"+hn+") & (MIPCHI2DV(PRIMARY) > 2)")

    SelJpsi = Selection('SelJpsi_'+name,
                        Algorithm = filter,
                        RequiredSelections = [ MyLooseJpsi ])

    printer  = PrintDecayTree("Print_"+name,
                              Inputs = [SelJpsi.outputLocation()])    

    SeqJpsi = SelectionSequence('SeqJpsi_'+name
                                , TopSelection = SelJpsi
                                , EventPreSelector = [ prescale ]
                                , PostSelectionAlgs = [ printer ])

    selections += [SeqJpsi]
    
dstWriter = SelDSTWriter("JpsiDSTWriter_"+name,
                         SelectionSequences = selections)

dstWriter.MicroDSTElements['default'] += [CloneParticleMCInfo()]

seq = dstWriter.sequence()

DaVinci().UserAlgorithms = [ seq ]

##############################################################################
DaVinci().EvtMax = 100
DaVinci().PrintFreq = 100 
# DaVinci().SkipEvents = 0
DaVinci().DataType = "2010" 
DaVinci().Simulation   = True
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
##############################################################################
# Bs->J/psi Phi MC09
#EventSelector().Input   = [
#    "   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ] 

#-- GAUDI jobOptions generated on Mon Jan 17 17:13:58 2011
#-- Contains event types : 
#--   13144006 - 1 files - 10000 events - 4.13 GBytes
# MC 2010 Beam3500GeV-VeloClosed-MagDown-Nu3
# bs->Jpsi, Phi
from Gaudi.Configuration import * 

EventSelector().Input   = [
    "   DATAFILE='castor:/castor/cern.ch/user/r/rlambert/testfiles/MC2010_BsJPsiPhi_00008405_00000506_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
