# $Id: StrippingBs2Psi2SPhiPrescaledAndDetached.py,v 1.1 2010-06-30 12:53:17 jpalac Exp $
'''
Module for construction of Bs->Psi(2S)Phi pre-scaled and detached 
stripping Selections and StrippingLines.
Provides functions to build Bs, Psi2S, Phi selections.
Provides class Bs2Psi2SPhiPrescaledConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - Bs2Psi2SPhiConf
   - makeBs2JpsiPhi
   - makePhi2KK
   - makePhi2KKLoose
   - makeJpsi2MuMuLoose
'''

__author__ = ['Neus Lopez March']
__date__ = '16/2/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('Bs2Psi2SPhiMuMuConf',
           'makePsi2S2MuMu',
           'makeChK',
           'makePhi2KK',
           'makeKstar',
           'makeKsLoose',
           'makeKs',
           'makeBs2Psi2SPhiMuMu',
           'makeBu2Psi2SKMuMu',
           'makeBd2Psi2SKstarMuMu',
           'makeBd2Psi2SKsMuMu',
           'makeInclPsi2SToMuMu',
           'default_config'
           )

config_params = {'muPID':0.,
                 'Psi2SMassWin':60.,
                 'Psi2SADOCACHI2CUT':30.,
                 'Psi2SVFASPF':16,
                 'ChKTRCHI2DOF':5,
                 'ChKPID':0, #before -2
                 'PhiWin':20,
                 'PhiPT':1000, #before 500
                 'PhiVFASPF':25,    #new change, it was 16 before
                 'PhiMAXTRCHI2DOF':5,
                 'PhiMINTRCHI2DOF':-2,
                 'KstMassDown':  826,
                 'KstMassUp': 966,
                 'KstAPT':1200, #before 500
                 'KstVFASPF':16,
                 'KstTRCHI2DOF':4,
                 'KstPIDK': 0, # before -2
                 'KstPIDpi': 10, #new
                 'KsVFASPF':20,
                 'KsBPVDLS':5,
                 'incl_LinePrescale':0.1, #before 0.1
                 'incl_LinePostscale':1,
                 'incl_LineTisTosHlt1':{ "Hlt1DiMuonHighMassDecision%TOS" : 0 },
                 'incl_LineTisTosHlt2':{ "Hlt2DiMuonPsi2SDecision%TOS" : 0 },
                 'BPVLTIME_detached':0.2, #before 0.15
                 'incl_DetachedLinePrescale':1, #before 0.5
                 'incl_DetachedLinePostscale':1,
                 'incl_DetachedLineTisTosHlt1':{ "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                 "Hlt1TrackMuonDecision%TOS" : 0,
                                                 "Hlt1TrackAllL0Decision%TOS" : 0 },
                 'incl_DetachedLineTisTosHlt2':{ "Hlt2DiMuonDetachedHeavyDecision%TOS" : 0,
                                                 "Hlt2DiMuonDetachedPsi2SDecision%TOS" : 0 },
                 'incl_DetachedCut':"BPVDLS > 3",
                 'BsMassCutDownPre':5000, #before 5000
                 'BsMassCutUpPre':5650,  #before 5650
                 'BsMassCutDownPost':5150, #before 5100
                 'BsMassCutUpPost':5550,  #before 5550
                 'BsVCHI2PDOF':20,     #new change, it was 10 before
                 'sig_PrescaledLinePrescale':0.5, #before 1.
                 'sig_PrescaledLinePostscale':1,
                 'sig_DetachedLinePrescale':1,
                 'sig_DetachedLinePostscale':1,
                 'BPVLTIME_unbiased': None,
                 'MINTREEPT':1000, #before 1000
                 'MINTREEPT2':1500, #before 1000
#                 'sig_UnbiasedLinePrescale':1,
#                 'sig_UnbiasedLinePostscale':1,
                 'ChKPT':500,
                 'K_PrescaledLinePrescale':0.1, #before 0.3 #before 0.2
                 'K_PrescaledLinePostscale':1,
                 'K_DetachedLinePrescale':1, #before 0.1
                 'K_DetachedLinePostscale':1,
#                 'K_UnbiasedLinePrescale':0.7, #before 1.
#                 'K_UnbiasedLinePostscale':1,
                 'KstarPT': 2,
                 'Kstar_PrescaledLinePrescale':0.07, #before 0.1
                 'Kstar_PrescaledLinePostscale':1,
                 'Kstar_DetachedLinePrescale':1, #before 1
                 'Kstar_DetachedLinePostscale':1,
#                 'Kstar_UnbiasedLinePrescale':0.6, #before 1
#                 'Kstar_UnbiasedLinePostscale':1,
                 'BKsVCHI2PDOF':10,
                 'Ks_PrescaledLinePrescale':1,
                 'Ks_PrescaledLinePostscale':1,
                 'Ks_DetachedLinePrescale':1,
                 'Ks_DetachedLinePostscale':1
#                 'Ks_UnbiasedLinePrescale':1,
#                 'Ks_UnbiasedLinePostscale':1
                 }

default_config = {
    'NAME'              : 'BetaSPsi2SMuMu',
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'Bs2Psi2SPhiMuMuConf',
    'CONFIG'    : config_params,
    'STREAMS' : ['Leptonic'] }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles import StdAllLoosePions
