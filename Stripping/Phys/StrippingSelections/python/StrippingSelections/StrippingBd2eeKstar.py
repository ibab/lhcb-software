# $Id: StrippingBd2eeKstar.py,v 1.2 2010-09-06 16:30:22 jhe Exp $

'''
Module for construction of Bd->eeKstar
'''

__author__=['Marie-Helene Schune', 'Jibo He']
__date__ = '02/09/2010'
__version__= '$Revision: 1.2 $'


__all__ = (
    'Bd2eeKstarConf',
    'makeEE',
    'makeKstar',
    'makeBd2eeKstar'
    )

defaulSettings =  {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  350.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :  100.   ,
        'ElectronIPCHI2'          :    2.25 ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeCombMinMass'           :    0.   ,  # MeV         
        'eeCombMaxMass'           : 1550.   ,  # MeV   
        'eeVertexCHI2'            :   15.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 1500.   ,  # MeV
        'eeFD'                    :    1.   ,  # mm
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  300.   ,  # MeV
        'PionP'                   : 3000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarComMassW'           :  200.   ,  # MeV  
        'KstarVertexCHI2'         :   25.   , 
        'KstarMassW'              :  130.   ,  # MeV
        'KstarIPCHI2'             :    1.   , 
        'KstarFDCHI2'             :    1.   , 
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :    9.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BIPCHI2'                 :   64.   ,  # pointing
        'BFDCHI2'                 :    9.   , 
        'BDIRA'                   :    0.999, 
        'BIP'                     :    0.05 ,  # mm
        'SumIPSCut'               : " & (SUMTREE(((ABSID=='K+') | (ABSID=='pi-') | (ID=='e+') | (ID=='e-')),sqrt(BPVIPCHI2()))>15)" 
        }
    

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class Bd2eeKstarConf(LineBuilder):
    
    __configuration_keys__ = (
        'LinePrescale',
        'LinePostscale',
        #
        'ElectronPT',
        'ElectronTrackCHI2pNDOF',
        'ElectronIPCHI2',
        'ElectronPIDepi',
        #
        'eeCombMinMass',
        'eeCombMaxMass',
        'eeVertexCHI2',
        'eeMinMass',
        'eeMaxMass',
        #
        'eeFD',
        #
        'KaonPT',
        'KaonP',
        'KaonTrackCHI2pNDOF',
        'KaonIPCHI2',
        'KaonPIDKpi',
        #
        'PionPT',
        'PionP',
        'PionTrackCHI2pNDOF',
        'PionIPCHI2',
        'PionPIDpiK',
        #
        'KstarComMassW',
        'KstarVertexCHI2',
        'KstarMassW',
        #
        'KstarIPCHI2',
        'KstarFDCHI2',
        #
        'BComMassW',
        'BVertexCHI2',
        'BMassW',
        'BIPCHI2',
        'BFDCHI2',
        'BDIRA',
        'BIP',
        'SumIPSCut' 
        )
    
    def __init__(self, name, config ):
        
        LineBuilder.__init__(self, name, config)
        
        Bd2eeKstarName = name
        self.SelEE = makeEE( 'eeFor'+Bd2eeKstarName,
                             ElectronPT = config['ElectronPT'],
                             ElectronTrackCHI2pNDOF = config['ElectronTrackCHI2pNDOF'],
                             ElectronIPCHI2 = config['ElectronIPCHI2'],
                             ElectronPIDepi = config['ElectronPIDepi'],
                             #
                             eeCombMinMass = config['eeCombMinMass'],
                             eeCombMaxMass = config['eeCombMaxMass'],
                             eeVertexCHI2 = config['eeVertexCHI2'],
                             eeMinMass = config['eeMinMass'],
                             eeMaxMass = config['eeMaxMass']
                             )
        
        self.SelKstar = makeKstar('KstarFor'+Bd2eeKstarName,
                                  KaonPT = config['KaonPT'],
                                  KaonP = config['KaonP'],
                                  KaonTrackCHI2pNDOF = config['KaonTrackCHI2pNDOF'],
                                  KaonIPCHI2 = config['KaonIPCHI2'],
                                  KaonPIDKpi = config['KaonPIDKpi'],
                                  #
                                  PionPT = config['PionPT'],
                                  PionP = config['PionP'],
                                  PionTrackCHI2pNDOF = config['PionTrackCHI2pNDOF'],
                                  PionIPCHI2 = config['PionIPCHI2'],
                                  PionPIDpiK = config['PionPIDpiK'],
                                  #
                                  KstarComMassW = config['KstarComMassW'],
                                  KstarVertexCHI2 = config['KstarVertexCHI2'],
                                  KstarMassW = config['KstarMassW']                   
                                  )
        
        self.SelBd2eeKstar = makeBd2eeKstar( "Sel_"+Bd2eeKstarName,
                                             SelEE = self.SelEE,
                                             SelKstar = self.SelKstar,
                                             eeFD = config['eeFD'],
                                             KstarIPCHI2 = config['KstarIPCHI2'],
                                             KstarFDCHI2 = config['KstarFDCHI2'],                
                                             BComMassW = config['BComMassW'],
                                             BVertexCHI2 = config['BVertexCHI2'],
                                             BMassW = config['BMassW'],
                                             BIPCHI2 = config['BIPCHI2'],
                                             BFDCHI2 = config['BFDCHI2'],
                                             BDIRA = config['BDIRA'],
                                             BIP = config['BIP'],
                                             SumIPSCut = config['SumIPSCut']
                                             )
                                             
        self.line = StrippingLine( Bd2eeKstarName+"Line",
                                   prescale = config['LinePrescale'],
                                   postscale = config['LinePostscale'],
                                   selection = self.SelBd2eeKstar
                                   )
        
        self.registerLine(self.line)
       

