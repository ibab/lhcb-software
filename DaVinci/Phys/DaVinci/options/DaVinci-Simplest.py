########################################################################
#
#  the simplest options file which reads some data and creates an ntuple.
#
#  Used for DaVinciTutorial 0 and 0.5
#
########################################################################


from Gaudi.Configuration import *
from Configurables import DaVinci

#take a Reco-14 Stripping-20 file for processing. Example DiMuon stream
DaVinci().Input = [ "PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/DIMUON.DST/00020241/0000/00020241_00000272_1.dimuon.dst"]


#configure differently depending on which data you use
DaVinci().DataType="2012"
DaVinci().DDDBtag="dddb-20120831"
DaVinci().CondDBtag="cond-20120831"


#save an ntuple with the luminosity
DaVinci().Lumi=True
DaVinci().TupleFile="DVnTuples.root"

########################################################################
#
# If this doesn't work, then maybe the data don't exist any more
# or maybe there is something wrong with your environment/gangarc
#
########################################################################
