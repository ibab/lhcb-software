############################################
# Stripping Lines for                      #
#     Z->(Tau+ ->n  Prong nu_tau)          #
#        (Tau- ->n' Prong nu_tau)          #
# LFV Group                                #
#                                          #
# @author: Ch. Elsasser elsasser@cern.ch   #
############################################








from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter 
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLoosePions



__author__=['Christian Elsasser']
__date__='16/04/2012'
__version__='$Revision: 3.0 $'

config_params = {'Z2TauTau_Prong_LinePrescale'  : 1.0,
    'Z2TauTau_Prong_LinePostscale' : 1.0,
    ###
    'TAU_MASS_LOW'                 : '600.0',    # MeV/c2
    'TAU_MASS_HIGH'                : '1600.0',   # MeV/c2
    ###
    'Z_MASS_LOW'                   : '45000.0',  # MeV/c2
    #'Z_MASS_HIGH'                 : not set,    # MeV/c2
    #'VCHI2_Z_MAX'                 : not set,    # dl
    ###
    'PT_HAD_MIN'                   : '1500',     # MeV/c
    'TRACKCHI2_HAD_MAX'            : '10',       # dl
    ###
    'PT_TAU_MIN'                   : '8000',    # MeV/c
    'VCHI2_TAU_MAX'                : '20'        # dl
    #'FDCHI2_TAU_MIN'              : '0'         # dl
}


default_name = "Z02TauTauProng"


"""
    Z->(Tau->n Prong)(Tau->n Prong)
    """

__all__ = ('Z02TauTauProngConf')

