class Electrons_Minimal_TrackUnfitted:
    """
    All Hlt2 electrons lines

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author Jibo.He@cern.ch
    @date 2010-06-11
    
    """

    __all__=('ActiveHlt2Lines', 'Thresholds')


    def ActiveHlt2Lines(self):
        """
        Returns a list of active lines
        """
        
        lines = [
            # Unbiased
            'Hlt2UnbiasedDiElectron',        
            'Hlt2UnbiasedDiElectronLowMass',
            'Hlt2UnbiasedDiElectronLowPID',
            'Hlt2UnbiasedJpsi2ee',          
            'Hlt2UnbiasedPsi2ee',           
            'Hlt2UnbiasedB2ee',
            # Biased
            'Hlt2BiasedDiElectron',
            'Hlt2BiasedDiElectronLowMass',
            'Hlt2BiasedDiElectronLowPID',
            'Hlt2BiasedJpsi2ee',          
            'Hlt2BiasedPsi2ee',           
            'Hlt2BiasedB2ee',
            # sigle electron
            'Hlt2SingleElectron',
            'Hlt2ElectronPlusTrack',  
            # exclusive
            'Hlt2B2KstareeRobustAndFittedLargeBMassWindow',
            'Hlt2B2KstareeRobustAndFittedLargeBMassWindowSignal'            
            ]
        
        return lines


    def Thresholds(self):
        
        d = { }

        # Inclusive Di-Electron lines
        from Hlt2Lines.Hlt2InclusiveDiElectronLines import Hlt2InclusiveDiElectronLinesConf       
        
        d.update( { Hlt2InclusiveDiElectronLinesConf :
                    {'Prescale'            : {      
                                                    # Unbiased Di-Electron
                                                    'Hlt2UnbiasedDiElectron'         :  1.0
                                                    ,'Hlt2UnbiasedDiElectronLowMass'  :  0.01
                                                    ,'Hlt2UnbiasedDiElectronLowPID'   :  0.001
                                                    ,'Hlt2UnbiasedJpsi2ee'            :  1.0
                                                    ,'Hlt2UnbiasedPsi2ee'             :  1.0
                                                    ,'Hlt2UnbiasedB2ee'               :  1.0
                                                    # Biased 
                                                    ,'Hlt2BiasedDiElectron'         :  1.0
                                                    ,'Hlt2BiasedDiElectronLowMass'  :  0.01
                                                    ,'Hlt2BiasedDiElectronLowPID'   :  0.001 
                                                    ,'Hlt2BiasedJpsi2ee'            :  1.0
                                                    ,'Hlt2BiasedPsi2ee'             :  1.0
                                                    ,'Hlt2BiasedB2ee'               :  1.0
                                                    }
                   #-----------------
                   # Track un-fitted 
                   #-----------------
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
                   
                   #------------------
                   # Biased
                   #------------------
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
                     
                     }
                    })


        from Hlt2Lines.Hlt2InclusiveElectronLines   import Hlt2InclusiveElectronLinesConf

        d.update({ Hlt2InclusiveElectronLinesConf : 
                { 'Prescale'             : { 'Hlt2SingleElectron'         :  1.0
                                             ,'Hlt2ElectronPlusTrack'      :  1.0
                                             
                                             }
                                             
                  ,'SingleElectron_PT'      :  3000.    # MeV
                  ,'SingleElectron_IP'      :     0.2   # mm
                  ,'SingleElectron_PIDe'    :     4.  
                  ,'SingleElectron_IPCHI2'  :    -1.    

                  ,'ElectronPlusTrack_ElecPT'       : 1500.   # MeV
                  ,'ElectronPlusTrack_ElecIP'       :    0.2  # mm
                  ,'ElectronPlusTrack_ElecPIDe'     :    4. 
                  ,'ElectronPlusTrack_ElecIPCHI2'   :   -1.
                  ,'ElectronPlusTrack_TrackPT'      : 1500.   # MeV
                  ,'ElectronPlusTrack_TrackIP'      :    0.2  # mm
                  ,'ElectronPlusTrack_TrackIPCHI2'  :   -1.
                  ,'ElectronPlusTrack_VtxCHI2'      :   25.                  
                  }
                             
            })


        
        from Hlt2Lines.Hlt2B2KstareeLines           import Hlt2B2KstareeLinesConf
        d.update({ Hlt2B2KstareeLinesConf : {
                'Prescale'   :  {  'Hlt2B2KstareeRobustAndFittedLargeBMassWindow'        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindowSignal'  : 1.  
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
