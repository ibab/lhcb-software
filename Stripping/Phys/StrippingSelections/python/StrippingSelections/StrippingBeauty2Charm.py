#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
'''
B->DX, Lb -> LcX, etc.

Author: M. Williams
'''
__author__ = ['Mike Williams']

moduleName = 'Beauty2Charm'

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

import re
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from PhysSelPython.Wrappers import EventSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter
#from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons, StdNoPIDsProtons
from StandardParticles import StdLooseResolvedPi0,StdLooseMergedPi0
from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons, \
     StdAllNoPIDsProtons, StdNoPIDsUpPions, StdLooseMuons
from Beauty2Charm_DBuilder import *
from Beauty2Charm_HHBuilder import *
from Beauty2Charm_HHHBuilder import *
from Beauty2Charm_B2DXBuilder import *
from Beauty2Charm_Lb2XBuilder import *
from Beauty2Charm_LTUnbiased import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

# Default configuration dictionary
config = {
    "ALL" : { # Cuts made on all charged input particles in all lines (excpt. upstream)
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4, 
    'TRGHP_MAX'     : 0.4
    },
    "UPSTREAM" : { # Cuts made on all upstream particles
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4
    },
    "KS0" : { # Cuts made on all K shorts
    'PT_MIN'        : '250*MeV',
    'BPVVDCHI2_MIN' : 36,
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
    'PTMIN1'       : '300*MeV',
    'PID'           : {'TIGHTERPI' : { 'P' : {'PIDp_MIN' : -10},
                                       'PI': {'PIDK_MAX' : 8},
                                       'K' : {'PIDK_MIN' : -10}},
                       'REALTIGHTK' : { 'P' : {'PIDp_MIN' : -10},
                                        'PI': {'PIDK_MAX' : 10},
                                        'K' : {'PIDK_MIN' : 4}}}
    },
    'PID' : {
    'P'  : {'PIDp_MIN' : -10},
    'PI' : {'PIDK_MAX' : 20},
    'K'  : {'PIDK_MIN' : -10},
    'TIGHT' : {    'P'  : {'PIDp_MIN' : -5},
                   'PI' : {'PIDK_MAX' : 10},
                   'K'  : {'PIDK_MIN' : -5}},
    'TIGHTER' : {    'P'  : {'PIDp_MIN' : 0},
                     'PI' : {'PIDK_MAX' : 10},
                     'K'  : {'PIDK_MIN' : 0}},
    'TIGHTPI' : { 'P' : {'PIDp_MIN' : -10},
                  'PI': {'PIDK_MAX' : 10},
                  'K' : {'PIDK_MIN' : -10}},
    'SPECIAL' : {    'P'  : {'PIDp_MIN' : -5},
                     'PI' : {'PIDK_MAX' : 5},
                     'K'  : {'PIDK_MIN' : 5}},
    'SPECIALPI': {'P' : {'PIDp_MIN' : -10},
                  'PI': {'PIDK_MAX' : 12},
                  'K' : {'PIDK_MIN' : -10}}
    },
    '2TOPO' : {'ANGLE_MIN': (2/57.),'M_MIN':19000,'DPHI_MIN':0},
    'BB' : {'ADDSUMPT':0,'COSANGLE_MAX':0.99,'COSDPHI_MAX':0,'M_MIN':0,'MAXPT_MIN': 4000},
    'D0INC' : {'PT_MIN' : 1000, 'IPCHI2_MIN': 100},
    "Prescales" : { # Prescales for individual lines
    'RUN_BY_DEFAULT' : True, # False = lines off by default
    'RUN_RE'         : ['.*'],  
    # Defaults are defined in, eg, Beauty2Charm_B2DXBuilder.py.  Put the full
    # line name here to override. E.g. 'B2D0HD2HHBeauty2CharmTOSLine':0.5.
    #'B2D0PiD2HHBeauty2CharmLine'      : 1.0,
    #'B2D0KD2HHBeauty2CharmLine'      : 1.0,
    #'B02DHHWSD2HHBeauty2CharmLine'       : 0.1,
    #'B2DPiPiD2HHHCFPIDBeauty2CharmLine'  : 1.0,
    #'B2DHHOSD2HHHCFPIDBeauty2CharmLine'  : 1.0,
    #'#B02DPiNoIPDs2HHHPIDBeauty2CharmLine': 1.0,
    #'Lb2XicPiXic2PKPiBeauty2CharmLine' 	 : 1.0,
    #'Lb2XicKXic2PKPiBeauty2CharmLine' 	 : 1.0,
    #'Lb2XicPiWSXic2PKPiBeauty2CharmLine' : 0.1,
    #'Lb2XicKWSXic2PKPiBeauty2CharmLine'  : 0.1,
    #'X2LcLcBeauty2CharmLine'    : 1.0,
    #'X2LcLcWSBeauty2CharmLine'  : 0.1,
    'B02DKLTUBD2HHHBeauty2CharmLine' : 0.04,
    #'B02D0D0Beauty2CharmLine'   : 1.0,
    #'B02DDWSBeauty2CharmLine'   : 1.0,
    #'B2D0DD02K3PiBeauty2CharmLine' : 1.0
    },
    'GECNTrkMax'   : 500
    }

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Beauty2CharmConf(LineBuilder):
    __configuration_keys__ = ('ALL','UPSTREAM','KS0','Pi0','D2X','B2X','Dstar','HH','HHH',
                              'PID','2TOPO','BB','D0INC','Prescales','GECNTrkMax')
 
    def __init__(self, moduleName, config) :
        
        LineBuilder.__init__(self, moduleName, config)

        # pre-filter inputs
        uppions = filterInputs('PiUP',[StdNoPIDsUpPions],config['UPSTREAM'])
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
        muons = filterInputs('MU',[StdLooseMuons],config['ALL']) # make muons (for D -> phi mu nu)

        # pre-filter hard inputs (these could have been used in HLT2)
        topoPions = topoInputs('Pi',[pions])
        topoKaons = topoInputs('K',[kaons])
        topoProtons = topoInputs('P',[protons])
        
        # make D->X, etc. inputs
        d = DBuilder(pions,kaons,ks,pi0,uppions,muons,config['D2X'],config['PID'])
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
        lb2x = Lb2XBuilder(lc,d,hh,topoPions,topoKaons,topoProtons,hhh,dst,
                           config['B2X'])
        self._makeLines(lb2x.lines,config)

        # Unbiased lines
        ltub = LTUnbiasedBuilder(d,config['B2X'])
        self._makeLines(ltub.lines,config)

        # UP D* lines
        for line in b2dx.lines:
            for sel in line.selections:
                if sel.name().find('WS') > 0: continue
                dstar_sel = makeB2DstarX(sel,uppions,config['B2X'])
                self._makeLine(ProtoLine([dstar_sel],line.pre),config)

        # Double Topo line
        from Configurables import DoubleTopoTool as DT
        code = "ACCEPT('DoubleTopoTool/DoubleTopoLine_DT')"
        alg = LoKi__VoidFilter('DoubleTopoLineFilter',Code=code)
        sel = EventSelection('DoubleTopoEventSel',Algorithm=alg)
        dt = DT('DoubleTopoLine_DT')
        dt.minAngle = config['2TOPO']['ANGLE_MIN']
        dt.minMass = config['2TOPO']['M_MIN']
        dt.minDPhi = config['2TOPO']['DPHI_MIN']
        hlt = "HLT_PASS_RE('Hlt2Topo.*Decision')"
        sline = StrippingLine('DoubleTopoLine',1.0,selection=sel,HLT=hlt)
        self.registerLine(sline)

        # pseudo double topo line
        sline = StrippingLine('PseudoDoubleTopoLine',1.0,
                              selection=makeDoubleTopo(topoKaons,config['BB']),
                              HLT=hlt)
        self.registerLine(sline)

        # B->D0X inclusive line
        cuts = "(PT > %d*MeV) & (ADMASS('D0') < 25*MeV) & (MIPCHI2DV(PRIMARY)>%d)" \
               % (config['D0INC']['PT_MIN'],config['D0INC']['IPCHI2_MIN'])
        sel = filterSelection('D02KPIPIDMTIGHT',cuts,d.kpi_pid_tight)
        sel = tosSelection(sel,{'Hlt2Topo2BodyBBDTDecision%TOS':0})
        hlt = "HLT_PASS_RE('Hlt2Topo2BodyBBDTDecision')"
        sline = StrippingLine('D02HHTopoTOSLine',1.0,selection=sel,HLT=hlt)
        self.registerLine(sline)



    def _makeLine(self,protoLine,config):
        tag = 'B2CBBDTBeauty2CharmFilter'
        default = config['Prescales']['RUN_BY_DEFAULT']
        run_re = config['Prescales']['RUN_RE']
        prescale_keys = config['Prescales'].keys()
        for line in protoLine.selections:
            name = line.name().replace(tag,'')+'Line'
            match = False
            for r in run_re:
                if re.match(r,name):
                    match = True
                    break
            if (not default) and (not match) and (not (name in prescale_keys)):
                continue
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
            #hlt = "HLT_PASS('Hlt1TrackAllL0Decision') & "\
            hlt = "(HLT_PASS_RE('Hlt2Topo.*Decision') | "\
                  "HLT_PASS_RE('Hlt2IncPhi.*Decision'))"
            sline = StrippingLine(name,protoLine.prescale(line,name,config),
                                  selection=tmpSel,checkPV=True,FILTER=filter,
                                  HLT=hlt)
            self.registerLine(sline)

    def _makeLines(self,lines,config):
        for line in lines: self._makeLine(line,config)
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
            


