########################################################################
#
#   Module for construction of B->hhh (h=K,pi,p) StrippingLines.
#
#
#   This Lines follow the "Topological" idea in which we may apply
#   PT and IP Cuts in three bodies and can recover from the 2 Body line.
#
#   __author__ = 'Rafael Coutinho'
#   __date__ = '22/10/2010'
#   __version__ = '$ 1.0 $'
#
########################################################################
                                                                                                                                                                                  
from os import environ
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine
from Configurables import OfflineVertexFitter
from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
modules =  CoreFactory('CoreFactory').Modules
for i in [ 'LoKiTrigger.decorators' ] :
  if i not in modules : modules.append(i)

StrippingKillTooManyTopoIP = VoidFilter('StrippingKillTooManyTopoIP' 
                                        , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 300 )")

#  Two-body Line
Bu3hFrom2h_2Body = CombineParticles("Bu3hFrom2h_2Body")
Bu3hFrom2h_2Body.InputLocations = ["StdLoosePions"]
Bu3hFrom2h_2Body.DecayDescriptors = ["rho(770)0 -> pi+ pi+","rho(770)0 -> pi+ pi-","rho(770)0 -> pi- pi-"]
Bu3hFrom2h_2Body.DaughtersCuts = {"pi+" : "(PT > 500.*MeV) & (P>5*GeV) & (MIPCHI2DV(PRIMARY)>16) & (TRCHI2DOF<5)","pi-" : "(PT > 500.*MeV) & (P>5*GeV) & (MIPCHI2DV(PRIMARY)>16) & (TRCHI2DOF<5)"}
Bu3hFrom2h_2Body.CombinationCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<0.15)"
Bu3hFrom2h_2Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('pi-'==ABSID))  ,PT)>1.5*GeV) & (BPVVDCHI2>64) & (BPVDIRA>0.)"

# Three-body Line
Bu3hFrom2h_3Body= CombineParticles("Bu3hFrom2h_3Body")
Bu3hFrom2h_3Body.InputLocations  = ["StdLoosePions", "Bu3hFrom2h_2Body"]
Bu3hFrom2h_3Body.DecayDescriptors = ["B+ -> rho(770)0 pi+","B+ -> rho(770)0 pi-"]
Bu3hFrom2h_3Body.DaughtersCuts = {"pi+" : "(PT > 500.*MeV) & (MIPCHI2DV(PRIMARY)>16)","pi-" : "(PT > 500.*MeV) & (MIPCHI2DV(PRIMARY)>16)",}
Bu3hFrom2h_3Body.CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<0.3)"
Bu3hFrom2h_3Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('pi-'==ABSID)),PT)>1.5*GeV) & (BPVDIRA>0.99998) & (VFASPF(VMINVDDV(PRIMARY))>3) & (VFASPF(VCHI2)<12) & (MIPDV(PRIMARY)<0.04) & (PT>1000*MeV) & (abs(CHILD(1,SUMQ) + CHILD(2,Q))==1)"
Bu3hFrom2h_3Body.addTool( OfflineVertexFitter() )
Bu3hFrom2h_3Body.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bu3hFrom2h_3Body.OfflineVertexFitter.useResonanceVertex = False

# Filters to apply Mass and MCor cuts
# Two-body Line
preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
             "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]

filterTopoStripping2Body = FilterDesktop('filterTopoStripping2Body'
                                         , InputLocations = ["Bu3hFrom2h_2Body"]
                                         , Code = "(M >4000*MeV) & (MCOR < 7000*MeV)"
                                         , Preambulo=preambulo
                                         )
               
#The 'robust' 3-body
filterTopoStripping3Body = FilterDesktop('filterTopoStripping3Body'
                                         , InputLocations = ["Bu3hFrom2h_3Body"]
                                         , Code = "(M >4000*MeV) & (MCOR < 7000*MeV)"
                                         , Preambulo=preambulo
                                         )


# Sequence for Two-body Line
Bu3hFrom2h_2BodySeq = GaudiSequencer("Bu3hFrom2h_2BodySeq")
Bu3hFrom2h_2BodySeq.Members += [Bu3hFrom2h_2Body]
Bu3hFrom2h_2BodySeq.Members += [filterTopoStripping2Body]
# Sequence for Three-body Line
Bu3hFrom2h_3BodySeq = GaudiSequencer("Bu3hFrom2h_3BodySeq")
Bu3hFrom2h_3BodySeq.Members += [Bu3hFrom2h_2Body]
Bu3hFrom2h_3BodySeq.Members += [Bu3hFrom2h_3Body]
Bu3hFrom2h_3BodySeq.Members += [filterTopoStripping3Body]


# The 2-body Line
line1 = StrippingLine('Bu3hFrom2h_2BodyLine'
               , prescale = 0.05
               , algos = [StrippingKillTooManyTopoIP, Bu3hFrom2h_2BodySeq]
               )
# The 3-body Line 
line2 = StrippingLine('Bu3hFrom2h_3BodyLine'
               , prescale = 1
               , algos = [StrippingKillTooManyTopoIP, Bu3hFrom2h_3BodySeq]
               )
