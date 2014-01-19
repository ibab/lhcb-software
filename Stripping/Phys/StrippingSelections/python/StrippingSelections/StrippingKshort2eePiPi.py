
__author__ = ['Carla Marin']
__date__ = '17/01/2014'
__version__ = '$1.0 $'

'''
stripping code for Kshort --> eePiPi
derived from StrippingKshort2MuMuPiPi.py by Mike Sokoloff.
'''

config_params =  {'ePT'        : 100.  ,    #MeV
                  'eMINIPCHI2' : 50    ,    #adimensional
                  'eTRCHI2'    : 3     ,    #adimensional
                  'ePIDe'      : -4    ,    #adimensional
                  'eGhostProb' : 0.3   ,    #adimensional
                  
                  'PionPT'        : 100.  ,    #MeV
                  'PionMINIPCHI2' : 50    ,    #adimensional
                  'PionTRCHI2'    : 3     ,    #adimensional
                  'PionPIDK'      : 5     ,    #adimensional
                  'PionGhostProb' : 0.3   ,    #adimensional
                  
                  #4body         
                  'KsMAXDOCA'        : 0.3       , #mm
                  'KsLifetime'       : 0.01*89.53, #adimensional
                  'KsIP'             : 1         , #mm
                  'MaxKsMass'        : 800.  ,    #MeV, comb mass high limit
                  
                  'Kshort2PiPieeLinePrescale'    : 1 ,
                  'Kshort2PiPieeLinePostscale'   : 1
                  }

__all__ = ('Kshort2PiPieeConf',
           'config_params')


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdNoPIDsPions, StdNoPIDsElectrons
#from StandardParticles import StdAllLoosePions, StdAllLooseElectrons

default_name = "Ks2PiPiee"

class Kshort2PiPieeConf(LineBuilder) :
    """
    Builder for Kshort --> pi,pi,e,e 
    """
    
    PiPiLine = None

        
    __configuration_keys__ = (  'ePT'
                                , 'eMINIPCHI2'
                                , 'eTRCHI2'
                                , 'ePIDe'
                                , 'eGhostProb'
                                , 'PionPT'
                                , 'PionMINIPCHI2'
                                , 'PionTRCHI2'
                                , 'PionGhostProb'
                                , 'PionPIDK'

                                #4body        
                                , 'KsMAXDOCA'  
                                , 'KsLifetime'     
                                , 'KsIP'   
                                , 'MaxKsMass'
                                
                                , 'Kshort2PiPieeLinePrescale'
                                , 'Kshort2PiPieeLinePostscale'
                                )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        

        PiPiLine_name = name+"_PiPi"
        
        
        
        # 5 : Make e's for Kshort2PiPiee 
        selElecsForPiPiee = makeElecsForPiPiee(name="ElecsFor"+name
                                               , ePT = config['ePT']
                                               , eMINIPCHI2 = config['eMINIPCHI2']
                                               , eTRCHI2 = config['eTRCHI2']
                                               , ePIDe = config['ePIDe']
                                               , eGhostProb = config['eGhostProb']
                                               )

        # 7 : Make Pions for Kshort2PiPiee 
        selPionsForPiPiee = makePionsForPiPiee(name="PionsFor"+name
                             , PionMINIPCHI2 = config['PionMINIPCHI2']
                             , PionTRCHI2 = config['PionTRCHI2']
                             , PionPIDK = config['PionPIDK']
                             , PionGhostProb = config['PionGhostProb']
                             , PionPT = config['PionPT']
                             )


                                        
        # 7 : Combine
        
        selKshort2PiPiee = self._makeKshort2PiPiee(name=PiPiLine_name,
                                           pionSel = selPionsForPiPiee,
                                           elecSel = selElecsForPiPiee,
                                           config = config)
        
        # 8 : Declare Lines
        
        self.PiPiLine = StrippingLine(PiPiLine_name+"Line",
                                    prescale = config['Kshort2PiPieeLinePrescale'],
                                    postscale = config['Kshort2PiPieeLinePostscale'],
                                    selection = selKshort2PiPiee
                                    )
                
        # 9 : register Line

        self.registerLine( self.PiPiLine )


#####################################################
    def _makeKshort2PiPiee(self, name, pionSel, elecSel, config):
        """
        Handy interface for Kshort2PiPiee
        """
        return makeKshort2PiPiee(name
                               , pionSel
                               , elecSel
                               , KsMAXDOCA = config['KsMAXDOCA']
                               , KsLifetime= config['KsLifetime']
                               , KsIP = config['KsIP']
                               , MaxKsMass = config['MaxKsMass'])


#####################################################
def makeKshort2PiPiee(name, pionSel, elecSel, MaxKsMass, KsMAXDOCA, KsIP, KsLifetime):

    """
    Makes the KS0 -> pi+ pi- e+ e-
    """

    _combcut = "(AM < %(MaxKsMass)s *MeV) & "\
               "(AMAXDOCA('') < %(KsMAXDOCA)s *mm)" %locals()


    _mothercut = "(M < %(MaxKsMass)s *MeV) &"\
                 "(MIPDV(PRIMARY) < %(KsIP)s *mm) & "\
                 "((BPVVDSIGN*M/P) > %(KsLifetime)s*2.9979e-01)" %locals()


    _Combine = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi- e+ e-",
                                CombinationCut = _combcut,
                                MotherCut = _mothercut
                                )
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ elecSel, pionSel] )                    
                    
#####################################################
def makeElecsForPiPiee(name, eTRCHI2, ePT, eMINIPCHI2, ePIDe, eGhostProb):
    """
    Electron selection
    """
    _code ="(MIPCHI2DV(PRIMARY) > %(eMINIPCHI2)s) &"\
           "(PT > %(ePT)s *MeV) &"\
           "(TRCHI2DOF < %(eTRCHI2)s) &"\
           "(TRGHOSTPROB < %(eGhostProb)s) &"\
           "(PIDe > %(ePIDe)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdNoPIDsElectrons ] )

#####################################################
def makePionsForPiPiee(name, PionTRCHI2, PionMINIPCHI2, PionGhostProb, PionPIDK, PionPT):
    """
    Pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) &"\
            "(TRGHOSTPROB < %(PionGhostProb)s) &"\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s) &"\
            "(PIDK < %(PionPIDK)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdNoPIDsPions ] )
