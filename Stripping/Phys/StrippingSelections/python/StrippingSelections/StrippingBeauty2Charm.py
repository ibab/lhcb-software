#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
'''
B->DX, Lb -> LcX, etc.

Author: M. Williams
'''
__author__ = ['Mike Williams']

moduleName = 'Beauty2Charm'

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons, StdNoPIDsProtons
from StandardParticles import StdLooseResolvedPi0,StdLooseMergedPi0
from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons, \
     StdAllNoPIDsProtons
from Beauty2Charm_DBuilder import *
from Beauty2Charm_DFilter import *
from Beauty2Charm_HHBuilder import *
from Beauty2Charm_HHHBuilder import *
from Beauty2Charm_B2DXBuilder import *
from Beauty2Charm_Lb2XBuilder import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

# Default configuration dictionary
config = {
    "ALL" : { # Cuts made on all charged input particles in all lines
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4
    },
    "KS0" : { # Cuts made on all K shorts
    'PT_MIN'        : '250*MeV',
    'MIPCHI2DV_MIN' : 4,
    'MM_MIN'        : '467.*MeV',
    'MM_MAX'        : '527.*MeV'
    },
    "Pi0" : { # Cuts made on all pi0's
    'PT_MIN'        : '500*MeV',
    'P_MIN'         : '2000*MeV',
    'CHILDCL1_MIN'  : 0.25,
    'CHILDCL2_MIN'  : 0.25
    },
    "D2X" : { # Cuts made on all D's used in all lines 
    'ASUMPT_MIN'    : '1800*MeV',
    'AMAXDOCA_MAX'  : '1.0*mm',
    'VCHI2DOF_MAX'  : 30,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '100*MeV'
    },
    "B2X" : { # Cuts made on all B's and Lb's used in all lines
    'AMAXDOCA_MAX'  : '1.0*mm',
    'SUMPT_MIN'     : '5000*MeV',
    'VCHI2DOF_MAX'  : 30,
    'BPVIPCHI2_MAX' : 25,
    'BPVLTIME_MIN'  : '0.3*ps',
    'BPVDIRA_MIN'   : 0.999,
    'AM_MIN'        : '4750*MeV', # Lb->X sets this to 5200*MeV
    'AM_MAX'        : '7000*MeV'
    },
    "Dstar" : { # Cuts made on all D*'s used in all lines 
    'AMAXDOCA_MAX'  : '1.0*mm',
    'VCHI2DOF_MAX'  : 30,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '50*MeV'
    },
    "HH": { # Cuts for rho, K*, phi, XHH Dalitz analyese, etc.
    'MASS_WINDOW'   : {'KST':'150*MeV','RHO':'150*MeV','PHI':'150*MeV'},
    'DAUGHTERS'     : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV'},
    'AMAXDOCA_MAX'  : '0.5*mm',
    'VCHI2DOF_MAX'  : 16,
    'BPVVDCHI2_MIN' : 16, 
    'BPVDIRA_MIN'   : 0,
    'ASUMPT_MIN'    : '1000*MeV',
    'pP_MIN'        : '5000*MeV' # for pH only (obviously)
    },
    "HHH": { # Cuts for PiPiPi, KPiPi analyese, etc.
    'MASS_WINDOW'   : {'A1':'3000*MeV','K1':'3000*MeV','PPH':'3600*MeV'},
    'KDAUGHTERS'    : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDK_MIN':'-5'},
    'PiDAUGHTERS'   : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDK_MAX':'10'},
    'pDAUGHTERS'    : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDp_MIN':'-5'},
    'AMAXDOCA_MAX'  : '0.40*mm',
    'VCHI2DOF_MAX'  : 8,
    'BPVVDCHI2_MIN' : 16, 
    'BPVDIRA_MIN'   : 0.98,
    'ASUMPT_MIN'    : '1250*MeV',
    'MIPCHI2DV_MIN' : 0.0,
    'BPVVDRHO_MIN'  : '0.1*mm',
    'BPVVDZ_MIN'    : '2.0*mm',
    'PTMIN1'       : '300*MeV'
    },
    "DTIGHT" : { # Tight Cuts on D mesons for B-->D+3H lines
    'MM_MIN'        : {'D':'1800*MeV','D0':'1790*MeV','Dst':'1950*MeV',
                       'Lc':'2216*MeV'},
    'MM_MAX'        : {'D':'2040*MeV','D0':'1940*MeV','Dst':'2050*MeV',
                       'Lc':'2356*MeV'},
    'VCHI2DOF_MAX'  : 8,
    'BPVVDCHI2_MIN' : 49,
    'MIPCHI2DV_MIN' : 0.0,
    'BPVDIRA_MIN'   : 0.98,
    'BPVVDRHO_MIN'  : '0.1*mm',
    'BPVVDZ_MIN'    : '0.0*mm',
    'piPIDK_MAX'    : 12,
    'pPIDp_MIN'    : -6,
    'KPIDK_MIN'    : -6
    },
    'LC2X' : { # Cuts for all Lambda_c's used in all lines
    'ASUMPT_MIN'    : '1800*MeV',
    'AMAXDOCA_MAX'  : '1.0*mm',
    'VCHI2DOF_MAX'  : 30,
    'BPVVDCHI2_MIN' : 36, 
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '100*MeV'
    },
    "Prescales" : { # Prescales for individual lines
    # Defaults are defined in, eg, Beauty2Charm_B2DXBuilder.py.  Put the full
    # line name here to override. E.g. 'B2D0HD2HHBeauty2CharmTOSLine':0.5.
    'B02DPiD2Pi0HHH_MergedBeauty2CharmTOSLine'   : 0.1,
    'B02DKD2Pi0HHH_MergedBeauty2CharmTOSLine'    : 0.1,
    'B02DPiD2Pi0HHH_MergedBeauty2CharmTISLine'   : 0.1,
    'B02DKD2Pi0HHH_MergedBeauty2CharmTISLine'    : 0.1,
    'B02DPiD2Pi0HHH_ResolvedBeauty2CharmTOSLine' : 0.1,
    'B02DKD2Pi0HHH_ResolvedBeauty2CharmTOSLine'  : 0.1,
    'B02DPiD2Pi0HHH_ResolvedBeauty2CharmTISLine' : 0.1,
    'B02DKD2Pi0HHH_ResolvedBeauty2CharmTISLine'  : 0.1
    },
    'GECNTrkMax'   : 500
    }

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Beauty2CharmConf(LineBuilder):
    __configuration_keys__ = ('ALL','KS0','Pi0','D2X','B2X','Dstar','HH','HHH',
                              'DTIGHT','LC2X','Prescales','GECNTrkMax')
 
    def __init__(self, moduleName, config) :
        
        LineBuilder.__init__(self, moduleName, config)

        # pre-filter all inputs (nothing is looser than this)
        pions = filterInputs('Pi',[StdAllNoPIDsPions],config['ALL'])
        kaons = filterInputs('K',[StdAllNoPIDsKaons],config['ALL'])
        protons = filterInputs('P',[StdAllNoPIDsProtons],config['ALL'])
        ks_dd = filterInputs('KS0_DD',[dataOnDemand("StdLooseKsDD")],
                                       config['KS0']) 
        ks_ll = filterInputs('KS0_LL',[dataOnDemand("StdLooseKsLL")],
                             config['KS0'])
        pi0_merged   = filterPi0s('Merged',[StdLooseMergedPi0],config['Pi0'])
        pi0_resolved = filterPi0s('Resolved',[StdLooseResolvedPi0],
                                  config['Pi0'])

        # pre-filter hard inputs (these could have been used in HLT2)
        topoPions = topoInputs('Pi',[pions])
        topoKaons = topoInputs('K',[kaons])
        topoProtons = topoInputs('P',[protons])
        
        # make D->X, etc. inputs
        d = DBuilder(pions, {"DD":[ks_dd],"LL":[ks_ll]}, 
                     {"Merged":[pi0_merged],"Resolved":[pi0_resolved]},
                     config['D2X'])
        dst = DstarBuilder(d.hh,pions,config['Dstar'])

        # X -> hh
        hh = HHBuilder(pions,kaons,protons,{"DD":[ks_dd],"LL":[ks_ll]}, 
                       {"Merged":[pi0_merged],"Resolved":[pi0_resolved]},
                       config['HH'])

        # X -> hhh
        hhh = HHHBuilder(pions,kaons,protons,config['HHH'])

        # Lc -> X
        lc = LcBuilder(pions,kaons,protons,config['LC2X'])

        # Filter D,D0,D* and Lc with tighter cuts
        df = DFilter(d,dst,lc,config['DTIGHT'])

        # make B->DX
        b2dx = B2DXBuilder(d,dst,topoPions,topoKaons,hh,df,hhh,config['B2X'])
        self._makeLines(b2dx.lines,config)

        # Lb -> X
        lb2x = Lb2XBuilder(lc,d,hh,topoPions,topoKaons,df,hhh,config['B2X'])
        self._makeLines(lb2x.lines,config)
        
    def _makeLine(self,protoLine,config):
        for line in protoLine.selections:
            tmpSel = Selection(line.name()+'FilterALL',
                               Algorithm=FilterDesktop(Code='ALL'),
                               RequiredSelections=[line])

            filter = {'Code' :
                      #"(TrSOURCE('Rec/Track/Best')) >> TrLONG >> "\
                      #"(TrSIZE < %s )" \
                      "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)"\
                      " < %s )" \
                      % config['GECNTrkMax'],
                      'Preambulo' : [ "from LoKiTracks.decorators import *",
                                      'from LoKiCore.functions    import *' ]
                      }
            hlt = "HLT_PASS('Hlt1TrackAllL0Decision') & "\
                  "(HLT_PASS_RE('Hlt2Topo(2|3|4)Body.*Decision') | "\
                  "HLT_PASS_RE('Hlt2IncPhi.*Decision'))"
            sline = StrippingLine(line.name()+'Line',
                                  protoLine.prescale(line,config),
                                  selection=tmpSel,checkPV=True,FILTER=filter,
                                  HLT=hlt)
            self.registerLine(sline)

    def _makeLines(self,lines,config):
        for line in lines: self._makeLine(line,config)
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
            


