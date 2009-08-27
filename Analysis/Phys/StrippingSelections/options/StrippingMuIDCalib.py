## #####################################################################
# A stripping selection for muon identification calibration trough
# the study of J/psi(1S) -> mu+ mu- and Lambda to p pi decays
#
# @authors S.Furcas, G.Lanfranchi, M.Palutan, A.Sarti 
# @date 2009-July-30
# 
## #####################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember


# Create Jpsi -> mumu candidates out of no pid muons
## ############################################################
muplcut = '(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & (P>6*GeV) & (PT>1500*MeV) & (TRCHI2DOF<3) & (ISLONG)'
mumicut = '(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & (P>3*GeV) & (PT>800*MeV) & (TRCHI2DOF<3) & (ISLONG) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1000*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV)'
childcuts = " (CHILDCUT(ISMUON,1))"
JpsiCombine = StrippingMember( CombineParticles
                               , 'Combine'
                               , InputLocations = [ 'StdNoPIDsMuons' ]
                               , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                               , DaughtersCuts = { 'mu+' : muplcut , 
                                                   'mu-' : mumicut }
                               , CombinationCut = "(ADAMASS('J/psi(1S)')<300*MeV)"
                               , MotherCut = "(VFASPF(VCHI2/VDOF)<6) & " + childcuts
                               )

# Define the line
## ############################################################
jp_line = StrippingLine('JpsiNoPID'
                        , prescale = 1.
                        , algos = [ JpsiCombine ]
                        , stream = 'Jpsi'
                        )


# Create Lambda_dd -> ppi candidates
## ############################################################
LambdaDDCombine = StrippingMember( CombineParticles
                                   , 'Combine'
                                   , InputLocations = [ "StdNoPIDsDownPions","StdNoPIDsDownProtons" ]
                                   , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                   , DaughtersCuts = { "p+"  :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISDOWN) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))",
                                                       "pi-" :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISDOWN) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))" }
                                   , CombinationCut = "(ADAMASS('Lambda0')<30*MeV)"
                                   , MotherCut = "(VFASPF(VCHI2)<6) & (500<BPVVDZ) & (2200>BPVVDZ) & (BPVDIRA>0.999999) "
                                   )

# Define the line
# ############################################################
lm_line = StrippingLine('LambdaDDNoPID'
                        , prescale = 1.
                        , algos = [ LambdaDDCombine ]
                        , stream = 'Lambda_dd'
                    )


# Create Lambda_LL -> ppi candidates
## ############################################################
LambdaLLCombine = StrippingMember( CombineParticles
                                   , 'Combine'
                                   , InputLocations = [ "StdNoPIDsPions","StdNoPIDsProtons" ]
                                   , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                   , DaughtersCuts = { "p+"  :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISLONG) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))",
                                                       "pi-" :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<2) & (ISLONG) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))" }
                                   , CombinationCut = "(ADAMASS('Lambda0')<30*MeV)"
                                   , MotherCut = "(VFASPF(VCHI2)<6) & (50<BPVVDZ) & (600>BPVVDZ) & (BPVDIRA>0.9999995) "
                                   )

# Define the line
# ############################################################
lm_line = StrippingLine('LambdaLLNoPID'
                        , prescale = 1.
                        , algos = [ LambdaDDCombine ]
                        , stream = 'Lambda_ll'
                    )

