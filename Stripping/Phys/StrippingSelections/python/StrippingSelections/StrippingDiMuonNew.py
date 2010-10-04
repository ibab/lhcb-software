# $Id: StrippingDiMuonNew.py, v 1.0 2010-09-06 16:30:22 jhe Exp $

'''
Inclusive DiMuon lines based on the lines by Gaia Lanfranchi, Alessio Sarti,
with inputs from Joel Bressieux, Giulia Manca, Matthew Needham and Patrick Robbe.

Including the following lines:
1. DiMuonLine
   FullDST (keep 10%) and MicroDST 
2. DiMuonSameSignLine
   FullDST (keep  1%) and MicroDST (keep 10%) 
3. DiMuonHighMassLine                     
4. DiMuonHighMassSameSignLine             
5. DiMuonLowMassLine
   Keep Hlt2UnbiasedDiMuonLowMassDecision triggered events 
6. Jpsi2MuMuLine

More details can be found here:
http://indico.cern.ch/contributionDisplay.py?contribId=2&confId=100755

--------------------------
To use this for Full DST
--------------------------
from StrippingSelections.StrippingDiMuonNew import DiMuonConf
FullDSTDiMuonConf = DiMuonConf( config = DiMuonConf.config_default )
stream.appendLines( FullDSTDiMuonConf.Lines )

--------------------------
For MicroDST
--------------------------
from StrippingSelections.StrippingDiMuonNew import DiMuonConf
MicroDSTDiMuonConf = DiMuonConf( name = 'MicroDST', config = DiMuonConf.config_microDST )
stream.appendLines( MicroDSTDiMuonConf.MicroDSTLines )
'''

__author__=['Jibo He']
__date__ = '30/09/2010'
__version__= '$Revision: 1.0 $'

__all__ = (
    'StrippingDiMuonConf'
    )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig

