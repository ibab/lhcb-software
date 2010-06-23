class Electrons_All:
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
        
        lines = []
        lines.extend(self.DiElectronHlt2Lines())
        lines.extend(self.ElectronHlt2Lines()) 
        lines.extend(self.ExclusiveHlt2Lines())
        
        return lines




    def DiElectronHlt2Lines(self):
        """
        Returns di electron lines 
        """
        
        lines = [
            # Unbiased
            'Hlt2UnbiasedDiElectron',        
            'Hlt2UnbiasedDiElectronLowMass',
            'Hlt2UnbiasedDiElectronLowPID',
            'Hlt2UnbiasedJpsi2ee',          
            'Hlt2UnbiasedPsi2ee',           
            'Hlt2UnbiasedB2ee',
            'Hlt2UnbiasedTFDiElectron',        
            'Hlt2UnbiasedTFDiElectronLowMass',
            'Hlt2UnbiasedTFDiElectronLowPID',
            'Hlt2UnbiasedTFJpsi2ee',          
            'Hlt2UnbiasedTFPsi2ee',           
            'Hlt2UnbiasedTFB2ee',
            # Biased
            'Hlt2BiasedDiElectron',
            'Hlt2BiasedDiElectronLowMass',
            'Hlt2BiasedDiElectronLowPID',
            'Hlt2BiasedJpsi2ee',          
            'Hlt2BiasedPsi2ee',           
            'Hlt2BiasedB2ee',   
            'Hlt2BiasedTFDiElectron',        
            'Hlt2BiasedTFDiElectronLowMass',
            'Hlt2BiasedTFDiElectronLowPID',
            'Hlt2BiasedTFJpsi2ee',          
            'Hlt2BiasedTFPsi2ee',           
            'Hlt2BiasedTFB2ee',
            # DY and Z
            'Hlt2Zee',
            'Hlt2DYee1',          
            'Hlt2DYee2',          
            'Hlt2DYee3',          
            'Hlt2ZeeUnfitted',     
            'Hlt2DYee1Unfitted', 
            'Hlt2DYee2Unfitted', 
            'Hlt2DYee3Unfitted'            
            ]

        return lines

        
    def ElectronHlt2Lines(self):       
        lines = [
            'Hlt2SingleElectron',
            'Hlt2ElectronPlusTrack',    
            'Hlt2SingleTFElectron',     
            'Hlt2TFElectronPlusTrack',
            # DY etc
            'Hlt2SingleHighPTElectron',        
            'Hlt2DYeh1',                       
            'Hlt2DYeh2',                       
            'Hlt2SingleHighPTElectronUnfitted',
            'Hlt2DYeh1Unfitted',               
            'Hlt2DYeh2Unfitted',            
            ]

        return lines


    def ExclusiveHlt2Lines(self):
        lines = [
            # Bs2Jpsi(ee)Phi
            'Hlt2Bs2JpsieePhiUnbiasedPT',
            'Hlt2Bs2JpsieePhiSignal',
            'Hlt2TFBs2JpsieePhiUnbiasedPT', 
            'Hlt2TFBs2JpsieePhiSignal',
            # Bu2eek
            'Hlt2Bu2eeK',
            'Hlt2Bu2eeKSignal',  
            'Hlt2Bu2eeKJpsi',
            'Hlt2Bu2eeKHighMass',
            # Bu2eeKstar
            'Hlt2B2KstareeRobustAndFitted',
            'Hlt2B2KstareeRobustAndFittedSignal', 
            'Hlt2B2KstareeFitted',
            'Hlt2B2KstareeFittedSignal',
            'Hlt2B2KstareeFittedLargeBMassWindow',
            'Hlt2B2KstareeFittedLargeBMassWindowSignal',
            'Hlt2B2KstareeRobustAndFittedLargeBMassWindow',
            'Hlt2B2KstareeRobustAndFittedLargeBMassWindowSignal'
            ]

        return lines


    def Thresholds(self):
        
        d = { }

        # Inclusive Di-Electron lines
        from Hlt2Lines.Hlt2InclusiveDiElectronLines import Hlt2InclusiveDiElectronLinesConf       
        
        d.update( { Hlt2InclusiveDiElectronLinesConf :
                    {'Prescale'            : {      'Hlt2Zee'                :  1.0
                                                    ,'Hlt2DYee1'             :  1.0
                                                    ,'Hlt2DYee2'             :  1.0
                                                    ,'Hlt2DYee3'             :  1.0
                                                    ,'Hlt2ZeeUnfitted'       :  1.0
                                                    ,'Hlt2DYee1Unfitted'    :  0.01
                                                    ,'Hlt2DYee2Unfitted'    :  0.01
                                                    ,'Hlt2DYee3Unfitted'    :  0.01
                                                    # Unbiased Di-Electron
                                                    ,'Hlt2UnbiasedDiElectron'         :  0.01
                                                    ,'Hlt2UnbiasedDiElectronLowMass'  :  0.01
                                                    ,'Hlt2UnbiasedDiElectronLowPID'   :  0.001
                                                    ,'Hlt2UnbiasedJpsi2ee'            :  0.01
                                                    ,'Hlt2UnbiasedPsi2ee'             :  0.01
                                                    ,'Hlt2UnbiasedB2ee'               :  0.01
                                                    # Unbiased and Track Fitted (TF)
                                                    ,'Hlt2UnbiasedTFDiElectron'         :  1.0
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
                { 'Prescale'             : { 'Hlt2SingleHighPTElectron'            : 1.0
                                            ,'Hlt2DYeh1'                           : 1.0
                                            ,'Hlt2DYeh2'                           : 1.0
                                            ,'Hlt2SingleHighPTElectronUnfitted'    : 0.01
                                            ,'Hlt2DYeh1Unfitted'                   : 0.01
                                            ,'Hlt2DYeh2Unfitted'                   : 0.01

                                            ,'Hlt2SingleElectron'         :  0.01
                                            ,'Hlt2ElectronPlusTrack'      :  0.01
                                            ,'Hlt2SingleTFElectron'       :  1.0
                                            ,'Hlt2TFElectronPlusTrack'    :  1.0
                                             }
                                             
                 
                  ,'SingleHighPTElectronPT'        : 20000      # MeV
                  ,'SingleHighPTElectronIP'        :   0.1      # mm
                  ,'SingleHighPTElectronTkChi2'    :    20     
                  
                  ,'DYeh1ePt'                 : 4000      # MeV
                  ,'DYeh1trPt'                : 4000      # MeV                  
                  ,'DYeh1MinMass'             :10000      # MeV                  
                  ,'DYeh2ePt'                 : 8000      # MeV
                  ,'DYeh2trPt'                : 8000      # MeV                  
                  ,'DYeh2MinMass'             :20000      # MeV                  
                  ,'DYehDphiMin'             :     0      # radian                 
                  ,'DYehVertexChi2'          :    20      #

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
                'Prescale'   :  {  'Hlt2B2KstareeRobustAndFitted'                        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedSignal'                  : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindow'        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindowSignal'  : 1.  
                                  ,'Hlt2B2KstareeFitted'                                 : 1.
                                  ,'Hlt2B2KstareeFittedSignal'                           : 1.
                                  ,'Hlt2B2KstareeFittedLargeBMassWindow'                 : 1.
                                  ,'Hlt2B2KstareeFittedLargeBMassWindowSignal'           : 1.                                     

                                  }
                
                ,'Postscale' :  {  'Hlt2B2KstareeRobustAndFitted'                        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedSignal'                  : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindow'        : 1.
                                  ,'Hlt2B2KstareeRobustAndFittedLargeBMassWindowSignal'  : 1.  
                                  ,'Hlt2B2KstareeFitted'                                 : 1.
                                  ,'Hlt2B2KstareeFittedSignal'                           : 1.
                                  ,'Hlt2B2KstareeFittedLargeBMassWindow'                 : 1.
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


        from Hlt2Lines.Hlt2B2JpsiXLines import Hlt2B2JpsiXLinesConf
        d.update({ Hlt2B2JpsiXLinesConf : {
            
                  'Prescale'    : { 
                                    'Hlt2Bs2JpsieePhiUnbiasedPT'   : 0.001
                                   ,'Hlt2Bs2JpsieePhiSignal'       : 0.001
                                   ,'Hlt2TFBs2JpsieePhiUnbiasedPT' : 0.01
                                   ,'Hlt2TFBs2JpsieePhiSignal'     : 1.
                                    }

                  # Bs->Jpsi(ee)Phi
                 , 'Bs2JpsieePhi_ElectronPT'          :  500.     # MeV
                 , 'Bs2JpsieePhi_ElectronMIPCHI2'     :    2.25   # adimensional
                 , 'Bs2JpsieePhi_JpsiVertexCHI2pDOF'  :   25.     # adimensional
                 , 'Bs2JpsieePhi_JpsiMassMin'         : 2500.     # MeV
                 , 'Bs2JpsieePhi_JpsiMassMax'         : 3300.     # MeV
                 , 'Bs2JpsieePhi_KaonMIPCHI2'         :    2.25   # adimensional
                 , 'Bs2JpsieePhi_PhiPT'               :  900.     # MeV
                 , 'Bs2JpsieePhi_PhiVertexCHI2pDOF'   :   25.     # adimensional
                 , 'Bs2JpsieePhi_PhiMassMin'          :  990.     # MeV
                 , 'Bs2JpsieePhi_PhiMassMax'          : 1050.     # MeV
                 , 'Bs2JpsieePhi_BsDIRA'              :    0.99   # adimensional
                 , 'Bs2JpsieePhi_BsVertexCHI2pDOF'    :   25.     # adimensional
                 , 'Bs2JpsieePhi_BsPseudoMassMin'     : 3800.     # MeV
                 , 'Bs2JpsieePhi_BsPseudoMassMax'     : 6800.     # MeV

                  
                  # TrackFitted Bs->Jpsi(ee)Phi
                 , 'TFBs2JpsieePhi_ElectronPT'          :  500.      # MeV
                 , 'TFBs2JpsieePhi_ElectronMIPCHI2'     :    2.25    # adimensional
                 , 'TFBs2JpsieePhi_JpsiVertexCHI2pDOF'  :   25.      # adimensional
                 , 'TFBs2JpsieePhi_JpsiMassMin'         : 2500.      # MeV
                 , 'TFBs2JpsieePhi_JpsiMassMax'         : 3300.      # MeV
                 , 'TFBs2JpsieePhi_KaonMIPCHI2'         :    2.25    # adimensional
                 , 'TFBs2JpsieePhi_PhiPT'               :  900.      # MeV
                 , 'TFBs2JpsieePhi_PhiVertexCHI2pDOF'   :   25.      # adimensional
                 , 'TFBs2JpsieePhi_PhiMassMin'          :  990.      # MeV
                 , 'TFBs2JpsieePhi_PhiMassMax'          : 1050.      # MeV
                 , 'TFBs2JpsieePhi_BsDIRA'              :    0.99    # adimensional
                 , 'TFBs2JpsieePhi_BsVertexCHI2pDOF'    :   25.      # adimensional
                 , 'TFBs2JpsieePhi_BsPseudoMassMin'     : 3800.      # MeV
                 , 'TFBs2JpsieePhi_BsPseudoMassMax'     : 6800.      # MeV           
 

            }            

            })
        

        return d
