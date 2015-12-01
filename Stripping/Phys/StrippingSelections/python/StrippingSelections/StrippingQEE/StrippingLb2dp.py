'''
Stripping selection of Lb -> d p (pi pi) 
   Lb -> dp
   Lb -> dppipi
'''

__author__ = ['Paula Alvarez']
__date__ = '02/11/2015'
__version__ = 'v1r0'

__all__ = ('Lb2dpConf',
           'default_config'
           )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays

from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsProtons,  StdAllNoPIDsKaons
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter as VoidFilter

default_config = {
    'NAME' : 'Lb2dp',
    'WGs'  : ['QEE'],
    'BUILDERTYPE' : 'Lb2dpConf',
    'CONFIG' : {
    'Prescale'             : 1.0 ,      
    'Prescale_pipi'        : 1.0 ,

    'TrackChi2Ndof'        : 3.0,
    'TrackGhostProb'       : 0.5,
    'TrackIPChi2'          : 16.,

    'PionPT'               : 500,
    'PionPIDKpi'          : 2,
    # 'PionProbNNpi'         : 0.05,
    
    'ProtonPT'             : 1000,
    'ProtonP'              : 15000,
    'ProtonPIDppi'         : 10,
    'ProtonPIDpK'          : 10,

    'KaonPT'               : 1000,
    'KaonP'                : 25000,
    
    'SumPT'                : 3000,

    'LbMassMin'            : 5200. ,
    'LbMassMax'            : 6000. ,
    'LbVtxChi2'            : 10. ,
    'LbVtxChi2_pipi'       : 10. ,
    'LbDIRA'               : 0.9999,
    'LbFDChi2'             : 150,
    'LbPT'                 : 1500

    },
    'STREAMS' : {'BhadronCompleteEvent' : ['StrippingLb2dp_Line' ,
                                           'StrippingLb2dp_pipiLine']}
    }



default_name = "Lb2dp"

