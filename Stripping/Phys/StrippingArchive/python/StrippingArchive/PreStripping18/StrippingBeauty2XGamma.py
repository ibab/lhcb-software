#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file StrippingBeauty2XGamma.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-11-17
# =============================================================================
"""Module for Stripping B->X gamma.

Based on the StrippingBeautry2Charm Stripping by M. Williams.
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"

# Gaudi configuration
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
# Standard Particles
from StandardParticles import StdLooseResolvedPi0, StdLooseMergedPi0, StdLooseAllPhotons
from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons, StdAllNoPIDsProtons
# Builders
from Beauty2XGamma_HHBuilder import HHBuilder as HHBuilder
#from Beauty2Charm_HHBuilder import HHBuilder as HHBuilder
# from Beauty2Charm_HHHBuilder import HHBuilder as HHHBuilder
from Beauty2XGamma_B2XGammaBuilder import B2XGammaBuilder
from Beauty2XGamma_Lb2XGammaBuilder import Lb2XGammaBuilder
from Beauty2XGamma_Utils import *

# Configuration dictionary
moduleName = "Beauty2XGamma"
config = { # Cuts made on all charged input particles in all lines
           "ALL" : {  'TRCHI2DOF_MAX' : 4,
                      'PT_MIN'        : '250*MeV',
                      'P_MIN'         : '1000*MeV',
                      'MIPCHI2DV_MIN' : 4 },
           # Cuts made on the photon
           "GAMMA" : { 'PT_MIN'       : '2500*MeV',
                       'CL_MIN'       : 0.25 
                     },
           # Cuts made on all K shorts
           "KS0" : { 'PT_MIN'        : '250*MeV',
                     'BPVVDCHI2_MIN' : 36,
                     'MM_MIN'        : '467.*MeV',
                     'MM_MAX'        : '527.*MeV' },
           # Cuts made on all pi0's
           "Pi0" : { 'PT_MIN'        : '1200*MeV',
                     'P_MIN'         : '10000*MeV',
                     'CHILDCL1_MIN'  : 0.25,
                     'CHILDCL2_MIN'  : 0.25 },
           # Cuts made on all B's and Lb's used in all lines
           "B2X" : { 'SUMPT_MIN'     : '4000*MeV',
                     'VCHI2DOF_MAX'  : 15,
                     'BPVIPCHI2_MAX' : 15,
                     'BPVLTIME_MIN'  : '0.2*ps',
                     'BPVDIRA_MIN'   : 0.9998,
                     'AM_MIN'        : '4000*MeV',
                     'AM_MAX'        : '7000*MeV',
                     'B2CBBDT_MIN'   : 0.00 
                     },
           # Cuts for rho, K*, phi, omega
           "HH": { 'MASS_WINDOW'   : {'KST':'150*MeV',
                                      'RHO':'250*MeV',
                                      'PHI':'15*MeV',
                                      'OMEGA':'30*MeV'}, 
                   'DAUGHTERS'     : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV'},
                   'LAMBDADAUGHTERS' : {'PT_MIN':'300*MeV','P_MIN':'2500*MeV'},
                   'AMAXDOCA_MAX'  : '0.5*mm',
                   'VCHI2DOF_MAX'  : 16,
                   'BPVVDCHI2_MIN' : 16, 
                   'BPVDIRA_MIN'   : 0.0,
                   'ASUMPT_MIN'    : '1000*MeV',
                   'pP_MIN'        : '10000*MeV' # for pH only (obviously)
                 },
           # Cuts for PiPiPi0 for omega decay
           # "HHH": { 'MASS_WINDOW'   : {'OMEGA': '30*MeV'},
           #          'KDAUGHTERS'    : {'PT_MIN':'100*MeV',
           #                             'P_MIN':'2000*MeV',
           #                             'PIDK_MIN':'-5'},
           #          'PiDAUGHTERS'   : {'PT_MIN':'100*MeV',
           #                             'P_MIN':'2000*MeV',
           #                             'PIDK_MAX':'10'},
           #          'pDAUGHTERS'    : {'PT_MIN':'100*MeV',
           #                             'P_MIN':'2000*MeV',
           #                             'PIDp_MIN':'-5'},
           #          'AMAXDOCA_MAX'  : '0.40*mm',
           #          'VCHI2DOF_MAX'  : 8,
           #          'BPVVDCHI2_MIN' : 16, 
           #          'BPVDIRA_MIN'   : 0.98,
           #          'ASUMPT_MIN'    : '1250*MeV',
           #          'MIPCHI2DV_MIN' : 0.0,
           #          'BPVVDRHO_MIN'  : '0.1*mm',
           #          'BPVVDZ_MIN'    : '2.0*mm',
           #          'PTMIN1'       : '300*MeV'},
           # PID cuts
           "PID" : { 'P'  : {'PIDp_MIN' : 0},
                     'PI' : {'PIDK_MAX' : 20},
                     'K'  : {'PIDK_MIN' : -10} },
    "Prescales"  : {},
    "GECNTrkMax" : 500 }

# Stripping class
class Beauty2XGamma(LineBuilder):
    __configuration_keys__ = ('ALL',
                              'B2X',
                              'GAMMA',
                              'GECNTrkMax',
                              'HH',
#                              'HHH',
                              'KS0',
                              'PID',
                              'Pi0',
                              'Prescales')

    def __init__(self, moduleName, config):
        LineBuilder.__init__(self, moduleName, config)
        ###########################################################################
        # Prepare the inputs
        ###########################################################################
        # Prefilter input tracks
        pions = filterInputs('Pi', [StdAllNoPIDsPions], config['ALL'])
        kaons = filterInputs('K', [StdAllNoPIDsKaons], config['ALL'])
        protons = filterInputs('P', [StdAllNoPIDsProtons], config['ALL'])
        # Prefilter KS
        ks_dd = filterInputs('KS0_DD',
                             [dataOnDemand("StdLooseKsDD")],
                             config['KS0']) # KS from Downstream-Dowstream
        ks_ll = filterInputs('KS0_LL',
                             [dataOnDemand("StdLooseKsLL")],
                             config['KS0']) # KS from Long-Long
        ks = {"DD": [ks_dd], "LL": [ks_ll]}
        # Prefilter photons
        photons = filterPhotons([StdLooseAllPhotons], config['GAMMA'])
        # Prefilter pi0
        pi0_merged   = filterPi0s('Merged', [StdLooseMergedPi0], config['Pi0'])
        pi0_resolved = filterPi0s('Resolved', [StdLooseResolvedPi0], config['Pi0'])
        pi0 = {"Merged": [pi0_merged], "Resolved": [pi0_resolved]}
        # Prefilter hard inputs
        topoPions = topoInputs('Pi', [pions])
        topoKaons = topoInputs('K', [kaons])
        topoProtons = topoInputs('P', [protons])
        ###########################################################################
        # Build particles
        ###########################################################################
        # X -> hh
        hh = HHBuilder(pions, kaons, protons, ks, pi0, config['HH'], config['PID'])
        # X -> hhh
        # hhh = HHHBuilder(pions, kaons, protons, config['HHH'])
        hhh = None
        # Lambdas
        # lambda = LambdaBuilder(pions, kaons, protons, config['D2X'], config['PID'])    
        ###########################################################################
        # B -> X Gamma
        ###########################################################################
        b2xgamma = B2XGammaBuilder(photons, topoPions, topoKaons, ks, pi0, hh, hhh, config['B2X'])
        self._makeLines(b2xgamma.lines, config)
        ###########################################################################
        # Lb -> X Gamma
        ###########################################################################
        lb2xgamma = Lb2XGammaBuilder(photons, hh, config['B2X'])
        self._makeLines(lb2xgamma.lines, config)
    
    def _makeLine(self,protoLine,config):
        tag = 'B2XGBBDTBeauty2XGammaFilter'
        for line in protoLine.selections:
            tmpSel = Selection(line.name() + 'FilterALL',
                               Algorithm=FilterDesktop(Code='ALL'),
                               RequiredSelections=[line])
            _filter = { 'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GECNTrkMax'],
                        'Preambulo' : [ "from LoKiTracks.decorators import *",
                                        "from LoKiCore.functions    import *" ]
                      }
            l0 = "|".join( [ "L0_CHANNEL('%s')" % chan for chan in [ 'Electron','Photon','Hadron','Muon','DiMuon'] ] )
            hlt = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision') & HLT_PASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision')"
            name = line.name().replace(tag,'')+'Line'
            sline = StrippingLine(name,
                                  protoLine.prescale(line, name, config),
                                  selection=tmpSel,
                                  checkPV=True,
                                  FILTER=_filter,
                                  #L0DU = l0,
                                  #HLT=hlt
                                  )
            self.registerLine(sline)
      
    def _makeLines(self, lines, config):
        for line in lines:
            self._makeLine(line, config)

# EOF
