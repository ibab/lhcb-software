# $Id: StrippingBd2eeKstarBDT.py,v 1.2 2010-09-06 16:30:22 jhe Exp $

'''
Module for construction of Bd->eeKstar based on BDT
'''

__author__=['Marie-Helene Schune', 'Jibo He']
__date__ = '20/01/2012'
__version__= '$Revision: 1.0 $'


__all__ = (
    'Bd2eeKstarBDTConf',
    'makeEE',
    'makeKstar',
    'makeBd2eeKstar',
    'default_config'
    )

default_config =  {
    'NAME'              :  'Bd2eeKstarBDT',
    'BUILDERTYPE'       :  'Bd2eeKstarBDTConf',
    'CONFIG'    : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronGhostProb'       :    0.5  ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeCuts'                  : "(HASVERTEX) & (VFASPF(VCHI2)<16) & (((MM<1.5*GeV)) | ((MM>2.2*GeV) & (MM<4.2*GeV)))",
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   ,
        'KaonGhostProb'           :    0.35 ,
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   ,
        'PionGhostProb'           :    0.35 ,
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
        'BDTCutValue'             :   -0.95 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml',
        #
        'RelatedInfoTools'       : [ { "Type" : "RelInfoBs2MuMuTrackIsolations"
                                       , "RecursionLevel" : 2
                                       , "Locations" : { 'Phys/StdAllLooseElectrons' : [ 'Electron1ISO', 'Electron2ISO' ] ,
                                                         'Phys/StdAllLooseKaons'     :  'KaonISO',
                                                         'Phys/StdAllLoosePions'     :  'PionISO'
                                                         }
                                       , "tracktype"  : 3
                                       , "angle"      : 0.27
                                       , "fc"         : 0.60
                                       , "doca_iso"   : 0.13
                                       , "ips"        : 3.0
                                       , "svdis"      : -0.15
                                       , "svdis_h"    : 30.
                                       , "pvdis"      : 0.5
                                       , "pvdis_h"    : 40.
                                       , "makeTrackCuts" : False
                                       , "IsoTwoBody" : False
                                       } ],       
        'RelatedInfoTools2'       : [ { "Type" : "RelInfoBs2MuMuTrackIsolations"
                                       , "RecursionLevel" : 2
                                       , "Locations" : { 'Phys/StdDiElectronFromTracks' : [ 'Electron1ISO', 'Electron2ISO' ] ,
                                                         'Phys/StdAllLooseKaons'     :  'KaonISO',
                                                         'Phys/StdAllLoosePions'     :  'PionISO'
                                                         }
                                       , "tracktype"  : 3
                                       , "angle"      : 0.27
                                       , "fc"         : 0.60
                                       , "doca_iso"   : 0.13
                                       , "ips"        : 3.0
                                       , "svdis"      : -0.15
                                       , "svdis_h"    : 30.
                                       , "pvdis"      : 0.5
                                       , "pvdis_h"    : 40.
                                       , "makeTrackCuts" : False
                                       , "IsoTwoBody" : False
                                       } ],       
        },
    'STREAMS'           : ['Bhadron' ],
    'WGs'               : ['RD']
    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
    
