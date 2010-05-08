############################################################################
#
# Stripping selection for prompt D->Kh events, reconstructed as D0->K+K-
#
############################################################################

__author__ = ['Harry Cliff']
__date__ = '20/04/2010'
__version__ = '$Revision: 1.2 $'

'''
D0->Kh stripping selection.
'''


from Gaudi.Configuration import *
from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, StrippingMember

#----------------------------------------
# 1) Make a preselection
#----------------------------------------
DKhPreselLoose = CombineParticles("DKhPreselLoose")

DKhPreselLoose.DecayDescriptor = "D0 -> K+ K-"
DKhPreselLoose.DaughtersCuts = { "K+" : "ISLONG & (PT>500) & (TRCHI2DOF<9)" } 
DKhPreselLoose.CombinationCut  = "(AM>1815) & (AM<2065) & (AP>5000)"
DKhPreselLoose.MotherCut       = "(BPVVDCHI2>10) & (BPVIPCHI2()<30) & (VFASPF(VCHI2/VDOF)<10) & (BPVDIRA>0.9999) & (MAXTREE('K+'==ABSID,PIDK)>0.0)"

DKhPreselLooseOutput = "Phys/"
DKhPreselLooseOutput += "DKhPreselLoose"

#------------------------------------------
# 2) Wrap StdNoPIDsPions and StdNoPIDsKaons
#------------------------------------------

SelStdNoPIDsKaons = DataOnDemand('SelStdNoPIDsKaons', Location = 'Phys/StdNoPIDsKaons')

#----------------------------------------
# 3) Make a selection sequence
#----------------------------------------

SelDKhPreselLoose = Selection("SelDKhPreselLoose",  Algorithm = DKhPreselLoose, RequiredSelections = [ SelStdNoPIDsKaons])

SelSeqDKhPreselLoose = SelectionSequence("SeqDKhPreselLoose", TopSelection = SelDKhPreselLoose)


#--------------------------------------------
# 4) Create StrippingLine with this selection 
#--------------------------------------------
DKhPreselLooseLine = StrippingLine("DKhPreselLooseLine"
                          , prescale = 1.
                          , algos = [ SelSeqDKhPreselLoose ]
                          )
