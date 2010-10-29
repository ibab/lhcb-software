# Line config for StrippingBd2KstarMuMuTriggered.py

Bd2KstarMuMuConfig = { 
    'BMassLow'           : 5050
    ,  'BMassHighWin'  :  500      
    ,  'BDIRA'               : 0.9999     
    ,  'BFlightCHI2'       : 100      
    ,  'BIPCHI2'            : 25        
    ,  'DaughterVertexChi2' : 12         
    ,  'BVertexCHI2'     : 12     
    ,  'IntDIRA'              : -0.9      
    ,  'IntFlightCHI2'      : 9         
    ,  'TrackChi2'         : 10        
    ,  'TrackIPChi2'      : 4           
    ,  'KstarHighMass'  : 2500.       
    ,  'KstarLowMass'  : 500.        
        }


# Vanya wants us to use the default!!!
D02KKpipiRegularConfig = {'TrackChi2max' : 5. 
                          ,'DDauKPTmin'          : 500.      
                          , 'DDauKPmin'     : 2000.   
                          , 'DDauPiPTmin'         : 400.     
                          , 'DDauPiPmin'        : 2000.    
                          , 'DDauKDelPIDmin'        : 0.     
                          , 'DDauPiDelPIDmin'            : -5.    
                          , 'DCombMassWind'             : 50.    
                          , 'DCombAMAXDOCAmax'            : 0.5    
                          , 'DMothPTmin'            : 2500.  
                          , 'DMothBPVDIRAmin'             : 0.9996   
                          , 'DMothMIPDVmax'          :  0.1
                          , 'DMothMIPCHI2DVmax'  :  25.                              
                          , 'DMothBPVVDmin'           : 1.0
                          , 'DMothBPVDCHI2min'           : 9.0                           
                          , 'DMothBPVVDZmin'              : 0.
                          , 'DMothVFASPFmax'               :  9.       
                          , 'DstarCombMassWind'               : 50. 
                          , 'DstarCombAMAXDOCAmax'                 :  0.5
                          , 'DstarMothVFASPFmax'            :  9.
                          , 'DstarMothPTmin'             : 2500.  
                          , 'DstarMothDMmin'               : 138. 
                          , 'DstarMothDMmax'           : 154.   
                          , 'DstarMothSlpiMIPDVmax'           : 0.500
                          , 'DstarMothSlpiMIPCHI2DVmax'   : 25. 
                          , 'DstarMothSlpiPTmin'       : 250.   
                          , 'LinePrescale'   : 1.
                          , 'LinePostscale'  : 1.}

PromptCharmConfig = { 'D0Prescale' : 0.25 , 'DplusPrescale' : 0.25 }



TrackEffMuonTTConfig = {'JpsiMassWin'                 : 500,
                        'UpsilonMassWin'              : 1500,
                        'ZMassWin'                    : 10000,
                        'JpsiMuonTTPT'                : 0,
                        'UpsilonMuonTTPT'             : 500,
                        'ZMuonTTPT'                   : 500,
                        'JpsiLongPT'                  : 1000,
                        'UpsilonLongPT'               : 1000,
                        'ZLongPT'                     : 10000,
                        'JpsiPT'                      : 500,
                        'UpsilonPT'                   : 1500,
                        'ZPT'                         : 3000,
                        'JpsiLongMuonMinIPCHI2'       : 5,
                        'UpsilonLongMuonMinIPCHI2'    : 5,
                        'ZLongMuonMinIPCHI2'          : 5,
                        'JpsiLongMuonTrackCHI2'       : 5,
                        'UpsilonLongMuonTrackCHI2'    : 5,
                        'ZLongMuonTrackCHI2'          : 5,
                        'VertexChi2'                  : 10,
                        'LongMuonPID'                 : 0,
                        'JpsiPrescale'                : 1,      
                        'UpsilonPrescale'             : 1,
                        'ZPrescale'                   : 1,
                        'Postscale'                   : 1
                        }
