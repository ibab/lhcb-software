
__author__ = ['Carla Marin']
__date__ = '21/08/2014'
__version__ = '$1.0 $'

'''
    stripping code for Kshort --> eePiPi
    derived from StrippingKshort2eePiPi.py from S20r0p3
'''

default_config =  {
    'NAME'        : 'Kshort2eePiPi',
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'Kshort2eePiPiConf',
    'CONFIG'      : {   'Kshort2eePiPiLinePrescale'    : 1 ,
                        'Kshort2eePiPiLinePostscale'   : 1 ,
                        
                        'ePT'           : 100.          , #MeV
                        'eMINIPCHI2'    : 16            , #adimensional
                        'ePIDe'         : -4            , #adimensional
                        'eGhostProb'    : 0.5           , #adimensional

                        'PionPT'        : 100           , #MeV
                        'PionMINIPCHI2' : 16            , #adimensional
                        'PionPIDK'      : 5             , #adimensional
                        'PionGhostProb' : 0.5           , #adimensional
    
                        #4body
                        'KsMAXDOCA'     : 1.            , #mm
                        'KsLifetime'    : 0.01*89.53    , #0.01*10^-12s
                        'KsIP'          : 1             , #mm
                        'MaxKsMass'     : 800.          , #MeV, comb mass high limit
                        'KsVtxChi2'     : 50            , 
                    },
    'STREAMS'     : ['Leptonic']
    }


__all__ = ('Kshort2eePiPiConf',
           'default_config'
           )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsElectrons
from StandardParticles import StdDiElectronFromTracks
#from StandardParticles import StdAllLoosePions, StdAllLooseElectrons

default_name = "Ks2eePiPi"

class Kshort2eePiPiConf(LineBuilder) :
    """
    Builder for Kshort --> pi,pi,e,e
    """
    
    PiPiLine = None
    
    
    __configuration_keys__ = ('ePT',
                              'eMINIPCHI2',
                              'ePIDe',
                              'eGhostProb',
                              'PionPT',
                              'PionMINIPCHI2',
                              'PionGhostProb',
                              'PionPIDK',
                              
                              #4body
                              'KsMAXDOCA',
                              'KsLifetime',
                              'KsIP',
                              'MaxKsMass',
                              
                              'Kshort2eePiPiLinePrescale',
                              'Kshort2eePiPiLinePostscale',
                              'KsVtxChi2',
                              )
                              
    def __init__(self, name, config):
        
        LineBuilder.__init__(self, name, config)
                                  
                                  
        PiPiLine_name = name+"_PiPi"
        
        # 1 : Make e's for Kshort2eePiPi
        selElecsForeePiPi = makeElecsForeePiPi(name   = "ElecsFor"+name,
                                               elecs  = StdAllNoPIDsElectrons,
                                               params = config,
                                               )
    
        selElecsFromTracksForeePiPi = makeElecsFromTracksForeePiPi(name   = "ElecsFromTracksFor"+name,
                                                                   elecs  = StdDiElectronFromTracks,
                                                                   params = config,
                                                                   )

        # 2 : Make Pions for Kshort2eePiPi
        selPionsForeePiPi = makePionsForeePiPi(name   = "PionsFor"+name,
                                               pions  = StdAllNoPIDsPions,
                                               params = config,
                                               )
            
        # 3 : Combine
        selKshort2eePiPi = self._makeKshort2eePiPi(name    = PiPiLine_name,
                                                   pionSel = selPionsForeePiPi,
                                                   elecSel = selElecsForeePiPi,
                                                   params  = config
                                                   )
                                                  
        selKshort2eePiPiFromTracks = self._makeKshort2eePiPiFromTracks(name    = PiPiLine_name+"_FromTracks",
                                                                       pionSel = selPionsForeePiPi,
                                                                       elecSel = selElecsFromTracksForeePiPi,
                                                                       params  = config
                                                                       )
                                                                                  
        # 4 : Declare Lines
        self.eePiPiLine = StrippingLine(PiPiLine_name+"Line",
                                        prescale  = config['Kshort2eePiPiLinePrescale'],
                                        postscale = config['Kshort2eePiPiLinePostscale'],
                                        selection = selKshort2eePiPi,
                                        MDSTFlag  = True
                                        )
        
        self.eePiPiFromTracksLine = StrippingLine(PiPiLine_name+"FromTracks"+"Line",
                                                  prescale  = config['Kshort2eePiPiLinePrescale'],
                                                  postscale = config['Kshort2eePiPiLinePostscale'],
                                                  selection = selKshort2eePiPiFromTracks,
                                                  MDSTFlag  = True
                                                  )
                                                                               
        # 5 : register Line
        self.registerLine( self.eePiPiLine )
        self.registerLine( self.eePiPiFromTracksLine )
    
