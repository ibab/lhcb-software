
__author__ = ['Malcolm John', 'B. Viaud', 'O. Kochebina']
__date__ = '25/6/2012'
__version__ = '$Revision: 1.7 $'

'''
D->Xu(Xu) mu mu , Xu=pi/K channels and control samples.
'''

config_params =  {'MuonP'         : 3000. ,    #MeV
                  'MuonPT'        : 500.  ,    #MeV
                  'MuonMINIPCHI2' : 6     ,    #adminensional
                  'MuonTRCHI2'    : 5     ,    #adminensional
                  
                  'PionP'         : 2000. ,    #MeV
                  'PionPT'        : 300.  ,    #MeV
                  'PionMINIPCHI2' : 6     ,    #adminensional
                  'PionTRCHI2'    : 5     ,    #adminensional
                  
                  'KaonP'         : 2000. ,    #MeV
                  'KaonPT'        : 300.  ,    #MeV
                  'KaonPIDK'      : -1.   ,    #adimensional
                  'KaonMINIPCHI2' : 6     ,    #adminensional
                  'KaonTRCHI2'    : 5     ,    #adminensional

                  'ProtonP'         : 2000. ,    #MeV
                  'ProtonPT'        : 300.  ,    #MeV
                  'ProtonMINIPCHI2' : 6     ,    #adminensional
                  'ProtonTRCHI2'    : 5     ,    #adminensional

                  #3 body
                  'DVCHI2DOF'     : 5     ,    #adminensional
                  'DMAXDOCA'      : 0.15  ,    #mm
                  'DIPCHI2'       : 25    ,    #adimensional
                  
                  #4body
                  'MINIPCHI2_hhmumu' : 2. ,  #adminensional
                  'PT_hhmumu'        : 300 , #MeV
                  'MuonPIDmu_hhmumu' : -1,   #adimensional
                  'DPT_hhmumu'           : 2500.,     #MeV
                  'DVCHI2DOF_hhmumu'     : 8     ,    #adminensional         
                  'DMAXDOCA_hhmumu'      : 0.2  ,    #mm
                  'DdauMAXIPCHI2_hhmumu' : 15    ,    #adimensinal
                  'DFDCHI2_hhmumu'       : 36     ,   #adimensional
                  'DIPCHI2_hhmumu'       : 20    ,    #adimensional

                  'DIPCHI2_4bodyCS'      : 16    ,    #adimensional
                  'MINIPCHI2_4bodyCS' : 3. ,  #adminensional

                  'DDIRA'         : 0.9999,    #adimensional
                  'DimuonMass'    : 250.  ,    #MeV
                  'DMassWin'                   : 200.  ,    #MeV, mass window
                  'DMassWin_CS_hhmumu'         : 100.  ,    #MeV, mass window
                  'LambdacMassWin'             : 100. , #MeV, mass window
                  'DMassLow'                   :1763.  ,    #MeV, low-mass veto

             
                  'GhostProbCut_hhmumu'     : 0.5,
                  
                  'D2PiMuMuOSLinePrescale'     : 1 ,
                  'D2PiMuMuOSLinePostscale'    : 1 ,
                  'D2PiMuMuSSLinePrescale'     : 1 ,
                  'D2PiMuMuSSLinePostscale'    : 1 ,
                  'D2KMuMuOSLinePrescale'      : 1 ,
                  'D2KMuMuOSLinePostscale'     : 1 ,
                  'D2KMuMuSSLinePrescale'      : 1 ,
                  'D2KMuMuSSLinePostscale'     : 1 ,
                  'D2PiPiPiCalLinePrescale'    : 0.01 ,
                  'D2PiPiPiCalLinePostscale'   : 1 ,
                  'D2K2PiLinePrescale'         : 0.005 ,
                  'D2K2PiLinePostscale'        : 1 ,
                  'D22KPiLinePrescale'         : 0.005 ,
                  'D22KPiLinePostscale'        : 1,
                  'D02KKMuMuLinePrescale'      : 1 ,
                  'D02KKMuMuLinePostscale'     : 1 ,
                  'D02PiPiMuMuLinePrescale'    : 1 ,
                  'D02PiPiMuMuLinePostscale'   : 1,
                  'D02KPiMuMuLinePrescale'     : 1 ,
                  'D02KPiMuMuLinePostscale'    : 1,
                  'D02KKPiPiLinePrescale'      : 0.01 ,
                  'D02KKPiPiLinePostscale'     : 1,
                  'D02K3PiLinePrescale'        : 0.005 ,
                  'D02K3PiLinePostscale'       : 1,
                  'D02PiPiPiPiLinePrescale'    : 0.01 ,
                  'D02PiPiPiPiLinePostscale'   : 1,
                  'Lambdac2PMuMuLinePrescale'  : 1 ,
                  'Lambdac2PMuMuLinePostscale' : 1 ,
                  'Lambdac2PPiPiLinePrescale'  : 0.01 ,
                  'Lambdac2PPiPiLinePostscale' : 1 
                  }

