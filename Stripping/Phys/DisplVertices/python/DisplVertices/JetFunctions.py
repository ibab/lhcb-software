"""
Common definitions of a list of PF/jet functions to make the configuration more readable
"""
# FIXME: (MCl) workaround for ROOT issue with autoparse of enums.
import cppyy
if not hasattr(cppyy.gbl.LHCb, 'ParticleFlowType'):
    try:
        cppyy.gbl.gROOT.ProcessLine('#include "Kernel/JetEnums.h"')
    except:
        from GaudiPython import loaddict
        loaddict("JetAccessoriesDict")
from LoKiCore.decorators import LHCb
from LoKiCore.math import max
from LoKiPhys.decorators import INFO

## selection by ParticleFlowType
PFTYPE = INFO(LHCb.ParticleFlowType.KEY, -1.)

## JetID variables from ExtraInfo
JNTRACKS          = INFO(LHCb.JetIDInfo.Ntracks    , -1.)
JN90              = INFO(LHCb.JetIDInfo.N90        , -1.)
JMTF              = INFO(LHCb.JetIDInfo.MTF        , -1.)
JMNF              = INFO(LHCb.JetIDInfo.MNF        , -1.)
JMPT              = INFO(LHCb.JetIDInfo.MPT        , -1.)
JCPF              = INFO(LHCb.JetIDInfo.CPF        , -1.)
JWIDTH            = INFO(LHCb.JetIDInfo.JetWidth   , -1.)
JNSATECAL         = INFO(LHCb.JetIDInfo.NSatCalo   , -1.)
JNSATHCAL         = INFO(LHCb.JetIDInfo.NSatCalo   , -1.)
JNSATCALO         = INFO(LHCb.JetIDInfo.NSatCalo   , -1.)
JNWITHPVINFO      = INFO(LHCb.JetIDInfo.NHasPV     , -1.)
JNWITHPVINFOCHI24 = INFO(LHCb.JetIDInfo.NIPChi2Inf4, -1.)

JMPF = max( JMTF , JMNF )