from StandardParticles import StdAllLooseMuons
from StandardParticles import StdAllLooseKaons
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


class Bs2Psi2SPhiMuMuConf(LineBuilder) :
  
    __configuration_keys__ = ('muPID',
                              'Psi2SMassWin',
                              'Psi2SADOCACHI2CUT',
                              'Psi2SVFASPF',
                              'ChKTRCHI2DOF',
                              'ChKPID',
                              'PhiWin',
                              'PhiPT',
                              'PhiVFASPF',
                              'PhiMAXTRCHI2DOF',
                              'PhiMINTRCHI2DOF',
                              'KstMassDown',
                              'KstMassUp', 
                              'KstAPT', 
                              'KstVFASPF', 
                              'KstTRCHI2DOF',
                              'KstPIDK',
                              'KstPIDpi',
                              'KsVFASPF',
                              'KsBPVDLS',
                              'incl_LinePrescale',
                              'incl_LinePostscale',
                              'incl_LineTisTosHlt1',
                              'incl_LineTisTosHlt2',
                              'BPVLTIME_detached',
                              'incl_DetachedLinePrescale',
                              'incl_DetachedLinePostscale',
                              'incl_DetachedLineTisTosHlt1',
                              'incl_DetachedLineTisTosHlt2',
                              'incl_DetachedCut',
                              'BsMassCutDownPre',
                              'BsMassCutUpPre',
                              'BsMassCutDownPost',
                              'BsMassCutUpPost',
                              'BsVCHI2PDOF',
                              'sig_PrescaledLinePrescale',
                              'sig_PrescaledLinePostscale',
                              'sig_DetachedLinePrescale',
                              'sig_DetachedLinePostscale',
                              'BPVLTIME_unbiased',
                              'MINTREEPT',
                              'MINTREEPT2',
#                              'sig_UnbiasedLinePrescale',
#                              'sig_UnbiasedLinePostscale',
                              'ChKPT',
                              'K_PrescaledLinePrescale',
                              'K_PrescaledLinePostscale',
                              'K_DetachedLinePrescale',
                              'K_DetachedLinePostscale',
#                              'K_UnbiasedLinePrescale',
#                              'K_UnbiasedLinePostscale',
                              'KstarPT',
                              'Kstar_PrescaledLinePrescale',
                              'Kstar_PrescaledLinePostscale',
                              'Kstar_DetachedLinePrescale',
                              'Kstar_DetachedLinePostscale',
#                              'Kstar_UnbiasedLinePrescale',
#                              'Kstar_UnbiasedLinePostscale',
                              'BKsVCHI2PDOF',
                              'Ks_PrescaledLinePrescale',
                              'Ks_PrescaledLinePostscale',
                              'Ks_DetachedLinePrescale',
                              'Ks_DetachedLinePostscale'
#                              'Ks_UnbiasedLinePrescale',
#                              'Ks_UnbiasedLinePostscale'
                              )

    def __init__(self, name, config) :

        self.name = name
        LineBuilder.__init__(self, name, config)

        #print 'self.name ' + self.name  

        incl = self.name +'_InclPsi2SToMuMu'
        #print 'inclusiveline ' + incl  
        incl_detached_name = incl + 'Detached'

        sig = self.name +'_Bs2Psi2SPhiMuMu'
        sig_unbiased_name = sig + 'Unbiased'
        sig_prescaled_name = sig + 'Prescaled'
        sig_detached_name = sig + 'Detached'

        K = self.name +'_Bu2Psi2SKMuMu'
        K_unbiased_name = K + 'Unbiased'
        K_prescaled_name = K + 'Prescaled'
        K_detached_name = K + 'Detached'

        Kstar = self.name + '_Bd2Psi2SKstarMuMu'
        Kstar_unbiased_name = Kstar + 'Unbiased'
        Kstar_prescaled_name = Kstar + 'Prescaled'
        Kstar_detached_name = Kstar + 'Detached'

        Ks = self.name + '_Bd2Psi2SKsMuMu'
        Ks_unbiased_name = Ks + 'Unbiased'
        Ks_prescaled_name = Ks + 'Prescaled'
        Ks_detached_name = Ks + 'Detached'


        self.selPsi2S2MuMu = makePsi2S2MuMu( self.name + '_Psi2SToMuMu',
                                             muPID = config['muPID'],
                                             Psi2SMassWin = config['Psi2SMassWin'],
                                             Psi2SADOCACHI2CUT = config['Psi2SADOCACHI2CUT'],
                                             Psi2SVFASPF = config['Psi2SVFASPF']
                                             )

        self.selChargedK = makeChK(self.name + '_ChKForPsi2SToMuMu' ,
                                   ChKTRCHI2DOF = config['ChKTRCHI2DOF'],
                                   ChKPID = config['ChKPID']
                                   ) 
        
        
        self.selPhi2KK = makePhi2KK(self.name + '_PhiForPsi2SToMuMu' ,
                                     PhiWin = config['PhiWin'],
                                     PhiPT = config['PhiPT'],
                                     PhiVFASPF = config['PhiVFASPF'],
                                     PhiMAXTRCHI2DOF = config['PhiMAXTRCHI2DOF'],
                                     PhiMINTRCHI2DOF = config['PhiMINTRCHI2DOF']
                                     )


        self.selKstar = makeKstar( self.name + '_KstarForPsi2SToMuMu',
                                  KstMassDown = config['KstMassDown'],
                                  KstMassUp = config['KstMassUp'],
                                  KstAPT = config['KstAPT'],
                                  KstVFASPF = config['KstVFASPF'],
                                  KstTRCHI2DOF = config['KstTRCHI2DOF'],
                                  KstPIDK = config['KstPIDK'],
                                  KstPIDpi = config['KstPIDpi']
                                  ) 

        self.selKsLoose = makeKsLoose( self.name + '_KsLooseForPsi2SToMuMu') 
        
        self.selKs = makeKs( self.name + '_KsForPsi2SToMuMu',
                            KsLooseSel = self.selKsLoose,
                            KsVFASPF = config['KsVFASPF'],
                            KsBPVDLS = config['KsBPVDLS']
                            )

