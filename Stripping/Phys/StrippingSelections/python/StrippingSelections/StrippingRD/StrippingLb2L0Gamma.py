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

from Configurables import CombineParticles
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop

from PhysSelPython.Wrappers      import Selection, MergedSelection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder

from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsProtons
from StandardParticles import StdLooseAllPhotons, StdAllLooseGammaDD, StdAllLooseGammaLL

default_config = {'NAME'        : 'Lb2L0Gamma',
                  'WGs'         : ['RD'],
                  'BUILDERTYPE' : 'StrippingLb2L0GammaConf',
                  'CONFIG'      : {# Prescales
                                   'Lb2L0GammaPrescale'          : 1.0,
                                   'Lb2L0GammaConvertedPrescale' : 1.0,
                                   # Trigger cuts
                                   'L0'                          : ['Photon', 'Electron', 'Hadron'],
                                   'HLT1'                         : [],
                                   'HLT2'                         : [],
                                   # Track cuts
                                   'Track_Chi2ndf_Max'           : 3.0,
                                   'Track_MinChi2ndf_Max'        : 2.0,
                                   'Track_GhostProb_Max'         : 0.4,
                                   'TrackLL_IPChi2_Min'          : 16.0,
                                   'Pion_P_Min'                  : 2000.0,
                                   'Proton_P_Min'                : 7000.0, # Can increase
                                   'Pion_Pt_Min'                 : 300.0,
                                   'Proton_Pt_Min'               : 800.0, # Can increase
                                   # Lambda0 cuts
                                   'Lambda0_VtxChi2_Max'         : 9.0,
                                   'Lambda0LL_IP_Min'            : 0.05,
                                   'Lambda0LL_MassWindow'        : 20.0,
                                   'Lambda0DD_MassWindow'        : 30.0,
                                   'Lambda0_Pt_Min'              : 1000.0,
                                   # Photon cuts
                                   'Photon_PT_Min'               : 2500.0,
                                   'Photon_CL_Min'               : 0.2,
                                   'PhotonCnv_PT_Min'            : 1000.0,
                                   'PhotonCnv_MM_Max'            : 100.0,
                                   'PhotonCnv_VtxChi2_Max'       : 9.0,
                                   # Lambda_b cuts
                                   'Lambdab_VtxChi2_Max'         : 9.0,
                                   'Lambdab_Pt_Min'              : 1000.0,
                                   'Lambdab_SumPt_Min'           : 5000.0,
                                   'Lambdab_IPChi2_Max'          : 9.0,
                                   'Lambdab_MTDOCAChi2_Max'      : 7.0,
                                   'Lambdab_MassWindow'          : 1100.0,
                                  },
                  'STREAMS' : ['Leptonic'],
                 }

