### @file
#	
#  Hlt2 exclusive selections shared particles for 2 body combinations	
#
#  @author S.Furcas
#  @date 2008-07-17
#
##
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DeterministicPrescaler
from Configurables import FilterDesktop, PhysDesktop, Fidel
from Configurables import LoKi__Hybrid__FilterCriterion as LoKiFilterCriterion
###########################################################################
# Define the sequencer. It must be a member of Hlt2Selections.
# There must be a Prescale algorithm defined as well. By default
# it kill do anything.
#
Hlt2SelB2TwoBodyFilterSequence = GaudiSequencer("Hlt2SelB2TwoBodyFilterSequence")
GaudiSequencer("Hlt2Selections").Members += [ Hlt2SelB2TwoBodyFilterSequence ]
Hlt2SelB2TwoBodyFilterSequence.Members += [ DeterministicPrescaler("PrescaleHlt2SelB2TwoBody") ]
###########################################################################
# Add all needed intermediate states. They are defined in Hlt2Shared.py,
# but not necessarily run. So don't forget to add them.
#
SeqIntermediatesForHlt2SelB2TwoBody = GaudiSequencer("SeqIntermediatesForHlt2SelB2TwoBody")
SeqIntermediatesForHlt2SelB2TwoBody.Members += [ GaudiSequencer("SeqHlt2SharedDiMuon"),
                                                 GaudiSequencer("SeqHlt2SharedPhi2KK"),
                                                 GaudiSequencer("SeqHlt2SharedDs2KKPi"),
                                                 GaudiSequencer("SeqHlt2SharedD02KPi"),
                                                 GaudiSequencer("SeqHlt2SharedKstar2KPi"),
                                                 GaudiSequencer("SeqHlt2SharedKsLL"),
                                                 GaudiSequencer("SeqHlt2SharedDstarWithD02KPi"),
                                                 GaudiSequencer("SeqHlt2SharedDplus2KPiPi"),
                                                 GaudiSequencer("SeqHlt2SharedKstarPlus"),
                                                 GaudiSequencer("SeqHlt2SharedLambda0") ]
SeqIntermediatesForHlt2SelB2TwoBody.IgnoreFilterPassed = TRUE
Hlt2SelB2TwoBodyFilterSequence.Members += [ SeqIntermediatesForHlt2SelB2TwoBody ]
###########################################################################
# Tighten cuts
#
Hlt2TightForB2TwoBody = FilterDesktop("Hlt2TightForB2TwoBody")
Hlt2SelB2TwoBodyFilterSequence.Members += [Hlt2TightForB2TwoBody]
Hlt2TightForB2TwoBody.addTool(PhysDesktop())
Hlt2TightForB2TwoBody.PhysDesktop.InputLocations  =  [ "Phys/Hlt2Muons",
                                                       "Phys/Hlt2Pions",
                                                       "Phys/Hlt2Kaons",
                                                       "Phys/Hlt2SharedDiMuon",
                                                       "Phys/Hlt2SharedPhi2KK",
                                                       "Phys/Hlt2SharedDs2KKPi",
                                                       "Phys/Hlt2SharedD02KPi",
                                                       "Phys/Hlt2SharedKstar2KPi",
                                                       "Phys/Hlt2SharedKsLL",
                                                       "Phys/Hlt2SharedDstarWithD02KPi",
                                                       "Phys/Hlt2SharedDplus2KPiPi",
                                                       "Phys/Hlt2SharedKstarPlus",
                                                       "Phys/Hlt2SharedLambda0"]
Hlt2TightForB2TwoBody.addTool(LoKiFilterCriterion("Filter"))

##Hlt2TightForB2TwoBody.Filter.Code = "((ABSID =='mu+') & (PT>2*GeV) & (MIPCHI2DV(PRIMARY)>9)) | 
## 	((ABSID =='K+') & (PT>2*GeV) & (MIPCHI2DV(PRIMARY)>9)) | 
## 	((ABSID =='pi+') & (PT>2*GeV) & (MIPCHI2DV(PRIMARY)>9)) | 
## 	((ABSID =='J/psi(1S)') & (ADMASS('J/psi(1S)')<100*MeV)& (PT>1*GeV) & (BPVVDCHI2>0)) |
## 	((ABSID =='phi(1020)') & (ADMASS('phi(1020)')<30*MeV)& (PT>1*GeV)&(BPVVDCHI2>0)) | 
## 	((ABSID =='K*(892)0') & (ADMASS('K*(892)0')<100*MeV)& (PT>2.5*GeV) &(BPVVDCHI2>0) ) |
## 	((ABSID =='D_s+') & (ADMASS('D_s+')<30*MeV)& (PT>2.5*GeV) & (MIPCHI2DV(PRIMARY)>100) & (BPVVDCHI2>9)&(BPVVDCHI2<100)) |
## 	((ABSID =='D0') & (ADMASS('D0')<35*MeV) & (PT>2.5*GeV) & (BPVVDCHI2>9)) |
## 	((ABSID =='KS0') & (ADMASS('KS0')<50*MeV) & (PT>1*GeV) &(BPVVDCHI2>9)) | 
## 	((ABSID =='D*(2010)+') & (PT>2.5*GeV) &(BPVVDCHI2>0)) | 
## 	((ABSID =='D+') & (ADMASS('D+')<30*MeV) & (PT>2.5*GeV) & (MIPCHI2DV(PRIMARY)>100)&(BPVVDCHI2>9))"

