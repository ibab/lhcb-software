"""
Select a Lambda_c -> pKpi signal without a PID
requirement on the proton.
Lambda_c has a muon tag, i.e.,
Lambda_b0 -> Lambda_c- mu+.
"""
__author__ = ['Mika Vesterinen']
__date__ = '19/03/2013'
__version__ = '$Revision: 0.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLoosePions, StdLooseMuons, StdLooseKaons, StdLooseProtons, StdNoPIDsProtons

__all__ = ('CharmFromBSemiForProtonPIDAllLinesConf',
           'default_config')

default_config = {
    'CharmFromBSemiForProtonPID' : {
        'WGs'         : ['ALL'],
        'BUILDERTYPE' : 'CharmFromBSemiForProtonPIDAllLinesConf',
        'CONFIG'      : {
            "prescale"              : 1 # adimensional
            ,"GEC_nLongTrk"         : 250 # adimensional
            ,"MAXGHOSTPROB"         : 0.35 # adimensional
            ,"Mu_PT"                : 1000 # MeV
            ,"Pi_PIDKMax"           : -2 # adimensional
            ,"K_PIDKMin"            : 8 # adimensional
            ,"H_PT"                 : 300 # MeV
            ,"LambdaC_MASSWIN"      : 80 # MeV
            ,"LambdaC_PTSUM"        : 2000 # MeV
            ,"LambdaC_DOCACHI2_MAX" : 6 # adimensional
            ,"LambdaC_VCHI2NDF"     : 3 # adimensional
            ,"LambdaC_FDCHI2"       : 20 # adimensional
            ,"LambdaC_DIRA"         : 0.99 # adimensional
            ,"LambdaC_DZ"           : 0.5 # mm
            ,"LambdaB_MASS_MIN"     : 3000 # MeV
            ,"LambdaB_MASS_MAX"     : 5000 # MeV
            ,"LambdaB_DOCACHI2_MAX" : 6 # adimensional
            ,"LambdaB_DIRA"         : 0.999 # adimensional
            ,"LambdaB_VCHI2NDF"     : 3 # adimentional
            },
        "STREAMS":["PID"]
        }
    }
        
class CharmFromBSemiForProtonPIDAllLinesConf(LineBuilder) :
    
    __configuration_keys__ = (
        "prescale"       
        ,"GEC_nLongTrk"  
        ,"MAXGHOSTPROB"  
        ,"Mu_PT"         
        ,"Pi_PIDKMax"    
        ,"K_PIDKMin"     
        ,"H_PT"       
        ,"LambdaC_MASSWIN"    
        ,"LambdaC_PTSUM"      
        ,"LambdaC_DOCACHI2_MAX" 
        ,"LambdaC_VCHI2NDF"  
        ,"LambdaC_FDCHI2"    
        ,"LambdaC_DIRA"      
        ,"LambdaC_DZ"        
        ,"LambdaB_MASS_MIN"  
        ,"LambdaB_MASS_MAX"   
        ,"LambdaB_DOCACHI2_MAX"
        ,"LambdaB_DIRA"      
        ,"LambdaB_VCHI2NDF"  
        )
    
    __confdict__={}
        
    def __init__(self, _name, config) :

        LineBuilder.__init__(self, _name, config)
        self.__confdict__=config

        #### define the cuts for the Lambda_b0 daughter tracks
        self.MuonCuts = "(TRGHOSTPROB < %(MAXGHOSTPROB)s) & (PIDmu > 0) & (PT > %(Mu_PT)s *MeV)" % self.__confdict__
        self.PionCuts = "(TRGHOSTPROB < %(MAXGHOSTPROB)s) & (PIDK < %(Pi_PIDKMax)s) & (PT > %(H_PT)s *MeV)" % self.__confdict__ 
        self.KaonCuts = "(TRGHOSTPROB < %(MAXGHOSTPROB)s) & (PIDK > %(K_PIDKMin)s) & (PT > %(H_PT)s *MeV)" % self.__confdict__ 
        self.ProtonCuts = "(TRGHOSTPROB < %(MAXGHOSTPROB)s) & (PT > %(H_PT)s *MeV)" % self.__confdict__

        #### Define the combination cuts 
        self.LambdaC_CombinationCut = "(ADAMASS('Lambda_c+') < %(LambdaC_MASSWIN)s *MeV)"\
                              "& (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3) > %(LambdaC_PTSUM)s *MeV)"\
                              "& (ADOCACHI2CUT(%(LambdaC_DOCACHI2_MAX)s, ''))" % self.__confdict__

        self.LambdaC_MotherCut = "(ADMASS('Lambda_c+') < %(LambdaC_MASSWIN)s *MeV)"\
                                 "& (VFASPF(VCHI2/VDOF) < %(LambdaC_VCHI2NDF)s) " \
                                 "& (BPVVDCHI2 > %(LambdaC_FDCHI2)s)"\
                                 "& (BPVDIRA> %(LambdaC_DIRA)s)" % self.__confdict__ 

        self.LambdaB_CombinationCut = "(AM > %(LambdaB_MASS_MIN)s *MeV)"\
                                      "& (AM < %(LambdaB_MASS_MAX)s *MeV)"\
                                      "& (ADOCACHI2CUT(%(LambdaB_DOCACHI2_MAX)s, ''))" % self.__confdict__ 

        self.LambdaB_MotherCut = "( MINTREE((ABSID=='Lambda_c+'),VFASPF(VZ)) - VFASPF(VZ) > %(LambdaC_DZ)s *mm )"\
                                 "& (BPVDIRA> %(LambdaB_DIRA)s)"\
                                 "& (VFASPF(VCHI2/VDOF) < %(LambdaB_VCHI2NDF)s)" % self.__confdict__ 

        #### make the Lambda_c -> p K pi 
        
        self.comb_Lc2PKPi = CombineParticles(DecayDescriptors = ['[Lambda_c+ -> p+ K- pi+]cc'],
                                             DaughtersCuts = { "pi+" : self.PionCuts,
                                                               "K-"  : self.KaonCuts,
                                                               "p+"  : self.ProtonCuts},
                                             CombinationCut = self.LambdaC_CombinationCut,
                                             MotherCut = self.LambdaC_MotherCut)
        
        self.sel_Lc2PKPi = Selection(name="SelLc2PKPiFor"+_name,
                                     Algorithm = self.comb_Lc2PKPi,
                                     RequiredSelections = [StdLoosePions,StdLooseKaons,StdNoPIDsProtons])

        #### combine the Lambda_c -> p K pi with a muon
        
        self.comb_Lb2LcMuNu = CombineParticles(DecayDescriptor = '[Lambda_b0 -> Lambda_c+ mu-]cc',
                                               DaughtersCuts = {"mu-" : self.MuonCuts},
                                               CombinationCut = self.LambdaB_CombinationCut,
                                               MotherCut = self.LambdaB_MotherCut)
                                               
        self.sel_Lb2LcMuNu = Selection(name="sel_Lb2LcMuNu"+_name,
                                       Algorithm = self.comb_Lb2LcMuNu,
                                       RequiredSelections = [self.sel_Lc2PKPi,StdLooseMuons])

        #### define the GECs
        GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s)" % self.__confdict__ ,
                 "Preambulo": ["from LoKiTracks.decorators import *"]}

        #### make the line
        self.StrippingLine = StrippingLine(_name+'_LbToLcMuNuLcToPKPi',
                                           prescale = config['prescale'],
                                           FILTER=GECs,
                                           selection = self.sel_Lb2LcMuNu)
        self.registerLine(self.StrippingLine)

## the end
