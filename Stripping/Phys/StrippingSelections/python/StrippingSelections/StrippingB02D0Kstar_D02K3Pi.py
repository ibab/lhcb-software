# $Id: StrippingB02D0Kstar_D02K3Pi.py,v 1.1 2010-07-26 14:46:56 poluekt Exp $
'''
Module for construction of B0->D0K*0, D0->Kpipi0 stripping Selections and StrippingLines.
Provides functions to build B0, D0, K*0 selections.
Provides class B02D0Kstar_D02K3PiConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - B02D0Kstar_D02K3PiConf
   - makeB02D0Kstar
   - makeD02K3Pi
   - makeKstar2KPi
'''

__author__ = 'Aurelien Martens'
__date__ = '23/06/2010'
__version__ = '$Revision: 1.1 $'

__all__ = ('B02D0Kstar_D02K3PiConf',
           'makeB02D0Kstar',
           'makeD02K3Pi',
           'makeKstar2KPi')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig

name = "B02D0Kstar_D02K3Pi"

class B02D0Kstar_D02K3PiConf(object) :
    """
    Builder of B0->D0K*0, D0->Kpipi0 stripping Selection and StrippingLine.
    Constructs  B0->D0K*0, D0->Kpipi0 Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bConf = B02D0Kstar_D02K3PiConf('B02D0Kstar_D02K3PiTest',config)
    >>> bLines = bConf.lines
    >>> for line in bLines :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selD02K3Pi           : nominal D0->Kpipipi Selection object
    selKstar2KPi         : nominal Kstar->Kpi Selection object
    selB02D0Kstar        : nominal B0->D0K*0 Selection object
    B02D0Kstar_D02K3PiLine : StrippingLine made out of selB02D0Kstar
    lines                  : List of lines, [line]

    Exports as class data member:
    B02D0Kstar_D02K3PiConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('DauTrackChi2Max',
                              'KstarDauMIPChi2Min',
                              'DDauMIPChi2Min',
                              'KstarVtxChi2Max',
                              'KstarMIPChi2Min',
                              'DVtxChi2Max',
                              'DVDChi2Min',
                              'BVtxChi2Max',
                              'BMIPChi2Max',
                              'BVDChi2Min',
                              'KstarDauPTMin',
                              'KstarDauPMin',
                              'DDauPTMin',
                              'DDauPMin',
                              'DPTMin',
                              'KstarPTMin',
                              'BDIRAMin',
                              'KstarMassMin',
                              'KstarMassMax',
                              'KstarCombMassMin',
                              'KstarCombMassMax',
                              'DMassMin',
                              'DMassMax',
                              'DCombMassMin',
                              'DCombMassMax',
                              'BMassMin',
                              'BMassMax',
                              'BCombMassMin',
                              'BCombMassMax',
                              'LinePrescale',
                              'LinePostscale'
                              )
    
    def __init__(self, 
                 name = 'B02D0Kstar_D02K3Pi',
                 config = {'DauTrackChi2Max'          : 10.      
                           , 'KstarDauMIPChi2Min'     : 6.25   
                           , 'DDauMIPChi2Min'         : 4.     
                           , 'KstarVtxChi2Max'        : 12.    
                           , 'KstarMIPChi2Min'        : 4.     
                           , 'DVtxChi2Max'            : 12.    
                           , 'DVDChi2Min'             : 36.    
                           , 'BVtxChi2Max'            : 10.    
                           , 'BMIPChi2Max'            : 12.25  
                           , 'BVDChi2Min'             : 100.   
                           , 'KstarDauPTMin'          : 300.   
                           , 'KstarDauPMin'           : 2000.  
                           , 'DDauPTMin'              : 150.   
                           , 'DDauPMin'               : 2000. 
                           , 'DPTMin'                 : 2000. 
                           , 'KstarPTMin'             : 1000.  
                           , 'BDIRAMin'               : 0.9997 
                           , 'KstarMassMin'           : 745.   
                           , 'KstarMassMax'           : 1046.  
                           , 'KstarCombMassMin'       : 645.   
                           , 'KstarCombMassMax'       : 1146.  
                           , 'DMassMin'               : 1768.  
                           , 'DMassMax'               : 2068.  
                           , 'DCombMassMin'           : 1668.  
                           , 'DCombMassMax'           : 2168.  
                           , 'BMassMin'               : 4800.  
                           , 'BMassMax'               : 5900.  
                           , 'BCombMassMin'           : 4700.  
                           , 'BCombMassMax'           : 6000.
                           , 'LinePrescale'   : 1.
                           , 'LinePostscale'  : 1.
                           }) :

        checkConfig(B02D0Kstar_D02K3PiConf.__configuration_keys__,
                    config)

        self.selKstar2KPi = makeKstar2KPi('KstarFor'+name,
                                          DauTrackChi2Max = config['DauTrackChi2Max'],
                                          KstarDauMIPChi2Min = config['KstarDauMIPChi2Min'],
                                          KstarDauPTMin = config['KstarDauPTMin'],
                                          KstarDauPMin = config['KstarDauPMin'],
                                          KstarPTMin = config['KstarPTMin'],
                                          KstarVtxChi2Max = config['KstarVtxChi2Max'],
                                          KstarMIPChi2Min = config['KstarMIPChi2Min'],
                                          KstarCombMassMin = config['KstarCombMassMin'],
                                          KstarCombMassMax = config['KstarCombMassMax'],
                                          KstarMassMin = config['KstarMassMin'],
                                          KstarMassMax = config['KstarMassMax'])

        self.selD02K3Pi = makeD02K3Pi('D0For'+name,
                                      DauTrackChi2Max = config['DauTrackChi2Max'],
                                      DDauMIPChi2Min = config['DDauMIPChi2Min'],
                                      DDauPTMin = config['DDauPTMin'],
                                      DDauPMin = config['DDauPMin'],
                                      DPTMin = config['DPTMin'],
                                      DVtxChi2Max = config['DVtxChi2Max'],
                                      DVDChi2Min = config['DVDChi2Min'],
                                      DCombMassMin = config['DCombMassMin'],
                                      DCombMassMax = config['DCombMassMax'],
                                      DMassMin = config['DMassMin'],
                                      DMassMax = config['DMassMax'])
        
        self.selB02D0Kstar = makeB02D0Kstar(name,  
                                            kstarSel = self.selKstar2KPi, 
                                            d0Sel = self.selD02K3Pi,
                                            BVtxChi2Max = config['BVtxChi2Max'],
                                            BMIPChi2Max = config['BMIPChi2Max'],
                                            BVDChi2Min = config['BVDChi2Min'],
                                            BDIRAMin = config['BDIRAMin'], 
                                            BCombMassMin = config['BCombMassMin'],
                                            BCombMassMax = config['BCombMassMax'],
                                            BMassMin = config['BMassMin'],
                                            BMassMax = config['BMassMax'])

        self.line = StrippingLine(name+"Line",
                                  prescale = config['LinePrescale'],
                                  postscale = config['LinePostscale'],
                                  algos = [ self.selB02D0Kstar ])
        
        self.lines = [self.line]

       
def makeKstar2KPi(name,
                  DauTrackChi2Max,
                  KstarDauMIPChi2Min,
                  KstarDauPTMin,
                  KstarDauPMin,
                  KstarPTMin,
                  KstarVtxChi2Max,
                  KstarMIPChi2Min,
                  KstarCombMassMin,
                  KstarCombMassMax,
                  KstarMassMin,
                  KstarMassMax) :
    """
    Create and return a Kstar->KPi Selection object.
    Starts from DataOnDemand 'Phys/StdNoPiDsKaons' and 'Phys/StdNoPIDsPions'.
    Arguments:
    name                  : name of the Selection.
    DauTrackChi2Max       : Maximum chi2 for Kstar daughter track.     
    KstarDauMIPChi2Min    : Minimum impact parameter (wrt PV) chi2 for Kstar daughter track.
    KstarDauPTMin         : Minimum transverse momentum for Kstar daughter track (MeV).
    KstarDauPMin          : Minimum momentum for Kstar daughter track (MeV).
    KstarPTMin            : Minimum transverse momentum for Kstar (MeV).
    KstarVtxChi2Max       : Maximum chi2 for Kstar vertex fit.
    KstarMIPChi2Min       : Maximum chi2 for impact parameter (wrt PV) of Kstar.
    KstarCombMassMin      : Minimum mass for the Kstar combination of daughters (MeV). 
    KstarCombMassMax      : Maximum mass for the Kstar combination of daughters (MeV).
    KstarMassMin          : Minimum mass for the Kstar (MeV).
    KstarMassMax          : Maximum mass for the Kstar (MeV).
    """ 

    _Kaon = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
    _Pion = DataOnDemand(Location = "Phys/StdNoPIDsPions")
    
    _KstarDauKineCut = "(PT> %(KstarDauPTMin)s *MeV) & (P> %(KstarDauPMin)s *MeV)" % locals()
    _KstarDauChi2Cut = "(MIPCHI2DV(PRIMARY)> %(KstarDauMIPChi2Min)s ) & (TRCHI2DOF< %(DauTrackChi2Max)s ) " % locals()
    _KstarDauCut = "( " + _KstarDauKineCut + " & " + _KstarDauChi2Cut + " )"
    
    _KstarCombCut = "(AM> %(KstarCombMassMin)s *MeV) & (AM< %(KstarCombMassMax)s *MeV) & (APT> %(KstarPTMin)s *MeV)" % locals()
    _KstarCut = "(M> %(KstarMassMin)s *MeV) & (M< %(KstarMassMax)s *MeV) & (PT> %(KstarPTMin)s ) & ((VFASPF(VCHI2/VDOF)< %(KstarVtxChi2Max)s ) & (MIPCHI2DV(PRIMARY)> %(KstarMIPChi2Min)s ))" % locals()
    
    _Kstar2KPi = CombineParticles("_combineFor"+name)
    _Kstar2KPi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
    _Kstar2KPi.DaughtersCuts =  { "K+" : _KstarDauCut,"pi+": _KstarDauCut }
    _Kstar2KPi.CombinationCut = _KstarCombCut
    _Kstar2KPi.MotherCut = _KstarCut

    return Selection(name
                     ,Algorithm = _Kstar2KPi
                     ,RequiredSelections = [ _Kaon, _Pion ]
                     )


def makeD02K3Pi(name,
                DauTrackChi2Max,
                DDauMIPChi2Min,
                DDauPTMin,
                DDauPMin,
                DPTMin,
                DVtxChi2Max,
                DVDChi2Min,
                DCombMassMin,
                DCombMassMax,
                DMassMin,
                DMassMax) :
    """
     Create and return a D0->K3Pi Selection object.
    Starts from DataOnDemand 'Phys/StdNoPiDsKaons', 'Phys/StdNoPIDsPions'.
    Arguments:
    name                  : name of the Selection.
    DauTrackChi2Max       : Maximum chi2 for Kstar daughter track.     
    DDauMIPChi2Min        : Minimum impact parameter (wrt PV) chi2 for D0 daughter track.
    DDauPTMin             : Minimum transverse momentum for D0 daughter track (MeV).
    DDauPMin              : Minimum momentum for D0 daughter track (MeV).
    DPTMin                : Minimum transverse momentum for D0 (MeV).
    DVtxChi2Max           : Maximum chi2 for Kstar vertex fit.
    DVDChi2Min            : Minimum chi2 for decay vertex distance (wrt PV) of D0.
    DCombMassMin          : Minimum mass for the D0 combination of daughters (MeV). 
    DCombMassMax          : Maximum mass for the D0 combination of daughters (MeV).
    DMassMin              : Minimum mass for the D0 (MeV).
    DMassMax              : Maximum mass for the D0 (MeV).
    """      
    
    _Kaon = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
    _Pion = DataOnDemand(Location = "Phys/StdNoPIDsPions")
       
    _DDauKineCut = "(PT> %(DDauPTMin)s *MeV) & (P> %(DDauPMin)s *MeV)" % locals()
    _DDauChi2Cut = "(MIPCHI2DV(PRIMARY)> %(DDauMIPChi2Min)s ) & (TRCHI2DOF< %(DauTrackChi2Max)s )" % locals()
    _DDauCut = "( " + _DDauKineCut + " & " + _DDauChi2Cut + " )"
      
    _DCombCut = "(AM> %(DCombMassMin)s *MeV) & (AM< %(DCombMassMax)s *MeV) & (APT> %(DPTMin)s *MeV)" % locals()
    _DCut = "(M> %(DMassMin)s *MeV) & (M< %(DMassMax)s *MeV) & (PT> %(DPTMin)s *MeV) & (VFASPF(VCHI2/VDOF)< %(DVtxChi2Max)s ) & (BPVVDCHI2> %(DVDChi2Min)s )" % locals()
    
    _D0 = CombineParticles("_combineFor"+name)
    _D0.DecayDescriptors = ["[D0 -> K- pi+ pi- pi+]cc" ]
    _D0.DaughtersCuts = { "K-" : _DDauCut, "pi+": _DDauCut }
    _D0.CombinationCut = _DCombCut 
    _D0.MotherCut = _DCut
    
    return Selection(name
                     ,Algorithm = _D0
                     ,RequiredSelections = [ _Kaon, _Pion ]
                     )


def makeB02D0Kstar(name,
                   kstarSel,
                   d0Sel,
                   BVtxChi2Max,
                   BMIPChi2Max,
                   BVDChi2Min,
                   BDIRAMin, 
                   BCombMassMin,
                   BCombMassMax,
                   BMassMin,
                   BMassMax
                   ) :
    """
    Create and return a B0 -> D0 (Kpipi0) Kstar0 (Kpi) Selection object.
    Arguments:
    name            : name of the Selection.
    kstarSel        : Kstar0->K Pi Selection object.
    d0Sel           : D0 -> K pi pi pi Selection object.
    BVtxChi2Max     : Maximum chi2 for B0 vertex fit.
    BMIPChi2Max     : Maximum chi2 for impact parameter (wrt PV) of B0.
    BVDChi2Min      : Minimum chi2 for decay vertex distance (wrt PV) of B0.
    BDIRAMin        : Minimum cosine of angle between reconstructed vector momentum and flight distance vector of B0.
    BCombMassMin    : Minimum mass for the B0 combination of daughters (MeV). 
    BCombMassMax    : Maximum mass for the B0 combination of daughters (MeV).    
    BMassMin        : Minimum mass for the B0 (MeV).       
    BMassMax        : Maximum mass for the B0 (MeV).          
    """
    
    _BCombCut = "(AM > %(BCombMassMin)s *MeV) & (AM < %(BCombMassMax)s *MeV)" % locals()
    _BCut = "(M > %(BMassMin)s *MeV) & (M < %(BMassMax)s *MeV) & (BPVDIRA > %(BDIRAMin)s ) & (VFASPF(VCHI2/VDOF)< %(BVtxChi2Max)s )  & (BPVVDCHI2> %(BVDChi2Min)s ) & (MIPCHI2DV(PRIMARY)< %(BMIPChi2Max)s )" % locals()
    
    _B0 = CombineParticles("_"+name)
    _B0.DecayDescriptors  = ["[B0 -> D~0 K*(892)0]cc","[B0 -> D0 K*(892)0]cc" ] 
    _B0.CombinationCut = _BCombCut
    _B0.MotherCut = _BCut
  
    return Selection(name
                     ,Algorithm = _B0
                     ,RequiredSelections = [d0Sel,kstarSel]
                     )
