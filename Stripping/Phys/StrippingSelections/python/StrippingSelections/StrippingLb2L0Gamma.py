#!/usr/bin/env python
# =============================================================================
# @file   StrippingLb2L0Gamma.py
# @author Albert Puig (albert.puig@epfl.ch)
# @date   20.08.2014
# =============================================================================
"""Stripping lines for Lambda_b -> Lambda0(->p pi) gamma.

These lines cover (a) calorimetric and (b) converted photons.

"""

__author__ = "Albert Puig Navarro"
__date__ = "20.08.2014"
__version__ = "1.0"
__all__ = ("default_config", "StrippingLb2L0GammaConf")

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables import FilterDesktop

from PhysSelPython.Wrappers      import Selection, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder

from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsProtons
from StandardParticles import StdLooseAllPhotons, StdAllLooseGammaDD, StdAllLooseGammaLL

default_config = {'NAME'        : 'StrippingLb2L0Gamma',
                  'WGs'         : ['RD'],
                  'BUILDERTYPE' : 'StrippingLb2L0GammaConf',
                  'CONFIG'      : {# Prescales
                                   'Lb2L0GammaPrescale'          : 1.0,
                                   'Lb2L0GammaConvertedPrescale' : 1.0,
                                   # Trigger cuts
                                   'L0'                          : ['Photon', 'Electron', 'Hadron'],
                                   'HLT'                         : [],
                                   # Track cuts
                                   'Track_Chi2ndf_Max'           : 3.0,
                                   'Track_MinChi2ndf_Max'        : 2.0,
                                   'Track_GhostProb_Max'         : 0.4,
                                   'Track_IPChi2_Min'            : 16.0,
                                   'Track_Pt_Min'                : 300.0,
                                   'Track_P_Min'                 : 1000.0,
                                   # Lambda0 cuts
                                   'Lambda0_VtxChi2_Max'         : 9.0,
                                   'Lambda0LL_MassWindow'        : 20.0,
                                   'Lambda0DD_MassWindow'        : 30.0,
                                   'Lambda0_Pt_Min'              : 1000.0,
                                   # Photon cuts
                                   'Photon_PT_Min'               : 2500.0,
                                   'PhotonCnv_PT_Min'            : 1000.0,
                                   'PhotonCnv_MM_Max'            : 100.0,
                                   'PhotonCnv_VtxChi2_Max'       : 9.0,
                                   # Lambda_b cuts
                                   'Lambdab_VtxChi2_Max'         : 9.0,
                                   'Lambdab_Pt_Min'              : 1000.0,
                                   'Lambdab_IPChi2_Max'          : 9.0,
                                   'Lambdab_MTDOCAChi2_Max'      : 10.0, # Can be lowered to 7
                                   'Lambdab_MassWindow'          : 1100.0,
                                  }
                 }

