#
# A stripping line for the topological trigger
#
# The second part of this file, now commented out, implemented the topological
# trigger for the stripping by loading the topological components from the HLT
# and redefining their input locations. This strategy has been abandoned because it
# interfered with the trigger when the full sequence L0-HLT-Stripping was being
# executed. 
#
# Instead, the topological trigger is now implemented as a stripping selection by hand,
# with the cuts corresponding to the 08/09 "hadronic scenario" optimization by P. Spradlin. 
#
# Note the both the 'robust' and chi2 cuts are applied on the same offline quality tracks
# in this case.  
#
# In the long run, this will be changed again once it is possible to reuse elements of
# HLT lines as stripping lines without interference.    
#
# Author: V. Gligorov 
from os import environ
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine

from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
modules =  CoreFactory('CoreFactory').Modules
for i in [ 'LoKiTrigger.decorators' ] :
  if i not in modules : modules.append(i)

StrippingKillTooManyTopoIP = VoidFilter('StrippingKillTooManyTopoIP' 
                                        , Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < 240 )")

#The 'robust' 2-body
TopoStripping2Body = CombineParticles("TopoStripping2Body")
TopoStripping2Body.InputLocations = ["StdNoPIDsKaons","StdNoPIDsPions" ]
TopoStripping2Body.DecayDescriptors = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-","K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"]
TopoStripping2Body.DaughtersCuts = {"pi+" : "(PT > 400.*MeV) & (P>2*GeV) & (MIPDV(PRIMARY)>0.025*mm)", "K+" : "(PT > 400.*MeV) & (P>2*GeV) & (MIPDV(PRIMARY)>0.025*mm)"}
TopoStripping2Body.CombinationCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<0.15) & AALLSAMEBPV"
TopoStripping2Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVD>2.0*mm) & (BPVVDR > 0.2*mm)"

#The 'robust' 3-body
TopoStripping3Body= CombineParticles("TopoStripping3Body")
TopoStripping3Body.InputLocations  = ["StdNoPIDsPions", "TopoStripping2Body"]
TopoStripping3Body.DecayDescriptors = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
TopoStripping3Body.DaughtersCuts = {"D*(2010)+" : "ALL", "pi+" : "(PT > 400.*MeV) & (P>2*GeV) & (MIPDV(PRIMARY)>0.025*mm)"}
TopoStripping3Body.CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
TopoStripping3Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVD>2.0*mm) & (BPVVDR > 0.2*mm)"

#The chi2-cut based 2-body
TopoStrippingTF2Body = CombineParticles("TopoStrippingTF2Body")
TopoStrippingTF2Body.InputLocations = [ "StdNoPIDsKaons","StdNoPIDsPions"  ]
TopoStrippingTF2Body.DecayDescriptors = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-","K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"]
TopoStrippingTF2Body.DaughtersCuts = {"pi+" : "(TRCHI2DOF<10) & (PT>400*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)", "K+" : "(TRCHI2DOF<10) & (PT>400*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)"}
TopoStrippingTF2Body.CombinationCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<0.1) & AALLSAMEBPV"
TopoStrippingTF2Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVDCHI2>169)"

#The chi2-cut based 3-body
TopoStrippingTF3Body= CombineParticles("TopoStrippingTF3Body")
TopoStrippingTF3Body.InputLocations  = [ "StdNoPIDsPions", "TopoStrippingTF2Body" ]
TopoStrippingTF3Body.DecayDescriptors = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
TopoStrippingTF3Body.DaughtersCuts = {"D*(2010)+" : "ALL", "pi+" : "(TRCHI2DOF<10) & (PT>400*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)"}
TopoStrippingTF3Body.CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
TopoStrippingTF3Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVDCHI2>169)"

#Now the filters that actually apply the mass/pointing cuts
#The 'robust' 2-body
filterTopoStripping2Body = FilterDesktop('filterTopoStripping2Body'
                               , InputLocations = ["TopoStripping2Body"]
                               , Code = "(M > 4*GeV) & (BPVTRGPOINTINGWPT<0.24)"
                               )
#The 'robust' 3-body
filterTopoStripping3Body = FilterDesktop('filterTopoStripping3Body'
                               , InputLocations = ["TopoStripping3Body"]
                               , Code = "(M > 4*GeV) & (BPVTRGPOINTINGWPT<0.24)"
                               )
#The chi2-cut based 2-body
filterTopoStrippingTF2Body = FilterDesktop('filterTopoStrippingTF2Body'
                               , InputLocations = ["TopoStrippingTF2Body"]
                               , Code = "(M > 4*GeV) & (BPVTRGPOINTINGWPT<0.16)"
                               )
#The chi2-cut based 3-body
filterTopoStrippingTF3Body = FilterDesktop('filterTopoStrippingTF3Body'
                               , InputLocations = ["TopoStrippingTF3Body"]
                               , Code = "(M > 4*GeV) & (BPVTRGPOINTINGWPT<0.16)"
                               )

#Now the sequencers...
#the 'robust' 2-body
TopoStripping2BodySeq = GaudiSequencer("TopoStripping2BodySeq")
TopoStripping2BodySeq.Members += [TopoStripping2Body]
TopoStripping2BodySeq.Members += [filterTopoStripping2Body]
#the 'robust' 3-body
TopoStripping3BodySeq = GaudiSequencer("TopoStripping3BodySeq")
TopoStripping3BodySeq.Members += [TopoStripping2Body]
TopoStripping3BodySeq.Members += [TopoStripping3Body]
TopoStripping3BodySeq.Members += [filterTopoStripping3Body]
#the chi2-cut based 2-body sequence
TopoStrippingTF2BodySeq = GaudiSequencer("TopoStrippingTF2BodySeq")
TopoStrippingTF2BodySeq.Members += [TopoStrippingTF2Body]
TopoStrippingTF2BodySeq.Members += [filterTopoStrippingTF2Body]
#the chi2-cut based 3-body sequence
TopoStrippingTF3BodySeq = GaudiSequencer("TopoStrippingTF3BodySeq")
TopoStrippingTF3BodySeq.Members += [TopoStrippingTF2Body]
TopoStrippingTF3BodySeq.Members += [TopoStrippingTF3Body]
TopoStrippingTF3BodySeq.Members += [filterTopoStrippingTF3Body]
#the 2-body decision
TopoStripping2BodySequence = GaudiSequencer("TopoStripping2BodySequence")
TopoStripping2BodySequence.Members += [TopoStripping2BodySeq]
TopoStripping2BodySequence.Members += [TopoStrippingTF2BodySeq]
#the 3-body decision
TopoStripping3BodySequence = GaudiSequencer("TopoStripping3BodySequence")
TopoStripping3BodySequence.Members += [TopoStripping3BodySeq]
TopoStripping3BodySequence.Members += [TopoStrippingTF3BodySeq]
#the 2-body line itself
line1 = StrippingLine('Topological2BodyLine'
#               , prescale = 1.
               , prescale = 0.1
               , algos = [StrippingKillTooManyTopoIP,TopoStripping2BodySequence]
               )
#the 3-body line itself
line2 = StrippingLine('Topological3BodyLine'
#               , prescale = 1.
               , prescale = 0.1
               , algos = [StrippingKillTooManyTopoIP,TopoStripping3BodySequence]
               )