####################
### Inlusive psi(2S). We keep it for as long as we can.
####################

        self.InclPsi2SToMuMu = makeInclPsi2SToMuMu(incl,  
                                                   Psi2SSel = self.selPsi2S2MuMu,
                                                   TisTosHlt1 = config['incl_LineTisTosHlt1'],
                                                   TisTosHlt2 = config['incl_LineTisTosHlt2']
                                                   )                                                         
        
        self.InclPsi2SToMuMu_line = StrippingLine(incl + "Line",
                                                  prescale = config['incl_LinePrescale'],
                                                  postscale = config['incl_LinePostscale'],
                                                  selection = self.InclPsi2SToMuMu,
                                                  EnableFlavourTagging = True, 
                                                  MDSTFlag = True )
        
        self.InclPsi2SToMuMuDetached = makeInclPsi2SToMuMu(incl_detached_name,  
                                                           Psi2SSel = self.selPsi2S2MuMu,
                                                           TisTosHlt1 = config['incl_DetachedLineTisTosHlt1'],
                                                           TisTosHlt2 = config['incl_DetachedLineTisTosHlt2'],
                                                           Cuts = config['incl_DetachedCut']
                                                           )                                                         
        
        self.InclPsi2SToMuMu_detached_line = StrippingLine(incl_detached_name + "Line",
                                                            prescale = config['incl_DetachedLinePrescale'],
                                                            postscale = config['incl_DetachedLinePostscale'],
                                                            selection = self.InclPsi2SToMuMuDetached,
                                                            EnableFlavourTagging = True, 
                                                            MDSTFlag = True )
        
        
#####################
### Bs->Psi(2S)Phi ## 
#####################
        self.selBs2Psi2SPhiPrescaled = makeBs2Psi2SPhiMuMu(sig_prescaled_name,  
                                                           Psi2SSel = self.selPsi2S2MuMu, 
                                                           PhiSel = self.selPhi2KK,
                                                           BsMassCutDownPre = config['BsMassCutDownPre'],
                                                           BsMassCutUpPre = config['BsMassCutUpPre'],
                                                           BsMassCutDownPost = config['BsMassCutDownPost'],
                                                           BsMassCutUpPost = config['BsMassCutUpPost'],
                                                           BsVCHI2PDOF = config['BsVCHI2PDOF']
                                                           )
        
        self.Bs2Psi2SPhi_prescaled_line = StrippingLine(sig_prescaled_name + "Line",
                                                        prescale = config['sig_PrescaledLinePrescale'],
                                                        postscale = config['sig_PrescaledLinePostscale'],
                                                        selection = self.selBs2Psi2SPhiPrescaled,
                                                        EnableFlavourTagging = True, 
                                                        MDSTFlag = True )
        
        self.selBs2Psi2SPhiDetached = makeBs2Psi2SPhiMuMu(sig_detached_name,  
                                                           Psi2SSel = self.selPsi2S2MuMu, 
                                                           PhiSel = self.selPhi2KK,
                                                           BsMassCutDownPre = config['BsMassCutDownPre'],
                                                           BsMassCutUpPre = config['BsMassCutUpPre'],
                                                           BsMassCutDownPost = config['BsMassCutDownPost'],
                                                           BsMassCutUpPost = config['BsMassCutUpPost'],
                                                           BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                           BPVLTIME = config['BPVLTIME_detached']
                                                           )
        
        self.Bs2Psi2SPhi_detached_line = StrippingLine(sig_detached_name + "Line",
                                                        prescale = config['sig_DetachedLinePrescale'],
                                                        postscale = config['sig_DetachedLinePostscale'],
                                                        selection = self.selBs2Psi2SPhiDetached,
                                                        EnableFlavourTagging = True, 
                                                        MDSTFlag = True )
        
        self.selBs2Psi2SPhiUnbiased = makeBs2Psi2SPhiMuMu(sig_unbiased_name,  
                                                          Psi2SSel = self.selPsi2S2MuMu, 
                                                          PhiSel = self.selPhi2KK,
                                                          BsMassCutDownPre = config['BsMassCutDownPre'],
                                                          BsMassCutUpPre = config['BsMassCutUpPre'],
                                                          BsMassCutDownPost = config['BsMassCutDownPost'],
                                                          BsMassCutUpPost = config['BsMassCutUpPost'],
                                                          BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                          BPVLTIME = config['BPVLTIME_unbiased'],
                                                          MINTREEPT = config['MINTREEPT']
                                                          )
        