class StrippingLb2L0GammaConf(LineBuilder):
    __configuration_keys__ = ('Lb2L0GammaPrescale'         ,
                              'Lb2L0GammaConvertedPrescale',
                              'L0'                         ,
                              'HLT'                        ,
                              'Track_Chi2ndf_Max'          ,
                              'Track_GhostProb_Max'        ,
                              'Track_IPChi2_Min'           ,
                              'Track_Pt_Min'               ,
                              'Track_P_Min'                ,
                              'Lambda0_VtxChi2_Max'        ,
                              'Lambda0LL_MassWindow'       ,
                              'Lambda0DD_MassWindow'       ,
                              'Lambda0_Pt_Min'             ,
                              'Photon_PT_Min'              ,
                              'PhotonCnv_PT_Min'           ,
                              'PhotonCnv_MM_Max'           ,
                              'PhotonCnv_VtxChi2_Max'      ,
                              'Lambdab_VtxChi2_Max'        ,
                              'Lambdab_Pt_Min'             ,
                              'Lambdab_IPChi2_Max'         ,
                              'Lambdab_MTDOCAChi2_Max'     ,
                              'Lambdab_MassWindow'         ,
                             )

    def __init__(self, name, config):
        self.name = name
        LineBuilder.__init__(self, name, config)
        #################################################################################
        # Configure trigger
        #################################################################################
        l0 = None
        if config['L0']:
            l0 = "L0_CHANNEL_RE('%s')" % ('|'.join(config['L0']))
        hlt = None
        if config['HLT']:
            hlt = "HLT_PASS_RE('%s')" % ('|'.join(config['HLT']))
        #################################################################################
        # Build Lambda_0
        #################################################################################
        tracks_code = """(CHILD(TRCHI2DOF,1) < %(Track_Chi2ndf_Max)s) &
                         (CHILD(TRCHI2DOF,2) < %(Track_Chi2ndf_Max)s) &
                         (MAXTREE(HASTRACK,TRCHI2DOF) < %(Track_MinChi2ndf_Max)s) &
                         (CHILD(TRGHOSTPROB,1) < %(Track_GhostProb_Max)s) &
                         (CHILD(TRGHOSTPROB,2) < %(Track_GhostProb_Max)s) &
                         (CHILD(MIPCHI2DV(PRIMARY),1) > %(Track_IPChi2_Min)s) &
                         (CHILD(MIPCHI2DV(PRIMARY),2) > %(Track_IPChi2_Min)s) &
                         (CHILD(PT,1) > %(Track_IPChi2_Min)s) &
                         (CHILD(PT,2) > %(Track_IPChi2_Min)s) &
                         (CHILD(P,1) > %(Track_P_Min)s) &
                         (CHILD(P,2) > %(Track_P_Min)s)"""
        lambda0_ll_dod = DataOnDemand(Location='Phys/StdLooseLambdaLL/Particles')
        lambda0_ll_code = """(PT>%(Lambda0_Pt_Min)s*MeV) &
                             (VFASPF(VCHI2/VDOF)<%(Lambda0_VtxChi2_Max)s) &
                             (ADMASS('Lambda0') < %(Lambda0LL_MassWindow)s*MeV)"""
        lambda0_ll_code = lambda0_ll_code + " & " + tracks_code
        lambda0_ll_filter = FilterDesktop(Code=lambda0_ll_code)
        lambda0_ll = Selection("LooseLambda0LL",
                               Algorithm=lambda0_ll_filter,
                               RequiredSelections=[lambda0_ll_dod])
        lambda0_dd_dod = DataOnDemand(Location='Phys/StdLooseLambdaDD/Particles')
        lambda0_dd_code = """(PT>%(Lambda0_Pt_Min)s*MeV) &
                             (VFASPF(VCHI2/VDOF)<%(Lambda0_VtxChi2_Max)s) &
                             (ADMASS('Lambda0') < %(Lambda0DD_MassWindow)s*MeV)"""
        lambda0_dd_code = lambda0_dd_code + " & " + tracks_code
        lambda0_dd_filter = FilterDesktop(Code=lambda0_dd_code)
        lambda0_dd = Selection("LooseLambda0DD",
                               Algorithm=lambda0_dd_filter,
                               RequiredSelections=[lambda0_dd_dod])
        lambda0 = MergedSelection("LooseLambda0",
                                  RequiredSelections=[lambda0_ll, lambda0_dd])
        #################################################################################
        # Filter photons
        #################################################################################
        photons_noncnv_filter = FilterDesktop(Code="(PT > %(Photon_PT_Min)s*MeV)" % config)
        photons_noncnv = Selection("Photons_NonCnv",
                                   Algorithm=photons_noncnv_filter,
                                   RequiredSelections=[StdLooseAllPhotons])
        photons_cnv_merged = MergedSelection("Photons_Cnv_Merge",
                                             RequiredSelections=[StdAllLooseGammaDD,StdAllLooseGammaLL])
        photons_cnv_code = """(HASVERTEX) &
                              (MM < %(PhotonCnv_MM_Max)s*MeV) &
                              (PT > %(PhotonCnv_PT_Min)s*MeV) &
                              (VFASPF(VCHI2/VDOF)<%(PhotonCnv_VtxChi2_Max)s)"""
        photons_cnv_filter = FilterDesktop(Code=photons_cnv_code % config)
        photons_cnv = Selection("Photons_Cnv",
                                Algorithm=photons_cnv_filter,
                                RequiredSelections=[photons_cnv_merged])
        #################################################################################
        # Build Lambda_b
        #################################################################################
        # With non-converted photons
        lambda_b_combine = CombineParticles("Lambdab_NonConv_Combine")
        lambda_b_combine.DecayDescriptor = "[Lambda_b0 -> Lambda0 gamma]cc"
        lambda_b_combine.DaughtersCuts = {'Lambda0': 'ALL', 'gamma': 'ALL'}
        lambda_b_combine.ParticleCombiners = {'' : 'ParticleAdder'}
        lambda_b_combine.CombinationCuts = "(ADAMASS('Lambda_b0') < %(Lambdab_MassWindow)s*MeV)"
        lambda_b_combine.MotherCut = """(PT > %(Lambdab_Pt_Min)s*MeV) &
                                        (MTDOCACHI2(1) < %(Lambdab_MTDOCAChi2_Max)s)"""
        lambda_b = Selection("Lambdab_NonConv_Sel",
                             Algorithm=lambda_b_combine,
                             RequiredSelections=[photons_noncnv, lambda0])
        # With converted photons
        lambda_b_cnv_combine = CombineParticles("Lambdab_Conv_Combine")
        lambda_b_cnv_combine.DecayDescriptor = "[Lambda_b0 -> Lambda0 gamma]cc"
        lambda_b_cnv_combine.DaughtersCuts = {'Lambda0': 'ALL', 'gamma': 'ALL'}
        lambda_b_cnv_combine.CombinationCuts = "(ADAMASS('Lambda_b0') < 1.5*%(Lambdab_MassWindow)s*MeV)"
        lambda_b_cnv_combine.MotherCut = """(HASVERTEX) & (VFASPF(VCHI2/VDOF)<%(Lambdab_VtxChi2_Max)s) &
                                            (PT > %(Lambdab_Pt_Min)s*MeV) &
                                            (BPVIPCHI2() < %(Lambdab_IPChi2_Max)s) &
                                            (MTDOCACHI2(1) < %(Lambdab_MTDOCAChi2_Max)s) &
                                            (ADMASS('Lambda_b0') < %(Lambdab_MassWindow)s*MeV)"""
        lambda_b_cnv = Selection("Lambdab_NonConv_Sel",
                                 Algorithm=lambda_b_cnv_combine,
                                 RequiredSelections=[photons_cnv, lambda0])
        #################################################################################
        # Build lines
        #################################################################################
        self.line = StrippingLine("Lb2L0Gamma",
                                  prescale=config["Lb2L0GammaPrescale"],
                                  L0DU=l0,
                                  HLT=hlt,
                                  checkPV=True,
                                  RelatedInfoTools=[],
                                  RequiredRawEvents=['Calo'],
                                  selection=lambda_b)
        self.registerLine(self.line)
        self.line_cnv = StrippingLine("Lb2L0GammaConverted",
                                      prescale=config["Lb2L0GammaPrescale"],
                                      L0DU=l0,
                                      HLT=hlt,
                                      checkPV=True,
                                      RelatedInfoTools=[],
                                      RequiredRawEvents=['Calo'],
                                      selection=lambda_b_cnv)
        self.registerLine(self.line_cnv)

# EOF

