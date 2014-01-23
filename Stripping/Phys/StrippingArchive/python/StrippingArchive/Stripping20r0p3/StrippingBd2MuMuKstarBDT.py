# $Id: StrippingBd2MuMuKstarBDT.py,v 1.2 2010-09-06 16:30:22 jhe Exp $

'''
Module for construction of Bd->MuMu Kstar based on BDT

'''

__author__=['Marie-Helene Schune', 'Jibo He']
__date__ = '20/01/2012'
__version__= '$Revision: 1.0 $'


__all__ = (
    'Bd2MuMuKstarBDTConf',
    'makeEE',
    'makeKstar',
    'makeBd2MuMuKstar'
    )

defaulSettings =  {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -5.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 5200.   ,  # MeV
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  150.   ,  # MeV
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :   16.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BDIRA'                   :    0.999,
        'BDTCutValue'             :   -0.98 ,
        'BDTWeightsFile'          : '$TMVASELECTIONSROOT/weights/Bd2eeKstar_BDTG.weights.xml'
        }

Bd2MuMuKPiSettings =  {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -5.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 5200.   ,  # MeV
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  1400.  ,  # MeV
        #
        'BComMassW'               :  550.   ,  # MeV
        'BVertexCHI2'             :   16.   ,  # /ndf
        'BMassW'                  :  500.   ,  # MeV  
        'BDIRA'                   :    0.999,
        'BDTCutValue'             :   -0.98 ,
        'BDTWeightsFile'          : '$TMVASELECTIONSROOT/weights/Bd2eeKstar_BDTG.weights.xml'
        }




from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
    
class Bd2MuMuKstarBDTConf(LineBuilder):
    
    __configuration_keys__ = (
        'LinePrescale',
        'LinePostscale',
        #
        'ElectronPT',
        'ElectronTrackCHI2pNDOF',
        'ElectronIPCHI2',
        'ElectronPIDepi',
        #
        'eeVertexCHI2',
        'eeMinMass',
        'eeMaxMass',
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
        'KstarVertexCHI2',
        'KstarMassW',
        #
        'BComMassW',
        'BVertexCHI2',
        'BMassW',
        'BDIRA',
        #
        'BDTCutValue',
        'BDTWeightsFile'
        )
    
    def __init__(self, name, config ):
        
        LineBuilder.__init__(self, name, config)
        
        Bd2MuMuKstarBDTName = name
        Bd2MuMuKstarBDTLineName = Bd2MuMuKstarBDTName + "Line"
        
        self.SelEE = makeEE( 'eeFor'+Bd2MuMuKstarBDTName,
                             ElectronPT = config['ElectronPT'],
                             ElectronTrackCHI2pNDOF = config['ElectronTrackCHI2pNDOF'],
                             ElectronIPCHI2 = config['ElectronIPCHI2'],
                             ElectronPIDepi = config['ElectronPIDepi'],
                             #
                             eeVertexCHI2 = config['eeVertexCHI2'],
                             eeMinMass = config['eeMinMass'],
                             eeMaxMass = config['eeMaxMass']
                             )
        
        self.SelKstar = makeKstar('KstarFor'+Bd2MuMuKstarBDTName,
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
                                  KstarVertexCHI2 = config['KstarVertexCHI2'],
                                  KstarMassW = config['KstarMassW']                   
                                  )
        
        self.SelBd2MuMuKstar = makeBd2MuMuKstar( "Sel_"+Bd2MuMuKstarBDTName,
                                             SelEE = self.SelEE,
                                             SelKstar = self.SelKstar,
                                             BComMassW = config['BComMassW'],
                                             BVertexCHI2 = config['BVertexCHI2'],
                                             BMassW = config['BMassW'],
                                             BDIRA = config['BDIRA']
                                             )

        self.CutBDTBd2MuMuKstar  = applyBDT( "CutBDT_" + Bd2MuMuKstarBDTName,
                                           LineName  = Bd2MuMuKstarBDTLineName,
                                           SelBd2MuMuKstar = self.SelBd2MuMuKstar,
                                           BDTCutValue = config['BDTCutValue'],
                                           BDTWeightsFile = config['BDTWeightsFile']
                                           )
        
        self.line = StrippingLine( Bd2MuMuKstarBDTLineName,
                                   prescale = config['LinePrescale'],
                                   postscale = config['LinePostscale'],
                                   selection = self.CutBDTBd2MuMuKstar
                                   )
        
        self.registerLine(self.line)
       

