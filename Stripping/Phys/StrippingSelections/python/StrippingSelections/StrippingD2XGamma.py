"""
Module for construction of D0-> phi gamma Stripping Selection and StrippingLine.
Provides class StrippingD2XGammaConf, which constructs the Selections and
StrippingLines given a configuration dictionary.
Exported selection makers: 'makePhoton', 'makePhi2KK', 'makeKstar', 'makeD02PhiGamma',
'makeBd2KstGamma'
Borrowed heavily from B2XGamma stripping
"""
__author__ = ['Fatima Soomro']
__date__ = '17/04/2012'
__version__ = '$Revision: 1.0 $'

__all__ = ('StrippingD2XGammaConf', 'makePhoton', 'makePhi2KK', 'makeKstar', 'makeBs2PhiGamma', 'makeBd2KstGamma')

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV, picosecond
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseAllPhotons

default_name = 'D2XGamma'
## default_confdict = {'TrChi2'              : 5.        # dimensionless
##                     ,'TrIPchi2Phi'         : 25.       # Dimensionless
##                     ,'kDLL'                : 2.0
##                     ,'TrPTPhi'             : 500. #MeV
##                     ## phi cuts
##                     ,'PhiMassWin'          : 50.       # MeV
##                     ,'PhiVCHI2'            : 16.       # dimensionless
##                     ## D0 cuts
##                     ,'D0MassWin'           : 200.     # MeV
##                     ,'D_BPVLTIME'          : 0.2 * picosecond ## not used ATM
##                     ,'D0_PT'               : 1000. #MeV
##                     ## photon
##                     ,'photonPT'            : 1600.     # MeV
##                     ## D*+ cuts
##                     ,'deltaMass'           : 160.0
##                     ,'DstVCHI2'            : 25.       # dimensionless
##                     ,'pvRefit'             : False
##                     # Pre- and postscales
##                     ,'D02PhiGammaPreScale'               : 1.0
##                     ,'D02PhiGammaPostScale'              : 1.0
##                     ,'PionLocation'   : "Phys/StdAllNoPIDsPions/Particles"
##                     ,'PhiLocation' : "Phys/StdLoosePhi2KK/Particles"
                    
##                     }
default_confdict = {
    'TrChi2'              : 5.
    ,'TrIPchi2Phi'        : 25.
    ,'TrPTPhi'            : 500.
    ,'kDLL'               : 2.0
    ,'PhiMassWin'         : 50.
    ,'PhiVCHI2'           : 16.
    ,'D0MassWin'          : 200.
    ,'D_BPVLTIME'         : 0.2 * picosecond ## not used ATM
    ,'D0_PT'              : 1000.
    ,'photonPT'           : 1700.
    ,'DstVCHI2'           : 25.
    ,'deltaMass'          : 160.0
    ,'pvRefit'             : False
    ,'D02PhiGammaPreScale'               : 1.0
    ,'D02PhiGammaPostScale'              : 1.0
    ,'PionLocation'   : "Phys/StdAllNoPIDsPions/Particles"
    ,'PhiLocation' : "Phys/StdLoosePhi2KK/Particles"
    }


class StrippingD2XGammaConf(LineBuilder):
    """
    Definition of D -> Phi Gamma stripping
    
    Constructs D0 -> Phi Gamma Selection and StrippingLine from
    a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> gammaConf = StrippingD2XGammaConf('StrippingD2XGammaTest',config)
    >>> gammaLines = gammaConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.
    Exports as class data member:
    StrippingD2XGammaConf.__configuration_keys__ : List of required configuration parameters.    
    """
    
 ##    Exports as instance data members:
##     selPhoton                    : Photon Selection object
##     selPhi2KK                    : nominal Phi -> K+K- Selection object
##     selKst                       : nominal K* -> K pi Selection object
##     selKstWide                   : K* -> K pi Selection object with wide K* mass window
##     selBs2PhiGammaWideBMass      : Bs -> Phi Gamma Selection object with wide Bs mass window
##     selBs2PhiGammaLooseDira      : Bs -> Phi Gamma Selection object with loose direction angle cut
##     selBs2PhiGamma               : nominal Bs -> Phi Gamma Selection object with wide Bs mass window
##     selBd2KstGammaWideBMass      : B0 -> K* Gamma Selection object with wide Bs mass window
##     selBd2KstGammaWideKstMass    : B0 -> K* Gamma Selection object with wide K* mass window
##     selBd2KstGammaLooseDira      : B0 -> K* Gamma Selection object with loose direction angle cut
##     selBd2KstGammaWide           : B0 -> K* Gamma Selection object with wide mass windows for K* and B
##     selBd2KstGamma               : nominal B0 -> K* Gamma object Object 
##     Bs2PhiGammaWideBMassLine     : Stripping line out of selBs2PhiGammaWideBMass
##     Bs2PhiGammaWideLooseDiraLine : Stripping line out of selBs2PhiGammaLooseDira
##     Bs2PhiGammaLine              : Stripping line out of selBs2PhiGamma
##     Bd2KstGammaWideBMassLine     : Stripping line out of selBd2KstGammaWideBMass
##     Bd2KstGammaWideKstMassLine   : Stripping line out of selBd2KstGammaWideKstMass
##     Bd2KstGammaLooseDiraLine     : Stripping line out of selBd2KstGammaLooseDira
##     Bd2KstGammaWide              : Stripping line out of selBd2KstGammaWide
##     Bd2KstGamma                  : Stripping line out of selBd2KstGamma
##     lines                  : List of lines
    
##     Exports as class data member:
##     StrippingB2XGammaConf.__configuration_keys__ : List of required configuration parameters.    
##     """
    __configuration_keys__ = ('TrChi2'
                              ,'TrIPchi2Phi'
                              ,'TrPTPhi'
                              ,'kDLL'
                              ,'PhiMassWin'
                              ,'PhiVCHI2'
                              ,'D0MassWin'
                              ,'D_BPVLTIME'
                              ,'D0_PT'
                              ,'photonPT'
                              ,'DstVCHI2'
                              ,'deltaMass'
                              ,'pvRefit'
                              # Pre- and postscales
                              ,'D02PhiGammaPreScale'
                              ,'D02PhiGammaPostScale'
                              ,'PionLocation'
                              ,'PhiLocation'
                              )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        # Selection of B daughters: photon, phi and kstar
        self.selPhoton = makePhoton('GammaFor%s' % name,
                                    config['photonPT'])
        self.selPhi2KK = makePhi2KK('PhiFor%s' % name,
                                    config['TrIPchi2Phi'],
                                    config['TrChi2'],
                                    config['kDLL'],
                                    config['TrPTPhi'],
                                    config['PhiMassWin'],
                                    config['PhiVCHI2'],
                                    config['PhiLocation'])

        self.selSlowPion= makeSlowPi('SlowPiFor%s' % name,
                                     config['TrChi2'],
                                     config['PionLocation'])

        self.selD0 = makeD02PhiGamma('D02PhiGammaFor%s' % name,
                                     self.selPhi2KK,
                                     self.selPhoton,
                                     config['D_BPVLTIME'],
                                     config['D0_PT'],
                                     config['D0MassWin'],
                                     config['pvRefit'])
        # D0->Phi Gamma selection
        self.selDst2D0Pi = makeDst2D0Pi('Dst2D0PiFor%s' % name,
                                        self.selSlowPion,
                                        self.selD0,
                                        config['deltaMass'],
                                        config['DstVCHI2'],
                                        config['pvRefit'])
        
        # Create and register stripping lines
        # D*+->D0 (Phi Gamma) pi+
        self.Dst2D0PiLine = StrippingLine("%sStripping_Dst2D0Pi" % name,
                                             prescale=config['D02PhiGammaPreScale'],
                                             postscale=config['D02PhiGammaPostScale'],
                                             selection=self.selDst2D0Pi)
        self.registerLine(self.Dst2D0PiLine)

def makePhoton(name, photonPT):
    """Create photon Selection object starting from DataOnDemand 'Phys/StdLooseAllPhotons'.

    @arg name: name of the Selection.
    @arg photonPT: PT of the photon
    
    @return: Selection object
    
    """