#        self.Bs2Psi2SPhi_unbiased_line = StrippingLine(sig_unbiased_name + "Line",
#                                                       prescale = config['sig_UnbiasedLinePrescale'],
#                                                       postscale = config['sig_UnbiasedLinePostscale'],
#                                                       selection = self.selBs2Psi2SPhiUnbiased
#                                                       )
        
#####################
### Bu->Psi(2S)K+ ###
#####################
        self.selBu2Psi2SKPrescaled = makeBu2Psi2SKMuMu(K_prescaled_name,  
                                                       Psi2SSel = self.selPsi2S2MuMu, 
                                                       ChKSel = self.selChargedK,
                                                       ChKPT = config['ChKPT'],
                                                       BsMassCutDownPre = config['BsMassCutDownPre'],
                                                       BsMassCutUpPre = config['BsMassCutUpPre'],
                                                       BsMassCutDownPost = config['BsMassCutDownPost'],
                                                       BsMassCutUpPost = config['BsMassCutUpPost'],
                                                       BsVCHI2PDOF = config['BsVCHI2PDOF']
                                                       )
        
        self.Bu2Psi2SK_prescaled_line = StrippingLine(K_prescaled_name + "Line",
                                                      prescale = config['K_PrescaledLinePrescale'],
                                                      postscale = config['K_PrescaledLinePostscale'],
                                                      selection = self.selBu2Psi2SKPrescaled,
                                                      EnableFlavourTagging = True, 
                                                      MDSTFlag = True )
        
        self.selBu2Psi2SKDetached = makeBu2Psi2SKMuMu(K_detached_name,  
                                                       Psi2SSel = self.selPsi2S2MuMu, 
                                                       ChKSel = self.selChargedK,
                                                       ChKPT = config['ChKPT'],
                                                       BsMassCutDownPre = config['BsMassCutDownPre'],
                                                       BsMassCutUpPre = config['BsMassCutUpPre'],
                                                       BsMassCutDownPost = config['BsMassCutDownPost'],
                                                       BsMassCutUpPost = config['BsMassCutUpPost'],
                                                       BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                       BPVLTIME = config['BPVLTIME_detached']
                                                       )
        
        self.Bu2Psi2SK_detached_line = StrippingLine(K_detached_name + "Line",
                                                      prescale = config['K_DetachedLinePrescale'],
                                                      postscale = config['K_DetachedLinePostscale'],
                                                      selection = self.selBu2Psi2SKDetached,
                                                      EnableFlavourTagging = True, 
                                                      MDSTFlag = True )
        
        self.selBu2Psi2SKUnbiased = makeBu2Psi2SKMuMu(K_unbiased_name,  
                                                      Psi2SSel = self.selPsi2S2MuMu, 
                                                      ChKSel = self.selChargedK,
                                                      ChKPT = config['ChKPT'],
                                                      BsMassCutDownPre = config['BsMassCutDownPre'],
                                                      BsMassCutUpPre = config['BsMassCutUpPre'],
                                                      BsMassCutDownPost = config['BsMassCutDownPost'],
                                                      BsMassCutUpPost = config['BsMassCutUpPost'],
                                                      BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                      BPVLTIME = config['BPVLTIME_unbiased'],
                                                      MINTREEPT2 = config['MINTREEPT2']
                                                      )
        
#        self.Bu2Psi2SK_unbiased_line = StrippingLine(K_unbiased_name + "Line",
#                                                     prescale = config['K_UnbiasedLinePrescale'],
#                                                     postscale = config['K_UnbiasedLinePostscale'],
#                                                     selection = self.selBu2Psi2SKUnbiased
#                                                     )

