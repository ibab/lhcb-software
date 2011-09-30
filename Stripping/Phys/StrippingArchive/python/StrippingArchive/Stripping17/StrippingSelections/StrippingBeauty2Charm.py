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
    'P_MIN'         : '1000*MeV',
    'CHILDCL1_MIN'  : 0.25,
    'CHILDCL2_MIN'  : 0.25,
    'FROM_B_P_MIN'  : '2000*MeV'
    },
    "D2X" : { # Cuts made on all D's and Lc's used in all lines 
    'ASUMPT_MIN'    : '1800*MeV',
    'AMAXDOCA_MAX'  : '0.5*mm',
    'VCHI2DOF_MAX'  : 10,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '100*MeV'
    },
    "B2X" : { # Cuts made on all B's and Lb's used in all lines
    'SUMPT_MIN'     : '5000*MeV',
    'VCHI2DOF_MAX'  : 10,
    'BPVIPCHI2_MAX' : 25,
    'BPVLTIME_MIN'  : '0.2*ps',
    'BPVDIRA_MIN'   : 0.999,
    'AM_MIN'        : '4750*MeV', # Lb->X sets this to 5200*MeV
    'AM_MAX'        : '7000*MeV', # B->Dh+-h0 sets this to 5800*MeV
    'B2CBBDT_MIN'   : 0.05
    },
    "Dstar" : { # Cuts made on all D*'s used in all lines 
    'AMAXDOCA_MAX'  : '0.5*mm',
    'VCHI2DOF_MAX'  : 10,
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
    'PID' : {
    'P'  : {'PIDp_MIN' : -10},
    'PI' : {'PIDK_MAX' : 20},
    'K'  : {'PIDK_MIN' : -10}
    },
    "Prescales" : { # Prescales for individual lines
    # Defaults are defined in, eg, Beauty2Charm_B2DXBuilder.py.  Put the full
    # line name here to override. E.g. 'B2D0HD2HHBeauty2CharmTOSLine':0.5.
    #'B2DDKBeauty2CharmLine' : 0.1,
    #'B02D0DKBeauty2CharmLine' : 0.1
    },
    'GECNTrkMax'   : 500
    }

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Beauty2CharmConf(LineBuilder):
    __configuration_keys__ = ('ALL','KS0','Pi0','D2X','B2X','Dstar','HH','HHH',
                              'PID','Prescales','GECNTrkMax')
 
    def __init__(self, moduleName, config) :
        
        LineBuilder.__init__(self, moduleName, config)

        # pre-filter inputs
        pions = filterInputs('Pi',[StdAllNoPIDsPions],config['ALL'])
        kaons = filterInputs('K',[StdAllNoPIDsKaons],config['ALL'])
        protons = filterInputs('P',[StdAllNoPIDsProtons],config['ALL'])
        ks_dd = filterInputs('KS0_DD',[dataOnDemand("StdLooseKsDD")],
                                       config['KS0']) 
        ks_ll = filterInputs('KS0_LL',[dataOnDemand("StdLooseKsLL")],
                             config['KS0'])
        ks = {"DD":[ks_dd],"LL":[ks_ll]}
        pi0_merged   = filterPi0s('Merged',[StdLooseMergedPi0],config['Pi0'])
        pi0_resolved = filterPi0s('Resolved',[StdLooseResolvedPi0],
                                  config['Pi0'])
        pi0 = {"Merged":[pi0_merged],"Resolved":[pi0_resolved]}
        pcut = 'P > %s' % config['Pi0']['FROM_B_P_MIN']
        pi0_fromB_merged = filterSelection('Pi0FromBMerged',pcut,[pi0_merged])
        pi0_fromB_resolved = filterSelection('Pi0FromBResolved',pcut,
                                             [pi0_resolved])
        pi0_fromB = {'Merged':[pi0_fromB_merged],
                     'Resolved':[pi0_fromB_resolved]}
        
        # pre-filter hard inputs (these could have been used in HLT2)
        topoPions = topoInputs('Pi',[pions])
        topoKaons = topoInputs('K',[kaons])
        topoProtons = topoInputs('P',[protons])
        
        # make D->X, etc. inputs
        d = DBuilder(pions,ks,pi0,config['D2X'],config['PID'])
        dst = DstarBuilder(d,pions,pi0,config['Dstar'],config['PID'])

        # X -> hh
        hh = HHBuilder(pions,kaons,protons,ks,pi0_fromB,config['HH'],
                       config['PID'])

        # X -> hhh
        hhh = HHHBuilder(pions,kaons,protons,config['HHH'])

        # Lc -> X
        lc = LcBuilder(pions,kaons,protons,config['D2X'],config['PID'])

        # make B->DX
        b2dx = B2DXBuilder(d,dst,topoPions,topoKaons,ks,pi0_fromB,hh,hhh,
                           config['B2X'])
        self._makeLines(b2dx.lines,config)

        # Lb -> X
        lb2x = Lb2XBuilder(lc,d,hh,topoPions,topoKaons,topoProtons,hhh,
                           config['B2X'])
        self._makeLines(lb2x.lines,config)
        
    def _makeLine(self,protoLine,config):
        tag = 'B2CBBDTBeauty2CharmFilter'
        for line in protoLine.selections:
            tmpSel = Selection(line.name()+'FilterALL',
                               Algorithm=FilterDesktop(Code='ALL'),
                               RequiredSelections=[line])
            filter = {'Code' :
                      "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)"\
                      " < %s )" \
                      % config['GECNTrkMax'],
                      'Preambulo' : [ "from LoKiTracks.decorators import *",
                                      'from LoKiCore.functions    import *' ]
                      }
            hlt = "HLT_PASS('Hlt1TrackAllL0Decision') & "\
                  "(HLT_PASS_RE('Hlt2Topo(2|3|4)Body.*Decision') | "\
                  "HLT_PASS_RE('Hlt2IncPhi.*Decision'))"
            name = line.name().replace(tag,'')+'Line'
            sline = StrippingLine(name,protoLine.prescale(line,name,config),
                                  selection=tmpSel,checkPV=True,FILTER=filter,
                                  HLT=hlt)
            self.registerLine(sline)

    def _makeLines(self,lines,config):
        for line in lines: self._makeLine(line,config)
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
            