class Bd2eeKstarBDTConf(LineBuilder):
    
    __configuration_keys__ = (
        'LinePrescale',
        'LinePostscale',
        #
        'ElectronPT',
        'ElectronTrackCHI2pNDOF',
        'ElectronGhostProb',
        'ElectronIPCHI2',
        'ElectronPIDepi',
        #
        'eeCuts',
        #
        'KaonPT',
        'KaonP',
        'KaonTrackCHI2pNDOF',
        'KaonGhostProb',
        'KaonIPCHI2',
        'KaonPIDKpi',
        #
        'PionPT',
        'PionP',
        'PionTrackCHI2pNDOF',
        'PionGhostProb',
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
        'BDTWeightsFile',
        'RelatedInfoTools', 
        'RelatedInfoTools2'
        )
    
    def __init__(self, name, config ):
        
        LineBuilder.__init__(self, name, config)
        
        Bd2eeKstarBDTName = name
        Bd2eeKstarBDTLineName = Bd2eeKstarBDTName + "Line"
        
        self.SelEE = makeEE( 'eeFor'+Bd2eeKstarBDTName,
                             ElectronPT = config['ElectronPT'],
                             ElectronTrackCHI2pNDOF = config['ElectronTrackCHI2pNDOF'],
                             ElectronGhostProb = config['ElectronGhostProb'],
                             ElectronIPCHI2 = config['ElectronIPCHI2'],
                             ElectronPIDepi = config['ElectronPIDepi'],
                             #
                             eeCuts = config['eeCuts']
                             )
        
        self.SelKstar = makeKstar('KstarFor'+Bd2eeKstarBDTName,
                                  KaonPT = config['KaonPT'],
                                  KaonP = config['KaonP'],
                                  KaonTrackCHI2pNDOF = config['KaonTrackCHI2pNDOF'],
                                  KaonGhostProb = config['KaonGhostProb'],
                                  KaonIPCHI2 = config['KaonIPCHI2'],
                                  KaonPIDKpi = config['KaonPIDKpi'],
                                  #
                                  PionPT = config['PionPT'],
                                  PionP = config['PionP'],
                                  PionTrackCHI2pNDOF = config['PionTrackCHI2pNDOF'],
                                  PionGhostProb = config['PionGhostProb'],
                                  PionIPCHI2 = config['PionIPCHI2'],
                                  PionPIDpiK = config['PionPIDpiK'],
                                  #
                                  KstarVertexCHI2 = config['KstarVertexCHI2'],
                                  KstarMassW = config['KstarMassW']                   
                                  )
        
        self.SelBd2eeKstar = makeBd2eeKstar( "Sel_"+Bd2eeKstarBDTName,
                                             SelEE = self.SelEE,
                                             SelKstar = self.SelKstar,
                                             BComMassW = config['BComMassW'],
                                             BVertexCHI2 = config['BVertexCHI2'],
                                             BMassW = config['BMassW'],
                                             BDIRA = config['BDIRA']
                                             )

        self.CutBDTBd2eeKstar  = applyBDT( "CutBDT_" + Bd2eeKstarBDTName,
                                           LineName  = Bd2eeKstarBDTLineName,
                                           SelBd2eeKstar = self.SelBd2eeKstar,
                                           BDTCutValue = config['BDTCutValue'],
                                           BDTWeightsFile = config['BDTWeightsFile']
                                           )
        
        self.line = StrippingLine( Bd2eeKstarBDTLineName,
                                   prescale = config['LinePrescale'],
                                   postscale = config['LinePostscale'],
                                   selection = self.CutBDTBd2eeKstar,
                                   RelatedInfoTools = config[ 'RelatedInfoTools']
                                   )
        
        #self.registerLine(self.line)

        # new line with new Brem cor 
        self.FilterEE = filterEE( 'filterEEFor'+Bd2eeKstarBDTName,
                                  ElectronPT = config['ElectronPT'],
                                  ElectronTrackCHI2pNDOF = config['ElectronTrackCHI2pNDOF'],
                                  ElectronGhostProb = config['ElectronGhostProb'],
                                  ElectronIPCHI2 = config['ElectronIPCHI2'],
                                  ElectronPIDepi = config['ElectronPIDepi'],
                                  #
                                  eeCuts = config['eeCuts']
                                  )
        
         
        self.SelBd2DieeKstar = makeBd2eeKstar( "Sel2_"+Bd2eeKstarBDTName,
                                               SelEE = self.FilterEE,
                                               SelKstar = self.SelKstar,
                                               BComMassW = config['BComMassW'],
                                               BVertexCHI2 = config['BVertexCHI2'],
                                               BMassW = config['BMassW'],
                                               BDIRA = config['BDIRA']
                                               )

        self.CutBDTBd2eeKstar2  = applyBDT( "CutBDT2_" + Bd2eeKstarBDTName,
                                            LineName  = Bd2eeKstarBDTLineName+"2",
                                            SelBd2eeKstar = self.SelBd2DieeKstar,
                                            BDTCutValue = config['BDTCutValue'],
                                            BDTWeightsFile = config['BDTWeightsFile']
                                            )
        
        self.line2 = StrippingLine( Bd2eeKstarBDTLineName+"2",
                                    prescale = config['LinePrescale'],
                                    postscale = config['LinePostscale'],
                                    selection = self.CutBDTBd2eeKstar2,
                                    RelatedInfoTools = config[ 'RelatedInfoTools2']
                                    )
        
        self.registerLine(self.line2) 
       

def makeEE( name,
            #
            ElectronPT,
            ElectronTrackCHI2pNDOF,
            ElectronGhostProb,
            ElectronIPCHI2,
            ElectronPIDepi,
            #
            eeCuts
            ):

    _StdLooseDetachedDiElectron = DataOnDemand(Location = "Phys/StdLooseDetachedDiElectron/Particles" )
    
    ElectronPCut = "(INTREE( (ID=='e+') & (PT> %(ElectronPT)s *MeV) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (PIDe>%(ElectronPIDepi)s) & (TRGHOSTPROB<%(ElectronGhostProb)s ) ))" % locals()

    ElectronMCut = "(INTREE( (ID=='e-') & (PT> %(ElectronPT)s *MeV) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (PIDe>%(ElectronPIDepi)s) & (TRGHOSTPROB<%(ElectronGhostProb)s ) ))" % locals()
    
    _EE = FilterDesktop( Code = eeCuts + " & " + ElectronPCut + " & " + ElectronMCut )
    
    return Selection( name,
                      Algorithm = _EE,
                      RequiredSelections = [ _StdLooseDetachedDiElectron ]
                      )