#####################
### B0->Psi(2S)K*  ##
#####################

        self.selBd2Psi2SKstarPrescaled = makeBd2Psi2SKstarMuMu(Kstar_prescaled_name,  
                                                               Psi2SSel = self.selPsi2S2MuMu, 
                                                               KstarSel = self.selKstar,
                                                               BsMassCutDownPre = config['BsMassCutDownPre'],
                                                               BsMassCutUpPre = config['BsMassCutUpPre'],
                                                               BsMassCutDownPost = config['BsMassCutDownPost'],
                                                               BsMassCutUpPost = config['BsMassCutUpPost'],
                                                               BsVCHI2PDOF = config['BsVCHI2PDOF']
                                                               )
        
        self.Bd2Psi2SKstar_prescaled_line = StrippingLine(Kstar_prescaled_name + "Line",
                                                          prescale = config['Kstar_PrescaledLinePrescale'],
                                                          postscale = config['Kstar_PrescaledLinePostscale'],
                                                          selection = self.selBd2Psi2SKstarPrescaled,
                                                          EnableFlavourTagging = True, 
                                                          MDSTFlag = True )
        
        self.selBd2Psi2SKstarDetached = makeBd2Psi2SKstarMuMu(Kstar_detached_name,  
                                                               Psi2SSel = self.selPsi2S2MuMu, 
                                                               KstarSel = self.selKstar,
                                                               BsMassCutDownPre = config['BsMassCutDownPre'],
                                                               BsMassCutUpPre = config['BsMassCutUpPre'],
                                                               BsMassCutDownPost = config['BsMassCutDownPost'],
                                                               BsMassCutUpPost = config['BsMassCutUpPost'],
                                                               BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                               BPVLTIME = config['BPVLTIME_detached']
                                                               )
        
        self.Bd2Psi2SKstar_detached_line = StrippingLine(Kstar_detached_name + "Line",
                                                          prescale = config['Kstar_DetachedLinePrescale'],
                                                          postscale = config['Kstar_DetachedLinePostscale'],
                                                          selection = self.selBd2Psi2SKstarDetached,
                                                          EnableFlavourTagging = True, 
                                                          MDSTFlag = True )
        
        self.selBd2Psi2SKstarUnbiased = makeBd2Psi2SKstarMuMu(Kstar_unbiased_name,  
                                                              Psi2SSel = self.selPsi2S2MuMu, 
                                                              KstarSel = self.selKstar,
                                                              BsMassCutDownPre = config['BsMassCutDownPre'],
                                                              BsMassCutUpPre = config['BsMassCutUpPre'],
                                                              BsMassCutDownPost = config['BsMassCutDownPost'],
                                                              BsMassCutUpPost = config['BsMassCutUpPost'],
                                                              BsVCHI2PDOF = config['BsVCHI2PDOF'],
                                                              BPVLTIME = config['BPVLTIME_unbiased'],
                                                              MINTREEPT2 = config['MINTREEPT2'],
                                                              KstarPT = config['KstarPT']
                                                              )
        
#        self.Bd2Psi2SKstar_unbiased_line = StrippingLine(Kstar_unbiased_name + "Line",
#                                                         prescale = config['Kstar_UnbiasedLinePrescale'],
#                                                         postscale = config['Kstar_UnbiasedLinePostscale'],
#                                                         selection = self.selBd2Psi2SKstarUnbiased
#                                                         )
        
#####################
### Bd->Psi(2S)KS ###
#####################

        self.selBd2Psi2SKsPrescaled = makeBd2Psi2SKsMuMu(Ks_prescaled_name,  
                                                         Psi2SSel = self.selPsi2S2MuMu, 
                                                         KsSel = self.selKs,
                                                         BsMassCutDownPre = config['BsMassCutDownPre'],
                                                         BsMassCutUpPre = config['BsMassCutUpPre'],
                                                         BsMassCutDownPost = config['BsMassCutDownPost'],
                                                         BsMassCutUpPost = config['BsMassCutUpPost'],
                                                         BKsVCHI2PDOF = config['BKsVCHI2PDOF']
                                                         )
        
        self.Bd2Psi2SKs_prescaled_line = StrippingLine(Ks_prescaled_name + "Line",
                                                       prescale = config['Ks_PrescaledLinePrescale'],
                                                       postscale = config['Ks_PrescaledLinePostscale'],
                                                       selection = self.selBd2Psi2SKsPrescaled,
                                                       EnableFlavourTagging = True, 
                                                       MDSTFlag = True )
        
        self.selBd2Psi2SKsDetached = makeBd2Psi2SKsMuMu(Ks_detached_name,  
                                                         Psi2SSel = self.selPsi2S2MuMu, 
                                                         KsSel = self.selKs,
                                                         BsMassCutDownPre = config['BsMassCutDownPre'],
                                                         BsMassCutUpPre = config['BsMassCutUpPre'],
                                                         BsMassCutDownPost = config['BsMassCutDownPost'],
                                                         BsMassCutUpPost = config['BsMassCutUpPost'],
                                                         BKsVCHI2PDOF = config['BKsVCHI2PDOF'],
                                                         BPVLTIME = config['BPVLTIME_detached']
                                                         )
        
        self.Bd2Psi2SKs_detached_line = StrippingLine(Ks_detached_name + "Line",
                                                       prescale = config['Ks_DetachedLinePrescale'],
                                                       postscale = config['Ks_DetachedLinePostscale'],
                                                       selection = self.selBd2Psi2SKsDetached,
                                                       EnableFlavourTagging = True, 
                                                       MDSTFlag = True )
        
        self.selBd2Psi2SKsUnbiased = makeBd2Psi2SKsMuMu(Ks_unbiased_name,  
                                                        Psi2SSel = self.selPsi2S2MuMu, 
                                                        KsSel = self.selKs,
                                                        BsMassCutDownPre = config['BsMassCutDownPre'],
                                                        BsMassCutUpPre = config['BsMassCutUpPre'],
                                                        BsMassCutDownPost = config['BsMassCutDownPost'],
                                                        BsMassCutUpPost = config['BsMassCutUpPost'],
                                                        BKsVCHI2PDOF = config['BKsVCHI2PDOF'],
                                                        BPVLTIME = config['BPVLTIME_unbiased'],
                                                        MINTREEPT = config['MINTREEPT']
                                                        )
        