def makeEE( name,
            #
            ElectronPT,
            ElectronTrackCHI2pNDOF,
            ElectronIPCHI2,
            ElectronPIDepi,
            #
            eeVertexCHI2,
            eeMinMass,
            eeMaxMass
            ):

    _StdLooseDetachedDiElectron = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles" )
    
    ElectronPCut = "(INTREE( (ID=='mu+') & (PT> %(ElectronPT)s *MeV) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (PIDmu>%(ElectronPIDepi)s) ))" % locals()
    
    ElectronMCut = "(INTREE( (ID=='mu-') & (PT> %(ElectronPT)s *MeV) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (PIDmu>%(ElectronPIDepi)s) ))" % locals()
    
    EEMomCut = "(VFASPF(VCHI2)< %(eeVertexCHI2)s) & (%(eeMinMass)s *MeV < MM) & (MM< %(eeMaxMass)s *MeV)" % locals()
    
    _EE = FilterDesktop( Code = EEMomCut + " & " + ElectronPCut + " & " + ElectronMCut )
    
    return Selection( name,
                      Algorithm = _EE,
                      RequiredSelections = [ _StdLooseDetachedDiElectron ]
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
              KstarVertexCHI2,
              KstarMassW
              ):

    _StdLooseDetachedKpi = DataOnDemand(Location = "Phys/StdLooseDetachedKpi/Particles" )
    
    KaonCut = "(INTREE( (ABSID=='K+') & (PT> %(KaonPT)s *MeV) & (P>%(KaonP)s *MeV) & (TRCHI2DOF < %(KaonTrackCHI2pNDOF)s) & (BPVIPCHI2()> %(KaonIPCHI2)s ) & (PIDK>%(KaonPIDKpi)s) & (TRGHOSTPROB<0.35) ))" % locals()
    PionCut = "(INTREE( (ABSID=='pi+') &  (PT> %(PionPT)s *MeV) & (P>%(PionP)s *MeV) & (TRCHI2DOF < %(PionTrackCHI2pNDOF)s) & (BPVIPCHI2()> %(PionIPCHI2)s ) & (PIDK<%(PionPIDpiK)s) & (TRGHOSTPROB<0.35) ))" % locals()
    
    KstarMomCut = "(VFASPF(VCHI2/VDOF)< %(KstarVertexCHI2)s) & (ADMASS('K*(892)0')< %(KstarMassW)s *MeV)" % locals()
    
    _Kstar = FilterDesktop( Code = KstarMomCut + " & " + KaonCut + " & " + PionCut )
    
    return Selection( name,
                      Algorithm = _Kstar,
                      RequiredSelections = [ _StdLooseDetachedKpi ]
                      )


def makeBd2MuMuKstar( name,
                    SelEE,
                    SelKstar,
                    #
                    BComMassW,
                    BVertexCHI2,
                    BMassW,
                    BDIRA
                    ):
    
    Bd2MuMuKstarBDTComCut = "(ADAMASS('B0')< %(BComMassW)s *MeV)" % locals()
    Bd2MuMuKstarBDTMomCut = "(ADMASS('B0')< %(BMassW)s *MeV) & (VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s) & (BPVDIRA> %(BDIRA)s )" % locals()
    
    _Bd2MuMuKstar = CombineParticles( DecayDescriptor = "[ B0 -> J/psi(1S) K*(892)0 ]cc",
                                    CombinationCut = Bd2MuMuKstarBDTComCut, 
                                    MotherCut = Bd2MuMuKstarBDTMomCut
                                    )
    return Selection( name,
                      Algorithm = _Bd2MuMuKstar,
                      RequiredSelections = [ SelEE, SelKstar ]
                      )


def applyBDT( name,
              LineName,
              SelBd2MuMuKstar,
              BDTCutValue,
              BDTWeightsFile ):
    
    _FilterBd2MuMuKstar = FilterDesktop( Code = "FILTER('Bd2eeKstarBDTSelection/Bd2MuMuKstarBDT')" )
    
    BDTSel = Selection( name,
                        Algorithm = _FilterBd2MuMuKstar,
                        RequiredSelections = [ SelBd2MuMuKstar ]
                        )

    """
    Name is special here, since this is the last algorithm,
    whose name seems to be the one of the stripping line....
    """
    from Configurables import Bd2eeKstarBDTSelection

    MyBDT = Bd2eeKstarBDTSelection( LineName + '.Bd2MuMuKstarBDT' )
    MyBDT.BDTCut = BDTCutValue
    MyBDT.WeightsFile = BDTWeightsFile
    
    return BDTSel 
    