##     # Configure clusterization
##     from Configurables import CaloClusterizationTool, CellularAutomatonAlg
##     clust = CellularAutomatonAlg("EcalClust")
##     clust.addTool(CaloClusterizationTool,'CaloClusterizationTool')
##     clust.CaloClusterizationTool.ETcut = 300
##     clust.CaloClusterizationTool.withET = True
    # Prepare selection
    _code = "(PT> %(photonPT)s*MeV)" % locals()
    print 'making the photons: ', _code
    _gammaFilter = FilterDesktop(Code=_code)
    _stdGamma = StdLooseAllPhotons
    return Selection(name, Algorithm=_gammaFilter, RequiredSelections=[_stdGamma])

def makePhi2KK(name, TrIPchi2Phi, TrChi2, kDLL, TrPTPhi, PhiMassWin, PhiVCHI2, PhiLocation) :
    """
    Create and return a Phi->KK Selection object, starting from DataOnDemand 'Phys/StdLoosePhi2KK'.
    
    @arg name: name of the Selection.
    @arg TrIPchi2Phi: minimum IP chi2 of the K+ tracks
    @arg TrChi2: minimum chi2 of the K+ tracks
    @arg PhiMassWin: selected Phi mass window
    @arg PhiVCHI2: vertex chi2 of the Phi
    @arg kDLL: PIDK-PIDpi
    
    @return: Selection object
    
    """
    
    goodKaon = '( ((PIDK-PIDpi)>%(kDLL)s) & (MIPCHI2DV(PRIMARY) > %(TrIPchi2Phi)s) & (TRCHI2DOF < %(TrChi2)s) & (PT > %(TrPTPhi)s))' % locals()
    goodPhi  = "( (VFASPF(VCHI2/VDOF) < %(PhiVCHI2)s) & (ADMASS('phi(1020)') < %(PhiMassWin)s*MeV) )" % locals()
    _code = goodPhi+" & CHILDCUT( " + goodKaon + ", 1 ) & CHILDCUT( " + goodKaon + ", 2 )"
    _phiFilter = FilterDesktop(Code=_code)
    _stdPhi2KK = DataOnDemand(Location=PhiLocation)
    print 'making phi:', _code
    return Selection(name, Algorithm=_phiFilter, RequiredSelections=[_stdPhi2KK])

def makeSlowPi(name, TrChi2, PionLocation) :
    """
    Create Photons from DataOnDemand 'Phys/StdLoosePions'.
    
    @arg name: name of the Selection.
    @arg TrChi2: minimum chi2 of the tracks
    
    @return: Selection object
    
    """
    _code = '(TRCHI2DOF < %(TrChi2)s)'% locals()
    print 'making slowPion:', _code
    _slowpiFilter = FilterDesktop(Code=_code)
    _slowPi = DataOnDemand(Location=PionLocation)
    return Selection(name, Algorithm=_slowpiFilter, RequiredSelections=[_slowPi])


## def makeKstar(name, TrIPchi2Kst, TrChi2, KstMassWin, KstVCHI2) :
##     """
##     Create and return a K*->Kpi Selection object, starting from DataOnDemand 'Phys/StdVeryLooseDetachedKst2Kpi'.
    
##     @arg name: name of the Selection.
##     @arg TrIPchi2Kst: tracks IP chi2
##     @arg TrChi2: tracks chi2
##     @arg KstMassWin: K* mass window
##     @arg KstVCHI2: vertex chi2 of the K*
    
##     @return: Selection object
    
##     """
##     _preambulo = ["goodTrack = ((MIPCHI2DV(PRIMARY) > %(TrIPchi2Kst)s) & (TRCHI2DOF < %(TrChi2)s))" % locals(),
##                   "goodKstar = (((VFASPF(VCHI2/VDOF) < %(KstVCHI2)s)) & (ADMASS('K*(892)0') < %(KstMassWin)s*MeV))" % locals()]
##     _code = "goodKstar & CHILDCUT( goodTrack , 1 ) & CHILDCUT( goodTrack , 2 )"
##     _kstFilter = FilterDesktop(Preambulo=_preambulo, Code=_code)
##     _stdKst2Kpi = DataOnDemand(Location="Phys/StdVeryLooseDetachedKst2Kpi/Particles")
##     return Selection(name, Algorithm=_kstFilter, RequiredSelections=[_stdKst2Kpi])