class Lb2dpConf(LineBuilder) :
    __configuration_keys__ = (
        'Prescale',   
        'Prescale_pipi',
        'TrackChi2Ndof',
        'TrackGhostProb',
        'TrackIPChi2',
        'PionPT',
        # 'PionProbNNpi',
        'PionPIDKpi',
        'ProtonPT',
        'ProtonP',
        'ProtonPIDppi',
        'ProtonPIDpK',
        'KaonPT',
        'KaonP',
        'LbMassMin',
        'LbMassMax',
        'LbVtxChi2' ,
        'LbVtxChi2_pipi' ,
        'LbDIRA',        
        'LbFDChi2',
        'LbPT',
        'SumPT'
        )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.name = name
        self.config = config


        _trackCuts = "((TRCHI2DOF< %(TrackChi2Ndof)s) & (TRGHOSTPROB< %(TrackGhostProb)s) & (MIPCHI2DV(PRIMARY)> %(TrackIPChi2)s)"% self.config
        _pionCuts = _trackCuts
        # _pionCuts += " & (PROBNNpi> %(PionProbNNpi)s ) & (PT> %(PionPT)s *MeV))"% self.config
        _pionCuts += " & ((PIDK - PIDpi)< %(PionPIDKpi)s ) & (PT> %(PionPT)s *MeV))"% self.config

        _kaonCuts = _trackCuts
        _kaonCuts += " & (P> %(KaonP)s *MeV)) & (PT> %(KaonPT)s *MeV)"% self.config

        _protonCuts = _trackCuts
        _protonCuts += " & ((PIDp-PIDpi)> %(ProtonPIDppi)s) & ((PIDp-PIDK)> %(ProtonPIDpK)s) & (P> %(ProtonP)s *MeV)) & (PT> %(ProtonPT)s *MeV)"%self.config



        self.PionForLb2Dp = self.createSubSel( OutputList = "PionFor" + self.name,
                                                InputList = DataOnDemand(Location = 'Phys/StdAllLoosePions/Particles'),
                                                Cuts = _pionCuts)

        self.ProtonForLb2Dp = self.createSubSel( OutputList = "ProtonFor" + self.name,
                                                  InputList = DataOnDemand(Location = 'Phys/StdAllNoPIDsProtons/Particles'),
                                                  Cuts = _protonCuts)

        self.DeuteronForLb2Dp = self.createSubSel( OutputList = "KaonFor" + self.name,
                                                   InputList = DataOnDemand(Location = 'Phys/StdAllNoPIDsKaons/Particles'),
                                                   Cuts = _kaonCuts)
        
        self.makeLb2Dp()
        self.makeLb2Dppipi()
        
    def createSubSel( self, OutputList, InputList, Cuts ) :
        '''create a selection using a FilterDesktop'''
        filter = FilterDesktop(Code = Cuts)
        return Selection( OutputList,
                          Algorithm = filter,
                          RequiredSelections = [ InputList ] )
    

    def makeLb2Dp(self):

        _decayDescriptors = ["[Lambda_b0 -> K+ p~-]cc"]

        # _daughtersCuts = {  "K+"  : "ALL" % self.config
        #                     ,"p~-"  : "ALL" % self.config }

        # _combinationCut = "(AM > %(LbMassMin)s *MeV)"\
        #                   "& ((ACHILD(PT,1)+ACHILD(PT,2)) > %(SumPT)s *MeV)"% self.config

        _combinationCut = "((ACHILD(PT,1)+ACHILD(PT,2)) > %(SumPT)s *MeV)"% self.config

        _motherCut = "(BPVDIRA > %(LbDIRA)s) & (VFASPF(VCHI2/VDOF) < %(LbVtxChi2)s) & (BPVVDCHI2> %(LbFDChi2)s)  & (PT> %(LbPT)s)"\
                     " & (M_Lb> %(LbMassMin)s) & (M_Lb< %(LbMassMax)s)"% self.config


        _preambulo = [ "from LoKiPhys.decorators import *",
                       "m_deut  = 1.875613*1000",
                       "P_deut = MINTREE(ABSID=='K+',P)",
                       "E_deut = sqrt(m_deut*m_deut + P_deut*P_deut)",
                       "Px_Lb   = (CHILD(PX,1)+CHILD(PX,2))",
                       "Py_Lb   = (CHILD(PY,1)+CHILD(PY,2))",
                       "Pz_Lb   = (CHILD(PZ,1)+CHILD(PZ,2))",
                       "P2_Lb = Px_Lb*Px_Lb + Py_Lb*Py_Lb + Pz_Lb*Pz_Lb",
                       "E_Lb = E_deut + MINTREE(ABSID=='p+',E)",
                       "M_Lb = sqrt(E_Lb*E_Lb - P2_Lb)"]
        
        _Lb2Dp = CombineParticles( DecayDescriptors = _decayDescriptors,
                                   # DaughtersCuts = _daughtersCuts,
                                   CombinationCut = _combinationCut,
                                   Preambulo = _preambulo,
                                   MotherCut = _motherCut)                            


        _Lb2Dp_sel = Selection( self.name,
                                Algorithm = _Lb2Dp,
                                RequiredSelections = [self.DeuteronForLb2Dp, self.ProtonForLb2Dp])

        Lb2DpLine = StrippingLine( self.name + '_Line',
                                   prescale = self.config['Prescale'],
                                   selection =  _Lb2Dp_sel ,
                                   RequiredRawEvents = ["Rich","Velo","Tracker","Trigger","Muon","Calo"])
        
        self.registerLine(Lb2DpLine)


        
    def makeLb2Dppipi(self):

        _decayDescriptors = ["[Lambda_b0 -> K+ p~- pi+ pi-]cc"]

        # _daughtersCuts = {  "K+"  : "ALL" % self.config
        #                     ,"p~-"  : "ALL" % self.config
        #                     ,"pi+" : "ALL" % self.config }

        _combinationCut = "(AM > %(LbMassMin)s *MeV)"\
                          "& ((ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) + ACHILD(PT,4)) > %(SumPT)s *MeV)"% self.config
        
        _motherCut = "(BPVDIRA > %(LbDIRA)s) & (VFASPF(VCHI2/VDOF) < %(LbVtxChi2_pipi)s) & (BPVVDCHI2> %(LbFDChi2)s) & (PT> %(LbPT)s)"% self.config

        _preambulo = [ "from LoKiPhys.decorators import *",
                       "m_deut  = 1.875613*1000",
                       "P_deut = MINTREE(ABSID=='K+',P)",
                       "E_deut = sqrt(m_deut*m_deut + P_deut*P_deut)",
                       "Px_Lb   = (CHILD(PX,1)+CHILD(PX,2)+CHILD(PX,3)+CHILD(PX,4))",
                       "Py_Lb   = (CHILD(PY,1)+CHILD(PY,2)+CHILD(PY,3)+CHILD(PY,4))",
                       "Pz_Lb   = (CHILD(PZ,1)+CHILD(PZ,2)+CHILD(PZ,3)+CHILD(PZ,4))",
                       "P2_Lb = Px_Lb*Px_Lb + Py_Lb*Py_Lb + Pz_Lb*Pz_Lb",
                       "E_Lb = E_deut + MINTREE(ABSID=='p+',E) + MINTREE(ID=='pi+',E) + MINTREE(ID=='pi-',E)",
                       "M_Lb = sqrt(E_Lb*E_Lb - P2_Lb)"]

        _Lb2Dp = CombineParticles( DecayDescriptors = _decayDescriptors,
                                   # DaughtersCuts = _daughtersCuts,
                                   CombinationCut = _combinationCut,
                                   Preambulo = _preambulo,
                                   MotherCut = _motherCut)                            


        _Lb2Dp_sel = Selection( self.name+"pipi",
                                Algorithm = _Lb2Dp,
                                RequiredSelections = [self.DeuteronForLb2Dp, self.ProtonForLb2Dp, self.PionForLb2Dp])

        Lb2DpLine = StrippingLine( self.name + '_pipiLine',
                                   prescale = self.config['Prescale'],
                                   selection =  _Lb2Dp_sel,
                                   RequiredRawEvents = ["Rich","Velo","Tracker","Trigger","Muon","Calo"])

        
        self.registerLine(Lb2DpLine)

       
       
       
       
