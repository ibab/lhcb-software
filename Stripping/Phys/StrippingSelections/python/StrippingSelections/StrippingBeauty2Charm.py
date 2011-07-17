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
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons, StdNoPIDsProtons
from Beauty2Charm_DBuilder import *
from Beauty2Charm_B2DXBuilder import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

# Default configuration dictionary
config = {
    "ALL" : {              # Cuts made on all input particles in all lines
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4
    },
    "D2X" : {              # Cuts made on all D's used in all lines 
    'ASUMPT_MIN'    : '1500*MeV',
    'AMAXDOCA_MAX'  : '1.0*mm',
    'VCHI2DOF_MAX'  : 30,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '100*MeV',
    '4H_ASUMPT_MIN' : '2000*MeV' 
    },
    "B2X" : {              # Cuts made on all B's used in all lines
    'AMAXDOCA_MAX'  : '1.0*mm',
    'SUMPT_MIN'     : '5000*MeV',
    'VCHI2DOF_MAX'  : 30,
    'BPVIPCHI2_MAX' : 25,
    'BPVLTIME_MIN'  : '0.3*ps',
    'BPVDIRA_MIN'   : 0.999,
    'MASS_WINDOW'   : '500*MeV'
    },
    "Dstar" : {            # Cuts made on all D*'s used in all lines 
    'AMAXDOCA_MAX'  : '1.0*mm',
    'VCHI2DOF_MAX'  : 30,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '50*MeV'
    },
    "Prescales" : {        # Prescales for individual lines
    "D2HH"         : 1.,
    "D2HHTIS"      : 1.,
    "D2HHH"        : 1.,
    "D2HHHTIS"     : 1., 
    "D2HHHWS"      : 0.1,
    "D2HHHH"       : 1,
    "D2HHHHTIS"    : 1
    },
    'GECNTrkMax'   : 500
    }

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Beauty2CharmConf(LineBuilder):
    __configuration_keys__ = ('ALL','D2X','B2X','Dstar','Prescales',
                              'GECNTrkMax')
 
    def __init__(self, moduleName, config) :
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory

        LineBuilder.__init__(self, moduleName, config)
        modules = CoreFactory('CoreFactory').Modules
        for i in ['LoKiTrigger.decorators']:
            if i not in modules : modules.append(i)

        # pre-filter all inputs (nothing is looser than this)
        pions = filterInputs('Pi',[StdNoPIDsPions],config['ALL'])
        kaons = filterInputs('K',[StdNoPIDsKaons],config['ALL'])

        # pre-filter hard inputs (these could have been used in HLT2)
        topoPions = topoInputs('Pi',[StdNoPIDsPions])
        topoKaons = topoInputs('K',[StdNoPIDsKaons])

        # make D->X, etc. inputs
        d = DBuilder(pions,config['D2X'])
        dst = DstarBuilder(d.hh,pions,config['Dstar'])

        # make B->DX
        b2dx = B2DXBuilder(d,dst,topoPions,topoKaons,config['B2X'])
        self._makeLines(b2dx.selections(),config)
        
    def _makeLine(self,sel,key,config):
        tmpSel = Selection(sel.name()+'FilterALL',
                           Algorithm=FilterDesktop(Code='ALL'),
                           RequiredSelections=[sel])
        filter = "TrSOURCE('Rec/Track/Best') >> TrLONG >> (TrSIZE < %s )" \
                 % config['GECNTrkMax']        
        line = StrippingLine(sel.name()+'Line',
                             prescale=config['Prescales'][key],
                             selection=tmpSel,
                             checkPV=True,FILTER=filter,
                             HLT="HLT_PASS_RE('Hlt2Topo.*Decision')")
        self.registerLine(line)

    def _makeLines(self,sels,config):
        for selDict in sels:
            self._makeLine(selDict['Selection'],selDict['PrescaleKey'],config)
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
            