#        self.Bd2Psi2SKs_unbiased_line = StrippingLine(Ks_unbiased_name + "Line",
#                                                      prescale = config['Ks_UnbiasedLinePrescale'],
#                                                      postscale = config['Ks_UnbiasedLinePostscale'],
#                                                      selection = self.selBd2Psi2SKsUnbiased
#                                                      )
        

        self.registerLine(self.InclPsi2SToMuMu_line)
        self.registerLine(self.InclPsi2SToMuMu_detached_line)
              
        self.registerLine(self.Bs2Psi2SPhi_prescaled_line)
        self.registerLine(self.Bs2Psi2SPhi_detached_line)
#        self.registerLine(self.Bs2Psi2SPhi_unbiased_line)
        
#        self.registerLine(self.Bu2Psi2SK_unbiased_line)
        self.registerLine(self.Bu2Psi2SK_detached_line)
        self.registerLine(self.Bu2Psi2SK_prescaled_line)
        
#        self.registerLine(self.Bd2Psi2SKstar_unbiased_line)
        self.registerLine(self.Bd2Psi2SKstar_detached_line)
        self.registerLine(self.Bd2Psi2SKstar_prescaled_line)
        
#        self.registerLine(self.Bd2Psi2SKs_unbiased_line)
        self.registerLine(self.Bd2Psi2SKs_detached_line)
        self.registerLine(self.Bd2Psi2SKs_prescaled_line)
        

def makePsi2S2MuMu(name,
                   muPID, # 0
                   Psi2SMassWin, #60
                   Psi2SADOCACHI2CUT, # 30
                   Psi2SVFASPF #16
                   ) :


    _stdLooseMuons = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    _daughtersCuts = {"mu+":"PIDmu > %(muPID)s"% locals(),"mu-":"PIDmu>%(muPID)s"% locals()}
    _preVertexCuts = "(ADAMASS('psi(2S)')<%(Psi2SMassWin)s*MeV) & (ADOCACHI2CUT( %(Psi2SADOCACHI2CUT)s ,''))" % locals()
    _motherCuts = "(VFASPF(VCHI2/VDOF) < %(Psi2SVFASPF)s) & (MFIT)" % locals()
    _Psi2S2MuMu = CombineParticles( DecayDescriptor = "psi(2S) -> mu+ mu-",
                                    DaughtersCuts = _daughtersCuts,
                                    CombinationCut = _preVertexCuts,
                                    MotherCut = _motherCuts,
                                    ReFitPVs = True
                                    )
#    print ' makePsi2S2MuMu ', name, 'MotherCuts:', _motherCuts, 'DaughtersCuts:', _daughtersCuts, 'preVertexCuts:', _preVertexCuts
    return Selection ( name,
                       Algorithm = _Psi2S2MuMu,
                       RequiredSelections = [_stdLooseMuons])


def makeChK(name,
            ChKTRCHI2DOF,#<5 
            ChKPID #>-2
            ) :
    
    _stdChK = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
    _code = "(TRCHI2DOF < %(ChKTRCHI2DOF)s) & (PIDK > %(ChKPID)s)"  % locals()
    _ChKFilter = FilterDesktop(Code = _code)

#    print ' makeChK ', name, 'Code ', _code
    return Selection (name,
                      Algorithm = _ChKFilter,
                      RequiredSelections = [_stdChK])

def makePhi2KK(name,
               PhiWin, # 20
               PhiPT, # 500
               PhiVFASPF, # 16
               PhiMAXTRCHI2DOF, # <5
               PhiMINTRCHI2DOF # >-2
               ) :

    _stdPhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK/Particles")   
    _code = "(ADMASS('phi(1020)') < %(PhiWin)s) & (PT > %(PhiPT)s *MeV) & (VFASPF(VCHI2) < %(PhiVFASPF)s) & (MAXTREE('K+'==ABSID, TRCHI2DOF) < %(PhiMAXTRCHI2DOF)s) & (MINTREE('K+'==ABSID, PIDK) > %(PhiMINTRCHI2DOF)s)" % locals()
    _phiFilter = FilterDesktop(Code = _code)

#    print ' makePhi2KK ', name, 'Code ', _code
    return Selection (name,
                      Algorithm = _phiFilter,
                      RequiredSelections = [_stdPhi2KK])



def makeKstar(name,
              KstMassDown, # 826
              KstMassUp, #966
              KstAPT, #>500
              KstVFASPF, #<16
              KstTRCHI2DOF, # 4
              KstPIDK, # -2
              KstPIDpi # -2
              ) :


    _stdKstar = DataOnDemand(Location = "Phys/StdLooseKstar2Kpi/Particles")   
    _code = "(in_range(%(KstMassDown)s,M,%(KstMassUp)s)) & (PT > %(KstAPT)s*MeV) " \
                        "& (VFASPF(VCHI2) < %(KstVFASPF)s)" \
                        "& (MAXTREE('K+'==ABSID,  TRCHI2DOF) < %(KstTRCHI2DOF)s )" \
                        "& (MAXTREE('pi-'==ABSID, TRCHI2DOF) < %(KstTRCHI2DOF)s )" \
                        "& (MINTREE('K+'==ABSID, PIDK) > %(KstPIDK)s)" \
                        "& (MINTREE('pi-'==ABSID, PIDK) < %(KstPIDpi)s)" % locals()

    _KstarFilter = FilterDesktop(Code = _code)