def filterEE( name,
              #
              ElectronPT,
              ElectronTrackCHI2pNDOF,
              ElectronGhostProb,
              ElectronIPCHI2,
              ElectronPIDepi,
              #
              eeCuts
              ):
    
    _StdLooseDetachedDiElectron = DataOnDemand(Location = "Phys/StdDiElectronFromTracks/Particles" )
    
    ElectronPCut = "(INTREE( (ID=='e+') & (PT> %(ElectronPT)s *MeV) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (PIDe>%(ElectronPIDepi)s) & (TRGHOSTPROB<%(ElectronGhostProb)s ) ))" % locals()

    ElectronMCut = "(INTREE( (ID=='e-') & (PT> %(ElectronPT)s *MeV) & (TRCHI2DOF < %(ElectronTrackCHI2pNDOF)s) & (BPVIPCHI2() > %(ElectronIPCHI2)s) & (PIDe>%(ElectronPIDepi)s) & (TRGHOSTPROB<%(ElectronGhostProb)s ) ))" % locals()
    
    _EE = FilterDesktop( Code = eeCuts + " & " + ElectronPCut + " & " + ElectronMCut )
    
    return Selection( name,
                      Algorithm = _EE,
                      RequiredSelections = [ _StdLooseDetachedDiElectron ]
                      )


def makeKstar(name,
              KaonPT,
              KaonP,
              KaonTrackCHI2pNDOF,
              KaonGhostProb,
              KaonIPCHI2,
              KaonPIDKpi,
              #
              PionPT,
              PionP,
              PionTrackCHI2pNDOF,
              PionGhostProb,
              PionIPCHI2,
              PionPIDpiK,
              #
              KstarVertexCHI2,
              KstarMassW
              ):

    _StdVeryLooseDetachedKst2Kpi = DataOnDemand(Location = "Phys/StdVeryLooseDetachedKst2Kpi/Particles" )
    
    KaonCut = "(INTREE( (ABSID=='K+') & (PT> %(KaonPT)s *MeV) & (P>%(KaonP)s *MeV) & (TRCHI2DOF < %(KaonTrackCHI2pNDOF)s) & (BPVIPCHI2()> %(KaonIPCHI2)s ) & (PIDK>%(KaonPIDKpi)s) & (TRGHOSTPROB<%(KaonGhostProb)s ) ))" % locals()
    PionCut = "(INTREE( (ABSID=='pi+') &  (PT> %(PionPT)s *MeV) & (P>%(PionP)s *MeV) & (TRCHI2DOF < %(PionTrackCHI2pNDOF)s) & (BPVIPCHI2()> %(PionIPCHI2)s ) & (PIDK<%(PionPIDpiK)s) & (TRGHOSTPROB<%(PionGhostProb)s) ))" % locals()
    
    KstarMomCut = "(VFASPF(VCHI2/VDOF)< %(KstarVertexCHI2)s) & (ADMASS('K*(892)0')< %(KstarMassW)s *MeV)" % locals()
    
    _Kstar = FilterDesktop( Code = KstarMomCut + " & " + KaonCut + " & " + PionCut )
                                   
    return Selection( name,
                      Algorithm = _Kstar,
                      RequiredSelections = [ _StdVeryLooseDetachedKst2Kpi ]
                      )


def makeBd2eeKstar( name,
                    SelEE,
                    SelKstar,
                    #
                    BComMassW,
                    BVertexCHI2,
                    BMassW,
                    BDIRA
                    ):
    
    Bd2eeKstarBDTComCut = "(ADAMASS('B0')< %(BComMassW)s *MeV)" % locals()
    Bd2eeKstarBDTMomCut = "(ADMASS('B0')< %(BMassW)s *MeV) & (VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s) & (BPVDIRA> %(BDIRA)s )" % locals()
    
    _Bd2eeKstar = CombineParticles( DecayDescriptor = "[ B0 -> J/psi(1S) K*(892)0 ]cc",
                                    CombinationCut = Bd2eeKstarBDTComCut, 
                                    MotherCut = Bd2eeKstarBDTMomCut
                                    )
    return Selection( name,
                      Algorithm = _Bd2eeKstar,
                      RequiredSelections = [ SelEE, SelKstar ]
                      )


def applyBDT( name,
              LineName,
              SelBd2eeKstar,
              BDTCutValue,
              BDTWeightsFile ):
    
    _FilterBd2eeKstar = FilterDesktop( Code = "FILTER('Bd2eeKstarBDTSelection/Bd2eeKstarBDT')" )
    
    BDTSel = Selection( name,
                        Algorithm = _FilterBd2eeKstar,
                        RequiredSelections = [ SelBd2eeKstar ]
                        )

    """
    Name is special here, since this is the last algorithm,
    whose name seems to be the one of the stripping line....
    """
    from Configurables import Bd2eeKstarBDTSelection

    MyBDT = Bd2eeKstarBDTSelection( LineName + '.Bd2eeKstarBDT' )
    MyBDT.BDTCut = BDTCutValue
    MyBDT.WeightsFile = BDTWeightsFile
    
    return BDTSel 
    
