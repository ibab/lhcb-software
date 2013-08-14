#!/usr/bin/env python
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
from StandardParticles import StdLooseResolvedPi0, StdLooseMergedPi0, StdLooseAllPhotons, StdAllLooseElectrons,StdLooseMuons
from StandardParticles import StdNoPIDsPions, StdNoPIDsDownPions, StdNoPIDsKaons, StdNoPIDsProtons
from StandardParticles import StdAllLooseGammaLL, StdAllLooseGammaDD

# Builders
from Beauty2XGamma_HHBuilder import HHBuilder as HHBuilder
from Beauty2XGamma_HHHBuilder import HHHBuilder as HHHBuilder
from Beauty2XGamma_B2XGammaBuilder import B2XGammaBuilder
from Beauty2XGamma_Lb2XGammaBuilder import Lb2XGammaBuilder
from Beauty2XGamma_Utils import *

# Configuration dictionary
moduleName = "Beauty2XGamma"
default_config = { # Cuts made on all charged input particles in all lines
           "ALL" : {  'TRCHI2DOF_MAX'  : 3,
                      'PT_MIN'         : '300*MeV',
                      'P_MIN'          : '1000*MeV',
                      'MIPCHI2DV_MIN'  : 16,
                      'TRGHP_MAX'      : 0.4},
           # Cuts made on the photon
           "GAMMA" : { 'PT_MIN'       : '2000*MeV',
                       'CL_MIN'       : 0.25
                     },
           # Cuts made on the converted photon
           "GAMMACONV" : { 'VCHI2DOF_MAX' : 9,
                           'MM_MAX'       : '100*MeV'
                        },
           # Cuts made on all K shorts
           "KS0" : { 'PT_MIN'        : '1000*MeV',
                     'BPVVDCHI2_MIN' : 81 ,
                     #'MIPCHI2DV_MIN' : 4,
                     'VCHI2DOF_MAX'  : 9,
                     'MM_MIN'        : '480.*MeV',
                     'MM_MAX'        : '515.*MeV' },
           # Cuts made on all pi0's
           "Pi0" : { 'PT_MIN'        : '1000*MeV',
                     'P_MIN'         : '0*MeV',
                     'CHILDCL1_MIN'  : 0.25,
                     'CHILDCL2_MIN'  : 0.25 },
           # Cuts made on all B's and Lb's used in all lines
           "B2X" : { 'SUMPT_MIN'     : '5000*MeV',
                     'VCHI2DOF_MAX'  : 9,
                     'BPVIPCHI2_MAX' : 9,
                     #'BPVLTIME_MIN'  : '0.0*ps',
                     'BPVDIRA_MIN'   : 0.9998,
                     'AM_MIN'        : '4000*MeV',
                     'AM_MAX'        : '7000*MeV',
                     #'BBDT_MIN'      : 0.0,
                     },
           # Cuts for rho, K*, phi, omega
           "HH": { 'MASS_WINDOW'      : {'KST':'150*MeV','RHO': ['520*MeV','2.0*GeV'],'PHI':'15*MeV','OMEGA':'30*MeV'},
                   'DAUGHTERS'        : {'PT_MIN':'500*MeV','P_MIN':'3000*MeV'},
                   'piLAMBDADAUGHTERS': {'PT_MIN':'300*MeV','P_MIN':'3000*MeV','PIDK_MAX':'2'},   # only for pH
                   'pLAMBDADAUGHTERS' : {'PT_MIN':'1200*MeV','P_MIN':'10000*MeV','PIDp_MIN':'10','PIDpK_MIN':'0'}, # only for pH
                   'kLAMBDADAUGHTERS' : {'PT_MIN':'500*MeV','P_MIN':'3000*MeV','PIDK_MIN':'5','PIDKp_MIN':'0'},    # only for pH
                   #'AMAXDOCA_MAX'  : '0.5*mm',
                   'VCHI2DOF_MAX'     : 9,
                   'BPVVDCHI2_MIN'    : 81,
                   #'BPVDIRA_MIN'   : 0.0,
                   'ASUMPT_MIN'       : '1500*MeV',
                   #'pP_MIN'        : '10000*MeV' # for pH only (obviously)
                   'ANGLE_MIN'        : 0.0003,
                 },
           # Cuts for omega -> 3 body decay
           "HHH": { 'MASS_WINDOW'   : {'OMEGA': '100*MeV', 'K1': ['0.8*GeV', '3.5*GeV']},
                    'DAUGHTERS'     : {'PT_MIN':'350*MeV','P_MIN':'2000*MeV'},
                    'AMAXDOCA_MAX'  : '0.50*mm',
                    'VCHI2DOF_MAX'  : 9,
                    'BPVVDCHI2_MIN' : 64,
                    #'BPVDIRA_MIN'   : 0.0,
                    'ASUMPT_MIN'    : '1600*MeV',
                    'PT_MIN'        : '500*MeV',
                    'MIPCHI2DV_MIN' : 0.0,
                    #'BPVVDRHO_MIN'  : '0.1*mm',
                    #'BPVVDZ_MIN'    : '2.0*mm',
                    #'PTMIN1'       : '300*MeV'},
                 },
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
                              'GAMMACONV',
                              'GECNTrkMax',
                              'HH',
                              'HHH',
                              'KS0',
                              'PID',
                              'Pi0',
                              'Prescales')

    def __init__(self, moduleName, default_config):
        LineBuilder.__init__(self, moduleName, default_config)
        ###########################################################################
        # Prepare the inputs
        ###########################################################################
        # Prefilter input tracks
        pions = filterInputs('Pi', [StdNoPIDsPions], default_config['ALL'])
        kaons = filterInputs('K', [StdNoPIDsKaons], default_config['ALL'])
        protons = filterInputs('P', [StdNoPIDsProtons], default_config['ALL'])
        muons = filterInputs('Mu', [StdLooseMuons], default_config['ALL'])
        # Prefilter KS
        mergedKS = MergedSelection('MergedKS0', RequiredSelections = [dataOnDemand("StdLooseKsDD"), dataOnDemand("StdLooseKsLL")])
        KS_filter = filterInputs('KS0', [mergedKS], default_config['KS0'])
        ks = [KS_filter]
        # Prefilter photons
        photons = filterPhotons([StdLooseAllPhotons], default_config['GAMMA'])
        mergedConvPhotons = MergedSelection('MergedConvPhotons', RequiredSelections = [StdAllLooseGammaDD,StdAllLooseGammaLL])
        photonsConv = filterPhotonsConv([mergedConvPhotons], default_config['GAMMACONV'])
        # Prefilter pi0
        pi0_merged   = filterPi0s('Merged', [StdLooseMergedPi0], default_config['Pi0'])
        pi0_resolved = filterPi0s('Resolved', [StdLooseResolvedPi0], default_config['Pi0'])
        pi0 = {"Merged": [pi0_merged], "Resolved": [pi0_resolved]}
        # Prefilter hard inputs
        topoPions = topoInputs('Pi', [pions])
        softtopoKaons = softtopoInputs('K', [kaons])
        topoKaons = topoInputs('K', [softtopoKaons])
        topoProtons = topoInputs('P', [protons])
        ###########################################################################
        # Build particles
        ###########################################################################
        # X -> hh
        hh = HHBuilder(pions, kaons, protons, muons, ks, pi0, default_config['HH'], default_config['PID'])
        # X -> hhh
        hhh = HHHBuilder(pions, kaons, protons, pi0, default_config['HHH'])
        #hhh = None
        ###########################################################################
        # B -> X Gamma
        ###########################################################################
        b2xgamma = B2XGammaBuilder(photons, photonsConv, topoPions, topoKaons, softtopoKaons, ks, pi0, hh, hhh, default_config['B2X'])
        #b2xgamma = B2XGammaBuilder(photons, topoPions, topoKaons, ks, pi0, hh, hhh, config['B2X'])
        self._makeLines(b2xgamma.lines, default_config)
        ###########################################################################
        # Lb -> X Gamma
        ###########################################################################
        lb2xgamma = Lb2XGammaBuilder(photons, hh, default_config['B2X'])
        self._makeLines(lb2xgamma.lines, default_config)

    def _makeLine(self,protoLine,default_config):
        tag = 'B2XGBBDTBeauty2XGammaFilter'
        for line in protoLine.selections:
            tmpSel = Selection(line.name() + 'FilterALL',
                               Algorithm=FilterDesktop(Code='ALL'),
                               RequiredSelections=[line])
            _filter = { 'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % default_config['GECNTrkMax'],
                        'Preambulo' : [ "from LoKiTracks.decorators import *",
                                        "from LoKiCore.functions    import *" ]
                      }
            l0 = "|".join( [ "L0_CHANNEL('%s')" % chan for chan in [ 'Electron','Photon','Hadron','Muon','DiMuon'] ] )
            hlt = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision') & HLT_PASS_RE('Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision')"
            name = line.name().replace(tag,'')+'Line'
            sline = StrippingLine(name,
                                  protoLine.prescale(line, name, default_config),
                                  selection=tmpSel,
                                  checkPV=True,
                                  FILTER=_filter,
                                  #L0DU = l0,
                                  #HLT=hlt
                                  )
            self.registerLine(sline)

    def _makeLines(self, lines, default_config):
        for line in lines:
            self._makeLine(line, default_config)

# EOF