def makeEE( name,
            #
            ElectronPT,
            ElectronTrackCHI2pNDOF,
            ElectronIPCHI2,
            ElectronPIDepi,
            #
            eeCombMinMass,
            eeCombMaxMass,
            eeVertexCHI2,
            eeMinMass,
            eeMaxMass
            ):

    from StandardParticles import StdAllLooseElectrons as ElectronsForBd2eeKstar
    
    ElectronCut = "(PT> %(ElectronPT)s *MeV) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (PIDe>%(ElectronPIDepi)s) " % locals()
    
    EEComCut = "(%(eeCombMinMass)s < AM) & (AM< %(eeCombMaxMass)s)" % locals() 
    EEMomCut = "(VFASPF(VCHI2)< %(eeVertexCHI2)s) & (%(eeMinMass)s < MM) & (MM< %(eeMaxMass)s)" % locals()
    
    _EE = CombineParticles( DecayDescriptor = "J/psi(1S) -> e+ e-",
                            DaughtersCuts = {"e+" : ElectronCut },
                            CombinationCut = EEComCut,
                            MotherCut = EEMomCut
                            )
    
    return Selection( name,
                      Algorithm = _EE,
                      RequiredSelections = [ ElectronsForBd2eeKstar ]
                      )


def makeKstar(name,
              KaonPT,
              KaonP,
              KaonTrackCHI2pNDOF,
              KaonIPCHI2,
              KaonPIDKpi,
              #
              PionPT,
              PionP,
              PionTrackCHI2pNDOF,
              PionIPCHI2,
              PionPIDpiK,
              #
              KstarComMassW,
              KstarVertexCHI2,
              KstarMassW
              ):

    from StandardParticles import StdLooseKaons as KaonsForBd2eeKstar
    from StandardParticles import StdLoosePions as PionsForBd2eeKstar    
    
    KaonCut = "(PT> %(KaonPT)s *MeV) & (P>%(KaonP)s *MeV) & (TRCHI2DOF < %(KaonTrackCHI2pNDOF)s) & (BPVIPCHI2()> %(KaonIPCHI2)s ) & (PIDK>%(KaonPIDKpi)s)" % locals()
    PionCut = "(PT> %(PionPT)s *MeV) & (P>%(PionP)s *MeV) & (TRCHI2DOF < %(PionTrackCHI2pNDOF)s) & (BPVIPCHI2()> %(PionIPCHI2)s ) & (PIDK<%(PionPIDpiK)s)" % locals()
    
    KstarComCut = "(ADAMASS('K*(892)0')< %(KstarComMassW)s *MeV)" % locals()
    
    KstarMomCut = "(VFASPF(VCHI2/VDOF)< %(KstarVertexCHI2)s) & (ADMASS('K*(892)0')< %(KstarMassW)s *MeV)" % locals()
    
    _Kstar = CombineParticles( DecayDescriptor = "[K*(892)0 -> K+ pi-]cc",
                               DaughtersCuts = { "K+":KaonCut, "pi-":PionCut },
                               CombinationCut = KstarComCut, 
                               MotherCut = KstarMomCut
                               )
    
    return Selection( name,
                      Algorithm = _Kstar,
                      RequiredSelections = [ KaonsForBd2eeKstar, PionsForBd2eeKstar  ]
                      )


def makeBd2eeKstar( name,
                    SelEE,
                    SelKstar,
                    #
                    eeFD,
                    KstarIPCHI2,
                    KstarFDCHI2,
                    #
                    BComMassW,
                    BVertexCHI2,
                    BMassW,
                    BIPCHI2,
                    BFDCHI2,
                    BDIRA,
                    BIP,
                    SumIPSCut
                    ):
    
    Bd2eeKstarComCut = "(ADAMASS('B0')< %(BComMassW)s *MeV)" % locals()
    Bd2eeKstarMomCut = "(ADMASS('B0')< %(BMassW)s *MeV) & (VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s) & (BPVIPCHI2()< %(BIPCHI2)s ) & (BPVDIRA> %(BDIRA)s ) & (BPVIP()< %(BIP)s *mm) & (BPVVDCHI2>%(BFDCHI2)s)" % locals()

    eeFinalCut = "(INTREE( (ID=='J/psi(1S)') & (BPVVD>%(eeFD)s*mm) ))" % locals()
    KstarFinalCut = "(INTREE( (ABSID=='K*(892)0') & (BPVIPCHI2()>%(KstarIPCHI2)s) & (BPVVDCHI2>%(KstarFDCHI2)s) ))" % locals()
    
    _Bd2eeKstar = CombineParticles( DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc",
                                    CombinationCut = Bd2eeKstarComCut, 
                                    MotherCut = Bd2eeKstarMomCut + "&" + eeFinalCut + "&" + KstarFinalCut + SumIPSCut
                                    )
    return Selection( name,
                      Algorithm = _Bd2eeKstar,
                      RequiredSelections = [ SelEE, SelKstar ]
                      )
    
    
