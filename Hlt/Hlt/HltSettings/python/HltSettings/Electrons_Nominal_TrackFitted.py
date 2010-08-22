class Electrons_Nominal_TrackFitted:
    """
    All Hlt2 electrons lines

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author Jibo.He@cern.ch
    @date 2010-06-23
    
    """

    __all__=('ActiveHlt2Lines', 'Thresholds')


    def ActiveHlt2Lines(self):
        """
        Returns a list of active lines
        """
        
        lines = [
            'Hlt2UnbiasedTFDiElectron',        
            'Hlt2UnbiasedTFDiElectronLowMass',
            'Hlt2UnbiasedTFDiElectronLowPID',
            'Hlt2UnbiasedTFJpsi2ee',          
            'Hlt2UnbiasedTFPsi2ee',           
            'Hlt2UnbiasedTFB2ee',
            # Biased
            'Hlt2BiasedTFDiElectron',        
            'Hlt2BiasedTFDiElectronLowMass',
            'Hlt2BiasedTFDiElectronLowPID',
            'Hlt2BiasedTFJpsi2ee',          
            'Hlt2BiasedTFPsi2ee',           
            'Hlt2BiasedTFB2ee',
            # single
            'Hlt2SingleTFElectron',     
            'Hlt2TFElectronPlusTrack',
            # DY etc
            'Hlt2SingleHighPTElectron',        
            'Hlt2DYeh1',                       
            'Hlt2DYeh2',         
            # Exclusive
            'Hlt2TFBs2JpsieePhiUnbiasedPT', 
            'Hlt2TFBs2JpsieePhiSignal',
            # 
            # Bu2eek already included in Exclusive_Nominal.py
            'Hlt2Bu2eeK', 
            'Hlt2Bu2eeKHighMass', 
            'Hlt2Bu2eeKJpsi', 
            'Hlt2Bu2eeKSignal', 
            #
            'Hlt2B2KstareeFittedLargeBMassWindow',
            'Hlt2B2KstareeFittedLargeBMassWindowSignal'
            ]
        
        return lines


    def Thresholds(self):
        
        d = { }
        
        # Inclusive Di-Electron lines
        from Hlt2Lines.Hlt2InclusiveDiElectronLines import Hlt2InclusiveDiElectronLinesConf       
        
        d.update( { Hlt2InclusiveDiElectronLinesConf :
                    {'Prescale'            : {      
                                                    # Unbiased and Track Fitted (TF)
                                                     'Hlt2UnbiasedTFDiElectron'         :  1.0
                                                    ,'Hlt2UnbiasedTFDiElectronLowMass'  :  1.0
                                                    ,'Hlt2UnbiasedTFDiElectronLowPID'   :  0.01
                                                    ,'Hlt2UnbiasedTFJpsi2ee'            :  1.0
                                                    ,'Hlt2UnbiasedTFPsi2ee'             :  1.0
                                                    ,'Hlt2UnbiasedTFB2ee'               :  1.0
                                                    # Biased and Track Fitted (TF)
                                                    ,'Hlt2BiasedTFDiElectron'         :  1.0
                                                    ,'Hlt2BiasedTFDiElectronLowMass'  :  0.01
                                                    ,'Hlt2BiasedTFDiElectronLowPID'   :  0.01
                                                    ,'Hlt2BiasedTFJpsi2ee'            :  1.0
                                                    ,'Hlt2BiasedTFPsi2ee'             :  1.0
                                                    ,'Hlt2BiasedTFB2ee'               :  1.0 
                                                    }
                       
                     
                   #-----------------
                   # Track fitted 
                   #-----------------
                   # Di-Electron 
                   ,'UnbiasedTFDiElectron_MinMass'    : 2000.      # MeV
                   ,'UnbiasedTFDiElectron_VtxCHI2'    :   25.
                   ,'UnbiasedTFDiElectron_PT'         : -999.      # MeV
                   ,'UnbiasedTFDiElectron_ElecPT'     : 1000.      # MeV
                   ,'UnbiasedTFDiElectron_ElecPIDe'   :    1.5 

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
                   # Biased, Track fitted 
                   #------------------
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
                     
                     }
                    })


        from Hlt2Lines.Hlt2InclusiveElectronLines   import Hlt2InclusiveElectronLinesConf

        d.update({ Hlt2InclusiveElectronLinesConf : 
                { 'Prescale'             : { 
                                             'Hlt2SingleTFElectron'       :  1.0
                                            ,'Hlt2TFElectronPlusTrack'    :  1.0
                                             }
                                                               
                  ,'SingleTFElectron_PT'      :  3000.    # MeV
                  ,'SingleTFElectron_IP'      :     0.2   # mm
                  ,'SingleTFElectron_PIDe'    :     4.  
                  ,'SingleTFElectron_IPCHI2'  :    -1.    

                  ,'TFElectronPlusTrack_ElecPT'       : 1500.   # MeV
                  ,'TFElectronPlusTrack_ElecIP'       :    0.2  # mm
                  ,'TFElectronPlusTrack_ElecIPCHI2'   :   -1.
                  ,'TFElectronPlusTrack_ElecPIDe'     :    4.
                  ,'TFElectronPlusTrack_TrackPT'      : 1500.   # MeV
                  ,'TFElectronPlusTrack_TrackIP'      :    0.2  # mm
                  ,'TFElectronPlusTrack_TrackIPCHI2'  :   -1.
                  ,'TFElectronPlusTrack_VtxCHI2'      :   25.
                  
                  }
                             
            })


        
        from Hlt2Lines.Hlt2B2KstareeLines           import Hlt2B2KstareeLinesConf
        d.update({ Hlt2B2KstareeLinesConf : {
                'Prescale'   :  {  'Hlt2B2KstareeFittedLargeBMassWindow'                 : 1.
                                  ,'Hlt2B2KstareeFittedLargeBMassWindowSignal'           : 1.  
                                  }
                
                #----------------------------
                # cuts for track unfitted
                #----------------------------
                ,  'RobustElectronPT'               :  200.           # MeV
                ,  'RobustElectronPIDe'             :   -2.
                ,  'RobusteeVertexCHI2'             :   64.
 		,  'RobustPionPT'                   :  200.           # MeV
                ,  'RobustPionIPCHI2'               :    1.           # adimentional
                ,  'RobustKaonPT'                   :  250.           # MeV
		,  'RobustKaonIPCHI2'               :    1.           # adimentional
		,  'RobustKstarMassWindow'          :  200.           # MeV
		,  'RobustKstarVertexCHI2'          :   64.           # adimentional
		,  'RobustKstarIPCHI2'             :     1.           # adimentional
                #----------------------------
                # cuts for track fitted 
                #----------------------------
                ,  'ElectronPT'               :  300.           # MeV
                ,  'ElectronPIDe'             :   -1.
                ,  'ElectronIPCHI2'           :    1.           # adimentional
                ,  'ElectronTrackCHI2pNDOF'   :   25.           #
		,  'eeVertexCHI2'             :   25.           # adimentional
                ,  'eeMinMass'                : -999.           # MeV
                ,  'eeMaxMass'                : 6000.           # MeV
		,  'PionPT'                   :  300.           # MeV
                ,  'PionTrackCHI2pNDOF'       :   25.
                ,  'PionIPCHI2'               :    2.25         # adimentional
                ,  'KaonPT'                   :  300.           # MeV
                ,  'KaonTrackCHI2pNDOF'       :   25.
		,  'KaonIPCHI2'               :    2.25         # adimentional
		,  'KstarMassWindow'          :  150.           # MeV
		,  'KstarVertexCHI2'          :   25.           # adimentional
		,  'KstarIPCHI2'              :    1.           # adimentional			
		,  'BLargeMassWindow'         : 1000.           # MeV
		,  'BMassWindow'              :  400.           # MeV	
		,  'BVertexCHI2'              :   25.           # adimentional	
		,  'BIPCHI2'                  :  100.           # 	
		,  'BDIRA'                    :    0.99955      # adimentional
		,  'BIP'                      :    1.           # mm
                #----------------------------
                # cuts for signal while different
                #----------------------------
                ,  'Signal_eeMaxMass'         : 2000.           # MeV
            
            }
            
            })

        return d