#####################################################
    def _makeKshort2eePiPi(self, name, pionSel, elecSel, params):
        """
        Handy interface for Kshort2eePiPi
        """
        return makeKshort2eePiPi(name,
                                 pionSel,
                                 elecSel,
                                 params
                                 )

#####################################################
    def _makeKshort2eePiPiFromTracks(self, name, pionSel, elecSel, params):
        """
        Handy interface for Kshort2eePiPi
        """
        return makeKshort2eePiPiFromTracks(name,
                                           pionSel,
                                           elecSel,
                                           params
                                           )

#####################################################
def makeKshort2eePiPi(name, pionSel, elecSel, params):
    
    """
    Makes the KS0 -> pi+ pi- e+ e-
    """
    
    _combcut =      "(AM < %(MaxKsMass)s *MeV) & "\
                    "(AMAXDOCA('') < %(KsMAXDOCA)s *mm)" % params
    
    
    _mothercut =    "(M < %(MaxKsMass)s *MeV) &"\
                    "(MIPDV(PRIMARY) < %(KsIP)s *mm) & "\
                    "((BPVVDSIGN*M/P) > %(KsLifetime)s*2.9979e-01) & "\
                    "(VFASPF(VCHI2/VDOF) < %(KsVtxChi2)s) " % params
    
    _Combine = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi- e+ e-",
                                CombinationCut = _combcut,
                                MotherCut = _mothercut
                                )

    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ elecSel, pionSel]
                     )

#####################################################
def makeKshort2eePiPiFromTracks(name, pionSel, elecSel, params):
    
    """
    Makes the KS0 -> pi+ pi- (J/psi(1S) -> e+ e-)
    """
    
    _combcut =      "(AM < %(MaxKsMass)s *MeV) & "\
                    "(AMAXDOCA('') < %(KsMAXDOCA)s *mm)" % params
    
    
    _mothercut =    "(M < %(MaxKsMass)s *MeV) &"\
                    "(MIPDV(PRIMARY) < %(KsIP)s *mm) & "\
                    "((BPVVDSIGN*M/P) > %(KsLifetime)s*2.9979e-01) & "\
                    "(VFASPF(VCHI2/VDOF) < %(KsVtxChi2)s) " % params

    _Combine = CombineParticles(DecayDescriptor = "KS0 -> pi+ pi- J/psi(1S)",
                                CombinationCut = _combcut,
                                MotherCut = _mothercut
                                )

    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ elecSel, pionSel]
                     )

#####################################################
def makeElecsForeePiPi(name, elecs, params):
    """
    Electron selection from StdNoPIDsElectrons
    """
    
    _code = "(PT > %(ePT)s) &"\
            "(MIPCHI2DV(PRIMARY) > %(eMINIPCHI2)s) &"\
            "(TRGHOSTPROB < %(eGhostProb)s) &"\
            "(PIDe > %(ePIDe)s)" % params
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ elecs ]
                     )

#####################################################
def makeElecsFromTracksForeePiPi(name, elecs, params):
    """
    Electron selection from StdDiElectronFromTracks
    """

    _code = "(MINTREE(ABSID<14,PT) > %(ePT)s) &"\
            "(MINTREE(ABSID<14,MIPCHI2DV(PRIMARY)) > %(eMINIPCHI2)s) &"\
            "(MAXTREE(ABSID<14,TRGHOSTPROB) < %(eGhostProb)s) &"\
            "(MINTREE(ABSID<14,PIDe) > %(ePIDe)s)"  % params

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ elecs ]
                     )

#####################################################
def makePionsForeePiPi(name, pions, params):
    """
    Pion selection from StdNoPIDsPions
    """
    _code = "(PT > %(PionPT)s) &"\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s) &"\
            "(TRGHOSTPROB < %(PionGhostProb)s) &"\
            "(PIDK < %(PionPIDK)s)" % params
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ pions ]
                     )