def makeD02PhiGamma(name, phiSel, gammaSel,  D_BPVLTIME,  D0_PT, D0MassWin, pvRefit):
    """
    Create and return a D0 -> Phi Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg phiSel: Phi -> K+ K+ selection
    @arg gammaSel: photon selection
    @arg D0PVIPchi2: IP chi2 of the D0 wrt the PV
    @arg D0MassWin: D0 mass window
    
    @return: Selection object
    
    """  
    _motherCut = "( (ADMASS('D0')<%(D0MassWin)s*MeV) & (PT> %(D0_PT)s) )" % locals() #(BPVIPCHI2() < %(D0PVIPchi2)s) & (BPVLTIME() > %(D_BPVLTIME)s) 
    _combinationCut = "(ADAMASS('D0')<1.1*%(D0MassWin)s*MeV)"  % locals()
    print 'making D0:',  _combinationCut,'&', _motherCut, 'pvRefit = ', pvRefit
    
    _D0 = CombineParticles(DecayDescriptor= "D0 -> phi(1020) gamma",
                           CombinationCut=_combinationCut,
                           MotherCut=_motherCut,
                           ReFitPVs=pvRefit)

    return Selection(name, Algorithm=_D0, RequiredSelections=[phiSel, gammaSel])
    

def makeDst2D0Pi(name, D0Sel, slowPionSel, deltaMass, DstVCHI2, pvRefit):
    """
    Create and return a Dst-> D0 (-> Phi Gamma) Pi Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg D0Sel: D0->Phi gamma selection
    @arg slowPionSel: slow pion selection
    @arg D0MassWin: Dst mass window
    @arg deltaMass: delta(D*-D0)
    @arg DstVCHI2: vertex chi2
    @return: Selection object
    
    """
    _combinationCut = "((AM - AM1) < 1.1*%(deltaMass)s*MeV)"  % locals()
    _motherCut =   "(VFASPF(VCHI2/VDOF) < %(DstVCHI2)s) & ((M - M1) < %(deltaMass)s*MeV)" % locals()
    print 'making Dstar:',  _combinationCut,'&', _motherCut, 'pvRefit =', pvRefit
    
    _Dst = CombineParticles(DecayDescriptors=[ 'D*(2010)+ -> D0 pi+', 'D*(2010)- -> D0 pi-'], # "[D*(2010)+ -> D0 pi+]cc",
                            CombinationCut =  _combinationCut, 
                            MotherCut = _motherCut ,
                            ReFitPVs=pvRefit)
    return Selection(name, Algorithm=_Dst, RequiredSelections=[slowPionSel, D0Sel])


## def makeBd2KstGamma(name, kstSel, gammaSel, B0DirAngle, B0PVIPchi2, B0MassWin):
##     """
##     Create and return a Bd -> K* Gamma Selection object, starting with the daughters' selections.

##     @arg name: name of the Selection.
##     @arg kstSel: K* -> K pi selection
##     @arg gammaSel: photon selection
##     @arg B0DirAngle: direction angle of the B0 wrt the PV
##     @arg B0PVIPchi2: IP chi2 of the B0 wrt the PV
##     @arg B0MassWin: B0 mass window
    
##     @return: Selection object

##     """  
##     _combinationCut = "ADAMASS('B0') < 1.5*%(B0MassWin)s*MeV" % locals()
##     _motherCut = "(acos(BPVDIRA) <%(B0DirAngle)s) & (BPVIPCHI2() < %(B0PVIPchi2)s) & (ADMASS('B0')<%(B0MassWin)s*MeV)" % locals()
##     _Bd = CombineParticles(DecayDescriptor="[B0 -> K*(892)0 gamma]cc",
##                            CombinationCut=_combinationCut,
##                            MotherCut=_motherCut,
##                            ReFitPVs=False)#True)
##     #_Bd.addTool(OfflineVertexFitter())
##     #_Bd.VertexFitters.update({"": "OfflineVertexFitter"})
##     #return Selection(name, Algorithm=_Bd, RequiredSelections=[kstSel, gammaSel])
##     return Selection(name, Algorithm=_Bd, RequiredSelections=[gammaSel, kstSel])
        
# EOF
