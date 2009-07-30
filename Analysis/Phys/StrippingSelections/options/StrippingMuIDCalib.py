## #####################################################################
# A stripping selection for D*+ -> D0(Kpi) pi+
#
# This is a relatively tight, high purity selection that does not use PID
# information.  It is intended for RICH calibration.
#
# @author Andrew Powell and Patrick Spradlin
# @date 2009-June-24
# 
## #####################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember


# Create Jpsi -> mumu candidates out of no pid muons
## ############################################################
muonscut = '(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & (P>3*GeV) & (PT>500*MeV) & (TRCHI2DOF<3) & (ISLONG) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1500*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<6000*MeV) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>500*MeV)'
childcuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1))"
JpsiCombine = StrippingMember( CombineParticles
                               , 'Combine'
                               , InputLocations = [ 'StdNoPIDsMuons' ]
                               , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                               , DaughtersCuts = { 'mu+' : muonscut
                                                   , 'mu-'  : muonscut }
                               , CombinationCut = "(ADAMASS('J/psi(1S)')<300*MeV)"
                               , MotherCut = "(VFASPF(VCHI2/VDOF)<6) & " + childcuts
                               )

# Define the line
## ############################################################
jp_line = StrippingLine('JpsiNoPID'
                        , prescale = 0.007
                        , algos = [ JpsiCombine ]
                        , stream = 'Jpsi'
                        )


# Create Lambda -> ppi candidates
## ############################################################
LambdaCombine = StrippingMember( CombineParticles
                                 , 'Combine'
                                 , InputLocations = [ "StdNoPIDsDownPions","StdNoPIDsDownProtons" ]
                                 , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                 , DaughtersCuts = { "p+"  :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<3) & (ISDOWN) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))",
                                                     "pi-" :  "(P>3000*MeV) & (PT>100*MeV) & (TRCHI2DOF<3) & (ISDOWN) & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))" }
                                 , CombinationCut = "(ADAMASS('Lambda0')<30*MeV)"
                                 , MotherCut = "(VFASPF(VCHI2)<6) & (500<BPVVDZ) & (BPVDIRA>0.999999) "
                                 )

# Define the line

# ############################################################
lm_line = StrippingLine('LambdaNoPID'
                        , prescale = 0.0005
                        , algos = [ LambdaCombine ]
                        , stream = 'Dstar'
                    )