class Z02TauTauProngConf(LineBuilder) :
    
    Z2TauTauLine            = None
    Z2TauTauSameSignLine    = None
    
    
    
    
    __configuration_keys__ = ('Z2TauTau_Prong_LinePrescale',
                              'Z2TauTau_Prong_LinePostscale',
                              ###
                              'TAU_MASS_LOW',
                              'TAU_MASS_HIGH',
                              ###
                              'Z_MASS_LOW',
                              #'Z_MASS_HIGH',
                              #'VCHI2_Z_MAX',
                              ###
                              'PT_HAD_MIN',
                              'TRACKCHI2_HAD_MAX',
                              ###
                              'PT_TAU_MIN',
                              'VCHI2_TAU_MAX'
                              #'FDCHI2_TAU_MIN'
                              )
    
    
    
    def __init__(self, name, config) :
        
        LineBuilder.__init__(self, name, config)
        self.name = name
        
        
        PVRange = True
        
        
        #self.__PVOutputLocation__                  = "Rec/Vertex/Primary" 
        
        ### DEFINE HADRONS
        selHad         = {name                  : self._createHadProng(    name   = "Hadron_"+self.name,
                                                                       config = config)
        }
        
        ### DEFINE TAUS
        selTau         = {name                  : self._createTauProng(    name   = "Tau_"+self.name,
                                                                       hadSel = selHad[self.name],
                                                                       config = config),
                name+"_SameSignTau"   : self._createTauProng_SameSign(    name   = "Tau_"+self.name+"_SameSignTau",
                                                                  hadSel = selHad[self.name],
                                                                  config = config),
            }
        
        
        
        
        
        ##### DEFINE LINES
        
        
        self.Z2TauTauLine             = StrippingLine(  name+"_Line",
                                                      prescale    = config['Z2TauTau_Prong_LinePrescale'],
                                                      postscale   = config['Z2TauTau_Prong_LinePostscale'],
                                                      checkPV     = PVRange,
                                                      selection   = self._createZ(name   = name,
                                                                                  tauSel = selTau[name],
                                                                                  config = config)
                                                      )
        
        self.Z2TauTauSameSignLine     = StrippingLine(  name+"_SameSign_Line",
                                                      prescale    = config['Z2TauTau_Prong_LinePrescale'],
                                                      postscale   = config['Z2TauTau_Prong_LinePostscale'],
                                                      checkPV     = PVRange,
                                                      selection   = self._createZ_SameSign(name   = name+"_SameSign",
                                                                                           tauSel = selTau[name],
                                                                                           config = config)
                                                      )
        
        self.Z2TauTauSameSignTauLine  = StrippingLine(  name+"_SameSignTau_Line",
                                                      prescale    = config['Z2TauTau_Prong_LinePrescale'],
                                                      postscale   = config['Z2TauTau_Prong_LinePostscale'],
                                                      checkPV     = PVRange,
                                                      selection   = self._createZ(name   = name+"_SameSignTau",
                                                                                  tauSel = selTau[name+"_SameSignTau"],
                                                                                  config = config)
                                                      )
        
        self.Z2TauTauSameSignAllLine  = StrippingLine(  name+"_SameSignAll_Line",
                                                      prescale    = config['Z2TauTau_Prong_LinePrescale'],
                                                      postscale   = config['Z2TauTau_Prong_LinePostscale'],
                                                      checkPV     = PVRange,
                                                      selection   = self._createZ_SameSign(name   = name+"_SameSignAll",
                                                                                           tauSel = selTau[name+"_SameSignTau"],
                                                                                           config = config)
                                                      )
        
        
        self.registerLine( self.Z2TauTauLine )
        self.registerLine( self.Z2TauTauSameSignLine )
        self.registerLine( self.Z2TauTauSameSignTauLine )
        self.registerLine( self.Z2TauTauSameSignAllLine )
    
    
    #print "Created Line"
    
    
    ### CREATING Z CANDIDATE
    def _createZ(self, name,tauSel,config) :
        """
        Z0 selection
        """
        
        
        _combcut        = "(AM > " + config['Z_MASS_LOW'] + "*MeV)"
        _Zcut           = "ALL"
        
        _CombineZ       = CombineParticles(  DecayDescriptors   = ["Z0 -> tau+ tau-"],
                                           CombinationCut       = _combcut,
                                           MotherCut            = _Zcut,
                                           WriteP2PVRelations   = False
                                           #Preambulo           = "",#_preambulo
                                           )
        _CombineZConf   = _CombineZ.configurable("Combine_"+name+"_Z")
        _CombineZConf.ParticleCombiners.update({'':'MomentumCombiner'}) 
        
        return Selection(   name+"_TauTau",
                         Algorithm            = _CombineZConf,
                         RequiredSelections   = [ tauSel ]
                         )
    
    
    def _createZ_SameSign(self,name,tauSel,config) :
        """
            Z0 selection Same Sign
            """
        
        
        _combcut        = "(AM > " + config['Z_MASS_LOW'] + "*MeV)"
        _Zcut           = "ALL"
        
        _CombineZ       = CombineParticles(  DecayDescriptors   = ['[Z0 -> tau+ tau+]cc'],
                                           CombinationCut       = _combcut,
                                           MotherCut            = _Zcut,
                                           WriteP2PVRelations   = False
                                           #Preambulo           = "",#_preambulo
                                           )
        _CombineZConf   = _CombineZ.configurable("Combine_"+name+"_Z")
        _CombineZConf.ParticleCombiners.update({'':'MomentumCombiner'}) 
        
        return Selection(   name+"_TauTau",
                         Algorithm            = _CombineZConf,
                         RequiredSelections   = [ tauSel ]
                         )
    
    
    
    ### CREATING PRONG TAU
    def _createTauProng(self,name,hadSel,config) :
        """
            Tau Prong selection
            """
        
        selectionCuts   = self._hadIntermediateKinematicCutsProng(config)
        
        _combcut        = selectionCuts[0] + " & "\
            "(AM > "                            + config['TAU_MASS_LOW']          + "*MeV)  & "\
            "(AM < "                            + config['TAU_MASS_HIGH']         + "*MeV)"#  & "\
        
        
        
        _taucut         = selectionCuts[1]
        
        
        _CombineTau     = CombineParticles(  DecayDescriptor           = "[tau+ -> pi+ pi+ pi-]cc",
                                           CombinationCut            = _combcut,
                                           MotherCut                 = _taucut
                                           )
        
        return Selection(   name            = name+"_TauProng",
                         Algorithm          = _CombineTau,
                         RequiredSelections = [hadSel]
                         )
    
    def _createTauProng_SameSign(self,name,hadSel,config) :
        """
        Tau Prong selection Same Sign
        """
        
        selectionCuts   = self._hadIntermediateKinematicCutsProng(config)
        
        _combcut        = selectionCuts[0] + " & "\
            "(AM > " + config['TAU_MASS_LOW']  + "*MeV)  & "\
            "(AM < " + config['TAU_MASS_HIGH'] + "*MeV)"#  & "\
        
        
        
        _taucut         = selectionCuts[1]
        
        
        _CombineTau     = CombineParticles(  DecayDescriptor = "[tau+ -> pi+ pi+ pi+]cc",
                                           CombinationCut    = _combcut,
                                           MotherCut         = _taucut
                                           )
        
        return Selection(   name            = name+"_TauProng",
                         Algorithm          = _CombineTau,
                         RequiredSelections = [hadSel]
                         )
    
    
    ### CREATING PRONG 
    def _createHadProng(self, name, config) :
        """
        Hadron selection Prong
        """
        selectionCuts   = self._hadFinalStateKinematicCutsProng(config)
        
        _filter         = FilterDesktop(Code = selectionCuts )
        
        return Selection(   name+"_HadProng",
                         Algorithm            = _filter,
                         RequiredSelections   = [ StdAllLoosePions ])
    
    
    
    ### KINEMATIC CUTS FOR TAU
    def _hadIntermediateKinematicCutsProng(self,config):
        _combcode = "(APT > "                + config['PT_TAU_MIN'] +"*MeV)"
        _ptvxcode = "(VFASPF(VCHI2PDOF) < "  + config['VCHI2_TAU_MAX'] +")"
        return [_combcode,_ptvxcode]
    
    ### KINEMATIC CUTS FOR PRONG
    def _hadFinalStateKinematicCutsProng(self,config):
        _code     = "(PT > " + config['PT_HAD_MIN'] +"*MeV) & "\
            "(TRCHI2DOF < "  + config['TRACKCHI2_HAD_MAX'] + ") & "\
            "~(ISMUON)"
        return _code 


### END OF FILE ###





