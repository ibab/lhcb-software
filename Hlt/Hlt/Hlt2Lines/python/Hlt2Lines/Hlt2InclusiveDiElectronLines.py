from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiElectronLinesConf(HltLinesConfigurableUser) :
    '''
          Put only the most discriminating variables as slots:
           - prescale factors, mass windows and dielectron pt
           - electron pt _not_ as slot as this strongly modifies the angular acceptances
    '''
    
    
    __slots__ = {  'Prescale'                  : {   'Hlt2Zee'               :  1.0
                                                    ,'Hlt2DYee1'             :  1.0
                                                    ,'Hlt2DYee2'             :  1.0
                                                    ,'Hlt2DYee3'             :  1.0
                                                    ,'Hlt2ZeeUnfitted'       :  1.0
                                                    ,'Hlt2DYee1Unfitted'    :  1.0
                                                    ,'Hlt2DYee2Unfitted'    :  1.0
                                                    ,'Hlt2DYee3Unfitted'    :  1.0
                                                    # Unbiased Di-Electron
                                                    ,'Hlt2UnbiasedDiElectron'         :  0.01
                                                    ,'Hlt2UnbiasedDiElectronLowMass'  :  0.01
                                                    ,'Hlt2UnbiasedDiElectronLowPID'   :  0.001
                                                    ,'Hlt2UnbiasedJpsi2ee'            :  0.01
                                                    ,'Hlt2UnbiasedPsi2ee'             :  0.01
                                                    ,'Hlt2UnbiasedB2ee'               :  0.01
                                                    # Unbiased and Track Fitted (TF)
                                                    ,'Hlt2UnbiasedTFDiElectron'         :  1.0
                                                    ,'Hlt2UnbiasedTFDiElectronHighMass' :  1.0
                                                    ,'Hlt2UnbiasedTFDiElectronVHighMass':  1.0
                                                    ,'Hlt2UnbiasedTFDiElectronLowMass'  :  1.0
                                                    ,'Hlt2UnbiasedTFDiElectronLowPID'   :  0.01
                                                    ,'Hlt2UnbiasedTFJpsi2ee'            :  1.0
                                                    ,'Hlt2UnbiasedTFPsi2ee'             :  1.0
                                                    ,'Hlt2UnbiasedTFB2ee'               :  1.0
                                                    # Biased 
                                                    ,'Hlt2BiasedDiElectron'         :  0.01
                                                    ,'Hlt2BiasedDiElectronLowMass'  :  0.01
                                                    ,'Hlt2BiasedDiElectronLowPID'   :  0.001 
                                                    ,'Hlt2BiasedJpsi2ee'            :  0.01
                                                    ,'Hlt2BiasedPsi2ee'             :  0.01
                                                    ,'Hlt2BiasedB2ee'               :  0.01
                                                    # Biased and Track Fitted (TF)
                                                    ,'Hlt2BiasedTFDiElectron'         :  1.0
                                                    ,'Hlt2BiasedTFDiElectronLowMass'  :  0.01
                                                    ,'Hlt2BiasedTFDiElectronLowPID'   :  0.01
                                                    ,'Hlt2BiasedTFJpsi2ee'            :  1.0
                                                    ,'Hlt2BiasedTFPsi2ee'             :  1.0
                                                    ,'Hlt2BiasedTFB2ee'               :  1.0 
                                                    }

                   ,'DiElectronVertexChi2'    :   20
                   ,'DiElectronIP'            :  0.1      # mm 
                   ,'DiElectronTkChi2'        :   20
                   ,'DiElectronDphiMin'       :    0      # radian
                   ,'ZeeMinMass'              :40000      # MeV
                   ,'ZeePt'                   :10000      # MeV
                   ,'DYPt'                    :  500      # MeV
                   ,'DY1MinPt'                : 2000      # MeV
                   ,'DY1MinMass'              : 5000      # MeV
                   ,'DY2MinPt'                : 4000      # MeV
                   ,'DY2MinMass'              :10000      # MeV
                   ,'DY3MinPt'                : 6000      # MeV
                   ,'DY3MinMass'              :15000      # MeV
                   
                   #-----------------
                   # Track un-fitted 
                   #-----------------
                   ,'UnbiasedDiElectron_L0Req'               :  "L0_CHANNEL('Electron')"
                   ,'UnbiasedDiElectron_Hlt1Req'             :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"  

                   # Di-Electron 
                   ,'UnbiasedDiElectron_MinMass'    : 2000.      # MeV
                   ,'UnbiasedDiElectron_VtxCHI2'    :   25.
                   ,'UnbiasedDiElectron_PT'         : -999.      # MeV
                   ,'UnbiasedDiElectron_ElecPT'     : 1000.      # MeV
                   ,'UnbiasedDiElectron_ElecPIDe'   :    1.5 

                   # Di-Electron Low mass
                   ,'UnbiasedDiElectronLowMass_MinMass'   :    0.      # MeV
                   ,'UnbiasedDiElectronLowMass_VtxCHI2'   :   25.
                   ,'UnbiasedDiElectronLowMass_PT'        : -999.      # MeV
                   ,'UnbiasedDiElectronLowMass_ElecPT'    : 1000.      # MeV
                   ,'UnbiasedDiElectronLowMass_ElecPIDe'  :    1.5
                   
                   # Di-Electron Low PID
                   ,'UnbiasedDiElectronLowPID_MinMass'   : 2000.      # MeV
                   ,'UnbiasedDiElectronLowPID_VtxCHI2'   :   25.
                   ,'UnbiasedDiElectronLowPID_PT'        : -999.      # MeV
                   ,'UnbiasedDiElectronLowPID_ElecPT'    : 1000.      # MeV
                   ,'UnbiasedDiElectronLowPID_ElecPIDe'  :   -2.
                   
                   # Jpsi -> ee 
                   ,'UnbiasedJpsi2ee_MinMass'     : 2500.      # MeV
                   ,'UnbiasedJpsi2ee_MaxMass'     : 3300.      # MeV
                   ,'UnbiasedJpsi2ee_VtxCHI2'     :   25.
                   ,'UnbiasedJpsi2ee_PT'          : -999.      # MeV
                   ,'UnbiasedJpsi2ee_ElecPT'      : 1000.      # MeV
                   ,'UnbiasedJpsi2ee_ElecPIDe'    :    1.5
                   
                   # Psi(2S) -> ee
                   ,'UnbiasedPsi2ee_MinMass'    : 3300.      # MeV
                   ,'UnbiasedPsi2ee_MaxMass'    : 3900.      # MeV
                   ,'UnbiasedPsi2ee_VtxCHI2'    :   25.
                   ,'UnbiasedPsi2ee_PT'         : -999.      # MeV
                   ,'UnbiasedPsi2ee_ElecPT'     : 1000.      # MeV
                   ,'UnbiasedPsi2ee_ElecPIDe'   :    1.5
                   
                   # B, upsilon -> ee
                   ,'UnbiasedB2ee_MinMass'      : 4800.      # MeV
                   ,'UnbiasedB2ee_MaxMass'      : 1.0e+10    # MeV
                   ,'UnbiasedB2ee_VtxCHI2'      :   25.
                   ,'UnbiasedB2ee_PT'           : -999.      # MeV
                   ,'UnbiasedB2ee_ElecPT'       : 1000.      # MeV
                   ,'UnbiasedB2ee_ElecPIDe'     :    1.5 
                   

                   #-----------------
                   # Track fitted 
                   #-----------------
                   ,'UnbiasedTFDiElectron_L0Req'               :  "L0_CHANNEL('Electron')"
                   ,'UnbiasedTFDiElectron_Hlt1Req'             :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"  
                   
                   # Di-Electron                   
                   ,'UnbiasedTFDiElectron_MinMass'    : 2000.      # MeV
                   ,'UnbiasedTFDiElectron_VtxCHI2'    :   25.
                   ,'UnbiasedTFDiElectron_PT'         : -999.      # MeV
                   ,'UnbiasedTFDiElectron_ElecPT'     : 1000.      # MeV
                   ,'UnbiasedTFDiElectron_ElecPIDe'   :    1.5 
                   # Di-Electron High Mass                   
                   ,'UnbiasedTFDiElectronHighMass_MinMass'    : 20000.      # MeV
                   ,'UnbiasedTFDiElectronHighMass_VtxCHI2'    :   25.
                   ,'UnbiasedTFDiElectronHighMass_TkChi2'     :   10.
                   ,'UnbiasedTFDiElectronHighMass_PT'         : -999.      # MeV
                   ,'UnbiasedTFDiElectronHighMass_ElecPT'     : 10000.      # MeV
                   ,'UnbiasedTFDiElectronHighMass_PrsMin' :    50. 
                   ,'UnbiasedTFDiElectronHighMass_EcalMin':   0.1 
                   ,'UnbiasedTFDiElectronHighMass_HcalMax':  0.05 
                   ,'UnbiasedTFDiElectronVHighMass_ElecPT'    : 20000.      # MeV
                   ,'UnbiasedTFDiElectronVHighMass_MinMass'   : 40000.      # MeV

                   # Di-Electron Low mass
                   ,'UnbiasedTFDiElectronLowMass_MinMass'   :     0.     # MeV
                   ,'UnbiasedTFDiElectronLowMass_VtxCHI2'   :    25.
                   ,'UnbiasedTFDiElectronLowMass_PT'        :  -999.     # MeV
                   ,'UnbiasedTFDiElectronLowMass_ElecPT'    :  1000.     # MeV
                   ,'UnbiasedTFDiElectronLowMass_ElecPIDe'  :     1.5
                   
                   # Di-Electron Low PID
                   ,'UnbiasedTFDiElectronLowPID_MinMass'   :  2000.     # MeV
                   ,'UnbiasedTFDiElectronLowPID_VtxCHI2'   :    25.
                   ,'UnbiasedTFDiElectronLowPID_PT'        :  -999.     # MeV
                   ,'UnbiasedTFDiElectronLowPID_ElecPT'    :  1000.     # MeV
                   ,'UnbiasedTFDiElectronLowPID_ElecPIDe'  :    -2.
                   
                   # Jpsi -> ee 
                   ,'UnbiasedTFJpsi2ee_MinMass'     : 2500.      # MeV
                   ,'UnbiasedTFJpsi2ee_MaxMass'     : 3300.      # MeV
                   ,'UnbiasedTFJpsi2ee_VtxCHI2'     :   25.
                   ,'UnbiasedTFJpsi2ee_PT'          : -999.      # MeV
                   ,'UnbiasedTFJpsi2ee_ElecPT'      : 1000.      # MeV
                   ,'UnbiasedTFJpsi2ee_ElecPIDe'    :    1.5      
                   
                   # Psi(2S) -> ee
                   ,'UnbiasedTFPsi2ee_MinMass'    : 3300.      # MeV
                   ,'UnbiasedTFPsi2ee_MaxMass'    : 3900.      # MeV
                   ,'UnbiasedTFPsi2ee_VtxCHI2'    :   25.
                   ,'UnbiasedTFPsi2ee_PT'         : -999.      # MeV
                   ,'UnbiasedTFPsi2ee_ElecPT'     : 1000.      # MeV
                   ,'UnbiasedTFPsi2ee_ElecPIDe'   :    1.5  

                   # B, upsilon -> ee
                   ,'UnbiasedTFB2ee_MinMass'      : 4800.      # MeV
                   ,'UnbiasedTFB2ee_MaxMass'      : 1.0e+10    # MeV
                   ,'UnbiasedTFB2ee_VtxCHI2'      :   25.
                   ,'UnbiasedTFB2ee_PT'           : -999.      # MeV
                   ,'UnbiasedTFB2ee_ElecPT'       : 1000.      # MeV
                   ,'UnbiasedTFB2ee_ElecPIDe'     :    1.5


                   #------------------
                   # Biased
                   #------------------
                   ,'BiasedDiElectron_L0Req'       :  "L0_CHANNEL('Electron')"
                   ,'BiasedDiElectron_Hlt1Req'     :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')" 
                   ,'BiasedDiElectron_MinMass'     : 2000.      # MeV
                   ,'BiasedDiElectron_VtxCHI2'     :   25.
                   ,'BiasedDiElectron_PT'          : -999.      # MeV
                   ,'BiasedDiElectron_ElecPT'      :  300.      # MeV
                   ,'BiasedDiElectron_ElecPIDe'    :    1.5
                   ,'BiasedDiElectron_ElecIP'      :    0.1     # mm
                   ,'BiasedDiElectron_ElecIPCHI2'  : -999.                   
                   
                   # Di-Electron Low mass
                   ,'BiasedDiElectronLowMass_MinMass'     :    0.      # MeV
                   ,'BiasedDiElectronLowMass_VtxCHI2'     :   25.
                   ,'BiasedDiElectronLowMass_PT'          : -999.      # MeV
                   ,'BiasedDiElectronLowMass_ElecPT'      :  300.      # MeV
                   ,'BiasedDiElectronLowMass_ElecPIDe'    :    1.5  
                   ,'BiasedDiElectronLowMass_ElecIP'      :    0.1     # mm
                   ,'BiasedDiElectronLowMass_ElecIPCHI2'  : -999.           
                   
                   # Di-Electron Low PID
                   ,'BiasedDiElectronLowPID_MinMass'     : 2000.      # MeV
                   ,'BiasedDiElectronLowPID_VtxCHI2'     :   25.
                   ,'BiasedDiElectronLowPID_PT'          : -999.      # MeV
                   ,'BiasedDiElectronLowPID_ElecPT'      :  300.      # MeV
                   ,'BiasedDiElectronLowPID_ElecPIDe'    :   -2.  
                   ,'BiasedDiElectronLowPID_ElecIP'      :    0.1     # mm
                   ,'BiasedDiElectronLowPID_ElecIPCHI2'  : -999.    

                   # Jpsi-> ee
                   ,'BiasedJpsi2ee_MinMass'     : 2500.      # MeV
                   ,'BiasedJpsi2ee_MaxMass'     : 3300.      # MeV
                   ,'BiasedJpsi2ee_VtxCHI2'     :   25.
                   ,'BiasedJpsi2ee_PT'          : -999.      # MeV
                   ,'BiasedJpsi2ee_ElecPT'      :  300.      # MeV
                   ,'BiasedJpsi2ee_ElecPIDe'    :    1.5  
                   ,'BiasedJpsi2ee_ElecIP'      :    0.1     # mm
                   ,'BiasedJpsi2ee_ElecIPCHI2'  : -999.

                   # Psi-> ee
                   ,'BiasedPsi2ee_MinMass'     : 3300.      # MeV
                   ,'BiasedPsi2ee_MaxMass'     : 3900.      # MeV
                   ,'BiasedPsi2ee_VtxCHI2'     :   25.
                   ,'BiasedPsi2ee_PT'          : -999.      # MeV
                   ,'BiasedPsi2ee_ElecPT'      :  300.      # MeV
                   ,'BiasedPsi2ee_ElecPIDe'    :    1.5  
                   ,'BiasedPsi2ee_ElecIP'      :    0.1     # mm
                   ,'BiasedPsi2ee_ElecIPCHI2'  : -999.
                   
                   # B-> ee
                   ,'BiasedB2ee_MinMass'     : 4800.      # MeV
                   ,'BiasedB2ee_MaxMass'     : 1.0e+10    # MeV
                   ,'BiasedB2ee_VtxCHI2'     :   25.
                   ,'BiasedB2ee_PT'          : -999.      # MeV
                   ,'BiasedB2ee_ElecPT'      :  300.      # MeV
                   ,'BiasedB2ee_ElecPIDe'    :    1.5  
                   ,'BiasedB2ee_ElecIP'      :    0.1     # mm
                   ,'BiasedB2ee_ElecIPCHI2'  : -999.

                   
                   #------------------
                   # Biased, Track fitted 
                   #------------------
                   ,'BiasedTFDiElectron_L0Req'               :  "L0_CHANNEL('Electron')"
                   ,'BiasedTFDiElectron_Hlt1Req'             :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"  
                   
                   ,'BiasedTFDiElectron_MinMass'     : 2000.      # MeV
                   ,'BiasedTFDiElectron_VtxCHI2'     :   25.
                   ,'BiasedTFDiElectron_PT'          : -999.      # MeV
                   ,'BiasedTFDiElectron_ElecPT'      :  300.      # MeV
                   ,'BiasedTFDiElectron_ElecPIDe'    :    1.5
                   ,'BiasedTFDiElectron_ElecIP'      :    0.1     # mm
                   ,'BiasedTFDiElectron_ElecIPCHI2'  : -999.                          
                   
                   # Di-Electron Low mass
                   ,'BiasedTFDiElectronLowMass_MinMass'     :    0.      # MeV
                   ,'BiasedTFDiElectronLowMass_VtxCHI2'     :   25.
                   ,'BiasedTFDiElectronLowMass_PT'          : -999.      # MeV
                   ,'BiasedTFDiElectronLowMass_ElecPT'      :  300.      # MeV
                   ,'BiasedTFDiElectronLowMass_ElecPIDe'    :    1.5
                   ,'BiasedTFDiElectronLowMass_ElecIP'      :    0.1     # mm
                   ,'BiasedTFDiElectronLowMass_ElecIPCHI2'  : -999.
                   
                   # Di-Electron Low PID
                   ,'BiasedTFDiElectronLowPID_MinMass'     : 2000.      # MeV
                   ,'BiasedTFDiElectronLowPID_VtxCHI2'     :   25.
                   ,'BiasedTFDiElectronLowPID_PT'          : -999.      # MeV
                   ,'BiasedTFDiElectronLowPID_ElecPT'      :  300.      # MeV
                   ,'BiasedTFDiElectronLowPID_ElecPIDe'    :   -2.
                   ,'BiasedTFDiElectronLowPID_ElecIP'      :    0.1     # mm
                   ,'BiasedTFDiElectronLowPID_ElecIPCHI2'  : -999.                   
                   
                   # Jpsi-> ee
                   ,'BiasedTFJpsi2ee_MinMass'     : 2500.      # MeV
                   ,'BiasedTFJpsi2ee_MaxMass'     : 3300.      # MeV
                   ,'BiasedTFJpsi2ee_VtxCHI2'     :   25.
                   ,'BiasedTFJpsi2ee_PT'          : -999.      # MeV
                   ,'BiasedTFJpsi2ee_ElecPT'      :  300.      # MeV
                   ,'BiasedTFJpsi2ee_ElecPIDe'    :    1.5  
                   ,'BiasedTFJpsi2ee_ElecIP'      :    0.1     # mm
                   ,'BiasedTFJpsi2ee_ElecIPCHI2'  : -999.
                   
                   # Psi-> ee
                   ,'BiasedTFPsi2ee_MinMass'     : 3300.      # MeV
                   ,'BiasedTFPsi2ee_MaxMass'     : 3900.      # MeV
                   ,'BiasedTFPsi2ee_VtxCHI2'     :   25.
                   ,'BiasedTFPsi2ee_PT'          : -999.      # MeV
                   ,'BiasedTFPsi2ee_ElecPT'      :  300.      # MeV
                   ,'BiasedTFPsi2ee_ElecPIDe'    :    1.5  
                   ,'BiasedTFPsi2ee_ElecIP'      :    0.1     # mm
                   ,'BiasedTFPsi2ee_ElecIPCHI2'  : -999.
                   
                   # B-> ee
                   ,'BiasedTFB2ee_MinMass'     : 4800.      # MeV
                   ,'BiasedTFB2ee_MaxMass'     : 1.0e+10    # MeV
                   ,'BiasedTFB2ee_VtxCHI2'     :   25.
                   ,'BiasedTFB2ee_PT'          : -999.      # MeV
                   ,'BiasedTFB2ee_ElecPT'      :  300.      # MeV
                   ,'BiasedTFB2ee_ElecPIDe'    :    1.5  
                   ,'BiasedTFB2ee_ElecIP'      :    0.1     # mm
                   ,'BiasedTFB2ee_ElecIPCHI2'  : -999.
                   
                   
                   ,'HltANNSvcID'  : {
                                      'UnbiasedDiElectron'         :  51200
                                     ,'UnbiasedDiElectronLowMass'  :  51210
                                     ,'UnbiasedDiElectronLowPID'   :  51220
                                     ,'UnbiasedJpsi2ee'            :  51201
                                     ,'UnbiasedPsi2ee'             :  51202
                                     ,'UnbiasedB2ee'               :  51203
                                     # Track fitted 
                                     ,'UnbiasedTFDiElectron'         :  51250
                                     ,'UnbiasedTFDiElectronHighMass' :  51254
                                     ,'UnbiasedTFDiElectronVHighMass':  51255
                                     ,'UnbiasedTFDiElectronLowMass'  :  51260
                                     ,'UnbiasedTFDiElectronLowPID'   :  51270 
                                     ,'UnbiasedTFJpsi2ee'            :  51251
                                     ,'UnbiasedTFPsi2ee'             :  51252
                                     ,'UnbiasedTFB2ee'               :  51253

                                     ,'BiasedDiElectron'         :  51100
                                     ,'BiasedDiElectronLowMass'  :  51110
                                     ,'BiasedDiElectronLowPID'   :  51120
                                     ,'BiasedJpsi2ee'            :  51101
                                     ,'BiasedPsi2ee'             :  51102
                                     ,'BiasedB2ee'               :  51103
                                     # Track fitted 
                                     ,'BiasedTFDiElectron'         :  51150
                                     ,'BiasedTFDiElectronLowMass'  :  51160
                                     ,'BiasedTFDiElectronLowPID'   :  51170 
                                     ,'BiasedTFJpsi2ee'            :  51151
                                     ,'BiasedTFPsi2ee'             :  51152
                                     ,'BiasedTFB2ee'               :  51153 
                                     }

                   }
    

    def __apply_configuration__(self) :
        self.__makeHlt2DYAndZ2eeLines()
        self.__makeHlt2UnbiasedDiElectronLines()
        self.__makeHlt2UnbiasedTFDiElectronLines()
        self.__makeHlt2BiasedDiElectronLines()
        self.__makeHlt2BiasedTFDiElectronLines()       


    def __makeHlt2DYAndZ2eeLines(self):
        ## @file
        #
        #  Hlt2 dielectron selections
        #
        #  @author D.R.Ward
        #  @date 2010-04-20
        #
        ##
        
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop, CombineParticles        
        from Hlt2SharedParticles.BasicParticles import Electrons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons
        
        #some string definitions... 

        
#        TKQuality =  " & (MIPDV(PRIMARY)<"+str(self.getProp('DiElectronIP'))+"*mm) & (TRCHI2DOF<"+str(self.getProp('DiElectronTkChi2'))+")"
        TKQuality =  " & (TRCHI2DOF<"+str(self.getProp('DiElectronTkChi2'))+")"                                
                            

        #--------------------------------------------
        '''
        DY->ee lowest mass range
        '''

        combineDY1 = Hlt2Member( CombineParticles # type
                                 , "CombineDY1" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY1MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY1MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2PDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY1MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ BiKalmanFittedElectrons ]
                                 )
       
        line1 = Hlt2Line('DYee1'
                        , prescale = self.prescale 
                        , algos = [ BiKalmanFittedElectrons, combineDY1 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee1Decision":   50291 } )

        #--------------------------------------------
        '''
        DY->ee middle mass range
        '''

        combineDY2 = Hlt2Member( CombineParticles # type
                                 , "CombineDY2" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY2MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY2MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2PDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY2MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ BiKalmanFittedElectrons ]
                                 )
       
        line2 = Hlt2Line('DYee2'
                        , prescale = self.prescale 
                        , algos = [ BiKalmanFittedElectrons, combineDY2 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee2Decision":   50292 } )

        #--------------------------------------------
        '''
        DY->ee highest mass range
        '''

        combineDY3 = Hlt2Member( CombineParticles # type
                                 , "CombineDY3" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY3MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY3MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2PDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY3MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ BiKalmanFittedElectrons ]
                                 )
       
        line3 = Hlt2Line('DYee3'
                        , prescale = self.prescale 
                        , algos = [ BiKalmanFittedElectrons, combineDY3 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee3Decision":   50293 } )
        #--------------------------------------------
        '''
        Zee
        '''

        combineZ = Hlt2Member( CombineParticles # type
                               , "CombineZ" # name 
                               , DecayDescriptor = "Z0 -> e+ e-"
                               , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('ZeePt'))+"*MeV)" + TKQuality ,
                                                   "e-" : "(PT>"+str(self.getProp('ZeePt'))+"*MeV)" + TKQuality }
                               , MotherCut = "(VFASPF(VCHI2PDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                               , CombinationCut = "(AM>"+str(self.getProp('ZeeMinMass'))+"*MeV) & "\
                                                  "(abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                               , InputLocations  = [ BiKalmanFittedElectrons ]
                               )
       
        line4 = Hlt2Line('Zee'
                        , prescale = self.prescale 
                        , algos = [ BiKalmanFittedElectrons, combineZ ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2ZeeDecision":   50295 } )

        ####################################################################
        #  Versions using unfitted tracks
        ####################################################################


        #--------------------------------------------
        '''
        DY->ee lowest mass range
        '''

        combineDY1U = Hlt2Member( CombineParticles # type
                                 , "CombineDY1U" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY1MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY1MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2PDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY1MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ Electrons ]
                                 )
       
        line1U = Hlt2Line('DYee1Unfitted'
                        , prescale = self.prescale 
                        , algos = [ Electrons, combineDY1U ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee1UnfittedDecision":   50296 } )

        #--------------------------------------------
        '''
        DY->ee middle mass range
        '''

        combineDY2U = Hlt2Member( CombineParticles # type
                                 , "CombineDY2U" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY2MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY2MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2PDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY2MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ Electrons ]
                                 )
       
        line2U = Hlt2Line('DYee2Unfitted'
                        , prescale = self.prescale 
                        , algos = [ Electrons, combineDY2U ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee2UnfittedDecision":   50297 } )

        #--------------------------------------------
        '''
        DY->ee highest mass range
        '''

        combineDY3U = Hlt2Member( CombineParticles # type
                                 , "CombineDY3U" # name 
                                 , DecayDescriptor = "Z0 -> e+ e-"
                                 , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('DY3MinPt'))+"*MeV)" + TKQuality ,
                                                     "e-" : "(PT>"+str(self.getProp('DY3MinPt'))+"*MeV)" + TKQuality }
                                 , MotherCut = "(VFASPF(VCHI2PDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                                 , CombinationCut = "(AM>"+str(self.getProp('DY3MinMass'))+"*MeV)"\
                                                 " & (abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                                 , InputLocations  = [ Electrons ]
                                 )
       
        line3U = Hlt2Line('DYee3Unfitted'
                        , prescale = self.prescale 
                        , algos = [ Electrons, combineDY3U ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DYee3UnfittedDecision":   50298 } )
        #--------------------------------------------
        '''
        Zee
        '''

        combineZU = Hlt2Member( CombineParticles # type
                               , "CombineZU" # name 
                               , DecayDescriptor = "Z0 -> e+ e-"
                               , DaughtersCuts = { "e+" : "(PT>"+str(self.getProp('ZeePt'))+"*MeV)" + TKQuality ,
                                                   "e-" : "(PT>"+str(self.getProp('ZeePt'))+"*MeV)" + TKQuality }
                               , MotherCut = "(VFASPF(VCHI2PDOF)<"+str(self.getProp('DiElectronVertexChi2'))+")"
                               , CombinationCut = "(AM>"+str(self.getProp('ZeeMinMass'))+"*MeV) & "\
                                                  "(abs(ACHILD(PHI,1)-ACHILD(PHI,2))>"+str(self.getProp('DiElectronDphiMin'))+")"
                               , InputLocations  = [ Electrons ]
                               )
       
        line4U = Hlt2Line('ZeeUnfitted'
                        , prescale = self.prescale 
                        , algos = [ Electrons, combineZU ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2ZeeUnfittedDecision":   50299 } )



    def __makeHlt2UnbiasedDiElectronLines(self):
        
        #####################################################################################
        #
        # Unbiased Di-Electron Lines using unfitted tracks 
        #
        # 1. 'Hlt2UnbiasedDiElectron',          Mass from 2. GeV
        # 2. 'Hlt2UnbiasedDiElectronLowMass'    Mass sidebands from 0.5 GeV, pre-scaled
        # 3. 'Hlt2UnbiasedDiElectronLowPID'     Mass from 2. GeV, PIDe is default (-2.)
        # 4. 'Hlt2UnbiasedJpsi2ee'              Jpsi -> ee
        # 5. 'Hlt2UnbiasedPsi2ee'               Psi(2S) -> ee
        # 6. 'Hlt2UnbiasedB2ee'                 For all with Mass > 5.2 GeV
        #
        # Author: Jibo.He@cern.ch
        # @ 12 May 2010 
        # 
        #####################################################################################

        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.DiElectron import DiElectron

        """
        #------------------------
        # L0 & Hlt1 Requirements
        #------------------------ 
        """
        L0Req   = self.getProp("UnbiasedDiElectron_L0Req")
        Hlt1Req = self.getProp("UnbiasedDiElectron_Hlt1Req")
        
        if not L0Req:
            L0Req = None
            
        if not Hlt1Req:
            Hlt1Req= None
        
        #--------------------------------------------
        # Filter DiElectron
        #--------------------------------------------
        
        FilterDiElectron = Hlt2Member( FilterDesktop # type
                                       , "FilterDiElectron" 
                                       , Code ="(MINTREE('e+'==ABSID,PT) > %(UnbiasedDiElectron_ElecPT)s *MeV)"\
                                       " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedDiElectron_ElecPIDe)s)"\
                                       " & (MM > %(UnbiasedDiElectron_MinMass)s *MeV)"\
                                       " & (VFASPF(VCHI2PDOF) < %(UnbiasedDiElectron_VtxCHI2)s)"\
                                       " & (PT > %(UnbiasedDiElectron_PT)s *MeV)" %self.getProps() 
                                       , InputLocations = [ DiElectron ]
                                       , InputPrimaryVertices = "None"
                                       , UseP2PVRelations = False
                                       )


        #--------------------------------------------
        # Unbiased DiElectron, prescaled
        #--------------------------------------------
        UnbiasedDiElectronLine = Hlt2Line("UnbiasedDiElectron"
                                          , prescale = self.prescale
                                          , L0DU = L0Req
                                          , HLT  = Hlt1Req
                                          , algos = [ DiElectron, FilterDiElectron ]
                                          , postscale = self.postscale
                                          )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedDiElectronDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedDiElectron'] } )

        #--------------------------------------------
        # Unbiased DiElectron, low mass prescaled
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedDiElectronLowMass"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedDiElectronLowMass_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedDiElectronLowMass_ElecPIDe)s)"\
                                      " & (MM > %(UnbiasedDiElectronLowMass_MinMass)s *MeV)"\
                                      " & (VFASPF(VCHI2PDOF) < %(UnbiasedDiElectronLowMass_VtxCHI2)s)"\
                                      " & (PT > %(UnbiasedDiElectronLowMass_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedDiElectronLowMassDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedDiElectronLowMass'] } ) 


        #--------------------------------------------
        # Unbiased DiElectron, low PID prescaled
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedDiElectronLowPID"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedDiElectronLowPID_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedDiElectronLowPID_ElecPIDe)s)"\
                                      " & (MM > %(UnbiasedDiElectronLowPID_MinMass)s *MeV)"\
                                      " & (VFASPF(VCHI2PDOF) < %(UnbiasedDiElectronLowPID_VtxCHI2)s)"\
                                      " & (PT > %(UnbiasedDiElectronLowPID_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedDiElectronLowPIDDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedDiElectronLowPID'] } )         

        #--------------------------------------------
        # Jpsi -> e+ e-
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedJpsi2ee"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedJpsi2ee_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedJpsi2ee_ElecPIDe)s)"\
                                      " & (in_range( %(UnbiasedJpsi2ee_MinMass)s *MeV, MM,  %(UnbiasedJpsi2ee_MaxMass)s *MeV))"\
                                      " & (VFASPF(VCHI2PDOF) < %(UnbiasedJpsi2ee_VtxCHI2)s)"\
                                      " & (PT > %(UnbiasedJpsi2ee_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedJpsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedJpsi2ee'] } ) 


        #--------------------------------------------
        # Psi(2S) -> e+ e-
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedPsi2ee"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedPsi2ee_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedPsi2ee_ElecPIDe)s)"\
                                      " & (in_range( %(UnbiasedPsi2ee_MinMass)s *MeV, MM , %(UnbiasedPsi2ee_MaxMass)s *MeV))"\
                                      " & (VFASPF(VCHI2PDOF) < %(UnbiasedPsi2ee_VtxCHI2)s)"\
                                      " & (PT > %(UnbiasedPsi2ee_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedPsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedPsi2ee'] } ) 


        #--------------------------------------------
        # B, Upsilon -> e+ e-
        #--------------------------------------------
        UnbiasedDiElectronLine.clone("UnbiasedB2ee"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ DiElectron, FilterDiElectron ]
                                     , FilterDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedB2ee_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedB2ee_ElecPIDe)s)"\
                                      " & (in_range(%(UnbiasedB2ee_MinMass)s *MeV), MM,  %(UnbiasedB2ee_MaxMass)s *MeV))"\
                                      " & (VFASPF(VCHI2PDOF) < %(UnbiasedB2ee_VtxCHI2)s)"\
                                      " & (PT > %(UnbiasedB2ee_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedB2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedB2ee'] } )



    def __makeHlt2UnbiasedTFDiElectronLines(self):

        #####################################################################################
        #
        # Unbiased Di-Electron Lines using fitted tracks 
        #
        # 1. 'Hlt2UnbiasedTFDiElectron',          Mass from 2. GeV
        # 2. 'Hlt2UnbiasedTFDiElectronLowMass'    Mass sidebands from 0.5 GeV, pre-scaled
        # 3. 'Hlt2UnbiasedTFDiElectronLowPID'     Mass from 2. GeV, PIDe is default (-2.)
        # 4. 'Hlt2UnbiasedTFJpsi2ee'              Jpsi -> ee
        # 5. 'Hlt2UnbiasedTFPsi2ee'               Psi(2S) -> ee
        # 6. 'Hlt2UnbiasedTFB2ee'                 For all with Mass > 5.2 GeV
        #
        # Author: Jibo.He@cern.ch
        # @ 12 May 2010 
        # 
        #####################################################################################
        
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedDiElectron import TrackFittedDiElectron


        """
        #------------------------
        # L0 & Hlt1 Requirements
        #------------------------ 
        """
        L0Req   = self.getProp("UnbiasedTFDiElectron_L0Req")
        Hlt1Req = self.getProp("UnbiasedTFDiElectron_Hlt1Req")
        
        if not L0Req:
            L0Req = None
            
        if not Hlt1Req:
            Hlt1Req= None


        #--------------------------------------------
        # Filter TFDiElectron
        #--------------------------------------------
        
        FilterTFDiElectron = Hlt2Member( FilterDesktop # type
                                         , "FilterTFDiElectron" 
                                         , Code ="(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFDiElectron_ElecPT)s *MeV)"\
                                         " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedTFDiElectron_ElecPIDe)s)"\
                                         " & (MM > %(UnbiasedTFDiElectron_MinMass)s *MeV)"\
                                         " & (VFASPF(VCHI2PDOF) < %(UnbiasedTFDiElectron_VtxCHI2)s)"\
                                         " & (PT > %(UnbiasedTFDiElectron_PT)s *MeV)" %self.getProps() 
                                         , InputLocations = [ TrackFittedDiElectron ]
                                         , InputPrimaryVertices = "None"
                                         , UseP2PVRelations = False
                                         )

        
        #--------------------------------------------
        # UnbiasedTF DiElectron, prescaled
        #--------------------------------------------

        UnbiasedTFDiElectronLine = Hlt2Line("UnbiasedTFDiElectron"
                                            , prescale = self.prescale
                                            , L0DU = L0Req
                                            , HLT  = Hlt1Req
                                            , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                            , postscale = self.postscale
                                            )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFDiElectronDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFDiElectron'] } ) 

        #--------------------------------------------
        # Filter TFDiElectronHighMass
        # D.R.Ward - for high mass/pT replace PIDe cut by simple calo cuts
        #--------------------------------------------
        
        FilterTFDiElectronHighMass = Hlt2Member( FilterDesktop # type
                                                 , "FilterTFDiElectronHighMass" 
                                                 , Code ="(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFDiElectronHighMass_ElecPT)s *MeV)"\
                                                 " & (MINTREE('e+'==ABSID,PPINFO(LHCb.ProtoParticle.CaloPrsE,0))>%(UnbiasedTFDiElectronHighMass_PrsMin)s)"\
                                                 " & (MINTREE('e+'==ABSID,PPINFO(LHCb.ProtoParticle.CaloEcalE,0)/P)>%(UnbiasedTFDiElectronHighMass_EcalMin)s)"\
                                                 " & (MAXTREE('e+'==ABSID,PPINFO(LHCb.ProtoParticle.CaloHcalE,0)/P)<%(UnbiasedTFDiElectronHighMass_HcalMax)s)"\
                                                 " & (MAXTREE('e+'==ABSID,TRCHI2DOF)<%(UnbiasedTFDiElectronHighMass_TkChi2)s)"\
                                                 " & (MM > %(UnbiasedTFDiElectronHighMass_MinMass)s *MeV)"\
                                                 " & (VFASPF(VCHI2PDOF) < %(UnbiasedTFDiElectronHighMass_VtxCHI2)s)"\
                                                 " & (PT > %(UnbiasedTFDiElectronHighMass_PT)s *MeV)" %self.getProps() 
                                                 , InputLocations = [ TrackFittedDiElectron ]
                                                 , InputPrimaryVertices = "None"
                                                 , UseP2PVRelations = False
                                                 )

        FilterTFDiElectronVHighMass = Hlt2Member( FilterDesktop # type
                                                  , "FilterTFDiElectronVHighMass" 
                                                  , Code ="(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFDiElectronVHighMass_ElecPT)s *MeV)"\
                                                  " & (MINTREE('e+'==ABSID,PPINFO(LHCb.ProtoParticle.CaloPrsE,0))>%(UnbiasedTFDiElectronHighMass_PrsMin)s)"\
                                                  " & (MINTREE('e+'==ABSID,PPINFO(LHCb.ProtoParticle.CaloEcalE,0)/P)>%(UnbiasedTFDiElectronHighMass_EcalMin)s)"\
                                                  " & (MAXTREE('e+'==ABSID,PPINFO(LHCb.ProtoParticle.CaloHcalE,0)/P)<%(UnbiasedTFDiElectronHighMass_HcalMax)s)"\
                                                  " & (MAXTREE('e+'==ABSID,TRCHI2DOF)<%(UnbiasedTFDiElectronHighMass_TkChi2)s)"\
                                                  " & (MM > %(UnbiasedTFDiElectronVHighMass_MinMass)s *MeV)"\
                                                  " & (VFASPF(VCHI2PDOF) < %(UnbiasedTFDiElectronHighMass_VtxCHI2)s)"\
                                                  " & (PT > %(UnbiasedTFDiElectronHighMass_PT)s *MeV)" %self.getProps() 
                                                  , InputLocations = [ TrackFittedDiElectron ]
                                                  , InputPrimaryVertices = "None"
                                                  , UseP2PVRelations = False
                                                  )
        #--------------------------------------------
        # UnbiasedTF DiElectron
        # D.R.Ward - (very )high mass versions
        #--------------------------------------------

        UnbiasedTFDiElectronHighMassLine = Hlt2Line("UnbiasedTFDiElectronHighMass"
                                            , prescale = self.prescale
                                            , L0DU = L0Req
                                            , HLT  = Hlt1Req
                                            , algos = [ TrackFittedDiElectron, FilterTFDiElectronHighMass ]
                                            , postscale = self.postscale
                                            )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFDiElectronHighMassDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFDiElectronHighMass'] } ) 

        UnbiasedTFDiElectronVHighMassLine = Hlt2Line("UnbiasedTFDiElectronVHighMass"
                                            , prescale = self.prescale
                                            , L0DU = L0Req
                                            , HLT  = Hlt1Req
                                            , algos = [ TrackFittedDiElectron, FilterTFDiElectronVHighMass ]
                                            , postscale = self.postscale
                                            )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFDiElectronVHighMassDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFDiElectronVHighMass'] } ) 
        
        
        #--------------------------------------------
        # UnbiasedTF DiElectron, low mass prescaled
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFDiElectronLowMass"
                                       , prescale = self.prescale
                                       , L0DU = L0Req
                                       , HLT  = Hlt1Req
                                       , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                       , FilterTFDiElectron =
                                       {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFDiElectronLowMass_ElecPT)s *MeV)"\
                                        " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedTFDiElectronLowMass_ElecPIDe)s)"\
                                        " & (MM > %(UnbiasedTFDiElectronLowMass_MinMass)s *MeV)"\
                                        " & (VFASPF(VCHI2PDOF) < %(UnbiasedTFDiElectronLowMass_VtxCHI2)s)"\
                                        " & (PT > %(UnbiasedTFDiElectronLowMass_PT)s *MeV)" %self.getProps() 
                                        }
                                       , postscale = self.postscale
                                       )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFDiElectronLowMassDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFDiElectronLowMass'] } ) 
        
        
        #--------------------------------------------
        # UnbiasedTF DiElectron, low PID prescaled
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFDiElectronLowPID"
                                       , prescale = self.prescale
                                       , L0DU = L0Req
                                       , HLT  = Hlt1Req
                                       , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                       , FilterTFDiElectron =
                                       {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFDiElectronLowPID_ElecPT)s *MeV)"\
                                        " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedTFDiElectronLowPID_ElecPIDe)s)"\
                                        " & (MM > %(UnbiasedTFDiElectronLowPID_MinMass)s *MeV)"\
                                        " & (VFASPF(VCHI2PDOF) < %(UnbiasedTFDiElectronLowPID_VtxCHI2)s)"\
                                        " & (PT > %(UnbiasedTFDiElectronLowPID_PT)s *MeV)" %self.getProps() 
                                        }
                                       , postscale = self.postscale
                                       )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFDiElectronLowPIDDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFDiElectronLowPID'] } )         


        #--------------------------------------------
        # Jpsi -> e+ e-
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFJpsi2ee"
                                       , prescale = self.prescale
                                       , L0DU = L0Req
                                       , HLT  = Hlt1Req
                                       , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                       , FilterTFDiElectron =
                                       {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFJpsi2ee_ElecPT)s *MeV)"\
                                        " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedTFJpsi2ee_ElecPIDe)s)"\
                                        " & (in_range( %(UnbiasedTFJpsi2ee_MinMass)s *MeV, MM,  %(UnbiasedTFJpsi2ee_MaxMass)s *MeV))"\
                                        " & (VFASPF(VCHI2PDOF) < %(UnbiasedTFJpsi2ee_VtxCHI2)s)"\
                                        " & (PT > %(UnbiasedTFJpsi2ee_PT)s *MeV)" %self.getProps() 
                                        }
                                       , postscale = self.postscale
                                       )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFJpsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFJpsi2ee'] } ) 


        #--------------------------------------------
        # Psi(2S) -> e+ e-
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFPsi2ee"
                                       , prescale = self.prescale
                                       , L0DU = L0Req
                                       , HLT  = Hlt1Req                                       
                                       , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                       , FilterTFDiElectron =
                                       {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFPsi2ee_ElecPT)s *MeV)"\
                                        " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedTFPsi2ee_ElecPIDe)s)"\
                                        " & (in_range( %(UnbiasedTFPsi2ee_MinMass)s *MeV, MM, %(UnbiasedTFPsi2ee_MaxMass)s *MeV))"\
                                        " & (VFASPF(VCHI2PDOF) < %(UnbiasedTFPsi2ee_VtxCHI2)s)"\
                                        " & (PT > %(UnbiasedTFPsi2ee_PT)s *MeV)" %self.getProps() 
                                        }
                                       , postscale = self.postscale
                                       )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFPsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFPsi2ee'] } ) 
        

        #--------------------------------------------
        # B, Upsilon -> e+ e-
        #--------------------------------------------
        UnbiasedTFDiElectronLine.clone("UnbiasedTFB2ee"
                                       , prescale = self.prescale
                                       , L0DU = L0Req
                                       , HLT  = Hlt1Req        
                                       , algos = [ TrackFittedDiElectron, FilterTFDiElectron ]
                                       , FilterTFDiElectron =
                                       {"Code" : "(MINTREE('e+'==ABSID,PT) > %(UnbiasedTFB2ee_ElecPT)s *MeV)"\
                                        " & (MINTREE('e+'==ABSID,PIDe) > %(UnbiasedTFB2ee_ElecPIDe)s)"\
                                        " & (in_range( %(UnbiasedTFB2ee_MinMass)s *MeV, MM, %(UnbiasedTFB2ee_MaxMass)s *MeV))"\
                                        " & (VFASPF(VCHI2PDOF) < %(UnbiasedTFB2ee_VtxCHI2)s)"\
                                        " & (PT > %(UnbiasedTFB2ee_PT)s *MeV)" %self.getProps() 
                                        }
                                       , postscale = self.postscale
                                       )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedTFB2eeDecision" :
                                              self.getProp('HltANNSvcID')['UnbiasedTFB2ee'] } )
        

    def __makeHlt2BiasedDiElectronLines(self):

        #####################################################################################
        #
        # Biased Di-Electron Lines using unfitted tracks 
        #
        # 1. 'Hlt2BiasedDiElectron',          Mass from 2. GeV
        # 2. 'Hlt2BiasedDiElectronLowMass'    Mass sidebands from 0.5 GeV, pre-scaled
        # 3. 'Hlt2BiasedDiElectronLowPID'     Mass from 2. GeV, PIDe is default (-2.)
        # 4. 'Hlt2BiasedJpsi2ee'              Jpsi -> ee
        # 5. 'Hlt2BiasedPsi2ee'               Psi(2S) -> ee
        # 6. 'Hlt2BiasedB2ee'                 For all with Mass > 5.2 GeV
        #
        # Author: Jibo.He@cern.ch
        # @ 12 May 2010 
        # 
        #####################################################################################       

        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.DiElectron import DiElectron
        from HltTracking.HltPVs import PV3D

        
        """
        #------------------------
        # L0 & Hlt1 Requirements
        #------------------------ 
        """
        L0Req   = self.getProp("BiasedDiElectron_L0Req")
        Hlt1Req = self.getProp("BiasedDiElectron_Hlt1Req")
        
        if not L0Req:
            L0Req = None
            
        if not Hlt1Req:
            Hlt1Req= None

        
        #--------------------------------------------
        # Filter Baised DiElectron
        #--------------------------------------------
        
        FilterBiasedDiElectron = Hlt2Member( FilterDesktop # type
                                             , "FilterBiasedDiElectron" 
                                             , Code ="(MINTREE('e+'==ABSID,PT) > %(BiasedDiElectron_ElecPT)s *MeV)"\
                                             " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedDiElectron_ElecPIDe)s)"\
                                             " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedDiElectron_ElecIP)s *mm)"\
                                             " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedDiElectron_ElecIPCHI2)s *mm)"\
                                             " & (MM > %(BiasedDiElectron_MinMass)s *MeV)"\
                                             " & (VFASPF(VCHI2PDOF) < %(BiasedDiElectron_VtxCHI2)s)"\
                                             " & (PT > %(BiasedDiElectron_PT)s *MeV)" %self.getProps() 
                                             , InputLocations = [ DiElectron ]
                                             )
        
        #--------------------------------------------
        # Biased DiElectron, prescaled
        #--------------------------------------------
        BiasedDiElectronLine = Hlt2Line("BiasedDiElectron"
                                        , prescale = self.prescale
                                        , L0DU = L0Req
                                        , HLT  = Hlt1Req
                                        , algos = [ PV3D()
                                                    , DiElectron
                                                    , FilterBiasedDiElectron ]
                                        , postscale = self.postscale
                                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiElectronDecision" :
                                              self.getProp('HltANNSvcID')['BiasedDiElectron'] } ) 

        #--------------------------------------------
        # Biased DiElectron, low mass prescaled
        #--------------------------------------------
        BiasedDiElectronLine.clone("BiasedDiElectronLowMass"
                                   , prescale = self.prescale
                                   , L0DU = L0Req
                                   , HLT  = Hlt1Req
                                   , algos = [ PV3D()
                                               , DiElectron
                                               , FilterBiasedDiElectron ]
                                   , FilterBiasedDiElectron =
                                   {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedDiElectronLowMass_ElecPT)s *MeV)"\
                                    " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedDiElectronLowMass_ElecPIDe)s)"\
                                    " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedDiElectronLowMass_ElecIP)s *mm)"\
                                    " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedDiElectronLowMass_ElecIPCHI2)s *mm)"\
                                    " & (MM > %(BiasedDiElectronLowMass_MinMass)s *MeV)"\
                                    " & (VFASPF(VCHI2PDOF) < %(BiasedDiElectronLowMass_VtxCHI2)s)"\
                                    " & (PT > %(BiasedDiElectronLowMass_PT)s *MeV)" %self.getProps() 
                                    }
                                   , postscale = self.postscale
                                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiElectronLowMassDecision" :
                                              self.getProp('HltANNSvcID')['BiasedDiElectronLowMass'] } )

        
        #--------------------------------------------
        # Biased DiElectron, low PID prescaled
        #--------------------------------------------        
        BiasedDiElectronLine.clone("BiasedDiElectronLowPID"
                                   , prescale = self.prescale
                                   , L0DU = L0Req
                                   , HLT  = Hlt1Req                                   
                                   , algos = [ PV3D()
                                               , DiElectron
                                               , FilterBiasedDiElectron ]
                                   , FilterBiasedDiElectron =
                                   {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedDiElectronLowPID_ElecPT)s *MeV)"\
                                    " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedDiElectronLowPID_ElecPIDe)s)"\
                                    " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedDiElectronLowPID_ElecIP)s *mm)"\
                                    " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedDiElectronLowPID_ElecIPCHI2)s *mm)"\
                                    " & (MM > %(BiasedDiElectronLowPID_MinMass)s *MeV)"\
                                    " & (VFASPF(VCHI2PDOF) < %(BiasedDiElectronLowPID_VtxCHI2)s)"\
                                    " & (PT > %(BiasedDiElectronLowPID_PT)s *MeV)" %self.getProps() 
                                    }
                                   , postscale = self.postscale
                                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiElectronLowPIDDecision" :
                                              self.getProp('HltANNSvcID')['BiasedDiElectronLowPID'] } )
        
        #--------------------------------------------
        # Biased Jpsi->ee
        #--------------------------------------------
        BiasedDiElectronLine.clone("BiasedJpsi2ee"
                                   , prescale = self.prescale
                                   , L0DU = L0Req
                                   , HLT  = Hlt1Req                                           
                                   , algos = [ PV3D()
                                               , DiElectron
                                               , FilterBiasedDiElectron ]
                                   , FilterBiasedDiElectron =
                                   {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedJpsi2ee_ElecPT)s *MeV)"\
                                    " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedJpsi2ee_ElecPIDe)s)"\
                                    " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedJpsi2ee_ElecIP)s *mm)"\
                                    " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedJpsi2ee_ElecIPCHI2)s *mm)"\
                                    " & (in_range( %(BiasedJpsi2ee_MinMass)s *MeV, MM, %(BiasedJpsi2ee_MaxMass)s *MeV))"\
                                    " & (VFASPF(VCHI2PDOF) < %(BiasedJpsi2ee_VtxCHI2)s)"\
                                    " & (PT > %(BiasedJpsi2ee_PT)s *MeV)" %self.getProps() 
                                    }
                                   , postscale = self.postscale
                                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedJpsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['BiasedJpsi2ee'] } )

        
        #--------------------------------------------
        # Biased Psi->ee
        #--------------------------------------------
        BiasedDiElectronLine.clone("BiasedPsi2ee"
                                   , prescale = self.prescale
                                   , L0DU = L0Req
                                   , HLT  = Hlt1Req        
                                   , algos = [ PV3D()
                                               , DiElectron
                                               , FilterBiasedDiElectron ]
                                   , FilterBiasedDiElectron =
                                   {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedPsi2ee_ElecPT)s *MeV)"\
                                    " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedPsi2ee_ElecPIDe)s)"\
                                    " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedPsi2ee_ElecIP)s *mm)"\
                                    " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedPsi2ee_ElecIPCHI2)s *mm)"\
                                    " & (in_range( %(BiasedPsi2ee_MinMass)s *MeV, MM,  %(BiasedPsi2ee_MaxMass)s *MeV))"\
                                    " & (VFASPF(VCHI2PDOF) < %(BiasedPsi2ee_VtxCHI2)s)"\
                                    " & (PT > %(BiasedPsi2ee_PT)s *MeV)" %self.getProps() 
                                    }
                                   , postscale = self.postscale
                                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedPsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['BiasedPsi2ee'] } )       
        
        
        #--------------------------------------------
        # Biased B->ee
        #--------------------------------------------
        BiasedDiElectronLine.clone("BiasedB2ee"
                                   , prescale = self.prescale
                                   , L0DU = L0Req
                                   , HLT  = Hlt1Req        
                                   , algos = [ PV3D()
                                               , DiElectron
                                               , FilterBiasedDiElectron ]
                                   , FilterBiasedDiElectron =
                                   {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedB2ee_ElecPT)s *MeV)"\
                                    " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedB2ee_ElecPIDe)s)"\
                                    " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedB2ee_ElecIP)s *mm)"\
                                    " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedB2ee_ElecIPCHI2)s *mm)"\
                                    " & (in_range( %(BiasedB2ee_MinMass)s *MeV, MM, %(BiasedB2ee_MaxMass)s *MeV))"\
                                    " & (VFASPF(VCHI2PDOF) < %(BiasedB2ee_VtxCHI2)s)"\
                                    " & (PT > %(BiasedB2ee_PT)s *MeV)" %self.getProps() 
                                    }
                                   , postscale = self.postscale
                                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedB2eeDecision" :
                                              self.getProp('HltANNSvcID')['BiasedB2ee'] } )             
        

    def __makeHlt2BiasedTFDiElectronLines(self):

        #####################################################################################
        #
        # Biased Di-Electron Lines using fitted tracks 
        #
        # 1. 'Hlt2BiasedTFDiElectron',          Mass from 2. GeV
        # 2. 'Hlt2BiasedTFDiElectronLowMass'    Mass sidebands from 0.5 GeV, pre-scaled
        # 3. 'Hlt2BiasedTFDiElectronLowPID'     Mass from 2. GeV, PIDe is default (-2.)
        # 4. 'Hlt2BiasedTFJpsi2ee'              Jpsi -> ee
        # 5. 'Hlt2BiasedTFPsi2ee'               Psi(2S) -> ee
        # 6. 'Hlt2BiasedTFB2ee'                 For all with Mass > 5.2 GeV
        #
        # Author: Jibo.He@cern.ch
        # @ 12 May 2010 
        # 
        #####################################################################################      
        
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedDiElectron import TrackFittedDiElectron
        from HltTracking.HltPVs import PV3D

        """
        #------------------------
        # L0 & Hlt1 Requirements
        #------------------------ 
        """
        L0Req   = self.getProp("BiasedTFDiElectron_L0Req")
        Hlt1Req = self.getProp("BiasedTFDiElectron_Hlt1Req")
        
        if not L0Req:
            L0Req = None

        if not Hlt1Req:
            Hlt1Req= None


        #--------------------------------------------
        # Filter baised track fitted DiElectron
        #--------------------------------------------
                
        FilterBiasedTFDiElectron = Hlt2Member( FilterDesktop # type
                                               , "FilterBiasedTFDiElectron" 
                                               , Code ="(MINTREE('e+'==ABSID,PT) > %(BiasedTFDiElectron_ElecPT)s *MeV)"\
                                               " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedTFDiElectron_ElecPIDe)s)"\
                                               " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedTFDiElectron_ElecIP)s *mm)"\
                                               " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedTFDiElectron_ElecIPCHI2)s *mm)"\
                                               " & (MM > %(BiasedTFDiElectron_MinMass)s *MeV)"\
                                               " & (VFASPF(VCHI2PDOF) < %(BiasedTFDiElectron_VtxCHI2)s)"\
                                               " & (PT > %(BiasedTFDiElectron_PT)s *MeV)" %self.getProps() 
                                               , InputLocations = [ TrackFittedDiElectron ]
                                               )
        
        #--------------------------------------------
        # Biased track fitted DiElectron, prescaled
        #--------------------------------------------
        BiasedTFDiElectronLine = Hlt2Line("BiasedTFDiElectron"
                                          , prescale = self.prescale
                                          , L0DU = L0Req
                                          , HLT  = Hlt1Req
                                          , algos = [ PV3D()
                                                      , TrackFittedDiElectron
                                                      , FilterBiasedTFDiElectron ]
                                          , postscale = self.postscale
                                          )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedTFDiElectronDecision" :
                                              self.getProp('HltANNSvcID')['BiasedTFDiElectron'] } ) 

        #--------------------------------------------
        # Biased track fitted DiElectron, low mass prescaled
        #--------------------------------------------
        BiasedTFDiElectronLine.clone("BiasedTFDiElectronLowMass"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ PV3D()
                                                 , TrackFittedDiElectron
                                                 , FilterBiasedTFDiElectron ]
                                     , FilterBiasedTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedTFDiElectronLowMass_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedTFDiElectronLowMass_ElecPIDe)s)"\
                                      " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedTFDiElectronLowMass_ElecIP)s *mm)"\
                                      " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedTFDiElectronLowMass_ElecIPCHI2)s *mm)"\
                                      " & (MM > %(BiasedTFDiElectronLowMass_MinMass)s *MeV)"\
                                      " & (VFASPF(VCHI2PDOF) < %(BiasedTFDiElectronLowMass_VtxCHI2)s)"\
                                      " & (PT > %(BiasedTFDiElectronLowMass_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedTFDiElectronLowMassDecision" :
                                              self.getProp('HltANNSvcID')['BiasedTFDiElectronLowMass'] } )

        
        #--------------------------------------------
        # Biased track fitted DiElectron, low PID prescaled
        #--------------------------------------------
        BiasedTFDiElectronLine.clone("BiasedTFDiElectronLowPID"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ PV3D()
                                                 , TrackFittedDiElectron
                                                 , FilterBiasedTFDiElectron ]
                                     , FilterBiasedTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedTFDiElectronLowPID_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedTFDiElectronLowPID_ElecPIDe)s)"\
                                      " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedTFDiElectronLowPID_ElecIP)s *mm)"\
                                      " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedTFDiElectronLowPID_ElecIPCHI2)s *mm)"\
                                      " & (MM > %(BiasedTFDiElectronLowPID_MinMass)s *MeV)"\
                                      " & (VFASPF(VCHI2PDOF) < %(BiasedTFDiElectronLowPID_VtxCHI2)s)"\
                                      " & (PT > %(BiasedTFDiElectronLowPID_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedTFDiElectronLowPIDDecision" :
                                              self.getProp('HltANNSvcID')['BiasedTFDiElectronLowPID'] } )

        
        #--------------------------------------------
        # Biased track fitted Jpsi->ee
        #--------------------------------------------
        BiasedTFDiElectronLine.clone("BiasedTFJpsi2ee"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ PV3D()
                                                 , TrackFittedDiElectron
                                                 , FilterBiasedTFDiElectron ]
                                     , FilterBiasedTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedTFJpsi2ee_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedTFJpsi2ee_ElecPIDe)s)"\
                                      " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedTFJpsi2ee_ElecIP)s *mm)"\
                                      " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedTFJpsi2ee_ElecIPCHI2)s *mm)"\
                                      " & (in_range( %(BiasedTFJpsi2ee_MinMass)s *MeV, MM,  %(BiasedTFJpsi2ee_MaxMass)s *MeV))"\
                                      " & (VFASPF(VCHI2PDOF) < %(BiasedTFJpsi2ee_VtxCHI2)s)"\
                                      " & (PT > %(BiasedTFJpsi2ee_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedTFJpsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['BiasedTFJpsi2ee'] } )

        
        #--------------------------------------------
        # Biased Psi->ee track fitted
        #--------------------------------------------
        BiasedTFDiElectronLine.clone("BiasedTFPsi2ee"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ PV3D()
                                                 , TrackFittedDiElectron
                                                 , FilterBiasedTFDiElectron ]
                                     , FilterBiasedTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedTFPsi2ee_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedTFPsi2ee_ElecPIDe)s)"\
                                      " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedTFPsi2ee_ElecIP)s *mm)"\
                                      " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedTFPsi2ee_ElecIPCHI2)s *mm)"\
                                      " & (in_range( %(BiasedTFPsi2ee_MinMass)s *MeV, MM, %(BiasedTFPsi2ee_MaxMass)s *MeV))"\
                                      " & (VFASPF(VCHI2PDOF) < %(BiasedTFPsi2ee_VtxCHI2)s)"\
                                      " & (PT > %(BiasedTFPsi2ee_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedTFPsi2eeDecision" :
                                              self.getProp('HltANNSvcID')['BiasedTFPsi2ee'] } )       
        
        
        #--------------------------------------------
        # Biased B->ee track fitted
        #--------------------------------------------
        BiasedTFDiElectronLine.clone("BiasedTFB2ee"
                                     , prescale = self.prescale
                                     , L0DU = L0Req
                                     , HLT  = Hlt1Req
                                     , algos = [ PV3D()
                                                 , TrackFittedDiElectron
                                                 , FilterBiasedTFDiElectron ]
                                     , FilterBiasedTFDiElectron =
                                     {"Code" : "(MINTREE('e+'==ABSID,PT) > %(BiasedTFB2ee_ElecPT)s *MeV)"\
                                      " & (MINTREE('e+'==ABSID,PIDe) > %(BiasedTFB2ee_ElecPIDe)s)"\
                                      " & (MINTREE('e+'==ABSID, MIPDV(PRIMARY)) > %(BiasedTFB2ee_ElecIP)s *mm)"\
                                      " & (MINTREE('e+'==ABSID, MIPCHI2DV(PRIMARY)) > %(BiasedTFB2ee_ElecIPCHI2)s *mm)"\
                                      " & (in_range( %(BiasedTFB2ee_MinMass)s *MeV, MM, %(BiasedTFB2ee_MaxMass)s *MeV))"\
                                      " & (VFASPF(VCHI2PDOF) < %(BiasedTFB2ee_VtxCHI2)s)"\
                                      " & (PT > %(BiasedTFB2ee_PT)s *MeV)" %self.getProps() 
                                      }
                                     , postscale = self.postscale
                                     )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedTFB2eeDecision" :
                                              self.getProp('HltANNSvcID')['BiasedTFB2ee'] } )    
        
