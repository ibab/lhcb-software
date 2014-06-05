################################################################################
##                          S T R I P P I N G  2 1                            ##
##                                                                            ##
##  Configuration for B&Q WG                                                  ##
##  Contact person: Lucio.Anderlini@cern.ch                                   ##
################################################################################

from GaudiKernel.SystemOfUnits import *


#########################################################
### StrippingFullDSTDiMuonDiMuonNoPVLine
### StrippingFullDSTDiMuonDiMuonHighMassLine
### StrippingFullDSTDiMuonDiMuonHighMassSameSignLine
### StrippingFullDSTDiMuonJpsi2MuMuTOSLine
### StrippingFullDSTDiMuonPsi2MuMuTOSLine
### StrippingFullDSTDiMuonJpsi2MuMuDetachedLine
### StrippingFullDSTDiMuonPsi2MuMuDetachedLine
### -----------------------------------------------------
###
### Defined in:                 StrippingDiMuonNew.py
### Proponent:                  Jibo.He@cern.ch
### Motivation:                 Several selections of dimuon written to FullDST
### Documentation:              
###   http://indico.cern.ch/contributionDisplay.py?contribId=2&confId=100755
###   https://twiki.cern.ch/twiki/bin/view/LHCbPhysics/StrippingBandQ
#########################################################
FullDSTDiMuon = {
    'BUILDERTYPE'       :       'DiMuonConf',
    'CONFIG'    : {
        'MicroDST'                                 :  False   ,
                
        # DiMuon line
        'DiMuon_Prescale'                          :     1.   ,
        'DiMuon_Postscale'                         :     1.   ,
        'DiMuon_checkPV'                           : False    ,  
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  3000.   ,  # MeV
        'DiMuon_MaxMass'                           :  1.0e+8  ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                : -1000.   ,  # MeV, no cut now 

        # DiMuonTIS line
        'DiMuonTIS_Prescale'                       :     1.   ,
        'DiMuonTIS_Postscale'                      :     1.   ,
        'DiMuonTIS_checkPV'                        : False    ,  
        'DiMuonTIS_TisTosSpecs'                    : { "L0(Muon|DiMuon|Electron|Photon|Hadron).*Decision%TIS":0,
                                                       "Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision%TIS" : 0,
                                                       "Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision%TIS" : 0 },

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.05  ,
        'DiMuonSameSign_Postscale'                 :     1.   ,
        'DiMuonSameSign_checkPV'                   : False    ,

        # DiMuonPrescaled line
        'DiMuonPrescaled_Prescale'                 :     0.1  ,
        'DiMuonPrescaled_Postscale'                :     1.   ,
        'DiMuonPrescaled_checkPV'                  : False    ,
        
        'DiMuonPrescaled_MuonPT'                   :   650.   ,  # MeV
        'DiMuonPrescaled_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonPrescaled_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonPrescaled_MinMass'                  :  3000.   ,  # MeV
        'DiMuonPrescaled_MaxMass'                  :  1.0e+8  ,  # MeV
        'DiMuonPrescaled_VCHI2PDOF'                :    20.   , 
        'DiMuonPrescaled_PT'                       : -1000.   ,  # MeV, no cut now 
        
        # DiMuonExclusive line
        'DiMuonExclusive_Prescale'                 :     0.   ,
        'DiMuonExclusive_Postscale'                :     1.   ,
        'DiMuonExclusive_checkPV'                  :  True    ,
        
        'DiMuonExclusive_MuonPT'                   :   650.   ,  # MeV
        'DiMuonExclusive_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonExclusive_MuonTRCHI2DOF'            :     5.   ,
        'DiMuonExclusive_MuonPIDCut'               :    ""    ,
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
        'DiMuonHighMass_checkPV'                   : False    ,
        
        'DiMuonHighMass_MuonPT'                    :  1000.   , # MeV
        'DiMuonHighMass_MuonP'                     :  8000.   , # MeV 
        'DiMuonHighMass_MuonTRCHI2DOF'             :     5.   ,
        'DiMuonHighMass_MuonPIDCut'                : " & (MINTREE('mu+'==ABSID,PIDmu)>0)",
        'DiMuonHighMass_MinMass'                   :  8500.   , # MeV
        'DiMuonHighMass_VCHI2PDOF'                 :    20.   ,
        'DiMuonHighMass_PT'                        : -1000.   , # MeV, no cut now

        # DiMuon High Mass Same Sign line
        'DiMuonHighMassSameSign_Prescale'          :     0.2  ,
        'DiMuonHighMassSameSign_Postscale'         :     1.   ,
        'DiMuonHighMassSameSign_checkPV'           : False    ,

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale'                   :     0.   ,
        'DiMuonLowMass_Postscale'                  :     1.   ,
        'DiMuonLowMass_checkPV'                    : False    , 
        
        'DiMuonLowMass_MuonPT'                     :   650.   ,  # MeV
        'DiMuonLowMass_MuonP'                      : -8000.   ,  # MeV, no cut now
        'DiMuonLowMass_MuonTRCHI2DOF'              :     5.   ,
        'DiMuonLowMass_MuonPIDCut'                 :    ""    , 
        'DiMuonLowMass_MinMass'                    :   500.   ,  # MeV
        'DiMuonLowMass_VCHI2PDOF'                  :    20.   ,       
        'DiMuonLowMass_PT'                         : -1000.   ,  # MeV, no cut now

        # Jpsi2MuMu line
        'Jpsi2MuMu_Prescale'                       :     1.   ,
        'Jpsi2MuMu_Postscale'                      :     1.   ,
        'Jpsi2MuMu_checkPV'                        : False    ,
        
        'Jpsi2MuMu_MuonPT'                         :   650.   ,  # MeV
        'Jpsi2MuMu_MuonP'                          : 10000.   ,  # MeV
        'Jpsi2MuMu_MuonPIDmu'                      :     0.   , 
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  3010.   ,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3170.   ,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  3000.   ,  # MeV
        'Jpsi2MuMu_TisTosSpecs'                    : { "L0.*Mu.*Decision%TOS":0,
                                                       "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                       "Hlt2DiMuonJPsiHighPTDecision%TOS" : 0 },

        # Psi2MuMu line
        'Psi2MuMu_Prescale'                        :     1.   ,
        'Psi2MuMu_Postscale'                       :     1.   ,
        'Psi2MuMu_checkPV'                         : False    ,

        'Psi2MuMu_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMu_MuonPT'                          :  1000.   ,  # MeV
        'Psi2MuMu_MuonP'                           : 10000.   ,  # MeV
        'Psi2MuMu_MuonPIDmu'                       :     0.   , 
        'Psi2MuMu_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMu_MassWindow'                      :   100.   ,  # MeV
        'Psi2MuMu_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMu_PT'                              :  3000.   ,   # MeV
        'Psi2MuMu_TisTosSpecs'                     : { "L0.*Mu.*Decision%TOS":0,
                                                       "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                       "Hlt2DiMuonPsi2SHighPTDecision%TOS" : 0 },
        
        # DiMuonDetached line
        'DiMuonDetached_Prescale'                 :     0.   ,
        'DiMuonDetached_Postscale'                :     1.   ,
        
        'DiMuonDetached_MuonPT'                   :   500.   ,  # MeV
        'DiMuonDetached_MuonP'                    : -8000.   ,  # MeV, no cut now
        'DiMuonDetached_MuonPIDmu'                :    -5.   ,
        'DiMuonDetached_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonDetached_MinMass'                  :  2950.   ,
        'DiMuonDetached_VCHI2PDOF'                :    20.   ,
        'DiMuonDetached_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonDetached_LTCuts'                   :   " & ((BPVDLS>3)|(BPVDLS<-3))"    , 
        
        # Jpsi2MuMuDetached line
        'Jpsi2MuMuDetached_Prescale'                       :     1.   ,
        'Jpsi2MuMuDetached_Postscale'                      :     1.   ,
        
        'Jpsi2MuMuDetached_MuonPT'                         :   550.   ,  # MeV
        'Jpsi2MuMuDetached_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMuDetached_MuonPIDmu'                      :     0.   , 
        'Jpsi2MuMuDetached_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMuDetached_MinMass'                        :  2996.916,  # MeV
        'Jpsi2MuMuDetached_MaxMass'                        :  3196.916,  # MeV
        'Jpsi2MuMuDetached_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMuDetached_PT'                             : -1000.   ,  # MeV
        'Jpsi2MuMuDetached_LTCuts'                         :    " & ((BPVDLS>3)|(BPVDLS<-3)) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)",

        # Psi2MuMuDetachedDetached line
        'Psi2MuMuDetached_Prescale'                        :     1.   ,
        'Psi2MuMuDetached_Postscale'                       :     1.   ,

        'Psi2MuMuDetached_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMuDetached_MuonPT'                          :   550.   ,  # MeV
        'Psi2MuMuDetached_MuonP'                           : -8000.   ,  # MeV, no cut now
        'Psi2MuMuDetached_MuonPIDmu'                       :     0.   , 
        'Psi2MuMuDetached_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMuDetached_MassWindow'                      :   100.   ,  # MeV
        'Psi2MuMuDetached_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMuDetached_PT'                              : -1000.   ,   # MeV, no cut now
        'Psi2MuMuDetached_LTCuts'                          :  " & ((BPVDLS>3)|(BPVDLS<-3)) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)"
        },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'BandQ' ]
    }


#########################################################
### StrippingMicroDSTDiMuonDiMuonIncLine
### StrippingMicroDSTDiMuonDiMuonTISLine          **NEW**
### StrippingMicroDSTDiMuonDiMuonSameSignLine
### StrippingMicroDSTDiMuonDiMuonPrescaledLine
### StrippingMicroDSTDiMuonJpsi2MuMuLine
### StrippingMicroDSTDiMuonPsi2MuMuLine
### -----------------------------------------------------
### Defined in:                 StrippingDiMuonNew.py
### Proponent:                  Jibo.He@cern.ch
### Motivation:                 Several selections of dimuon written to MicroDST
### Documentation:              
###   http://indico.cern.ch/contributionDisplay.py?contribId=2&confId=100755
###   https://twiki.cern.ch/twiki/bin/view/LHCbPhysics/StrippingBandQ
#########################################################
MicroDSTDiMuon = {
    'BUILDERTYPE' : 'DiMuonConf',
    'CONFIG' : {
        'MicroDST'                                 :   True   ,
    
        # DiMuon line
        'DiMuon_Prescale'                          :     1.   ,
        'DiMuon_Postscale'                         :     1.   ,
        'DiMuon_checkPV'                           : False    ,  
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  3000.   ,  # MeV
        'DiMuon_MaxMass'                           :  1.0e+8  ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                : -1000.   ,  # MeV

        # DiMuonTIS line
        'DiMuonTIS_Prescale'                       :     1.   ,
        'DiMuonTIS_Postscale'                      :     1.   ,
        'DiMuonTIS_checkPV'                        : False    ,  
        'DiMuonTIS_TisTosSpecs'                    : { "L0(Muon|DiMuon|Electron|Photon|Hadron).*Decision%TIS":0,
                                                       "Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision%TIS" : 0,
                                                       "Hlt2(?!Forward)(?!DebugEvent)(?!Express)(?!Lumi)(?!Transparent)(?!PassThrough).*Decision%TIS" : 0 },

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.05 ,
        'DiMuonSameSign_Postscale'                 :     1.   ,
        'DiMuonSameSign_checkPV'                   :  False   ,

        # DiMuonPrescaled line
        'DiMuonPrescaled_Prescale'                 :     0.1  ,
        'DiMuonPrescaled_Postscale'                :     1.   ,
        'DiMuonPrescaled_checkPV'                  : False    ,
        
        'DiMuonPrescaled_MuonPT'                   :   650.   ,  # MeV
        'DiMuonPrescaled_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonPrescaled_MuonTRCHI2DOF'            :     5.   ,
        'DiMuonPrescaled_MinMass'                  :  3000.   ,  # MeV
        'DiMuonPrescaled_MaxMass'                  :  1.0e+8  ,  # MeV
        'DiMuonPrescaled_VCHI2PDOF'                :    20.   , 
        'DiMuonPrescaled_PT'                       : -1000.   ,  # MeV, no cut now 

        # DiMuonExclusive line
        'DiMuonExclusive_Prescale'                 :     1.   ,
        'DiMuonExclusive_Postscale'                :     1.   ,
        'DiMuonExclusive_checkPV'                  :  True    ,
        
        'DiMuonExclusive_MuonPT'                   :   650.   ,  # MeV
        'DiMuonExclusive_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonExclusive_MuonTRCHI2DOF'            :     5.   ,
        'DiMuonExclusive_MuonPIDCut'               :    ""    ,
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
        'DiMuonHighMass_MuonPIDCut'                : " & (MINTREE('mu+'==ABSID,PIDmu)>0)",
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
        'DiMuonLowMass_MuonPIDCut'                 : ""       , 
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
        'Jpsi2MuMu_MuonPIDmu'                      :    -5.   ,  
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  2976.916,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3216.916,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  3000.0  ,  # MeV
        'Jpsi2MuMu_TisTosSpecs'                    : { "L0.*Mu.*Decision%TOS":0,
                                                       "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                       "Hlt2DiMuonJPsiHighPTDecision%TOS" : 0 },

        # Psi2MuMu line
        'Psi2MuMu_Prescale'                        :     1.   ,
        'Psi2MuMu_Postscale'                       :     1.   ,
        'Psi2MuMu_checkPV'                         :  True    ,

        'Psi2MuMu_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMu_MuonPT'                          :   650.   ,  # MeV
        'Psi2MuMu_MuonP'                           : -8000.   ,  # MeV
        'Psi2MuMu_MuonPIDmu'                       :    -5.   , 
        'Psi2MuMu_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMu_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMu_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMu_PT'                              :  3000.   ,  # MeV
        'Psi2MuMu_TisTosSpecs'                     : { "L0.*Mu.*Decision%TOS":0,
                                                       "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                       "Hlt2DiMuonPsi2SHighPTDecision%TOS" : 0 }, 

        # DiMuonDetached line
        'DiMuonDetached_Prescale'                 :     1.   ,
        'DiMuonDetached_Postscale'                :     1.   ,
        
        'DiMuonDetached_MuonPT'                   :   500.   ,  # MeV
        'DiMuonDetached_MuonP'                    : -8000.   ,  # MeV, no cut now
        'DiMuonDetached_MuonPIDmu'                :    -5.   ,
        'DiMuonDetached_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonDetached_MinMass'                  :  2950.   ,
        'DiMuonDetached_VCHI2PDOF'                :    20.   ,
        'DiMuonDetached_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonDetached_LTCuts'                   :    " & ((BPVDLS>5) | (BPVDLS<-3))"   ,  # mm,
        
        # Jpsi2MuMuDetached line
        'Jpsi2MuMuDetached_Prescale'                       :     1.   ,
        'Jpsi2MuMuDetached_Postscale'                      :     1.   ,
        
        'Jpsi2MuMuDetached_MuonPT'                         :   500.   ,  # MeV
        'Jpsi2MuMuDetached_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMuDetached_MuonPIDmu'                      :    -5.   , 
        'Jpsi2MuMuDetached_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMuDetached_MinMass'                        :  2976.916,  # MeV
        'Jpsi2MuMuDetached_MaxMass'                        :  3216.916,  # MeV
        'Jpsi2MuMuDetached_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMuDetached_PT'                             : -1000.   ,  # MeV
        'Jpsi2MuMuDetached_LTCuts'                         :   " & ((BPVDLS>3 | (BPVDLS<-3))"  ,

        # Psi2MuMuDetachedDetached line
        'Psi2MuMuDetached_Prescale'                        :     1.   ,
        'Psi2MuMuDetached_Postscale'                       :     1.   ,

        'Psi2MuMuDetached_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMuDetached_MuonPT'                          :   500.   ,  # MeV
        'Psi2MuMuDetached_MuonP'                           : -8000.   ,  # MeV, no cut now
        'Psi2MuMuDetached_MuonPIDmu'                       :    -5.   , 
        'Psi2MuMuDetached_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMuDetached_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMuDetached_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMuDetached_PT'                              : -1000.   ,   # MeV, no cut now
        'Psi2MuMuDetached_LTCuts'                          :   " & ((BPVDLS>3) | (BPVDLS<-3))"    
        }, 
    'STREAMS' : [ 'Leptonic' ] ,
    'WGs'    : [ 'BandQ' ]
    }





#########################################################
### StrippingCcbar2PhiPhiLine
### StrippingCcbar2PhiPhiDetachedLine
### StrippingCcbar2PhiKKDetachedLine
### StrippingCcbar2PhiBs2JpsiPhiLine
### StrippingCcbar2PhiB2JpsiKLine
### StrippingCcbar2PhiPiPiDetachedLine
### StrippingCcbar2PhiBs2TriPhiLine
### -----------------------------------------------------
### Defined in:                 StrippingCcbar2PhiPhi.py
### Proponent:                  sergey.barsuk@cern.ch, Jibo.He@cern.ch
### Motivation:                 
###   Module for selecting Ccbar->PhiPhi, including two lines:
###   1. Prompt line, with tight PT, PID cuts, requiring Hlt Tis, since there 
###      is no lifetime unbiased phi trigger yet.
###   2. Detached line, with loose PT, PID cuts, but with IPS cuts on Kaons. 
### Documentation:              n/a
#########################################################
Ccbar2Phi = {
    'BUILDERTYPE' : 'Ccbar2PhiPhiConf',
    'CONFIG' : {
        'TRCHI2DOF'        :     5.  ,
        'KaonPIDK'         :     5.  ,
        'KaonPT'           :   650.  , # MeV
        'PhiVtxChi2'       :    16.  ,
        'PhiMassW'         :    12.  , 
        'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
        'CombMinMass'      :  2750.  , # MeV, before Vtx fit
        'MaxMass'          :  4000.  , # MeV, after Vtx fit
        'MinMass'          :  2800.  , # MeV, after Vtx fit
        'Phi_TisTosSpecs'  : { "Hlt1Global%TIS" : 0 },
        'PionCuts'         :  "(PT>0.7*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>36.) & (PIDK<10)" ,
        'KaonCuts'         :  "(PT>0.5*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>25.) & (PIDK>5)",
        'LooseKaonCuts'    :  "(PT>0.5*GeV) & (TRCHI2DOF<5) & (MIPCHI2DV(PRIMARY)>9.)"
        },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
    }


#########################################################
### StrippingCcbar2PpbarLine
### -----------------------------------------------------
### Defined in:                 StrippingCcbar2PpbarNew.py
### Proponent:                  sergey.barsuk@cern.ch, Jibo.He@cern.ch
### Motivation:                 Module for Charmonium->p pbar: Normal line
### Documentation:              n/a
#########################################################
Ccbar2Ppbar = {
    'BUILDERTYPE'	: 'Ccbar2PpbarConf',
    'CONFIG'	: { 'LinePrescale'     :    1.   ,
                    'LinePostscale'    :    1.   ,
                    
                    'SpdMult'          :   300.  , # dimensionless, Spd Multiplicy cut 
                    'ProtonPT'         :  1950.  , # MeV
                    'ProtonP'          :    10.  , # GeV
                    'ProtonTRCHI2DOF'  :     4.  ,
                    'ProtonPIDppi'     :    20.  , # CombDLL(p-pi)
                    'ProtonPIDpK'      :    15.  , # CombDLL(p-K)
                    'ProtonIPCHI2Cut'  :    ""   ,
                    'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
                    'CombMinMass'      :  2750.  , # MeV, before Vtx fit
                    'MaxMass'          :  4000.  , # MeV, after Vtx fit
                    'MinMass'          :  2800.  , # MeV, after Vtx fit
                    'VtxCHI2'          :     9.  , # dimensionless
                    'CCCut'            :  " & (PT>6*GeV)"    
                   },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
}

#########################################################
### StrippingCcbar2PpbarExclusiveLine
### -----------------------------------------------------
### Defined in:                 StrippingCcbar2PpbarNew.py
### Proponent:                  sergey.barsuk@cern.ch, Jibo.He@cern.ch
### Motivation:                 Module for Charmonium->p pbar: Exclusive
### Documentation:              n/a
#########################################################
Ccbar2PpbarExclusive = {
    'BUILDERTYPE'       : 'Ccbar2PpbarConf',
    'CONFIG'    : {
        'LinePrescale'     :     1.  ,
        'LinePostscale'    :     1.  ,
        
        'SpdMult'          :    20.  , # dimensionless, Spd Multiplicy cut 
        'ProtonPT'         :   550.  , # MeV
        'ProtonP'          :    -2.  , # MeV
        'ProtonTRCHI2DOF'  :     5.  ,
        'ProtonPIDppi'     :    20.  , # CombDLL(p-pi)
        'ProtonPIDpK'      :    15.  , # CombDLL(p-K)
        'ProtonIPCHI2Cut'  :    ""   ,
        'CombMaxMass'      :  1.0e+6 , # MeV, before Vtx fit
        'CombMinMass'      :     0.  , # MeV, before Vtx fit
        'MaxMass'          :  1.0e+6 , # MeV, after Vtx fit
        'MinMass'          :     0.  , # MeV, after Vtx fit
        'VtxCHI2'          :     9.  , # dimensionless
        'CCCut'            :  ""     
        },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
    }


#########################################################
### StrippingCcbar2PpbarDetachedLine
### -----------------------------------------------------
### Defined in:                 StrippingCcbar2PpbarNew.py
### Proponent:                  sergey.barsuk@cern.ch, Jibo.He@cern.ch
### Motivation:                 Module for Charmonium->p pbar: Detached line
### Documentation:              n/a
#########################################################
Ccbar2PpbarDetached = {
    'BUILDERTYPE'       : 'Ccbar2PpbarConf',
    'CONFIG'    : {
      'LinePrescale'     :     1.  ,
      'LinePostscale'    :     1.  ,
      
      'SpdMult'          :   600.  , # dimensionless, Spd Multiplicy cut 
      'ProtonPT'         :  1000.  , # MeV
      'ProtonP'          :    -2.  , # MeV
      'ProtonTRCHI2DOF'  :     5.  ,
      'ProtonPIDppi'     :    15.  , # CombDLL(p-pi)
      'ProtonPIDpK'      :    10.  , # CombDLL(p-K)
      'ProtonIPCHI2Cut'  : " & (BPVIPCHI2()>9)",
      'CombMaxMass'      :  1.0e+6 , # MeV, before Vtx fit
      'CombMinMass'      :  2650.  , # MeV, before Vtx fit
      'MaxMass'          :  1.0e+6 , # MeV, after Vtx fit
      'MinMass'          :  2700.  , # MeV, after Vtx fit
      'VtxCHI2'          :     9.  , # dimensionless
      'CCCut'            :  " & (BPVDLS>5)"     
      },
    'STREAMS' : [ 'Charm' ] ,
    'WGs'    : [ 'BandQ' ]
    }


#########################################################
### StrippingBetac2PhiPBetac2PhiPLine
### -----------------------------------------------------
### Defined in:                 StrippingBetac2PhiP.py
### Proponent:                  Yury.Shcheglov@cern.ch
### Motivation:                 Pentaquark searches
### Documentation:              
###   https://twiki.cern.ch/twiki/bin/view/LHCbPhysics/BetaC2PhiProton
#########################################################
Betac2PhiP = {
    'BUILDERTYPE'  : 'StrippingBetac2PhiPConf',
    'CONFIG'       : {
                    'Daug_TRCHI2DOF_MAX'        : 10.
                  , 'Daug_P_MIN'                : 10000.0 * MeV
                  , 'Daug_PT_MIN'               : 400.0 * MeV
                  , 'Daug_MIPDV'                : 0.05 * mm
                  , 'Proton_PIDpi_MIN'          : 25.0
                  , 'Proton_PIDK_MIN'           : 5.0
                  , 'Phi_WIN'                   : 10.0 * MeV
                  , 'Phi_PT'                    : 0.0 * MeV
                  , 'Betac_AM_MIN'              : 1950.0 * MeV
                  , 'Betac_AM_MAX'              : 2800.0 * MeV
                  , 'Betac_BPVDIRA_MIN'         : 0.999
                  , 'Betac_VCHI2VDOF_MAX'       : 10.0
                  , 'Betac_BPVLTIME_MIN'        : 0.0 * ns
                  , 'Betac_BPVLTIME_MAX'        : 0.006 * ns
                  , 'Hlt2IncPhiDecision'        : "HLT_PASS_RE('Hlt2IncPhiDecision')"
                  , 'Hlt2CharmHadD2HHHDecision':  "HLT_PASS_RE('Hlt2CharmHadD2HHHDecision')"
                  , 'PrescaleBetac2PhiP'        : 1.0
                  , 'PostscaleBetac2PhiP'       : 1.0
    },
    'STREAMS' : [ 'CharmCompleteEvent' ],                                                                
    'WGs'    : ['BandQ']                                                                
    }



#########################################################
### StrippingXibcXibc2LcJpsi
### StrippingXibcX2JpsiProton
### -----------------------------------------------------
### Defined in:                 StrippingXibc.py
### Proponent:                  Lucio.Anderlini@cern.ch
### Motivation:                 Inclusive lines for Xibc searches
### Documentation:              
###   https://indico.cern.ch/event/257864/session/1/contribution/32/material/slides/0.pdf
#########################################################

Xibc = {
    'BUILDERTYPE'  : 'XibcBuilder',
    'CONFIG'       : {
      'Pions4Lambdac_MINIPCHI2'             : 0.0
      , 'Pion4Lambdac_TRPCHI2'              : 0.015
      , 'Pion4Lambdac_ProbNNpi'             : 0.2
      , 'Pion4Lambdac_PT'                   : '250*MeV'
      , 'Protons4Lambdac_TRPCHI2'           : 0.05
      , 'Protons4Lambdac_minP'              : '0*GeV'
      , 'Kaon4Lambdac_TRPCHI2'              : 0.05
      , 'Protons4Lambdac_ProbNNp'           : 0.05
      , 'Protons4Lambdac_PT'                : '450*MeV'
      , 'Kaon4Lambdac_PT'                   : '450*MeV'
      , 'Kaon4Lambdac_ProbNNk'              : 0.02
      , 'Kaon4Lambdac_minP'                 : '0*GeV'
      , 'Muon4Jpsi_PIDmu'                   : 0
      , 'Muon4Jpsi_TRPCHI2'                 : 0.005
      , 'Lambdac_MassWindowLarge'           : '120*MeV'
      , 'Lambdac_MassWindowTight'           : '30*MeV'		
      , 'Lambdac_MinAPT'                    : '1500*MeV' 
      , 'Lambdac_MaxADOCA'                  : '0.5*mm'
      , 'Lambdac_BPVDIRA'                   : 0.98
      , 'Lambdac_minTAU'                    : -0.0001 	
      , 'Lambdac_ENDVERTEXCHI2'             : 5						
      , 'Jpsi_MassWindowLarge'              : '150*MeV'
      , 'Jpsi_MassWindowTight'              : '50*MeV'
      , 'Muon4Jpsi_PT'                      : '650*MeV'
      , 'Jpsi_ENDVERTEXCHI2'                : 10
      , 'Xibc_ENDVERTEXCHI2'                : 7
      , 'Xibc_MassWindow'                   : '1.5*GeV'
      , 'Xibc_MINPVIP'                      : 1e3			#DISABLED FOR INCLUSIVITY
      , 'Xibc2LcJpsiPrescale'               : 1.0
####### #
      , 'Protons4Lambda0_ProbNNp'           : 0.02
      , 'LongProtons4Lambda0_MINIPCHI2'     : 2
      , 'Protons4Lambda0_PT'                : '600*MeV'
      , 'Pions4Lambda0_ProbNNpi'            : 0.2
      , 'Pions4Lambda0_PT'                  : '0*MeV'
      , 'Pions4Lambda0_MINIPCHI2'           : 0.
      , 'Lambda0_MassWindowLarge'           : '180*MeV'
      , 'Lambda0_MassWindowTight'           : '30*MeV'
      , 'Lambda0_APT'                       : '700*MeV'
      , 'Lambda0_ENDVERTEXCHI2'             : 10	
      , 'Lambda0_DownProtonTrackPvalue'     : 0
      , 'Lambda0_DownPionTrackPvalue'       : 0
      , 'Lambda0_minFD'                     : '1*mm/GeV'
      , 'Pions4Ximinus_PT'                  : '0*MeV'
      , 'Pions4Ximinus_ProbNNpi'            : 0.2
      , 'Pions4Ximinus_TRPCHI2'             : 0
      , 'Pions4Ximinus_MINIPCHI2'           : 0
      , 'Ximinus_MassWindowLarge'           : '120*MeV'
      , 'Ximinus_MassWindowTight'           : '40*MeV'
      , 'Ximinus_APT'                       : '800*MeV'
      , 'Ximinus_FlightDistance'            : '0.1*mm/GeV'
      , 'Ximinus_ENDVERTEXCHI2'             : 20
      , 'Xic0_MassWindowLarge'              : '600*MeV'
      , 'Xic0_MassWindowTight'              : '100*MeV'
      , 'Pions4Xic0_PT'                     : '200*MeV'
      , 'Pions4Xic0_ProbNNpi'               : 0.2
      , 'Pions4Xic0_TRPCHI2'                : 0
      , 'Xic0_APT'                          : '0*MeV'
      , 'Xic0_ENDVERTEXCHI2'                : 20
      , 'Xibc0_MassWindow'                  : '1.5*GeV'
      , 'Xibc0_ENDVERTEXCHI2'               : 20
####### 
      , 'HighMassBaryon_MassLowEdge'        : '4.5*GeV'
      , 'HighMassBaryon_MinAPT'             : '1*GeV'
      , 'ProtonsForHighMassBaryon_TRPCHI2'  : 0.1
      , 'ProtonsForHighMassBaryon_PT'       : '1.5*GeV'
      , 'ProtonsForHighMassBaryon_P'        : '5*GeV'
      , 'ProtonsForHighMassBaryon_ProbNNp'  : 0.1
      , 'JpsiForHighMassBaryon_PT'          : '1*GeV'
      , 'JpsiForHighMassBaryon_MassWin'     : '40*MeV'
      , 'JpsiForHighMassBaryon_MuonPIDmu'   : 0
      , 'JpsiProtonForHighMassBaryonCosth'  : 0.992
#######
      , "JpsiKp_CtrlLine_Prescale"          : 1.0
      , "JpsiKp_MinTAU"                     : '0.10*ps'
      , "JpsiKp_MassMin"                    : 5200 #MeV/c2
      , "JpsiKp_MassLbThreshold"            : 5750 #MeV/c2
      , "JpsiKp_MassMax"                    : 8000 #MeV/c2
      , "JpsiKp_MaxVertexChi2"              : 10
      , "JpsiKp_Jpsi_MinPT"                 : 700 #MeV/c
      , "JpsiKp_Jpsi_MassWin"               : 40  #MeV/cc
      , "JpsiKp_mu_MaxTrackGhostProb"       : 0.4
      , "JpsiKp_mu_MinPIDmu"                : 0
      , "JpsiKp_p_MinPt"                    : 300 #MeV
      , "JpsiKp_p_MinProbNNp"               : 0.05
      , "JpsiKp_p_MaxTrackGhostProb"        : 0.4
      , "JpsiKp_p_MinTrackPvalue"           : 0.1
      , "JpsiKp_p_MinP"                     : 2000 #MeV/c
      , "JpsiKp_K_MinPT"                    : 300 #MeV
      , "JpsiKp_K_MinProbNNk"               : 0.02
      , "JpsiKp_K_MaxTrackGhostProb"        : 0.4
      , "JpsiKp_K_MinTrackPvalue"           : 0.1
      , "JpsiKp_K_MinP"                     : 2000 #MeV/c
########
			, 'GlobalGhostProb_Max'               : 0.4
      , 'LongTrackGEC'                      : 150 
    },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : ['BandQ']
    }


#########################################################
### StrippingBc2JpsiMuLine
### -----------------------------------------------------
### Defined in:                 StrippingBc2JpsiMuXNew.py
### Proponent:                  Lucio.Anderlini@cern.ch, Jibo.He@cern.ch
### Motivation:                 Used for Bc lifetime measurement w/ SL channel
### Documentation:              n/a              
### Note on RAW event:          Muon Raw event was used in the analysis
###                             to study the effect of shared hits between
###                             same-sign muons in the final state
#########################################################
Bc2JpsiMu = {
    'BUILDERTYPE' : 'Bc2JpsiMuXConf',
    'CONFIG' : {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
   
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional
    'MuonPT'              : 1400.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :  150.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              :  -10.   ,  # MeV, not applied
   
    'MuonBcTRCHI2DOF'     :    5.   ,  # adimentional    
    'MuonBcPT'            : 2500.   ,  # MeV
    'MuonBcP'             :   -5.   ,  # MeV, not applied now
    'BcUpperMass'         : 6400.   ,  # MeV, Upper limit for partial rec.
    'BcLowerMass'         : 3200.   ,  # MeV, Lower limit for partial rec.
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                : 6000.      # MeV, May incrase up to 5000 MeV if needed      
    },
    'STREAMS' : [ 'Dimuon' ] ,
    'WGs'    : [ 'BandQ' ]
  }


#########################################################
### StrippingBc2JpsiHLine
### -----------------------------------------------------
### Defined in:                 StrippingBc2JpsiHNew.py
### Proponent:                  Jibo.He@cern.ch
### Motivation:                 Bc -> J/psi pi   to MicroDST
### Documentation:              n/a              
#########################################################
Bc2JpsiH = {
    'BUILDERTYPE'       : 'Bc2JpsiHConf',
    'CONFIG'    : {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
   
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional
    'MuonPT'              : 1200.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :   80.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              : 2000.   ,  # MeV
   
    'PionTRCHI2DOF'       :    5.   ,  # adimentional    
    'PionPT'              : 2000.   ,  # MeV
    'PionP'               :   -5.   ,  # MeV, not applied now
    'BcMassWindow'        :  400.   ,  # MeV, mass window
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                : 6000.   ,  

    'LifetimeCut'         :   ""    
    },
    'STREAMS' : [ 'Leptonic' ],
    'WGs'    : [ 'BandQ' ]
    }

#########################################################
### StrippingBc2JpsiHDetachedLine
### -----------------------------------------------------
### Defined in:                 StrippingBc2JpsiHNew.py
### Proponent:                  Jibo.He@cern.ch
### Motivation:                 Bc -> J/psi pi   to FullDST
### Documentation:              n/a              
#########################################################
Bc2JpsiHDetached = {
    'BUILDERTYPE'   : 'Bc2JpsiHConf',
    'CONFIG'    : {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
    
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional 
    'MuonPT'              :  500.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :  100.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              :  -10.   ,  # MeV, not applied 
    
    'PionTRCHI2DOF'       :    5.   ,  # adimentional     
    'PionPT'              : 1000.   ,  # MeV
    'PionP'               :   -5.   ,  # MeV, not applied now
    'BcMassWindow'        :  400.   ,  # MeV, mass window
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                :    0.   ,  # MeV, May incrase up to 5000 MeV if needed       

    'LifetimeCut'         : " & (BPVLTIME()>0.2*ps) & (INTREE( (ABSID=='pi+') & (BPVIPCHI2()>9)))"
    },
    'STREAMS'   : [ 'Dimuon' ],
    'WGs'    : [ 'BandQ' ]
}


#########################################################
### StrippingHeavyBaryonXib2JpsiXi
### StrippingHeavyBaryonXibzero2JpsiXistar
### StrippingHeavyBaryonOmegab2JpsiOmega
### -----------------------------------------------------
### Defined in:                 StrippingHeavyBaryons.py
### Proponent:                  raphael.marki@epfl.ch, Yasmine.Amhis@cern.ch
### Motivation:                 Stripping line for HeavyBaryons studies
### Documentation:              n/a              
#########################################################
HeavyBaryon = {
    'BUILDERTYPE'  : 'HeavyBaryonsConf',
    'CONFIG'       : {    'TRCHI2DOF'             :       4.,
                          'PionPIDK'              :       5.,
                          'JpsiMassWindow'        :      80.,
                          'KaonPIDK'              :      -5.,
                          'DLSForLongLived'       :       5.,
                          'XiMassWindow'          :      30.,
                          'OmegaMassWindow'       :      30.,
                          "XibminusMassWindow"    :     300.,
                          "XibzeroMassWindow"     :     500.,
                          "OmegabminusMassWindow" :     500.
                          },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : ['BandQ']
    }



###############################################################################
### StrippingChiCJPsiGammaConvChicLine
### StrippingChiCJPsiGammaConvChibLine
### -----------------------------------------------------
###   DISABLES: StrippingChiCJPsiGammaConvChicSymLine (obsolete)
###   DISABLES: StrippingChiCJPsiGammaConvChibSymLine (obsolete)
### -----------------------------------------------------
### Defined in:                 StrippingChiCJPsiGammaConv.py
### Proponent:                  
### Motivation:                 Stripping line for HeavyBaryons studies
### Documentation:              n/a              
###############################################################################
ChiCJPsiGammaConv = {                                  
    'BUILDERTYPE'  : 'StrippingChiCJPsiGammaConvConf', 
    'CONFIG'       : {                                 
    'trackChi2'               :    3.0
    , 'MuPTMin'               :    400 #MeV #Can be tightened to 600 MeV
    , 'MuPMin'                :   8000 #MeV
    , 'JPsiMassMin'           :    3.0 # GeV
    , 'JPsiMassMax'           :    3.2 # GeV
    , 'JPsiPTMin'             :     2 # GeV
    , 'JPsi_PIDmu'            :    0. 
    , 'JPsiVertexChi2'        :   25.0
    , 'UpsilonMassMin'        :    9.0  #GeV
    , 'UpsilonMassMax'        :    12.9 #GeV
    , 'UpsilonVertexChi2'     :   25.0
    , 'UpsilonPTMin'          :    0.9 # GeV
    , 'Upsilon_PIDmu'         :    0. 
    , 'eDLLe'                 :   0.0
    , 'GammaEEMass'           :   100.0 #MeV
    , 'GammaEEPt'             :   580.0 #MeV
    , 'mMinChiCRaw'           :   2.9 #GeV
    , 'mMaxChiCRaw'           :   4.8 #GeV
    , 'mMinChiCFit'           :   3.0 #GeV
    , 'mMaxChiCFit'           :   4.7 ##GeV
    , 'mMinChiBRaw'           :   9.4 #GeV
    , 'mMaxChiBRaw'           :   13.2 #GeV
    , 'mMinChiBFit'           :   9.5 #GeV
    , 'mMaxChiBFit'           :   13.0  #GeV
    , 'PrescaleChi'          :   1.0
    , 'PostscaleChi'         :   1.0
    },                                                 
    'STREAMS' : { 'Dimuon': 
                    [
                      'StrippingChiCJPsiGammaConvChibLine',
                      'StrippingChiCJPsiGammaConvChicLine' 
                    ]
                },                          
    'WGs'    : [ 'BandQ' ]                             
    }                                                  



#########################################################
### StrippingDiMuonAndGammaForCharmAssociative
### StrippingDoubleDiMuonForCharmAssociative
### StrippingChiAndDiMuonForCharmAssociative
### StrippingDiChiForCharmAssociative
### StrippingDiMuonAndWForCharmAssociative
### StrippingChiAndWForCharmAssociative
### -----------------------------------------------------
### Defined in:                 StrippingCharmAssociative.py
### Proponent:                  Ivan.Belyaev@cern.ch
### Motivation:                 stripping of associative ``onium'' production
### Documentation:              
###     - dimuon + dimuon 
###     - dimuon + high-pt gamma
### 
###    Parasitic:
### 
###     - dimuon + ( dimuon + gamma ) [ mimic   Chi_(b,c) + dimuon  ] 
###     -       2x ( dimuon + gamma ) [ mimic 2xChi_(b,c)           ] 
### 
###    Accociative W+ production:
###     - dimuon             & W+  
###     - ( dimuon + gamma ) & W+      [ mimic Chi_(b,c)] 
###
#########################################################
CharmAssociative = {
    'BUILDERTYPE'  :'StrippingCharmAssociativeConf',
    'CONFIG'  : {
    'PhotonCuts'      : ' PT > 4.0 * GeV  '                                 , 
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) ' ,
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : ' ( PT > 450 * MeV ) & ( CL > 0.05 ) ' , 
    #
    ## W+- selection
    #
    'WCuts'           : " ( 'mu+'== ABSID ) & ( PT > 15 * GeV )" ,
    #
    ## Global Event cuts 
    #
    'CheckPV'         : True , 
    #
    ## Technicalities:
    #
    'Preambulo'       : [
    #
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                    ,
    #
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" , 
    #
    ## dimuons:
    "psi             = ADAMASS ( 'J/psi(1S)'  ) < 125 * MeV"         ,
    "psi_prime       = ADAMASS (   'psi(2S)'  ) < 125 * MeV"         ,
    "mu2_tight       = ( chi2vx < 10    ) & ( MINTREE ( 'mu+' == ABSID , PT ) > 900 * MeV ) " ,
    "dimu_tight      = ( PT > 3.0 * GeV ) & mu2_tight " ,
    "psi_tight       = ( ADMASS ( 'J/psi(1S)' ) < 100 * MeV ) & dimu_tight " ,
    "psi_prime_tight = ( ADMASS (   'psi(2S)' ) < 100 * MeV ) & dimu_tight " ,
    "dimuon_heavy    = ( M > 4.9 * GeV ) & dimu_tight "                      ,
    "dimuon_tight    = psi_tight | psi_prime_tight | dimuon_heavy " ,
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    #
    ## pescales 
    'DiMuonAndGammaPrescale' : 1.0 ,
    'DoubleDiMuonPrescale'   : 1.0 ,
    'ChiAndDiMuonPrescale'   : 1.0 ,
    'DiChiPrescale'          : 1.0 ,
    'DiMuonAndWPrescale'     : 1.0 , 
    'ChiAndWPrescale'        : 1.0
    },
    'STREAMS' : [ 'Leptonic' ] ,
    'WGs'     : [ 'BandQ'    ]
    }


#########################################################
### StrippingBuToKX3872_BuToKX3872LooseLine
### StrippingBuToKX3872_BuToKPsi2SLooseLine
### StrippingBuToKX3872_BuToKX3872Line
### StrippingBuToKX3872_BuToKPsi2SLine
### -----------------------------------------------------
### Defined in:                 StrippingBuToKX3872.py
### Proponent:                  "Jeremy Dickens" (no more available, RENEW!)
### Motivation:                 B+ -> K+ X3872, B+ -> K+ Psi(2S) 
###                              and a looser B+ -> K+ JPsi pi+ pi+
### Documentation:              n/a
#########################################################
BuToKX3872 = {
    'BUILDERTYPE'       : 'StrippingBu2KX3872Conf',
    'CONFIG' : {
     'Prescale_BuToKX3872'    : 1.0,
    'Postscale_BuToKX3872'   : 1.0,

    'Prescale_BuToKX3872Loose'    : 1.0,
    'Postscale_BuToKX3872Loose'   : 1.0,

    'Prescale_BuToKPsi2S'    : 1.0,
    'Postscale_BuToKPsi2S'   : 1.0,

    'Prescale_BuToKPsi2SLoose'    : 1.0,
    'Postscale_BuToKPsi2SLoose'   : 1.0,

    # B cuts
    'Bu_Comb_MassWindow'         : 450.0,
    'Bu_Comb_MassWindowLoose'    : 500.0,
    'Bu_MassWindow'              : 400.0,
    'Bu_MassWindowLoose'         : 400.0,
    'Bu_VertexCHI2'              :   4.0,
    'Bu_VertexCHI2Loose'         :   5.0,
    'Bu_IPCHI2'                  :  15.0,
    'Bu_IPCHI2Loose'             :  20.0,
    'Bu_FlightCHI2'              :  40.0,
    'Bu_FlightCHI2Loose'         :  30.0,
    'Bu_DIRA'                    :   0.9995,
    'Bu_DIRALoose'               :   0.9995,

    # X3872 / Psi(2S) cuts
    'X3872_Comb_MassWindow'      : 180.0,
    'X3872_Comb_MassWindowLoose' : 220.0,
    'X3872_MassWindow'           : 150.0,
    'X3872_MassWindowLoose'      : 190.0,
    'X3872_VertexCHI2'           :   8.0,
    'X3872_VertexCHI2Loose'      :  10.0,

    # Track cuts
    'Track_CHI2nDOF'       : 4.0,

    # Kaon cuts
    'Kaon_MinIPCHI2'       : 6.0,
    'Kaon_MinIPCHI2Loose'  : 5.0,
    'Kaon_PT'              : 200.0,
    'Kaon_PTLoose'         : 150.0,

    # Pion cuts
    'Pion_MinIPCHI2'       : 6.0,
    'Pion_MinIPCHI2Loose'  : 5.0,

    # JPsi cuts
    'JPsi_MassWindow'       :  70.0,
    'JPsi_MassWindowLoose'  :  70.0,
    'JPsi_VertexCHI2'       :  10.0,
    'JPsi_VertexCHI2Loose'  :  10.0,

    # Muon cuts
    'Muon_MinIPCHI2'        :   3.0,
    'Muon_MinIPCHI2Loose'   :   2.0,
    'Muon_PT'               : 500.0,
    'Muon_IsMuon'           : True
     },
    'STREAMS' : [ 'Leptonic' ],
    'WGs'    : [ 'BandQ' ]
    }


####################################################
### StrippingXic2HHH
###-------------------------------------------------
### Defined in: StrippingXic2HHH.py
### Proponent:  Yury.Shcheglov@cern.ch
### Motivation: Xic studies and pentaquark searches
### Documentation:
###  (Yury Shcheglov 2014-01-14 wrote:)  
###  1) Measurements ratios of the production  rates
###    R(pT)= N(Xic->p+\phi(KK)) / N( Lc->p+\phi(KK)) -
###  new doubly Cabbibo supressed decay Xic->p+phi to the  
###  known Lc->p+phi decay and  
###  R (pT) = N(Xic->p + Kstar(Kpi)) / N( Lc->p +  Kstar(Kpi)) 
###  - used to cross-check the pT distribution  shape of the first ratio;
###  
###  2) pentaquark search (both, long-lived charmed pentaquark Beta+ and
###  pentaquark Theta+ (recently JLab reported peak at M = 1540 MeV);
###  
###  The preliminary result for the measurement of the  rates ratios
###  shows the some difference  in the pT shape of
###  the ratios    N(Xic->p+\phi(KK)) / N( Lc->p+\phi(KK)) and
###  N(Xic->p + Kstar(Kpi)) / N( Lc->p +  Kstar(Kpi))  at the range  pT = 4-7 GeV. 
###  But experimental errors for the first ratio are large enough 
###  and it will be good to increase statistic for  the Xic->p+\phi(KK).
###  
###  For this goal from the stripping lines were rejected 
###  too tight cuts connected with kaons IPCHI2, which were 
###  found non-effective to suppress the background. 
###  Besides we refused from the tight DIRA cut to avoid 
###  correlation between pT and lifetime particle. 
###  Additionally, to keep  a reasonable retention 
###  factor in the  stripping lines, results of ANN for 
###  the particle identification ( PROBNNp(pi,K)) 
###  were used in the event selections. 
###  Besides, all  pT restrictions were 
###  choosen  more loose for all stripping lines.
###  
###  All efforts above give us a hope to reduce in 1.5-2 times our
###  experimental errors in the ratio N(Xic->p+\phi(KK)) / N( Lc->p+\phi(KK)) and 
###  to increase the sensitivity to the possible pentaquarks contributions due 
###  to more large statistics  for the signal.  
###  The common idea of all our modifications in the streeping lines is 
###  to keep signal efficiencies as high as it is a possible and 
###  simultaneously to keep possible systematic errors of 
###  measurements at the  some reasonable level. 
###
##################################################


Xic2HHH = {
    'WGs'           : ['BandQ'],
    'BUILDERTYPE'   : 'StrippingXic2HHHConf',
    'CONFIG'        : {  'Daug_All_PT_MIN'          : 300.0 * MeV
                        , 'Daug_P_MIN'              : 3000.0 * MeV
                        , 'Daug_TRCHI2DOF_MAX'      : 10.0
                        , 'Daug_1of3_BPVIPCHI2_MIN' : 0.5
                        , 'Proton_PIDp_MIN'         : 10.0 
                        , 'K_IPCHI2_MIN'            : 0.0 
                        , 'Comb_MASS_MIN'           : 1950.0 * MeV 
                        , 'Comb_MASS_MAX'           : 2800.0 * MeV 
                        , 'Comb_ADOCAMAX_MAX'       : 0.3 * mm   
                        , 'Xic_PT_MIN'              : 1500.0 * MeV
                        , 'Xic_VCHI2VDOF_MAX'       : 8.0
                        , 'Xic_BPVVDCHI2_MIN'       : 0.0
                        , 'Xic_BPVDIRA_MIN'         : 0.9
                        , 'Xic_BPVIPCHI2_MAX'       : 10.
                        , 'Xic_BPVLTIME_MAX'        : 0.0025 * ns
                        , 'Xic_BPVLTIME_MIN'        : 0.0002 * ns
                        , 'HltFilter'               : "HLT_PASS('Hlt2CharmHadD2HHHDecision')"
                        , 'PrescaleXic2PKPi'        : 1.0
                        , 'PostscaleXic2PKPi'       : 1.0
                        , 'PrescaleXic2PKK'         : 1.0
                        , 'PostscaleXic2PKK'        : 1.0
                        , 'PrescaleXic2PV0'         : 1.0
                        , 'PostscaleXic2PV0'        : 1.0
                        , 'PrescaleXic2KLam'        : 0.0 
                        , 'PostscaleXic2KLam'       : 0.0 
                        , 'ExtraInfoTools' : [
                            { "Type" : "ConeVariables", 
                              "ConeAngle" : 1.5, 
                              "ConeNumber" : 1, 
                              "Variables" : ['angle', 'mult', 'ptasy']
                            }, 
                            { "Type" : "ConeVariables", 
                              "ConeAngle" : 15., 
                              "ConeNumber" : 2, 
                              "Variables" : ['angle', 'mult', 'ptasy']
                            }, 
                            { "Type" : "VertexIsolation"}
                        ] 
                       },
    'STREAMS' : [
        'Charm' 
        ],
}   



###############################################################
### StrippingXibStarToXibZeroXibToJpsiXiPi                
### StrippingXibStarToXibZeroXibToJpsiXiPi_WS             
### StrippingXibStarToXibZeroXibToJpsiLambdaKPi           
### StrippingXibStarToXibZeroXibToJpsiLambdaKPi_WS        
### StrippingXibStarToXibZeroXibToJpsiPKKPi               
### StrippingXibStarToXibZeroXibToJpsiPKKPi_WS            
### StrippingXibStarToXibZeroXibToJpsiPK                  
### StrippingXibStarToXibZeroXibToJpsiPK_WS               
### StrippingXibStarToXibZeroXibToD0PK                    
### StrippingXibStarToXibZeroXibToD0PK_WS                 
### StrippingXibStarToXibZeroXibToDpPKPi                  
### StrippingXibStarToXibZeroXibToDpPKPi_WS               
### StrippingXibStarToXibZeroXibToLcK                     
### StrippingXibStarToXibZeroXibToLcK_WS                  
### StrippingXibStarToXibZeroXibToXicpPi                  
### StrippingXibStarToXibZeroXibToXicpPi_WS               
### StrippingXibStarToXibZeroXibToXic0PiPi                
### StrippingXibStarToXibZeroXibToXic0PiPi_WS             
### StrippingXibStarToXibZeroXibToPK                      
### StrippingXibStarToXibZeroXibToPK_WS                   
### StrippingXibStarToXibZeroXibToXicpMu                  
### StrippingXibStarToXibZeroXibToXicpMu_WS               
### StrippingXibStarToXibZeroXibToXic0PiMu                
### StrippingXibStarToXibZeroXibToXic0PiMu_WS             
### StrippingXibStarToXibZeroXibToLcKPiPi                 
### StrippingXibStarToXibZeroXibToLcKPiPi_WS              
### StrippingXibStarToXibZeroControlXib_XibToJpsiXiPi     
### StrippingXibStarToXibZeroControlXib_XibToJpsiLambdaKPi
### StrippingXibStarToXibZeroControlXib_XibToJpsiPKKPi    
### StrippingXibStarToXibZeroControlXib_XibToJpsiPK       
### StrippingXibStarToXibZeroControlXib_XibToD0PK         
### StrippingXibStarToXibZeroControlXib_XibToDpPKPi       
### StrippingXibStarToXibZeroControlXib_XibToLcK          
### StrippingXibStarToXibZeroControlXib_XibToXicpPi       
### StrippingXibStarToXibZeroControlXib_XibToXic0PiPi     
### StrippingXibStarToXibZeroControlXib_XibToPK           
### StrippingXibStarToXibZeroControlXib_XibToXicpMu       
### StrippingXibStarToXibZeroControlXib_XibToXic0PiMu     
### StrippingXibStarToXibZeroControlXib_XibToLcKPiPi      
###------------------------------------------------------------
### Defined in: StrippingXibStarToXibZero.py
### Proponent:  matthew.john.charles@cern.ch
### Motivation: FCNC searches, Xib* searches, selection tuning
### Documentation:  
###   https://indico.cern.ch/conferenceDisplay.py?confId=269979
###############################################################

XibStarToXibZero = {
    'WGs'           : ['BandQ'],
    'BUILDERTYPE'   : 'XibStarBuilder',
    'CONFIG'        : {  
                        'LongTrackGEC'              : 300
                        , 'prescaleSignalDefault'   : 1.0
                        , 'prescaleSignalJpsi'      : 1.0
                        , 'prescaleControlHadronic' : 0.1
                        , 'prescaleControlMuonic'   : 1.0
                        , 'XibStar_PT_Min'          : 2500.0*MeV
                      },
    'STREAMS' : [
        'Bhadron' 
        ],
}


#########################################################
### StrippingD02KpiForPromptCharm               
### StrippingDstarForPromptCharm                
### StrippingDForPromptCharm                    
### StrippingDsForPromptCharm                   
### StrippingLambdaCForPromptCharm              
### StrippingLambdaC2pKKForPromptCharm          
### StrippingSigmaCForPromptCharm               
### StrippingLambdaCstarForPromptCharm          
### StrippingDiCharmForPromptCharm              
### StrippingDiMuonAndCharmForPromptCharm       
### StrippingDoubleDiMuonForPromptCharm         
### StrippingChiAndCharmForPromptCharm          
### StrippingChiAndWForPromptCharm              
### StrippingXicc+ForPromptCharm                
### StrippingXicc++ForPromptCharm               
### StrippingDsLamCForPromptCharm               
### StrippingCharmAndWForPromptCharm            
### StrippingDiMuonAndWForPromptCharm           
### -----------------------------------------------------
### Defined in:                 StrippingPromptCharm.py
### Proponent:                  Ivan.Belyaev@cern.ch
### Motivation:                 coherent stripping of stable charm hadrons
### Documentation: https://indico.cern.ch/conferenceDisplay.py?confId=270130
#########################################################

PromptCharm = {
    'WGs'           : ['BandQ'],
    'BUILDERTYPE'   : 'StrippingPromptCharmConf',
    'CONFIG'        : { 'D0Prescale'       : 0.05 ,
                        'D+Prescale'       : 0.05 ,
                        'D*Prescale'       : 0.1  ,
                        'DsPrescale'       : 0.5  ,
                      },
    'STREAMS' : [
        'Charm' 
        ],
}


#########################################################
### StrippingCC2DD
### -----------------------------------------------------
### Defined in:                 StrippingCC2DD.py
### Proponent:                  Andrea.Bizzeti@fi.infn.it
### Motivation:                 Low PT 2x charm, psi(3770)
### Documentation:              https://indico.cern.ch/conferenceDisplay.py?confId=269979
#########################################################
CC2DD = {        
    'WGs'               : ['BandQ'],
    'BUILDERTYPE'       : 'CC2DDConf',
    'CONFIG'   :  {
####### D0 / D~0 -> K pi cuts
                   'D0MassWin'     : "60*MeV",
                   'D0PT'          :  "0*MeV",
                   'D0VtxChi2Ndof' :  10.,
                   'D0Bpvdira'     : -10.,
                   'D0Bpvdls'      :   4.,
                   'D0daughterBpvIpChi2'    : 6.,
                   'D0daughterPT'           : "600*MeV",
                   'D0daughterP'            : "5*GeV",
                   'D0daughterTrkChi2'      : 3.,
                   'D0daughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'D0daughterKaonProbNNk'  : 0.1,
                   'D0daughterPionProbNNpi' : 0.1,
######## Dplus/Dminus -> Kpipi cuts, used also for D_s+/D_s- ->KKpi
                   'DpmMassWin'     : "60*MeV",
                   'DpmPT'          :  "0*MeV",
                   'DpmVtxChi2Ndof' :  10.,
                   'DpmBpvdira'     : -10.,
                   'DpmBpvdls'      :   4.,
                   'DpmdaughterBpvIpChi2'    : 6.,
                   'DpmdaughterPT'           : "500*MeV",  ## lower than for D0
                   'DpmdaughterP'            : "5*GeV",
                   'DpmdaughterTrkChi2'      : 3.,
                   'DpmdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'DpmdaughterKaonProbNNk'  : 0.1,
                   'DpmdaughterPionProbNNpi' : 0.1,
######## psi(3779) -> D D  cuts
#                   'CCMassCut'     : "(AM<5000*MeV)",
# no mass constraint
                   'CCMassCut'      : "(AM>0)",
                   'CCVtxChi2Ndof'  : 10,
                   'CCMaxD0ChildPT' : "1500*MeV",
                   'CCMaxD0TreePT'  : "1200*MeV",
                   'CCMaxD0MinTreeIpChi2'   : "0.",    ## unused for the moment
                 },
    'STREAMS' : [
        'CharmCompleteEvent' 
        ],
}   