__all__ = ('D2XMuMuConf', 'makeD2PiPiPi', 'makeD2PiMuMuOS', 'makeD2PiMuMuSS', 'makeD2KMuMuOS', 'makeD2KMuMuSS', 'makeD2K2Pi', 'makeD22KPi', 'makeD02KKMuMu', 'makeD02PiPiMuMu', 'makeD02KPiMuMu', 'makeD02KKPiPi', 'makeD02K3Pi','makeD02PiPiPiPi','makeLambdac2PMuMu','makeLambdac2PPiPi' )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons, StdLoosePions, StdLooseMuons, StdNoPIDsPions, StdNoPIDsKaons, StdLooseProtons, StdAllNoPIDsProtons
from StandardParticles import StdAllLooseKaons, StdAllLoosePions, StdAllLooseMuons, StdAllNoPIDsPions, StdAllNoPIDsKaons

default_name = "D2XMuMu"

class D2XMuMuConf(LineBuilder) :
    """
    Builder for D2XMuMu, X = pi/K 
    """
    
    PiCalLine = None
    PiOSLine = None
    PiSSLine = None
    KOSLine = None
    KSSLine = None
    PiPiLine = None
    KPiLine = None

    KKLine = None
    KKPiPiLine = None
    K3PiLine = None
    PiPiPiPiLine = None
    K2PiLine = None
    TwoKPiLine = None

    Lambdac2PMuMuLine = None
    Lambdac2PPiPiLine = None
    
        
    __configuration_keys__ = (  'MuonP'
                                , 'MuonPT'
                                , 'MuonMINIPCHI2'
                                , 'MuonTRCHI2'
                                
                                , 'PionP'
                                , 'PionPT'
                                , 'PionMINIPCHI2'
                                , 'PionTRCHI2'
                                
                                , 'KaonP'
                                , 'KaonPT'
                                , 'KaonPIDK'
                                , 'KaonMINIPCHI2'
                                , 'KaonTRCHI2'

                                , 'ProtonP'
                                , 'ProtonPT'
                                , 'ProtonMINIPCHI2'
                                , 'ProtonTRCHI2'
                                
                                 #3 body
                                , 'DVCHI2DOF'    
                                , 'DMAXDOCA'    #  Added by B. VIAUD for hhmumu modes
                                , 'DIPCHI2'     
                                
                                #4body
                                , 'MINIPCHI2_hhmumu' #  Added by B. VIAUD for hhmumu modes
                                , 'PT_hhmumu' #   Added by B. VIAUD for hhmumu modes
                                , 'MuonPIDmu_hhmumu'#   Added by O. KOCHEBINA for hhmumu modes
                                , 'DPT_hhmumu' #   Added by O. KOCHEBINA for hhmumu modes
                                , 'DdauMAXIPCHI2_hhmumu'# Added by B. VIAUD for hhmumu modes
                                , 'DVCHI2DOF_hhmumu'        
                                , 'DMAXDOCA_hhmumu'  
                                , 'DFDCHI2_hhmumu'     
                                , 'DIPCHI2_hhmumu'   
                                
                                , 'DIPCHI2_4bodyCS'   
                                , 'MINIPCHI2_4bodyCS' 
     
                                , 'DDIRA'
                                , 'DimuonMass'
                                , 'DMassWin'
                                , 'DMassWin_CS_hhmumu'# Added by B. VIAUD for hhmumu modes
                                , 'LambdacMassWin'#   Added by O. KOCHEBINA for Lambda_c modes
                                , 'DMassLow'
                                , 'GhostProbCut_hhmumu' #   Added by O. KOCHEBINA for hhmumu modes
                                
                                , 'D2PiPiPiCalLinePrescale'
                                , 'D2PiPiPiCalLinePostscale'
                                , 'D2PiMuMuOSLinePrescale'
                                , 'D2PiMuMuOSLinePostscale'
                                , 'D2PiMuMuSSLinePrescale'
                                , 'D2PiMuMuSSLinePostscale'
                                , 'D2KMuMuOSLinePrescale'
                                , 'D2KMuMuOSLinePostscale'
                                , 'D2KMuMuSSLinePrescale'
                                , 'D2KMuMuSSLinePostscale'
                                , 'D2K2PiLinePrescale'
                                , 'D2K2PiLinePostscale'
                                , 'D22KPiLinePrescale'
                                , 'D22KPiLinePostscale'
                                , 'D02KKMuMuLinePrescale'
                                , 'D02KKMuMuLinePostscale'
                                , 'D02PiPiMuMuLinePrescale'
                                , 'D02PiPiMuMuLinePostscale'
                                , 'D02KPiMuMuLinePrescale'
                                , 'D02KPiMuMuLinePostscale'
                                , 'D02KKPiPiLinePrescale'
                                , 'D02KKPiPiLinePostscale'
                                , 'D02K3PiLinePrescale'
                                , 'D02K3PiLinePostscale'
                                , 'D02PiPiPiPiLinePrescale'
                                , 'D02PiPiPiPiLinePostscale'
                                , 'Lambdac2PMuMuLinePrescale' 
                                , 'Lambdac2PMuMuLinePostscale'
                                , 'Lambdac2PPiPiLinePrescale' 
                                , 'Lambdac2PPiPiLinePostscale' 
                                )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        
        PiCalLine_name = name+"_PiCal"
        PiOSLine_name = name+"_PiOS"
        PiSSLine_name = name+"_PiSS"
        KOSLine_name = name+"_KOS"
        KSSLine_name = name+"_KSS"
        K2PiLine_name = name+"_K2Pi"
        TwoKPiLine_name = name+"_2KPi"

        KKLine_name = name+"_KK"
        PiPiLine_name = name+"_PiPi"
        KPiLine_name = name+"_KPi"
        KKPiPiLine_name = name+"_KKPiPi"
        K3PiLine_name = name+"_K3Pi"
        PiPiPiPiLine_name = name+"_PiPiPiPi"
        
        Lambdac2PMuMuLine_name = name+"_Lambdac2PMuMu"
        Lambdac2PPiPiLine_name = name+"_Lambdac2PPiPi"
        
        # 1 : Make kaons
        selKaons = makeKaons(name="KaonsFor"+name
                             , KaonP = config['KaonP']
                             , KaonPT = config['KaonPT']
                             , KaonPIDK = config['KaonPIDK']
                             , KaonMINIPCHI2 = config['KaonMINIPCHI2']
                             , KaonTRCHI2 = config['KaonTRCHI2'])
        
        # 2 : Make pions
        selPions = makePions(name="PionsFor"+name
                             , PionP = config['PionP']
                             , PionPT = config['PionPT']
                             , PionMINIPCHI2 = config['PionMINIPCHI2']
                             , PionTRCHI2 = config['PionTRCHI2'])
        
        # 3 : Make muons
        selMuons = makeMuons(name="MuonsFor"+name
                             , MuonP = config['MuonP']
                             , MuonPT = config['MuonPT']
                             , MuonMINIPCHI2 = config['MuonMINIPCHI2']
                             , MuonTRCHI2 = config['MuonTRCHI2'])

        # 3b : Make muons candidates from pions
        pionsAsMuons = makePionsAsMuons(name="PionsAsMuonsFor"+name
                                        , MuonP = config['MuonP']
                                        , MuonPT = config['MuonPT']
                                        , MuonMINIPCHI2 = config['MuonMINIPCHI2']
                                        , MuonTRCHI2 = config['MuonTRCHI2'])

        # 4 : Make protons 
        selProtons = makeProtons(name="ProtonsFor"+name
                                 , ProtonP = config['ProtonP']
                                 , ProtonPT = config['ProtonPT']
                                 , ProtonMINIPCHI2 = config['ProtonMINIPCHI2']
                                 , ProtonTRCHI2 = config['ProtonTRCHI2'])
                                                             
      


        # 1a : Make Kaons for 3 body Control Samples
        selKaonsFor3bodyCS  = makeKaonsFor3bodyCS(name="HadFor3bodyCSAndFor"+name
                                             , KaonP = config['KaonP']
                                             , KaonPT = config['KaonPT']
                                             , KaonMINIPCHI2 = config['KaonMINIPCHI2']
                                             , KaonTRCHI2 = config['KaonTRCHI2'])
        
        # 2a : Make Pions for 3 body  Control samples 
        selPionsFor3bodyCS = makePionsFor3bodyCS(name="PionsFor3bodyCSAndFor"+name
                                                 , PionP = config['PionP']
                                                 , PionPT = config['PionPT']
                                                 , PionMINIPCHI2 = config['PionMINIPCHI2']
                                                 , PionTRCHI2 = config['PionTRCHI2'])

        # 4 : Make protons for 3 body  Control samples 
        selProtonsFor3bodyCS = makeProtonsFor3bodyCS(name="ProtonsFor3bodyCSAndFor"+name
                                 , ProtonP = config['ProtonP']
                                 , ProtonPT = config['ProtonPT']
                                 , ProtonMINIPCHI2 = config['ProtonMINIPCHI2']
                                 , ProtonTRCHI2 = config['ProtonTRCHI2'])



        # 5 : Make Muons for hhmumu 
        selMuonsForhhmumu = makeMuonsForhhmumu(name="MuonsForhhmumuAndFor"+name
                                               , MuonP = config['MuonP']
                                               , MuonPT = config['PT_hhmumu']
                                               , MuonMINIPCHI2 = config['MINIPCHI2_hhmumu']
                                               , MuonTRCHI2 = config['MuonTRCHI2']
                                               , MuonPIDmu_CS_hhmumu = config['MuonPIDmu_hhmumu']
                                               , ghostProbCut = config['GhostProbCut_hhmumu'])

        # 6 : Make Kaons for hhmumu 
        selKaonsForhhmumu  = makeKaonsForhhmumu(name="KaonsForhhmumuAndFor"+name
                                                , KaonP = config['KaonP']
                                                , KaonPT = config['PT_hhmumu']
                                                , KaonPIDK = config['KaonPIDK']
                                                , KaonMINIPCHI2 = config['MINIPCHI2_hhmumu']
                                                , KaonTRCHI2 = config['KaonTRCHI2']
                                                , ghostProbCut = config['GhostProbCut_hhmumu'])

        # 7 : Make Pions for hhmumu 
        selPionsForhhmumu = makePionsForhhmumu(name="PionsForForhhmumuAndFor"+name
                                               , PionP = config['PionP']
                                               , PionPT = config['PT_hhmumu']
                                               , PionMINIPCHI2 = config['MINIPCHI2_hhmumu']
                                               , PionTRCHI2 = config['PionTRCHI2']
                                               , ghostProbCut = config['GhostProbCut_hhmumu'])



        # 6a : Make Kaons for 4 body Control Samples
        selKaonsFor4bodyCS  = makeKaonsFor4bodyCS(name="HadFor4bodyCSAndFor"+name
                                                  , KaonP = config['KaonP']
                                                  , KaonPT = config['PT_hhmumu']
                                                  , KaonMINIPCHI2 = config['MINIPCHI2_4bodyCS']
                                                  , KaonTRCHI2 = config['KaonTRCHI2']
                                                  , ghostProbCut = config['GhostProbCut_hhmumu'])
        
        # 7a : Make Pions for 4 body Control samples 
        selPionsFor4bodyCS = makePionsFor4bodyCS(name="PionsFor4bodyCSAndFor"+name
                                                 , PionP = config['PionP']
                                                 , PionPT = config['PT_hhmumu']
                                                 , PionMINIPCHI2 = config['MINIPCHI2_4bodyCS']
                                                 , PionTRCHI2 = config['PionTRCHI2']
                                                 , ghostProbCut = config['GhostProbCut_hhmumu'])

      

        

                                        
        # 7 : Combine
        selD2PiPiPi   = self._makeD2PiPiPi(name=PiCalLine_name,
                                             pionSel = selPions,
                                             muonSel = pionsAsMuons,
                                             config = config)
        
        selD2PiMuMuOS = self._makeD2PiMuMuOS(name=PiOSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selD2PiMuMuSS = self._makeD2PiMuMuSS(name=PiSSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selD2KMuMuOS = self._makeD2KMuMuOS(name=KOSLine_name,
                                           kaonSel = selKaons,
                                           muonSel = selMuons,
                                           config = config)

        selD2KMuMuSS = self._makeD2KMuMuSS(name=KSSLine_name,
                                           kaonSel = selKaons,
                                           muonSel = selMuons,
                                           config = config)

        selD2K2Pi = self._makeD2K2Pi(name=K2PiLine_name,
                                     kaonSel = selKaonsFor3bodyCS,
                                     pionSel = selPionsFor3bodyCS,
                                     config = config)
        
        selD22KPi = self._makeD22KPi(name=TwoKPiLine_name,
                                     kaonSel = selKaonsFor3bodyCS,
                                     pionSel = selPionsFor3bodyCS,
                                     config = config)
        
        
        
        selD02KKMuMu = self._makeD02KKMuMu(name=KKLine_name,
                                           kaonSel = selKaonsForhhmumu,
                                           muonSel = selMuonsForhhmumu,
                                           config = config)
        
        selD02PiPiMuMu = self._makeD02PiPiMuMu(name=PiPiLine_name,
                                           pionSel = selPionsForhhmumu,
                                           muonSel = selMuonsForhhmumu,
                                           config = config)
        
        selD02KPiMuMu = self._makeD02KPiMuMu(name=KPiLine_name,
                                             kaonSel = selKaonsForhhmumu,
                                             pionSel = selPionsForhhmumu,
                                             muonSel = selMuonsForhhmumu,
                                             config = config)
        
        selD02KKPiPi = self._makeD02KKPiPi(name=KKPiPiLine_name,
                                           kaonSel = selKaonsFor4bodyCS,
                                           pionSel = selPionsFor4bodyCS,
                                           config = config)

        selD02K3Pi = self._makeD02K3Pi(name=K3PiLine_name,
                                           kaonSel = selKaonsFor4bodyCS,
                                           pionSel = selPionsFor4bodyCS,
                                           config = config)

        selD02PiPiPiPi = self._makeD02PiPiPiPi(name=PiPiPiPiLine_name,
                                       pionSel = selPionsFor4bodyCS,
                                       config = config)

        selLambdac2PMuMu   = self._makeLambdac2PMuMu(name=Lambdac2PMuMuLine_name,
                                                     protonSel = selProtons,
                                                     muonSel = selMuons,
                                                     config = config)
        
        selLambdac2PPiPi   = self._makeLambdac2PPiPi(name=Lambdac2PPiPiLine_name,
                                                     protonSel = selProtonsFor3bodyCS,
                                                     muonSel = selPionsFor3bodyCS,
                                                     config = config)
        



        
        # 8 : Declare Lines
        self.PiCalLine = StrippingLine(PiCalLine_name+"Line",
                                      prescale = config['D2PiPiPiCalLinePrescale'],
                                      postscale = config['D2PiPiPiCalLinePostscale'],
                                      selection = selD2PiPiPi
                                       )
        
        self.PiOSLine = StrippingLine(PiOSLine_name+"Line",
                                      prescale = config['D2PiMuMuOSLinePrescale'],
                                      postscale = config['D2PiMuMuOSLinePostscale'],
                                      selection = selD2PiMuMuOS
                                      )
        
        self.PiSSLine = StrippingLine(PiSSLine_name+"Line",
                                      prescale = config['D2PiMuMuSSLinePrescale'],
                                      postscale = config['D2PiMuMuSSLinePostscale'],
                                      selection = selD2PiMuMuSS
                                      )
        
        self.KOSLine = StrippingLine(KOSLine_name+"Line",
                                     prescale = config['D2KMuMuOSLinePrescale'],
                                     postscale = config['D2KMuMuOSLinePostscale'],
                                     selection = selD2KMuMuOS
                                     )
        
        self.KSSLine = StrippingLine(KSSLine_name+"Line",
                                     prescale = config['D2KMuMuSSLinePrescale'],
                                     postscale = config['D2KMuMuSSLinePostscale'],
                                     selection = selD2KMuMuSS
                                     )

        self.K2PiLine = StrippingLine(K2PiLine_name+"Line",
                                      prescale = config['D2K2PiLinePrescale'],
                                      postscale = config['D2K2PiLinePostscale'],
                                      selection = selD2K2Pi
                                      )

        self.TwoKPiLine = StrippingLine(TwoKPiLine_name+"Line",
                                        prescale = config['D22KPiLinePrescale'],
                                        postscale = config['D22KPiLinePostscale'],
                                        selection = selD22KPi
                                        )

        
        
        self.KKLine = StrippingLine(KKLine_name+"Line",
                                    prescale = config['D02KKMuMuLinePrescale'],
                                    postscale = config['D02KKMuMuLinePostscale'],
                                    selection = selD02KKMuMu
                                    )
        
        self.PiPiLine = StrippingLine(PiPiLine_name+"Line",
                                    prescale = config['D02PiPiMuMuLinePrescale'],
                                    postscale = config['D02PiPiMuMuLinePostscale'],
                                    selection = selD02PiPiMuMu
                                    )
        
        self.KPiLine = StrippingLine(KPiLine_name+"Line",
                                     prescale = config['D02KPiMuMuLinePrescale'],
                                     postscale = config['D02KPiMuMuLinePostscale'],
                                     selection = selD02KPiMuMu
                                     )
        
        self.KKPiPiLine = StrippingLine(KKPiPiLine_name+"Line",
                                    prescale = config['D02KKPiPiLinePrescale'],
                                    postscale = config['D02KKPiPiLinePostscale'],
                                    selection = selD02KKPiPi
                                    )

        self.K3PiLine = StrippingLine(K3PiLine_name+"Line",
                                        prescale = config['D02K3PiLinePrescale'],
                                        postscale = config['D02K3PiLinePostscale'],
                                        selection = selD02K3Pi
                                        )

        self.PiPiPiPiLine = StrippingLine(PiPiPiPiLine_name+"Line",
                                        prescale = config['D02PiPiPiPiLinePrescale'],
                                        postscale = config['D02PiPiPiPiLinePostscale'],
                                        selection = selD02PiPiPiPi
                                        )

 
        self.Lambdac2PMuMuLine = StrippingLine(Lambdac2PMuMuLine_name+"Line",
                                               prescale = config['Lambdac2PMuMuLinePrescale'],
                                               postscale = config['Lambdac2PMuMuLinePostscale'],
                                               selection = selLambdac2PMuMu
                                               )

        self.Lambdac2PPiPiLine = StrippingLine(Lambdac2PPiPiLine_name+"Line",
                                               prescale = config['Lambdac2PPiPiLinePrescale'],
                                               postscale = config['Lambdac2PPiPiLinePostscale'],
                                               selection = selLambdac2PPiPi
                                               )
        
        

        # 9 : register Line
        self.registerLine( self.KSSLine )
        self.registerLine( self.KOSLine )
        self.registerLine( self.PiSSLine )
        self.registerLine( self.PiOSLine )
        self.registerLine( self.PiCalLine )
        self.registerLine( self.K2PiLine )
        self.registerLine( self.TwoKPiLine )
        
        self.registerLine( self.KKLine )
        self.registerLine( self.PiPiLine )
        self.registerLine( self.KPiLine )
        self.registerLine( self.KKPiPiLine )
        self.registerLine( self.K3PiLine )
        self.registerLine( self.PiPiPiPiLine )

        self.registerLine( self.Lambdac2PMuMuLine )
        self.registerLine( self.Lambdac2PPiPiLine )


#####################################################
    def _makeD2PiPiPi(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiPiPi
        """
        return makeD2PiPiPi(name
                            , pionSel
                            , muonSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , DMassWin = config['DMassWin']
                            , DMassLow = config['DMassLow']
                            , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiMuMuOS(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiMuMuOS
        """
        return makeD2PiMuMuOS(name
                              , pionSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiMuMuSS(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiMuMuSS
        """
        return makeD2PiMuMuSS(name
                              , pionSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KMuMuOS(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D2XMuMu
        """
        return makeD2KMuMuOS(name
                             , kaonSel
                             , muonSel
                             , DMAXDOCA = config['DMAXDOCA']
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KMuMuSS(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D2XMuMu
        """
        return makeD2KMuMuSS(name
                             , kaonSel
                             , muonSel
                             , DMAXDOCA = config['DMAXDOCA']
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeD2K2Pi(self, name, kaonSel, pionSel, config):
        """
        Handy interface for D2K2pi
        """
        return makeD2K2Pi(name
                             , kaonSel
                             , pionSel
                             , OneKaonPID = config['KaonPIDK']
                             , DMAXDOCA = config['DMAXDOCA']
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])



#####################################################
    def _makeD22KPi(self, name, kaonSel, pionSel, config):
        """
        Handy interface for D22Kpi
        """
        return makeD22KPi(name
                             , kaonSel
                             , pionSel
                             , OneKaonPID = config['KaonPIDK']
                             , DMAXDOCA = config['DMAXDOCA']
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])





#####################################################
    def _makeD02KKMuMu(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D0KKMuMu
        """
        return makeD02KKMuMu(name
                             , kaonSel
                             , muonSel
                             , DMAXDOCA = config['DMAXDOCA_hhmumu']
                             , DPT = config['DPT_hhmumu']
                             , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                             , DFDCHI2= config['DFDCHI2_hhmumu']
                             , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2_hhmumu']
                             , DMassWin = config['DMassWin_CS_hhmumu']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeD02PiPiMuMu(self, name, pionSel, muonSel, config):
        """
        Handy interface for D0PiPiMuMu
        """
        return makeD02PiPiMuMu(name
                               , pionSel
                               , muonSel
                               , DMAXDOCA = config['DMAXDOCA_hhmumu']
                               , DPT = config['DPT_hhmumu']
                               , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                               , DFDCHI2= config['DFDCHI2_hhmumu']
                               , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                               , DDIRA = config['DDIRA']
                               , DIPCHI2 = config['DIPCHI2_hhmumu']
                               , DMassWin = config['DMassWin_CS_hhmumu']
                               , DMassLow = config['DMassLow']
                               , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD02KPiMuMu(self, name, kaonSel, pionSel, muonSel, config):
        """
        Handy interface for D0KPiMuMu
        """
        return makeD02KPiMuMu(name
                              , kaonSel
                              , pionSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA_hhmumu']
                              , DPT = config['DPT_hhmumu']
                              , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                              , DFDCHI2= config['DFDCHI2_hhmumu']
                              , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2_hhmumu']
                              , DMassWin = config['DMassWin_CS_hhmumu']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])
    
#####################################################
    def _makeD02KKPiPi(self, name, kaonSel, pionSel, config):
        """
        Handy interface for D0KKpipi
        """
        return makeD02KKPiPi(name
                             , kaonSel
                             , pionSel
                             , OneKaonPID = config['KaonPIDK']
                             , DMAXDOCA = config['DMAXDOCA_hhmumu']
                             , DPT = config['DPT_hhmumu']
                             , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                             , DFDCHI2= config['DFDCHI2_hhmumu']
                             , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2_4bodyCS']
                             , DMassWin = config['DMassWin_CS_hhmumu']
                             , DMassLow = config['DMassLow'])

#####################################################
    def _makeD02K3Pi(self, name, kaonSel, pionSel, config):
        """
        Handy interface for D0K3pi
        """
        return makeD02K3Pi(name
                             , kaonSel
                             , pionSel
                             , OneKaonPID = config['KaonPIDK']
                             , DPT = config['DPT_hhmumu'] 
                             , DMAXDOCA = config['DMAXDOCA_hhmumu']
                             , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                             , DFDCHI2= config['DFDCHI2_hhmumu']
                             , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2_4bodyCS']
                             , DMassWin = config['DMassWin_CS_hhmumu']
                             , DMassLow = config['DMassLow'])


#####################################################
    def _makeD02PiPiPiPi(self, name, pionSel, config):
        """
        Handy interface for D0pipipipi
        """
        return makeD02PiPiPiPi(name
                               , pionSel
                               , DMAXDOCA = config['DMAXDOCA_hhmumu']
                               , DPT = config['DPT_hhmumu']
                               , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                               , DFDCHI2= config['DFDCHI2_hhmumu']
                               , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                               , DDIRA = config['DDIRA']
                               , DIPCHI2 = config['DIPCHI2_4bodyCS']
                               , DMassWin = config['DMassWin_CS_hhmumu']
                               , DMassLow = config['DMassLow'])



#####################################################
    def _makeLambdac2PMuMu(self, name, protonSel, muonSel, config):
        """
        Handy interface for Lambdac2PMuMu
        """
        return makeLambdac2PMuMu(name
                            , protonSel
                            , muonSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , LambdacMassWin = config['LambdacMassWin']
                            , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeLambdac2PPiPi(self, name, protonSel, muonSel, config):
        """
        Handy interface for Lambdac2PMuMu
        """
        return makeLambdac2PPiPi(name
                                 , protonSel
                                 , muonSel
                                 , DMAXDOCA = config['DMAXDOCA']
                                 , DVCHI2DOF = config['DVCHI2DOF']
                                 , DDIRA = config['DDIRA']
                                 , DIPCHI2 = config['DIPCHI2']
                                 , LambdacMassWin = config['LambdacMassWin']
                                 , DimuonMass = config['DimuonMass'])


#
# Out of class
#####################################################
def makeD2PiPiPi(name, pionSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ (pi+ pi-)
    """
    rhos=Selection("pre"+name,
                   Algorithm = CombineParticles(DecayDescriptor = "rho(770)0 -> pi+ pi-",MotherCut = "ALL"),
                   RequiredSelections=[muonSel] )

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ rho(770)0]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ rhos, pionSel ] )

#####################################################
def makeD2PiMuMuOS(name, pionSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ mu+ mu- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel ] )

#####################################################
def makeD2PiMuMuSS(name, pionSel, muonSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> pi+ mu- mu-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> pi+ mu- mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [  muonSel, pionSel ] )

#####################################################
def makeD2KMuMuOS(name, kaonSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ mu+ mu-
    """
    
    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel ] )

#####################################################
def makeD2KMuMuSS(name, kaonSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> K+ mu- mu-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> K+ mu- mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel ] )

#####################################################
def makeD2K2Pi(name, kaonSel, pionSel, OneKaonPID, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    
    """
    Makes the D+ -> K- pi+ pi+ 
    """
    
    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV) &" \
               "(AHASCHILD( (ABSID=='K+') & (PIDK-PIDpi > %(OneKaonPID)s)  ))" %locals()

    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    
    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K- pi+ pi+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, pionSel ] )



#####################################################
def makeD22KPi(name, kaonSel, pionSel, OneKaonPID, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    
    """
    Makes the D+ -> K- K+ pi+ 
    """
    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM23 > %(DimuonMass)s *MeV) &" \
               "(AM > %(DMassLow)s *MeV) &" \
               "(AHASCHILD( (ABSID=='K+') & (PIDK-PIDpi > %(OneKaonPID)s)  ))" %locals()
    
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ K- pi+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, pionSel ] )






#####################################################
def makeD02KKMuMu(name, kaonSel, muonSel, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DimuonMass, DPT):
    
    """
    Makes the D0 -> K+ K- mu+ mu-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM34 > %(DimuonMass)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()


    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "D0 -> K+ K- mu+ mu-",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel ] )


#####################################################
def makeD02PiPiMuMu(name, pionSel, muonSel, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DimuonMass, DPT):
    
    """
    Makes the D0 -> pi+ pi- mu+ mu-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM34 > %(DimuonMass)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()


    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "D0 -> pi+ pi- mu+ mu-",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel] )

#####################################################
def makeD02KPiMuMu(name, kaonSel, pionSel, muonSel, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DimuonMass, DPT):
    
    """
    Makes the D0 -> K- pi+ mu+ mu-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM34 > %(DimuonMass)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()


    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D0 -> K- pi+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel, pionSel] )


#####################################################
def makeD02KKPiPi(name, kaonSel, pionSel, OneKaonPID, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DPT):
    
    """
    Makes the D0 -> K+ K- pi+ pi-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AHASCHILD( (ABSID=='K+') & (PIDK-PIDpi > %(OneKaonPID)s)  )) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()

    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "D0 -> K+ K- pi+ pi-",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, pionSel ] )



#####################################################
def makeD02K3Pi(name, kaonSel, pionSel, OneKaonPID, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DPT):
    
    """
    Makes the D0 -> K- pi+ pi+ pi-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AHASCHILD( (ABSID=='K+') & (PIDK-PIDpi > %(OneKaonPID)s)  )) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()

    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D0 -> K- pi+ pi- pi+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, pionSel ] )



#####################################################
def makeD02PiPiPiPi(name, pionSel, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DPT):
    
    """
    Makes the D0 -> pi+ pi- pi+ pi-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()

    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "D0 -> pi+ pi- pi+ pi-",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ pionSel ] )

####################################################
def makeLambdac2PMuMu(name, protonSel, muonSel,DMAXDOCA, LambdacMassWin, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the Lambda_c+ -> p+ mu+ mu-
    """

    _combcut = "(ADAMASS('Lambda_c+') < %(LambdacMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
	       "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[Lambda_c+ -> p+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ protonSel, muonSel ] )

####################################################
def makeLambdac2PPiPi(name, protonSel, muonSel,DMAXDOCA, LambdacMassWin, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the Lambda_c+ -> p+ pi+ pi-
    """

    _combcut = "(ADAMASS('Lambda_c+') < %(LambdacMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[Lambda_c+ -> p+ pi+ pi-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ protonSel, muonSel ] )


#####################################################


                    
#####################################################
def makeKaons(name, KaonP, KaonPT, KaonPIDK, KaonMINIPCHI2, KaonTRCHI2):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
						"(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(PIDK-PIDpi > %(KaonPIDK)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseKaons ] )

#####################################################
def makePions(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2):
    """
    Pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
						"(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
                    
#####################################################
def makeMuons(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
						"(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseMuons ] )


#####################################################
def makeProtons(name, ProtonP, ProtonPT, ProtonMINIPCHI2, ProtonTRCHI2):
    """
    Proton selection
    """
    _code = "(TRCHI2DOF < %(ProtonTRCHI2)s) & "\
						"(P > %(ProtonP)s *MeV) & "\
            "(PT > %(ProtonPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(ProtonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseProtons ] )
                        
#####################################################
def makePionsAsMuons(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
						"(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
                    



#####################################################
def makeKaonsFor3bodyCS(name, KaonP, KaonPT, KaonMINIPCHI2, KaonTRCHI2):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
						"(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsKaons ] )


#####################################################
def makePionsFor3bodyCS(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2):
    """
    pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
						"(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsPions ] )


#####################################################
def makeProtonsFor3bodyCS(name, ProtonP, ProtonPT, ProtonMINIPCHI2, ProtonTRCHI2):
    """
    Proton selection
    """
    _code = "(TRCHI2DOF < %(ProtonTRCHI2)s) & "\
						"(P > %(ProtonP)s *MeV) & "\
            "(PT > %(ProtonPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(ProtonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsProtons ] )




#####################################################
def makeKaonsForhhmumu(name, KaonP, KaonPT, KaonPIDK, KaonMINIPCHI2, KaonTRCHI2, ghostProbCut):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
						"(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(PIDK-PIDpi > %(KaonPIDK)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLooseKaons ] )


#####################################################
def makeMuonsForhhmumu(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2, MuonPIDmu_CS_hhmumu, ghostProbCut):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
						"(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(PIDmu-PIDpi > %(MuonPIDmu_CS_hhmumu)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLooseMuons ] )

#####################################################
def makePionsForhhmumu(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2, ghostProbCut):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
						"(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLoosePions ] )



#####################################################
def makeKaonsFor4bodyCS(name, KaonP, KaonPT, KaonMINIPCHI2, KaonTRCHI2, ghostProbCut):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
						"(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsKaons ] )


#####################################################
def makePionsFor4bodyCS(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2, ghostProbCut):
    """
    pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
						"(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsPions ] )




'''


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingD2XMuMuSSConf(LHCbConfigurableUser):
    """
    Definition of nominal D->Xu mu nu stripping
    """
    
    __slots__ = {
	"prefix"	: "",
        #Muons
        "MuonTRCHI2"    : 10.   ,#adimensional
        "MuonP"         : 3000. ,#MeV
        "MuonPT"        : 500. ,#MeV
#        "MuonPIDK"      : -5.   ,#adimensional
#        "MuonPIDmu"     : -5.   ,#adimensional
#        "MuonPIDp"      : -5.   ,#adimensional
        "MuonMINIPCHI2" : 5     ,#adminensional
        #Xu
        #Pi Channel
        "PionTRCHI2"    : 10.   ,#adimensional
        "PionP"         : 2000. ,#MeV
        "PionPT"        : 300.  ,#MeV
        "PionPIDK"      : -10.  ,#adimensional
#        "PionPIDmu"     : -10.  ,#adimensional
#        "PionPIDp"      : -10.  ,#adimensional
        "PionMINIPCHI2" : 9     ,#adminensional
        #K Channel
        "KaonTRCHI2"    : 10.   ,#adimensional
        "KaonP"         : 2000. ,#MeV
        "KaonPT"        : 300.  ,#MeV
        "KaonPIDK"      : -1.   ,#adimensional
        "KaonMINIPCHI2" : 9     ,#adminensional
        #D
        "DVCHI2DOF"     : 8   ,#adminensional
        "DFDCHI2"       : 64   ,#adminensional
        "DDIRA"         : 0.9999 ,#adimensional
        'DMassWin'      : 200,   # MeV, high mass window
        'DMassLow'      : 1760   # MeV, high mass window
        }


    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
      
    def _NominalPiSelection( self ):
        return "(TRCHI2DOF < %(PionTRCHI2)s )&  (P> %(PionP)s *MeV) &  (PT> %(PionPT)s *MeV)"\
               "& (PIDpi-PIDK> %(PionPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s )"
      
    def _NominalKSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2PiMuMuSSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._D2PiMuMuSS()])
    def PiOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2PiMuMuOSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._D2PiMuMuOS()])

    def KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2KMuMuSSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._D2KMuMuSS()])
    
    def KOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2KMuMuOSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._D2KMuMuOS()])

    def _muonFilter( self ):
        from Configurables import FilterDesktop
        _mu = FilterDesktop(self.getProp("prefix") + "Mu_forD2XMuMu",
                            InputLocations = ["Phys/StdLooseMuons"])
        _mu.Code = self._NominalMuSelection() % self.getProps()
        return _mu

    def _pionFilter( self ):
        from Configurables import FilterDesktop
        _pi = FilterDesktop(self.getProp("prefix") + "Pi_forD2XMuMu",
                            InputLocations = ["Phys/StdLoosePions"])
        _pi.Code = self._NominalPiSelection() % self.getProps()
        return _pi

    def _kaonFilter( self ):
        from Configurables import FilterDesktop
        _ka = FilterDesktop(self.getProp("prefix") + "K_forD2XMuMu",
                            InputLocations = ["Phys/StdLooseKaons"])
        _ka.Code = self._NominalKSelection() % self.getProps()
        return _ka
    
    def _D2PiMuMuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMuMu = CombineParticles(self.getProp("prefix") + "D2PiMuMuSS")
        _PiMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
				  "Phys/" + self.getProp("prefix") + "Pi_forD2XMuMu"]
        _PiMuMu.DecayDescriptors = ["[D- -> pi+ mu- mu-]cc"]
        _PiMuMu.CombinationCut = " (AM>%(DMassLow)s*MeV) & (ADAMASS('D-') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
                          "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _PiMuMu.ReFitPVs = True
        return _PiMuMu

    def _D2PiMuMuOS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMuMu = CombineParticles(self.getProp("prefix") + "D2PiMuMuOS")
        _PiMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
				  "Phys/" + self.getProp("prefix") + "Pi_forD2XMuMu"]
        _PiMuMu.DecayDescriptors = ["[D+ -> pi+ mu+ mu-]cc"]
        _PiMuMu.CombinationCut = "(AM>%(DMassLow)s*MeV)& (ADAMASS('D+') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
                          "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _PiMuMu.ReFitPVs = True
        return _PiMuMu
    
    def _D2KMuMuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMuMu = CombineParticles(self.getProp("prefix") + "D2KMuMuSS")
        _KMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
				 "Phys/" + self.getProp("prefix") + "K_forD2XMuMu"]
        _KMuMu.DecayDescriptors = ["[D- -> K+ mu- mu-]cc"]
        _KMuMu.CombinationCut = "(AM>%(DMassLow)s*MeV)& (ADAMASS('D-') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
            "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _KMuMu.ReFitPVs = True
        return _KMuMu
    
    def _D2KMuMuOS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMuMu = CombineParticles(self.getProp("prefix") + "D2KMuMuOS")
        _KMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
				 "Phys/" + self.getProp("prefix") + "K_forD2XMuMu"]
        _KMuMu.DecayDescriptors = ["[D+ -> K+ mu+ mu-]cc"]
        _KMuMu.CombinationCut = " (AM>%(DMassLow)s*MeV)& (ADAMASS('D+') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
            "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _KMuMu.ReFitPVs = True
        return _KMuMu
    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
        
'''