class StrippingLb2L0GammaConf(LineBuilder):
    __configuration_keys__ = ('Lb2L0GammaPrescale'         ,
                              'Lb2L0GammaConvertedPrescale',
                              'L0'                         ,
                              'HLT1'                       ,
                              'HLT2'                       ,
                              'Track_Chi2ndf_Max'          ,
                              'Track_GhostProb_Max'        ,
                              'Track_MinChi2ndf_Max'       ,
                              'TrackLL_IPChi2_Min'         ,
                              'Pion_P_Min'                 ,
                              'Proton_P_Min'               ,
                              'Pion_Pt_Min'                ,
                              'Proton_Pt_Min'              ,
                              'Lambda0_VtxChi2_Max'        ,
                              'Lambda0LL_IP_Min'           ,
                              'Lambda0LL_MassWindow'       ,
                              'Lambda0DD_MassWindow'       ,
                              'Lambda0_Pt_Min'             ,
                              'Photon_PT_Min'              ,
                              'Photon_CL_Min'              ,
                              'PhotonCnv_PT_Min'           ,
                              'PhotonCnv_MM_Max'           ,
                              'PhotonCnv_VtxChi2_Max'      ,
                              'Lambdab_VtxChi2_Max'        ,
                              'Lambdab_Pt_Min'             ,
                              'Lambdab_SumPt_Min'          ,
                              'Lambdab_IPChi2_Max'         ,
                              'Lambdab_MTDOCAChi2_Max'     ,
                              'Lambdab_MassWindow'         ,
                              #'RelatedInfoTools'           ,
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
        hlt1 = None
        if config['HLT1']:
            hlt1 = "HLT_PASS_RE('%s')" % ('|'.join(config['HLT1']))
        hlt2 = None
        if config['HLT2']:
            hlt2 = "HLT_PASS_RE('%s')" % ('|'.join(config['HLT2']))
        #################################################################################
        # Build Lambda_0
        #################################################################################
        tracks_code = """(MAXTREE(TRCHI2DOF, HASTRACK) < %(Track_Chi2ndf_Max)s) &
                         (MINTREE(TRCHI2DOF, HASTRACK) < %(Track_MinChi2ndf_Max)s) &
                         (MAXTREE(TRGHOSTPROB, HASTRACK) < %(Track_GhostProb_Max)s) &
                         (INTREE(('p+'==ABSID) & (PT > %(Proton_Pt_Min)s))) &
                         (INTREE(('pi+'==ABSID) & (PT > %(Pion_Pt_Min)s))) &
                         (INTREE(('p+'==ABSID) & (P > %(Proton_P_Min)s))) &
                         (INTREE(('pi+'==ABSID) & (P > %(Pion_P_Min)s)))"""
        lambda0_ll_dod = DataOnDemand(Location='Phys/StdLooseLambdaLL/Particles')
        lambda0_ll_code = """(PT>%(Lambda0_Pt_Min)s*MeV) &
                             (VFASPF(VCHI2/VDOF)<%(Lambda0_VtxChi2_Max)s) &
                             (MINTREE(MIPCHI2DV(PRIMARY), ISLONG) > %(TrackLL_IPChi2_Min)s) &
                             (MIPDV(PRIMARY) > %(Lambda0LL_IP_Min)s*mm) &
                             (ADMASS('Lambda0') < %(Lambda0LL_MassWindow)s*MeV)"""
        lambda0_ll_code = (lambda0_ll_code + " & " + tracks_code) % config
        lambda0_ll_filter = FilterDesktop(Code=lambda0_ll_code)
        lambda0_ll = Selection("LooseLambda0LL",
                               Algorithm=lambda0_ll_filter,
                               RequiredSelections=[lambda0_ll_dod])
        lambda0_dd_dod = DataOnDemand(Location='Phys/StdLooseLambdaDD/Particles')
        lambda0_dd_code = """(PT>%(Lambda0_Pt_Min)s*MeV) &
                             (VFASPF(VCHI2/VDOF)<%(Lambda0_VtxChi2_Max)s) &
                             (ADMASS('Lambda0') < %(Lambda0DD_MassWindow)s*MeV)"""
        lambda0_dd_code = (lambda0_dd_code + " & " + tracks_code) % config
        lambda0_dd_filter = FilterDesktop(Code=lambda0_dd_code)
        lambda0_dd = Selection("LooseLambda0DD",
                               Algorithm=lambda0_dd_filter,
                               RequiredSelections=[lambda0_dd_dod])
        lambda0 = MergedSelection("LooseLambda0",
                                  RequiredSelections=[lambda0_ll, lambda0_dd])
        #################################################################################
        # Filter photons
        #################################################################################
        photons_noncnv_filter = FilterDesktop(Code="(PT > %(Photon_PT_Min)s*MeV) & (CL > %(Photon_CL_Min)s)" % config)
        photons_noncnv = Selection("Photons_NonCnv",
                                   Algorithm=photons_noncnv_filter,
                                   RequiredSelections=[StdLooseAllPhotons])
        photons_cnv_merged = MergedSelection("Photons_Cnv_Merge",
                                             RequiredSelections=[StdAllLooseGammaDD,StdAllLooseGammaLL])
        photons_cnv_code = """(HASVERTEX) &
                              (MM < %(PhotonCnv_MM_Max)s*MeV) &
                              (PT > %(PhotonCnv_PT_Min)s*MeV) &
                              (VFASPF(VCHI2/VDOF)<%(PhotonCnv_VtxChi2_Max)s)""" % config
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
        lambda_b_combine.ParticleCombiners = {'' : 'MomentumCombiner:PUBLIC'}
        lambda_b_combine.CombinationCut = """(ADAMASS('Lambda_b0') < %(Lambdab_MassWindow)s*MeV) &
                                             (ASUM(PT) > %(Lambdab_SumPt_Min)s )""" % config
        lambda_b_combine.MotherCut = """(PT > %(Lambdab_Pt_Min)s*MeV) &
                                        (MTDOCACHI2(1) < %(Lambdab_MTDOCAChi2_Max)s)""" % config
        lambda_b = Selection("Lambdab_NonConv_Sel",
                             Algorithm=lambda_b_combine,
                             RequiredSelections=[photons_noncnv, lambda0])
        # With converted photons
        lambda_b_cnv_combine = CombineParticles("Lambdab_Conv_Combine")
        lambda_b_cnv_combine.DecayDescriptor = "[Lambda_b0 -> Lambda0 gamma]cc"
        lambda_b_cnv_combine.DaughtersCuts = {'Lambda0': 'ALL', 'gamma': 'ALL'}
        lambda_b_cnv_combine.ParticleCombiners = {"": "OfflineVertexFitter:PUBLIC"}
        lambda_b_cnv_combine.CombinationCut = "(ADAMASS('Lambda_b0') < 1.5*%(Lambdab_MassWindow)s*MeV)" % config
        lambda_b_cnv_combine.MotherCut = """(HASVERTEX) & (VFASPF(VCHI2/VDOF)<%(Lambdab_VtxChi2_Max)s) &
                                            (PT > %(Lambdab_Pt_Min)s*MeV) &
                                            (BPVIPCHI2() < %(Lambdab_IPChi2_Max)s) &
                                            (ADMASS('Lambda_b0') < %(Lambdab_MassWindow)s*MeV)""" % config
                                            #(MTDOCACHI2(1) < %(Lambdab_MTDOCAChi2_Max)s) &
        lambda_b_cnv = Selection("Lambdab_Conv_Sel",
                                 Algorithm=lambda_b_cnv_combine,
                                 RequiredSelections=[photons_cnv, lambda0])
        #################################################################################
        # Build lines
        #################################################################################
        sels_line = {'Phys/StdLooseLambdaLL' : 'Lambda0',
                     'Phys/StdLooseLambdaDD' : 'Lambda0'}
        self.line = StrippingLine("Lb2L0Gamma",
                                  prescale=config["Lb2L0GammaPrescale"],
                                  L0DU=l0,
                                  HLT1=hlt1,
                                  HLT2=hlt2,
                                  checkPV=True,
                                  RelatedInfoTools=[self.get_cone_relinfo(sels_line, 1.7, 1),
                                                    self.get_cone_relinfo(sels_line, 1.35, 1),
                                                    self.get_cone_relinfo(sels_line, 1.0, 1),
                                                    self.get_vtxisol_relinfo(sels_line),
                                                   ],
                                  RequiredRawEvents=['Calo'],
                                  MDSTFlag = True,
                                  selection=lambda_b)
        self.registerLine(self.line)
        sels_line_cnv = {'Phys/StdLooseLambdaLL' : 'Lambda0',
                         'Phys/StdLooseLambdaDD' : 'Lambda0',
                         lambda_b_cnv: 'Lambdab'}
        self.line_cnv = StrippingLine("Lb2L0GammaConverted",
                                      prescale=config["Lb2L0GammaPrescale"],
                                      L0DU=l0,
                                      HLT1=hlt1,
                                      HLT2=hlt2,
                                      checkPV=True,
                                      RelatedInfoTools=[self.get_cone_relinfo(sels_line_cnv, 1.7, 1),
                                                        self.get_cone_relinfo(sels_line_cnv, 1.35, 1),
                                                        self.get_cone_relinfo(sels_line_cnv, 1.0, 1),
                                                        self.get_vtxisol_relinfo(sels_line),
                                                       ],
                                      RequiredRawEvents=['Calo'],
                                      MDSTFlag = True,
                                      selection=lambda_b_cnv)
        self.registerLine(self.line_cnv)


    @staticmethod
    def get_cone_relinfo(selections, angle, recursion=0):
        locations = {}
        for selection, selection_prefix in selections.items():
            locations[selection] = 'ConeVarsInfo/%s/%s' % (selection_prefix, angle)
        return { 'Type'           : 'RelInfoConeVariables',
                 'ConeAngle'      : angle,
                 'Variables'      : [ 'CONEANGLE', 'CONEMULT',
                                      'CONEP', 'CONEPASYM',
                                      'CONEPT', 'CONEPTASYM' ],
                 'RecursionLevel' : recursion,
                 'Locations'      : locations,
                }

    @staticmethod
    def get_vtxisol_relinfo(selections, recursion=0):
        locations = {}
        for selection, selection_prefix in selections.items():
            locations[selection] = 'VertexIsoInfo/%s' % selection_prefix
        return { 'Type'           : 'RelInfoVertexIsolation',
                 'Variables'      : [ 'VTXISONUMVTX',
                                      'VTXISODCHI2ONETRACK', 'VTXISODCHI2MASSONETRACK',
                                      'VTXISODCHI2TWOTRACK', 'VTXISODCHI2MASSTWOTRACK' ],
                 'RecursionLevel' : recursion,
                 'Locations'      : locations,
                }

# EOF