#    print ' makePhi2KK ', name, 'Code ', _code
    return Selection (name,
                      Algorithm = _KstarFilter,
                      RequiredSelections = [_stdKstar])




def makeKsLoose( name ):
    return MergedSelection(name,
                           RequiredSelections = [DataOnDemand(Location = "Phys/StdLooseKsDD/Particles"),
                                                 DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")] )

def makeKs(name,
           KsLooseSel,
           KsVFASPF, #20
           KsBPVDLS #5
           ):
    _code = "(VFASPF(VCHI2)<%(KsVFASPF)s) & (BPVDLS>%(KsBPVDLS)s)" % locals()
    _KsFilter = FilterDesktop(Code = _code)

#    print ' makeKs ', name, 'Code ', _code
    return Selection (name,
                      Algorithm = _KsFilter,
                      RequiredSelections = [KsLooseSel])



####################
### Inlusive psi(2S). We keep it for as long as we can.
####################
def makeInclPsi2SToMuMu(name,
                        Psi2SSel,
                        TisTosHlt1,
                        TisTosHlt2,
                        Cuts = None
                        ) :
    _code = "ALL"
    if Cuts != None :
        _code = Cuts

    _InclFilter = FilterDesktop(Code = _code)

    InclPsi2SToMuMu = Selection (name,
                                 Algorithm = _InclFilter,
                                 RequiredSelections = [Psi2SSel])
    
    InclPsi2SToMuMuHlt1TOS = filterTisTos( name+"Hlt1TOS",
                                           DiMuonInput = InclPsi2SToMuMu,
                                           myTisTosSpecs = TisTosHlt1
                                           )
    
    InclPsi2SToMuMuTOS = filterTisTos( name+"TOS",
                                       DiMuonInput = InclPsi2SToMuMuHlt1TOS,
                                       myTisTosSpecs = TisTosHlt2
                                       )
    
    return InclPsi2SToMuMuTOS



#####################
### Bs->Psi(2S)Phi ##
#####################

def makeBs2Psi2SPhiMuMu(name,
                        Psi2SSel,
                        PhiSel,
                        BsMassCutDownPre,
                        BsMassCutUpPre,
                        BsMassCutDownPost,
                        BsMassCutUpPost,
                        BsVCHI2PDOF,
                        BPVLTIME = None,
                        MINTREEPT = None
                        ) :
    
    _preVertexCuts = "in_range(%(BsMassCutDownPre)s,AM,%(BsMassCutUpPre)s)" % locals()
    _motherCuts = "in_range(%(BsMassCutDownPost)s,M,%(BsMassCutUpPost)s) & (VFASPF(VCHI2PDOF)<%(BsVCHI2PDOF)s)" % locals()
    
    if BPVLTIME != None :
        _motherCuts += "& (BPVLTIME()> %(BPVLTIME)s*ps)" % locals()
#    print 'makeBs2Psi2SPhiMuMu first', name, 'MotherCuts:', _motherCuts,  'preVertexCuts:', _preVertexCuts


    if MINTREEPT != None :
        _motherCuts += "& (MINTREE('phi(1020)'==ABSID, PT) > %(MINTREEPT)s*MeV)" % locals()
#    print 'makeBs2Psi2SPhiMuMu second', name, 'MotherCuts:', _motherCuts, 'preVertexCuts:', _preVertexCuts
    
    _Bs = CombineParticles( DecayDescriptor = "B_s0 -> psi(2S) phi(1020)",
                            CombinationCut = _preVertexCuts,
                            MotherCut = _motherCuts,
                            ReFitPVs = True
                            )

    return Selection ( name,
                       Algorithm = _Bs,
                       #RequiredSelections = [PhiSel, Psi2SSel])
                       RequiredSelections = [Psi2SSel, PhiSel])


        
#####################
### Bu->Psi(2S)K+ ###
#####################

def makeBu2Psi2SKMuMu(name,
                      Psi2SSel,
                      ChKSel,
                      ChKPT, #500
                      BsMassCutDownPre,
                      BsMassCutUpPre,
                      BsMassCutDownPost,
                      BsMassCutUpPost,
                      BsVCHI2PDOF,
                      BPVLTIME = None,
                      MINTREEPT2 = None #1000
                      ) :

    _daughtersCuts= {"K+": "(PT > %(ChKPT)s)" % locals()}
    _preVertexCuts = "in_range(%(BsMassCutDownPre)s,AM,%(BsMassCutUpPre)s)" % locals()
    _motherCuts = "in_range(%(BsMassCutDownPost)s,M,%(BsMassCutUpPost)s) & (VFASPF(VCHI2PDOF)<%(BsVCHI2PDOF)s)" % locals()
    
    if BPVLTIME != None :
        _motherCuts += "& (BPVLTIME()> %(BPVLTIME)s*ps)" % locals()
#    print 'makeBu2Psi2SKMuMu first', name, 'MotherCuts:', _motherCuts, 'DaughtersCuts:', _daughtersCuts, 'preVertexCuts:', _preVertexCuts


    if MINTREEPT2 != None :
        _motherCuts += "& (MINTREE('K+'==ABSID, PT) > %(MINTREEPT2)s*MeV)" % locals()
#    print 'makeBu2Psi2SKMuMu second', name, 'MotherCuts:', _motherCuts, 'DaughtersCuts:', _daughtersCuts, 'preVertexCuts:', _preVertexCuts
    
    _Bs = CombineParticles( DecayDescriptor = "[B+ -> psi(2S) K+]cc",
                            DaughtersCuts = _daughtersCuts,
                            CombinationCut = _preVertexCuts,
                            MotherCut = _motherCuts,
                            ReFitPVs = True
                            )

    return Selection ( name,
                       Algorithm = _Bs,
#                       RequiredSelections = [ChKSel, Psi2SSel])
                       RequiredSelections = [Psi2SSel, ChKSel])




#####################
### B0->Psi(2S)K*  ##
#####################

def makeBd2Psi2SKstarMuMu(name,
                          Psi2SSel,
                          KstarSel,
                          BsMassCutDownPre,
                          BsMassCutUpPre,
                          BsMassCutDownPost,
                          BsMassCutUpPost,
                          BsVCHI2PDOF,
                          BPVLTIME = None,
                          MINTREEPT2 = None, #1000
                          KstarPT = None # > 2
                          ) :

    _preVertexCuts = "in_range(%(BsMassCutDownPre)s,AM,%(BsMassCutUpPre)s)" % locals()
    _motherCuts = "in_range(%(BsMassCutDownPost)s,M,%(BsMassCutUpPost)s) & (VFASPF(VCHI2PDOF)<%(BsVCHI2PDOF)s)" % locals()
    
    if BPVLTIME != None :
        _motherCuts += "& (BPVLTIME()> %(BPVLTIME)s*ps)" % locals()
#    print 'makeBd2Psi2SKstarMuMu first', name, 'MotherCuts:', _motherCuts,  'preVertexCuts:', _preVertexCuts


    if MINTREEPT2 != None :
        _motherCuts += "& (PT>%(KstarPT)s*GeV) & (MINTREE('K*(892)0'==ABSID, PT)> %(MINTREEPT2)s*MeV)" % locals()
#    print 'makeBd2Psi2SKstarMuMu second', name, 'MotherCuts:', _motherCuts,  'preVertexCuts:', _preVertexCuts

    
    _Bs = CombineParticles( DecayDescriptor = "[B0 -> psi(2S) K*(892)0]cc",
                            CombinationCut = _preVertexCuts,
                            MotherCut = _motherCuts,
                            ReFitPVs = True
                            )

    return Selection ( name,
                       Algorithm = _Bs,
#                       RequiredSelections = [KstarSel, Psi2SSel])
                       RequiredSelections = [Psi2SSel, KstarSel])


#####################
### Bd->Psi(2S)KS ###
#####################
def makeBd2Psi2SKsMuMu(name,
                       Psi2SSel,
                       KsSel,
                       BsMassCutDownPre,
                       BsMassCutUpPre,
                       BsMassCutDownPost,
                       BsMassCutUpPost,
                       BKsVCHI2PDOF, #different 20--->he posat 10
                       BPVLTIME = None,
                       MINTREEPT = None #1000
                       ) :

    _preVertexCuts = "in_range(%(BsMassCutDownPre)s,AM,%(BsMassCutUpPre)s)" % locals()
    _motherCuts = "in_range(%(BsMassCutDownPost)s,M,%(BsMassCutUpPost)s) & (VFASPF(VCHI2PDOF)<%(BKsVCHI2PDOF)s)" % locals()
    
    if BPVLTIME != None :
        _motherCuts += "& (BPVLTIME()> %(BPVLTIME)s*ps)" % locals()
#    print 'makeBd2Psi2SKsMuMu first', name, 'MotherCuts:', _motherCuts,  'preVertexCuts:', _preVertexCuts
    
    
    if MINTREEPT != None :
        _motherCuts += "& (MINTREE('KS0'==ABSID, PT)> %(MINTREEPT)s*MeV)" % locals()
#    print 'holaaaaa makeBd2Psi2SKsMuMu second', name, 'MotherCuts:', _motherCuts,  'preVertexCuts:', _preVertexCuts

    
    _Bs = CombineParticles( DecayDescriptor = "B0 -> psi(2S) KS0",
                            CombinationCut = _preVertexCuts,
                            MotherCut = _motherCuts,
                            ReFitPVs = True
                            )

    return Selection ( name,
                       Algorithm = _Bs,
#                       RequiredSelections = [KsSel, Psi2SSel])
                       RequiredSelections = [Psi2SSel, KsSel])


#####################
### TISTOS Filter ###
#####################
def filterTisTos( name, DiMuonInput, myTisTosSpecs ) :
    
    from Configurables import TisTosParticleTagger
    
    myTagger = TisTosParticleTagger(name + "_TisTosTagger")
    myTagger.TisTosSpecs = myTisTosSpecs

    # To speed it up, TisTos only with tracking system
    
    myTagger.ProjectTracksToCalo = False
    myTagger.CaloClustForCharged = False
    myTagger.CaloClustForNeutral = False
    myTagger.TOSFrac = { 4:0.0, 5:0.0 }
    
    return Selection(name + "_SelTisTos",
                     Algorithm = myTagger,
                     RequiredSelections = [ DiMuonInput ] )