Hlt2TightForB2TwoBody.Filter.Code ="((ABSID =='mu+') & (PT>2*GeV) & (MIPCHI2DV(PRIMARY)>9)) |        
                             ((ABSID =='K+') & (PT>2*GeV) & (MIPCHI2DV(PRIMARY)>9)) | 
                             ((ABSID =='pi+') & (PT>2*GeV) & (MIPCHI2DV(PRIMARY)>9)) | 
                             ((ABSID =='J/psi(1S)') & (ADMASS('J/psi(1S)')<100*MeV)& (PT>1*GeV) ) |                
                             ((ABSID =='phi(1020)') & (ADMASS('phi(1020)')<30*MeV)& (PT>1*GeV)) |              
                             ((ABSID =='K*(892)0') & (ADMASS('K*(892)0')<100*MeV)& (PT>2.5*GeV)) |                 
                             ((ABSID =='D_s+') & (ADMASS('D_s+')<30*MeV)& (PT>2.5*GeV) & (MIPCHI2DV(PRIMARY)>20) )|
                             ((ABSID =='D0') & (ADMASS('D0')<35*MeV) & (PT>2.5*GeV) ) |                
                             ((ABSID =='KS0') & (ADMASS('KS0')<50*MeV) & (PT>1.0*GeV)) |               
                             ((ABSID =='D*(2010)+') & (PT>2.5*GeV)) |                
                             ((ABSID =='D+') & (ADMASS('D+')<30*MeV) & (PT>2.5*GeV) & (MIPCHI2DV(PRIMARY)>20))"

###########################################################################
# Run FIDEL
#
Hlt2SelB2TwoBody = Fidel("Hlt2SelB2TwoBody")
Hlt2SelB2TwoBodyFilterSequence.Members += [ Hlt2SelB2TwoBody ] 
Hlt2SelB2TwoBody.addTool(PhysDesktop())
Hlt2SelB2TwoBody.PhysDesktop.InputLocations  = [ "Phys/Hlt2TightForB2TwoBody" ] 
Hlt2SelB2TwoBody.pid = 511
Hlt2SelB2TwoBody.maxChi2 = 40
Hlt2SelB2TwoBody.minMass = 5000.0
Hlt2SelB2TwoBody.maxMass = 7000.0
Hlt2SelB2TwoBody.maxPointing = 0.01
Hlt2SelB2TwoBody.maxIpchi2 = 25
Hlt2SelB2TwoBody.minProb = 0.1
Hlt2SelB2TwoBody.minPt = 5000.
Hlt2SelB2TwoBody.minCts = 6.5
Hlt2SelB2TwoBody.minFsB1Res = -1.
Hlt2SelB2TwoBody.minFsB2Res = -1.
Hlt2SelB2TwoBody.minFsB1ch = 0.
Hlt2SelB2TwoBody.minFsB2ch = 0.
Hlt2SelB2TwoBody.minFsB1KS0 = 10.
Hlt2SelB2TwoBody.minFsB2KS0 = 10.
Hlt2SelB2TwoBody.minChFs = 9.
Hlt2SelB2TwoBody.minFs = 0.
Hlt2SelB2TwoBody.minQ = -1
Hlt2SelB2TwoBody.maxQ = 1
Hlt2SelB2TwoBody.minB    = 0
Hlt2SelB2TwoBody.maxB    = 0
Hlt2SelB2TwoBody.minL    = 0
Hlt2SelB2TwoBody.maxL    = 0
Hlt2SelB2TwoBody.checkQ = TRUE
Hlt2SelB2TwoBody.checkB = TRUE 
Hlt2SelB2TwoBody.checkL = TRUE
Hlt2SelB2TwoBody.cc     = TRUE
Hlt2SelB2TwoBody.basicparticle = FALSE
Hlt2SelB2TwoBody.OutputLevel = 3
