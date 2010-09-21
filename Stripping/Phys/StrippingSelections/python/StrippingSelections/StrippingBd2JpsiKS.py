# $Id: StrippingBd2JpsiKS.py,v 1.5 2010-09-05 22:01:54 gcowan Exp $

__author__ = 'Fernando Rodrigues'
__date__ = '26/08/2010'
__version__ = '$Revision: 1.5 $'

'''
Bd->JpsiKS stripping selection re-tuned to eh MC09 data,
using LoKi::Hybrid and python configurables. PV refitting is done. 
Note1: This stripping is use to the biased and unbiased offline selection.
Note2: Two lines: "line1" related to KSLL and "line2" related to KSDD.
'''

__all__ = ('name', 'Bd_KSLL', 'Bd_KSDD', 'seqLL', 'seqDD')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from Configurables import LoKi__Hybrid__PlotTool as PlotTool
#import GaudiKernel.SystemOfUnits as Units

name = "Bd2JpsiKS"

##################
# J/Psi -> mu mu #
##################
SelJPsi2MuMu = DataOnDemand(Location = "Phys/StdMassConstrainedJpsi2MuMu")

################
# KS long long #
################
_stdKSLL = DataOnDemand(Location = "Phys/StdLooseKsLL")


############################
# KS downstream downstream #
############################
_stdKSDD = DataOnDemand(Location = "Phys/StdLooseKsDD")


##############################
# Bd -> JpsiKS,KSLL (Biased) #
##############################
_Bd_KSLL = CombineParticles("BLL_" + name,
                            DecayDescriptor = "[B0 -> J/psi(1S) KS0]cc",
                            CombinationCut = "ADAMASS('B0') < 500.*MeV",
                            MotherCut = "ALL",
                            ReFitPVs = True)
_Bd_KSLL.addTool( OfflineVertexFitter() )
_Bd_KSLL.VertexFitters.update( { "" : "OfflineVertexFitter"} )
_Bd_KSLL.OfflineVertexFitter.useResonanceVertex = False
Bd_KSLL = Selection ("SelBLL"+name,
                     Algorithm = _Bd_KSLL,
                     RequiredSelections = [_stdKSLL, SelJPsi2MuMu])

##############################
# Bd -> JpsiKS,KSDD (Biased) #
##############################
_Bd_KSDD = CombineParticles("BDD_"+ name,
                            DecayDescriptor = "[B0 -> J/psi(1S) KS0]cc",
                            CombinationCut = "ADAMASS('B0') < 500.*MeV",
                            MotherCut = "ALL",
                            ReFitPVs = True)
_Bd_KSDD.addTool( OfflineVertexFitter() )
_Bd_KSDD.VertexFitters.update( { "" : "OfflineVertexFitter"} )
_Bd_KSDD.OfflineVertexFitter.useResonanceVertex = False
Bd_KSDD = Selection ("SelBDD"+name,
                     Algorithm = _Bd_KSDD,
                     RequiredSelections = [_stdKSDD, SelJPsi2MuMu])

##############################
# build the Sel Sequence     #
##############################
seqLL = SelectionSequence("SeqLL"+name, 
                          TopSelection = Bd_KSLL)
seqDD = SelectionSequence("SeqDD"+name, 
                          TopSelection = Bd_KSDD)


from StrippingConf.StrippingLine import StrippingLine, StrippingMember

############################################
# Create StrippingLine with this selection #
############################################
line1 = StrippingLine(name+'_KSLL_Line'
               , prescale = 1
               , algos = [seqLL]
               , postscale = 1)

line2 = StrippingLine(name+'_KSDD_Line'
               , prescale = 1
               , algos = [seqDD]
               , postscale = 1)