class DiMuonConf(object):
    
    __configuration_keys__ = (
        # DiMuon line
        'DiMuon_Prescale',
        'DiMuon_Postscale',
        
        'DiMuon_MuonPT',
        'DiMuon_MuonP',
        'DiMuon_MuonTRCHI2DOF', 
        'DiMuon_MinMass',
        'DiMuon_VCHI2PDOF',
        'DiMuon_PT',

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale',
        'DiMuonSameSign_Postscale',
        
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale',
        'DiMuonHighMass_Postscale',
        
        'DiMuonHighMass_MuonPT',
        'DiMuonHighMass_MuonP',
        'DiMuonHighMass_MuonTRCHI2DOF', 
        'DiMuonHighMass_MinMass',
        'DiMuonHighMass_VCHI2PDOF',
        'DiMuonHighMass_PT',

        # DiMuon High Mass line
        'DiMuonHighMassSameSign_Prescale',
        'DiMuonHighMassSameSign_Postscale',                

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale',
        'DiMuonLowMass_Postscale',
        
        'DiMuonLowMass_MuonPT',
        'DiMuonLowMass_MuonP',
        'DiMuonLowMass_MuonTRCHI2DOF', 
        'DiMuonLowMass_MinMass',
        'DiMuonLowMass_VCHI2PDOF',       
        'DiMuonLowMass_PT',

        # Tight Jpsi line
        'Jpsi2MuMu_Prescale',
        'Jpsi2MuMu_Postscale',
        
        'Jpsi2MuMu_MuonPT',
        'Jpsi2MuMu_MuonP',
        'Jpsi2MuMu_MuonTRCHI2DOF', 
        'Jpsi2MuMu_MinMass',
        'Jpsi2MuMu_MaxMass',
        'Jpsi2MuMu_VCHI2PDOF',
        'Jpsi2MuMu_PT'
        )
    
    config_default= {
        # DiMuon line
        'DiMuon_Prescale'                          :     0.1  ,
        'DiMuon_Postscale'                         :     1.   ,        
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  2900.   ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                : -1000.   ,  # MeV, no cut now 

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.01 ,
        'DiMuonSameSign_Postscale'                 :     1.   , 
        
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale'                  :     1.   ,
        'DiMuonHighMass_Postscale'                 :     1.   ,
        
        'DiMuonHighMass_MuonPT'                    :  1000.   , # MeV
        'DiMuonHighMass_MuonP'                     : -8000.   , 
        'DiMuonHighMass_MuonTRCHI2DOF'             :     5.   , 
        'DiMuonHighMass_MinMass'                   :  7500.   , # MeV
        'DiMuonHighMass_VCHI2PDOF'                 :    20.   ,
        'DiMuonHighMass_PT'                        : -1000.   , # MeV, no cut now 

        # DiMuon High Mass Same Sign line
        'DiMuonHighMassSameSign_Prescale'          :     1.   ,
        'DiMuonHighMassSameSign_Postscale'         :     1.   ,        

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale'                   :     1.   ,
        'DiMuonLowMass_Postscale'                  :     1.   ,
        
        'DiMuonLowMass_MuonPT'                     :   650.   ,  # MeV
        'DiMuonLowMass_MuonP'                      : -8000.   ,  # MeV, no cut now
        'DiMuonLowMass_MuonTRCHI2DOF'              :     5.   , 
        'DiMuonLowMass_MinMass'                    :   500.   ,  # MeV
        'DiMuonLowMass_VCHI2PDOF'                  :    20.   ,       
        'DiMuonLowMass_PT'                         : -1000.   ,  # MeV, no cut now

        # Jpsi2MuMu line
        'Jpsi2MuMu_Prescale'                       :     1.   ,
        'Jpsi2MuMu_Postscale'                      :     1.   ,
        
        'Jpsi2MuMu_MuonPT'                         :   650.   ,  # MeV
        'Jpsi2MuMu_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  3040.   ,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3140.   ,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  1500.0     # MeV  
        }

    config_microDST = {
        # DiMuon line
        'DiMuon_Prescale'                          :     1.   ,
        'DiMuon_Postscale'                         :     1.   ,        
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  2900.   ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                : -1000.   ,  # MeV, no cut now 

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.1  ,
        'DiMuonSameSign_Postscale'                 :     1.   , 
        
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale'                  :     1.   ,
        'DiMuonHighMass_Postscale'                 :     1.   ,
        
        'DiMuonHighMass_MuonPT'                    :  1000.   , # MeV
        'DiMuonHighMass_MuonP'                     : -8000.   , 
        'DiMuonHighMass_MuonTRCHI2DOF'             :     5.   , 
        'DiMuonHighMass_MinMass'                   :  7500.   , # MeV
        'DiMuonHighMass_VCHI2PDOF'                 :    20.   ,
        'DiMuonHighMass_PT'                        : -1000.   , # MeV, no cut now 

        # DiMuon High Mass Same Sign line
        'DiMuonHighMassSameSign_Prescale'          :     1.   ,
        'DiMuonHighMassSameSign_Postscale'         :     1.   ,        

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale'                   :     1.   ,
        'DiMuonLowMass_Postscale'                  :     1.   ,
        
        'DiMuonLowMass_MuonPT'                     :   650.   ,  # MeV
        'DiMuonLowMass_MuonP'                      : -8000.   ,  # MeV, no cut now
        'DiMuonLowMass_MuonTRCHI2DOF'              :     5.   , 
        'DiMuonLowMass_MinMass'                    :   500.   ,  # MeV
        'DiMuonLowMass_VCHI2PDOF'                  :    20.   ,       
        'DiMuonLowMass_PT'                         : -1000.   ,  # MeV, no cut now

        # Jpsi2MuMu line
        'Jpsi2MuMu_Prescale'                       :     1.   ,
        'Jpsi2MuMu_Postscale'                      :     1.   ,
        
        'Jpsi2MuMu_MuonPT'                         :   650.   ,  # MeV
        'Jpsi2MuMu_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  3040.   ,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3140.   ,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  1500.0     # MeV  
        }

    
    def __init__(self,
                name = None,
                config = None
                ):
        
        checkConfig(DiMuonConf.__configuration_keys__, config)

        # if name not set outside, set it to empty 
        if name == None:
            name = ""

        """
        DiMuon line
        """
        self.SelDiMuon = filterDiMuon( name + 'DiMuonInc' +'_SelMuMu',
                                       MuonPT        = config['DiMuon_MuonPT'],
                                       MuonP         = config['DiMuon_MuonP'],
                                       MuonTRCHI2DOF = config['DiMuon_MuonTRCHI2DOF'],
                                       MuMuMinMass   = config['DiMuon_MinMass'],
                                       MuMuVCHI2PDOF = config['DiMuon_VCHI2PDOF'],
                                       MuMuPT        = config['DiMuon_PT']
                                       )

        self.DiMuonLine = StrippingLine( name + 'DiMuon' + 'Line',
                                         prescale  = config['DiMuon_Prescale'],
                                         postscale = config['DiMuon_Postscale'],
                                         algos = [self.SelDiMuon]
                                         )

        """
        DiMuon same sign line
        """
        self.SelDiMuonSameSign = filterDiMuonSameSign( name + 'DiMuonSameSign' + '_SelMuMu',
                                                       MuonPT        = config['DiMuon_MuonPT'],
                                                       MuonP         = config['DiMuon_MuonP'],
                                                       MuonTRCHI2DOF = config['DiMuon_MuonTRCHI2DOF'],
                                                       MuMuMinMass   = config['DiMuon_MinMass'],
                                                       MuMuVCHI2PDOF = config['DiMuon_VCHI2PDOF'],
                                                       MuMuPT        = config['DiMuon_PT']
                                                       )
        self.DiMuonSameSignLine = StrippingLine( name + 'DiMuonSameSign' + 'Line',
                                                 prescale  = config['DiMuonSameSign_Prescale'],
                                                 postscale = config['DiMuonSameSign_Postscale'],
                                                 algos = [self.SelDiMuonSameSign]
                                                 )

        
        """
        DiMuon High Mass line
        """
        self.SelDiMuonHighMass = filterDiMuon( name + 'DiMuonHighMass' + '_SelMuMu',
                                               MuonPT        = config['DiMuonHighMass_MuonPT'],
                                               MuonP         = config['DiMuonHighMass_MuonP'],
                                               MuonTRCHI2DOF = config['DiMuonHighMass_MuonTRCHI2DOF'],
                                               MuMuMinMass   = config['DiMuonHighMass_MinMass'],
                                               MuMuVCHI2PDOF = config['DiMuonHighMass_VCHI2PDOF'],
                                               MuMuPT        = config['DiMuonHighMass_PT']
                                               )
        self.DiMuonHighMassLine = StrippingLine( name + 'DiMuonHighMass' + 'Line',
                                                 prescale  = config['DiMuonHighMass_Prescale'],
                                                 postscale = config['DiMuonHighMass_Postscale'],
                                                 algos = [self.SelDiMuonHighMass]
                                                 )


        
        """
        DiMuon High Mass Same Sign line
        """
        self.SelDiMuonHighMassSameSign = filterDiMuonSameSign( name + 'DiMuonHighMassSameSign' + '_SelMuMu',
                                                               MuonPT        = config['DiMuonHighMass_MuonPT'],
                                                               MuonP         = config['DiMuonHighMass_MuonP'],
                                                               MuonTRCHI2DOF = config['DiMuonHighMass_MuonTRCHI2DOF'],
                                                               MuMuMinMass   = config['DiMuonHighMass_MinMass'],
                                                               MuMuVCHI2PDOF = config['DiMuonHighMass_VCHI2PDOF'],
                                                               MuMuPT        = config['DiMuonHighMass_PT']
                                                               )
        
        self.DiMuonHighMassSameSignLine = StrippingLine( name + 'DiMuonHighMassSameSign' + 'Line',
                                                         prescale  = config['DiMuonHighMassSameSign_Prescale'],
                                                         postscale = config['DiMuonHighMassSameSign_Postscale'],
                                                         algos = [self.SelDiMuonHighMassSameSign]
                                                         )


        
        """
        DiMuon Low Mass line
        """
        self.SelDiMuonLowMass = filterDiMuon( name + 'DiMuonLowMass' + '_SelMuMu',
                                              MuonPT        = config['DiMuonLowMass_MuonPT'],
                                              MuonP         = config['DiMuonLowMass_MuonP'],
                                              MuonTRCHI2DOF = config['DiMuonLowMass_MuonTRCHI2DOF'],
                                              MuMuMinMass   = config['DiMuonLowMass_MinMass'],
                                              MuMuVCHI2PDOF = config['DiMuonLowMass_VCHI2PDOF'],
                                              MuMuPT        = config['DiMuonLowMass_PT']
                                              )
        self.DiMuonLowMassLine = StrippingLine( name + 'DiMuonLowMass' + 'Line',
                                                HLT = "HLT_PASS('Hlt2UnbiasedDiMuonLowMassDecision')",
                                                prescale  = config['DiMuonLowMass_Prescale'],
                                                postscale = config['DiMuonLowMass_Postscale'],
                                                algos = [ self.SelDiMuonLowMass ]
                                                )
        
        """
        Jpsi-> mumu tight line
        """
        self.SelJpsi2MuMu = filterJpsi2MuMu( name + 'Jpsi2MuMu' + '_SelMuMu',
                                             MuonPT        = config['Jpsi2MuMu_MuonPT'],
                                             MuonP         = config['Jpsi2MuMu_MuonP'],
                                             MuonTRCHI2DOF = config['Jpsi2MuMu_MuonTRCHI2DOF'],
                                             MuMuMinMass   = config['Jpsi2MuMu_MinMass'],
                                             MuMuMaxMass   = config['Jpsi2MuMu_MaxMass'],
                                             MuMuVCHI2PDOF = config['Jpsi2MuMu_VCHI2PDOF'],
                                             MuMuPT        = config['Jpsi2MuMu_PT']
                                             )
        self.Jpsi2MuMuLine = StrippingLine( name + 'Jpsi2MuMu' + 'Line',
                                            prescale  = config['Jpsi2MuMu_Prescale'],
                                            postscale = config['Jpsi2MuMu_Postscale'],
                                            algos = [self.SelJpsi2MuMu]
                                            )
        
        self.Lines = [
            self.DiMuonLine,
            self.DiMuonSameSignLine,
            self.DiMuonHighMassLine,
            self.DiMuonHighMassSameSignLine,
            self.DiMuonLowMassLine,
            self.Jpsi2MuMuLine
            ]
        
        self.MicroDSTLines = [
            self.DiMuonLine,
            self.DiMuonSameSignLine            
            ]


