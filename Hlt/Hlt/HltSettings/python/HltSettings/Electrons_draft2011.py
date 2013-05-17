class Electrons_draft2011:
    """
    All Hlt2 electrons lines
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author Jibo.He@cern.ch
    @date 2011-02-10
    
    """

    __all__=('ActiveHlt2Lines', 'Thresholds')

    
    def ActiveHlt2Lines(self):
        """
        Returns a list of active lines
        """
        
        lines = [
            #'Hlt2DiElectron',        
            #'Hlt2DiElectronLowMass',
            #'Hlt2DiElectronLowPID',
            #'Hlt2DiElectronJpsi',          
            #'Hlt2DiElectronPsi',           
            'Hlt2DiElectronB',
            'Hlt2DiElectronHighMass',
            #'Hlt2DiElectronVHighMass',
            # single
            #'Hlt2SingleTFElectron',     
            'Hlt2SingleElectronTFLowPt',     
            'Hlt2SingleElectronTFHighPt',
            'Hlt2SingleTFVHighPtElectron',
            #'Hlt2TFElectronPlusTrack',
            #'Hlt2TFElectronPlusTrackNoIP'
            ]
        
        return lines


    def Thresholds(self):
        
        d = { }
        
        # Inclusive Di-Electron lines
        from Hlt2Lines.Hlt2InclusiveDiElectronLines import Hlt2InclusiveDiElectronLinesConf       
        
        d.update( {

            Hlt2InclusiveDiElectronLinesConf :
            {'Prescale'            : {      
                                                    # Unbiased and Track Fitted (TF)
                                                     'Hlt2DiElectron'         :  1.0
                                                    ,'Hlt2DiElectronHighMass' :  1.0
                                                    ,'Hlt2DiElectronVHighMass':  1.0
                                                    ,'Hlt2DiElectronLowMass'  :  1.0
                                                    ,'Hlt2DiElectronLowPID'   :  0.01
                                                    ,'Hlt2DiElectronJpsi'     :  1.0
                                                    ,'Hlt2DiElectronPsi'      :  1.0
                                                    ,'Hlt2DiElectronB'        :  1.0
                                                    }
                       
             
                   #-----------------
                   # Track fitted 
                   #-----------------
                   ,'DiElectron_L0Req'               :  "L0_CHANNEL('Electron')"
                   ,'DiElectron_Hlt1Req'             :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"  
                   
                   # Di-Electron                   
                   ,'DiElectron_MinMass'    : 2000.      # MeV
                   ,'DiElectron_VtxCHI2'    :   25.
                   ,'DiElectron_PT'         : -999.      # MeV
                   ,'DiElectron_ElecPT'     : 1000.      # MeV
                   ,'DiElectron_ElecPIDe'   :    1.5
                   ,'DiElectron_ElecTkChi2' :   10.
                   
                   # Di-Electron High Mass                   
                   ,'DiElectronHighMass_MinMass'    : 20000.      # MeV
                   ,'DiElectronHighMass_VtxCHI2'    :   25.
                   ,'DiElectronHighMass_TkChi2'     :   10.
                   ,'DiElectronHighMass_PT'         : -999.      # MeV
                   ,'DiElectronHighMass_ElecPT'     : 10000.      # MeV
                   ,'DiElectronHighMass_PrsMin' :    50. 
                   ,'DiElectronHighMass_EcalMin':   0.1 
                   ,'DiElectronHighMass_HcalMax':  0.05 
                   ,'DiElectronVHighMass_ElecPT'    : 20000.      # MeV
                   ,'DiElectronVHighMass_MinMass'   : 40000.      # MeV

                   # Di-Electron Low mass
                   ,'DiElectronLowMass_MinMass'   :     0.     # MeV
                   ,'DiElectronLowMass_VtxCHI2'   :    25.
                   ,'DiElectronLowMass_PT'        :  -999.     # MeV
                   ,'DiElectronLowMass_ElecPT'    :  1000.     # MeV
                   ,'DiElectronLowMass_ElecPIDe'  :     1.5
                   ,'DiElectronLowMass_ElecTkChi2':    10.
                   
                   # Di-Electron Low PID
                   ,'DiElectronLowPID_MinMass'   :  2000.     # MeV
                   ,'DiElectronLowPID_VtxCHI2'   :    25.
                   ,'DiElectronLowPID_PT'        :  -999.     # MeV
                   ,'DiElectronLowPID_ElecPT'    :  1000.     # MeV
                   ,'DiElectronLowPID_ElecPIDe'  :    -2.
                   ,'DiElectronLowPID_ElecTkChi2':    10.
                   
                   # Jpsi -> ee 
                   ,'DiElectronJpsi_MinMass'     : 2500.      # MeV
                   ,'DiElectronJpsi_MaxMass'     : 3300.      # MeV
                   ,'DiElectronJpsi_VtxCHI2'     :   25.
                   ,'DiElectronJpsi_PT'          : -999.      # MeV
                   ,'DiElectronJpsi_ElecPT'      : 1000.      # MeV
                   ,'DiElectronJpsi_ElecPIDe'    :    1.5      
                   ,'DiElectronJpsi_ElecTkChi2'  :   10. 
                   
                   # Psi(2S) -> ee
                   ,'DiElectronPsi_MinMass'    : 3300.      # MeV
                   ,'DiElectronPsi_MaxMass'    : 3900.      # MeV
                   ,'DiElectronPsi_VtxCHI2'    :   25.
                   ,'DiElectronPsi_PT'         : -999.      # MeV
                   ,'DiElectronPsi_ElecPT'     : 1000.      # MeV
                   ,'DiElectronPsi_ElecPIDe'   :    1.5
                   ,'DiElectronPsi_ElecTkChi2' :   10.

                   # B, upsilon -> ee
                   ,'DiElectronB_MinMass'      : 4800.      # MeV
                   ,'DiElectronB_MaxMass'      : 1.0e+10    # MeV
                   ,'DiElectronB_VtxCHI2'      :   25.
                   ,'DiElectronB_PT'           : -999.      # MeV
                   ,'DiElectronB_ElecPT'       : 1000.      # MeV
                   ,'DiElectronB_ElecPIDe'     :    1.5
                   ,'DiElectronB_ElecTkChi2'   :   10.                                          
                     }
                    })


        from Hlt2Lines.Hlt2InclusiveElectronLines   import Hlt2InclusiveElectronLinesConf

        d.update({ Hlt2InclusiveElectronLinesConf : 
                { 'Prescale'             : { 
                                             'Hlt2SingleTFElectron'        :  1.0
                                            ,'Hlt2SingleTFLowPtElectron'   :  0.1
                                            ,'Hlt2SingleTFHighPtElectron'  :  1.0
                                            ,'Hlt2SingleTFVHighPtElectron' :  1.0
                                            ,'Hlt2TFElectronPlusTrack'     :  1.0
                                            ,'Hlt2TFElectronPlusTrackNoIP' :  0.1

                                             }

                  
                  ,'SingleTFElectron_L0Req'   :  "L0_CHANNEL('Electron')"
                  ,'SingleTFElectron_Hlt1Req' :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"   
                  ,'SingleTFElectron_PT'      :  3000.    # MeV
                  ,'SingleTFElectron_IP'      :     0.2   # mm
                  ,'SingleTFElectron_PIDe'    :     4.  
                  ,'SingleTFElectron_IPCHI2'  :    -1.
                  ,'SingleTFElectron_TrCHI2'  :     5.

                  
                  ,'SingleTFLowPtElectron_L0Req'   :  "L0_CHANNEL('Electron')"
                  ,'SingleTFLowPtElectron_Hlt1Req' :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"   
                  ,'SingleTFLowPtElectron_PT'      :  4800.    # MeV
                  ,'SingleTFLowPtElectron_PIDe'    :     4.  
                  ,'SingleTFLowPtElectron_TrCHI2'  :     5.
                  
                  
                  ,'SingleTFHighPtElectron_PT':      10000.    # MeV
                  ,'SingleTFHighPtElectron_PrsMin':     50.
                  ,'SingleTFHighPtElectron_EcalMin':     0.1    
                  ,'SingleTFHighPtElectron_HcalMax':     0.05
                  ,'SingleTFHighPtElectron_TkChi2' :    20.
                  ,'SingleTFVHighPtElectron_PT':     20000.    # MeV

                  
                  ,'TFElectronPlusTrack_L0Req'        :  "L0_CHANNEL('Electron')"
                  ,'TFElectronPlusTrack_Hlt1Req'      :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"                     
                  ,'TFElectronPlusTrack_ElecPT'       : 1500.   # MeV
                  ,'TFElectronPlusTrack_ElecIP'       :    0.5  # mm
                  ,'TFElectronPlusTrack_ElecIPCHI2'   :   -1.
                  ,'TFElectronPlusTrack_ElecPIDe'     :    4.
                  ,'TFElectronPlusTrack_ElecTrCHI2'   :    5.
                  ,'TFElectronPlusTrack_TrackPT'      : 1500.   # MeV
                  ,'TFElectronPlusTrack_TrackIP'      :    0.5  # mm
                  ,'TFElectronPlusTrack_TrackIPCHI2'  :   -1.
                  ,'TFElectronPlusTrack_TrackTrCHI2'  :    5.
                  ,'TFElectronPlusTrack_VtxCHI2'      :   25.

                  ,'TFElectronPlusTrackNoIP_L0Req'        :  "L0_CHANNEL('Electron')"
                  ,'TFElectronPlusTrackNoIP_Hlt1Req'      :  "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"   
                  ,'TFElectronPlusTrackNoIP_ElecPT'       : 1500.   # MeV
                  ,'TFElectronPlusTrackNoIP_ElecPIDe'     :    4.
                  ,'TFElectronPlusTrackNoIP_ElecTrCHI2'   :    5.
                  ,'TFElectronPlusTrackNoIP_TrackPT'      : 1500.   # MeV
                  ,'TFElectronPlusTrackNoIP_TrackTrCHI2'  :    5.
                  ,'TFElectronPlusTrackNoIP_VtxCHI2'      :   25.
                                    
                  }
                             
            })
        

        return d
