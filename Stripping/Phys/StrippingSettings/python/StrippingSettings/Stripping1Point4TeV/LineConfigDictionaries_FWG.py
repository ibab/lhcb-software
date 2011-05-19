"""
Line config dictionaries for the FWG

Stripping1Point4TeV definitions.
Dictionaries have the name of the line builder instance.
"""
#######
# FWG #
#######
FullDSTDiMuon = {
    'BUILDERTYPE' : 'DiMuonConf',
    'CONFIG' : {
        'MicroDST'                                 :   True   ,
    
        # DiMuon line
        'DiMuon_Prescale'                          :     1.   ,
        'DiMuon_Postscale'                         :     1.   ,
        'DiMuon_checkPV'                           :  False   ,  
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  2900.   ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                : -1000.   ,  # MeV, no cut now 

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.1  ,
        'DiMuonSameSign_Postscale'                 :     1.   ,
        'DiMuonSameSign_checkPV'                   :  False   ,

        # DiMuonExclusive line
        'DiMuonExclusive_Prescale'                 :     1.   ,
        'DiMuonExclusive_Postscale'                :     1.   ,
        'DiMuonExclusive_checkPV'                  :  True    ,
        
        'DiMuonExclusive_MuonPT'                   :   650.   ,  # MeV
        'DiMuonExclusive_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonExclusive_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonExclusive_MinMass'                  :  2900.   ,
        'DiMuonExclusive_VCHI2PDOF'                :    20.   ,
        'DiMuonExclusive_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonExclusive_DZ'                       :    -1.   ,  # mm, upstream of any PV

        # DiMuonNoPV line
        'DiMuonNoPV_Prescale'                      :     1.   ,
        'DiMuonNoPV_Postscale'                     :     1.   ,
               
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale'                  :     1.   ,
        'DiMuonHighMass_Postscale'                 :     1.   ,
        'DiMuonHighMass_checkPV'                   :  True    ,
        
        'DiMuonHighMass_MuonPT'                    :   650.   , # MeV
        'DiMuonHighMass_MuonP'                     : -8000.   , 
        'DiMuonHighMass_MuonTRCHI2DOF'             :     5.   , 
        'DiMuonHighMass_MinMass'                   :  8000.   , # MeV
        'DiMuonHighMass_VCHI2PDOF'                 :    20.   ,
        'DiMuonHighMass_PT'                        : -1000.   , # MeV, no cut now 

        # DiMuon High Mass Same Sign line
        'DiMuonHighMassSameSign_Prescale'          :     1.   ,
        'DiMuonHighMassSameSign_Postscale'         :     1.   ,
        'DiMuonHighMassSameSign_checkPV'           :  True    ,

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale'                   :     1.   ,
        'DiMuonLowMass_Postscale'                  :     1.   ,
        'DiMuonLowMass_checkPV'                    :  True    , 
        
        'DiMuonLowMass_MuonPT'                     :   650.   ,  # MeV
        'DiMuonLowMass_MuonP'                      : -8000.   ,  # MeV, no cut now
        'DiMuonLowMass_MuonTRCHI2DOF'              :     5.   , 
        'DiMuonLowMass_MinMass'                    :   500.   ,  # MeV
        'DiMuonLowMass_VCHI2PDOF'                  :    20.   ,       
        'DiMuonLowMass_PT'                         : -1000.   ,  # MeV, no cut now

        # Jpsi2MuMu line
        'Jpsi2MuMu_Prescale'                       :     1.   ,
        'Jpsi2MuMu_Postscale'                      :     1.   ,
        'Jpsi2MuMu_checkPV'                        :  True    ,
        
        'Jpsi2MuMu_MuonPT'                         :   650.   ,  # MeV
        'Jpsi2MuMu_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  3010.   ,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3170.   ,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  1500.0  ,  # MeV

        # Psi2MuMu line
        'Psi2MuMu_Prescale'                        :     1.   ,
        'Psi2MuMu_Postscale'                       :     1.   ,
        'Psi2MuMu_checkPV'                         :  True    ,

        'Psi2MuMu_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMu_MuonPT'                          :  1000.   ,  # MeV
        'Psi2MuMu_MuonP'                           :  8000.   ,  # MeV
        'Psi2MuMu_PIDmu'                           :     0.   , 
        'Psi2MuMu_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMu_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMu_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMu_PT'                              :  2000.   ,  # MeV

        # DiMuonDetached line
        'DiMuonDetached_Prescale'                 :     1.   ,
        'DiMuonDetached_Postscale'                :     1.   ,
        
        'DiMuonDetached_MuonPT'                   :   500.   ,  # MeV
        'DiMuonDetached_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonDetached_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonDetached_MinMass'                  :  2950.   ,
        'DiMuonDetached_VCHI2PDOF'                :    20.   ,
        'DiMuonDetached_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonDetached_DLS'                      :     5.   ,  # mm, upstream of any PV
        
        # Jpsi2MuMuDetached line
        'Jpsi2MuMuDetached_Prescale'                       :     1.   ,
        'Jpsi2MuMuDetached_Postscale'                      :     1.   ,
        
        'Jpsi2MuMuDetached_MuonPT'                         :   500.   ,  # MeV
        'Jpsi2MuMuDetached_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMuDetached_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMuDetached_MinMass'                        :  2976.916,  # MeV
        'Jpsi2MuMuDetached_MaxMass'                        :  3216.916,  # MeV
        'Jpsi2MuMuDetached_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMuDetached_PT'                             : -1000.   ,  # MeV
        'Jpsi2MuMuDetached_DLS'                            :     3. 
        }, 
    'STREAMS' : [ 'AllStreams' ] ,
    'WGs'    : [ 'FWG' ]
    }