def filterDiMuon( name,
                  MuonPT,
                  MuonP,
                  MuonTRCHI2DOF,
                  MuMuMinMass,
                  MuMuVCHI2PDOF,
                  MuMuPT 
                  ):
    
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( "_Filter" + name,
                           Code = MuonCut + " & " + MuMuCut )

    return Selection( name,
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )


def filterDiMuonSameSign( name,
                          MuonPT,
                          MuonP,
                          MuonTRCHI2DOF,
                          MuMuMinMass,
                          MuMuVCHI2PDOF,
                          MuMuPT
                          ):
    
    _StdLooseDiMuonSameSign = DataOnDemand( Location = 'Phys/StdLooseDiMuonSameSign' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( "_Filter" + name,
                           Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name,
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuonSameSign ]
                      )


def filterJpsi2MuMu( name,
                     MuonPT,
                     MuonP,
                     MuonTRCHI2DOF,
                     MuMuMinMass,
                     MuMuMaxMass, 
                     MuMuVCHI2PDOF,
                     MuMuPT
                     ):
    
    _StdLooseJpsi2MuMu = DataOnDemand( Location = 'Phys/StdLooseJpsi2MuMu' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (MM < %(MuMuMaxMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( "_Filter" + name,
                           Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name,
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseJpsi2MuMu ]
                      )


def filterSignal( name,
                  MuonPT,
                  MuonP,
                  MuonTRCHI2DOF,
                  MuMuMinMass,
                  MuMuMaxMass, 
                  MuMuVCHI2PDOF,
                  MuMuPT
                  ):
        
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (MM < %(MuMuMaxMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( "_Filter" + name,
                           Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name,
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )

