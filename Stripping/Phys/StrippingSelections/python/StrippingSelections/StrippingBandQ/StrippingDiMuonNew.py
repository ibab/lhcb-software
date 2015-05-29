'''
Inclusive DiMuon lines based on the lines by Gaia Lanfranchi, Alessio Sarti,
with inputs from Joel Bressieux, Giulia Manca, Matthew Needham and Patrick Robbe.

Including the following lines:
1. DiMuonLine
   FullDST (keep 10%) and MicroDST 
2. DiMuonSameSignLine
   FullDST (keep  1%) and MicroDST (keep 10%)
3. DiMuonExclusiveLine
   Selection is the same as DiMuonLine at present,
   a cut of "1mm upstream of any PV" applied further  
4. DiMuonNoPVLine
   Using the same selection as DiMuonExclusiveLine at present (except the PV cut)
   Requring no reconstructed PV.   
5. DiMuonHighMassLine                     
6. DiMuonHighMassSameSignLine             
7. DiMuonLowMassLine
   Keep Hlt2UnbiasedDiMuonLowMassDecision triggered events 
8. Jpsi2MuMuLine
9. Psi2MuMuLine
10. DiMuonDetachedLine
11. Jpsi2MuMuDetachedLine


More details can be found here:
http://indico.cern.ch/contributionDisplay.py?contribId=2&confId=100755

--------------------------
To include lines for DiMuon stream
--------------------------
from StrippingSelections.StrippingDiMuonNew import DiMuonConf
from StrippingSelections.StrippingDiMuonNew import config_default as config_FullDSTDiMuon
FullDSTDiMuonConf = DiMuonConf( name = None, config =config_FullDSTDiMuon  )
stream.appendLines( FullDSTDiMuonConf.lines() )

--------------------------
For MicroDST
--------------------------
from StrippingSelections.StrippingDiMuonNew import DiMuonConf
from StrippingSelections.StrippingDiMuonNew import config_microDST as MicroDSTDiMuon
MicroDSTDiMuonConf = DiMuonConf( name = 'MicroDST', config = MicroDSTDiMuon )
stream.appendLines( MicroDSTDiMuonConf.lines() )


'''

__author__=['Jibo He']
__date__ = '30/09/2010'
__version__= '$Revision: 1.0 $'

__all__ = (
    'DiMuonConf'
    , 'default_config'
  )


default_config = {
    'TestFullDSTDiMuon':{
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
        'DiMuonSameSign_Prescale'                  :    1.  ,
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
        'DiMuonLowMass_HLTFILTER'                : "(HLT_PASS('Hlt2DiMuonLowMassDecision'))",

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
        
        'Jpsi2MuMuDetached_MuonPT'                         :   500.   ,  # MeV
        'Jpsi2MuMuDetached_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMuDetached_MuonPIDmu'                      :     0.   , 
        'Jpsi2MuMuDetached_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMuDetached_MinMass'                        :  2996.916,  # MeV
        'Jpsi2MuMuDetached_MaxMass'                        :  3196.916,  # MeV
        'Jpsi2MuMuDetached_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMuDetached_PT'                             : -1000.   ,  # MeV
        'Jpsi2MuMuDetached_LTCuts'                         :    " & ((BPVDLS>3)|(BPVDLS<-3))",

        # Psi2MuMuDetachedDetached line
        'Psi2MuMuDetached_Prescale'                        :     1.   ,
        'Psi2MuMuDetached_Postscale'                       :     1.   ,

        'Psi2MuMuDetached_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMuDetached_MuonPT'                          :   500.   ,  # MeV
        'Psi2MuMuDetached_MuonP'                           : -8000.   ,  # MeV, no cut now
        'Psi2MuMuDetached_MuonPIDmu'                       :     0.   , 
        'Psi2MuMuDetached_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMuDetached_MassWindow'                      :   100.   ,  # MeV
        'Psi2MuMuDetached_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMuDetached_PT'                              : -1000.   ,   # MeV, no cut now
        'Psi2MuMuDetached_LTCuts'                          :  " & ((BPVDLS>3)|(BPVDLS<-3))"
        },
    'STREAMS' : { 'Dimuon' : [
    "StrippingTestFullDSTDiMuonPsi2MuMuTOSLine",
    "StrippingTestFullDSTDiMuonJpsi2MuMuTOSLine",
    "StrippingTestFullDSTDiMuonPsi2MuMuDetachedLine",
    "StrippingTestFullDSTDiMuonJpsi2MuMuDetachedLine",
    "StrippingTestFullDSTDiMuonDiMuonNoPVLine",
    'StrippingTestFullDSTDiMuonDiMuonHighMassLine',
    'StrippingTestFullDSTDiMuonDiMuonHighMassSameSignLine'
    
    ]},
    'WGs'    : [ 'BandQ' ]
    },

    'TestMicroDSTDiMuon' :{
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
        'DiMuonSameSign_Prescale'                  :     0.5 ,
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
        'DiMuonLowMass_HLTFILTER'                : "(HLT_PASS('Hlt2DiMuonLowMassDecision'))",

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
    'STREAMS' : {'Leptonic' :[
                      "StrippingTestMicroDSTDiMuonDiMuonSameSignLine",
                      "StrippingTestMicroDSTDiMuonDiMuonIncLine"
                      ]} ,
    'WGs'    : [ 'BandQ' ]
    }
    }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from Configurables import LoKi__VoidFilter
from PhysSelPython.Wrappers import Selection, DataOnDemand, EventSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class DiMuonConf(LineBuilder):
    
    __configuration_keys__ = (
        "MicroDST",
        
        # DiMuon line
        'DiMuon_Prescale',
        'DiMuon_Postscale',
        'DiMuon_checkPV',
        
        'DiMuon_MuonPT',
        'DiMuon_MuonP',
        'DiMuon_MuonTRCHI2DOF', 
        'DiMuon_MinMass',
        'DiMuon_MaxMass',   
        'DiMuon_VCHI2PDOF',
        'DiMuon_PT',
#        'DiMuon_PT_oldTh',

        # DiMuonTIS line
        'DiMuonTIS_Prescale',
        'DiMuonTIS_Postscale',
        'DiMuonTIS_TisTosSpecs',
        'DiMuonTIS_checkPV',

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale',
        'DiMuonSameSign_Postscale',
        'DiMuonSameSign_checkPV',
        
        # DiMuonPrescaled line
        'DiMuonPrescaled_Prescale',
        'DiMuonPrescaled_Postscale',
        'DiMuonPrescaled_checkPV',
        
        'DiMuonPrescaled_MuonPT',
        'DiMuonPrescaled_MuonP',
        'DiMuonPrescaled_MuonTRCHI2DOF', 
        'DiMuonPrescaled_MinMass',
        'DiMuonPrescaled_MaxMass',   
        'DiMuonPrescaled_VCHI2PDOF',
        'DiMuonPrescaled_PT', 
        
        # DiMuonExclusive line
        'DiMuonExclusive_Prescale',
        'DiMuonExclusive_Postscale',
        'DiMuonExclusive_checkPV',
        
        'DiMuonExclusive_MuonPT',
        'DiMuonExclusive_MuonP',
        'DiMuonExclusive_MuonTRCHI2DOF',
        'DiMuonExclusive_MuonPIDCut',
        'DiMuonExclusive_MinMass',
        'DiMuonExclusive_VCHI2PDOF',
        'DiMuonExclusive_PT',
        'DiMuonExclusive_DZ',

        # DiMuonNoPV line
        'DiMuonNoPV_Prescale',
        'DiMuonNoPV_Postscale',
        
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale',
        'DiMuonHighMass_Postscale',
        'DiMuonHighMass_checkPV',
        
        'DiMuonHighMass_MuonPT',
        'DiMuonHighMass_MuonP',
        'DiMuonHighMass_MuonTRCHI2DOF',
        'DiMuonHighMass_MuonPIDCut',
        'DiMuonHighMass_MinMass',
        'DiMuonHighMass_VCHI2PDOF',
        'DiMuonHighMass_PT',

        # DiMuonHighMassSameSign line
        'DiMuonHighMassSameSign_Prescale',
        'DiMuonHighMassSameSign_Postscale',
        'DiMuonHighMassSameSign_checkPV',

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale',
        'DiMuonLowMass_Postscale',
        'DiMuonLowMass_checkPV',
        
        'DiMuonLowMass_MuonPT',
        'DiMuonLowMass_MuonP',
        'DiMuonLowMass_MuonTRCHI2DOF',
        'DiMuonLowMass_MuonPIDCut',
        'DiMuonLowMass_MinMass',
        'DiMuonLowMass_VCHI2PDOF',       
        'DiMuonLowMass_PT',
        'DiMuonLowMass_HLTFILTER',
        # Tight Jpsi line
        'Jpsi2MuMu_Prescale',
        'Jpsi2MuMu_Postscale',
        'Jpsi2MuMu_checkPV',
        
        'Jpsi2MuMu_MuonPT',
        'Jpsi2MuMu_MuonP',
        'Jpsi2MuMu_MuonPIDmu',
        'Jpsi2MuMu_MuonTRCHI2DOF', 
        'Jpsi2MuMu_MinMass',
        'Jpsi2MuMu_MaxMass',
        'Jpsi2MuMu_VCHI2PDOF',
        'Jpsi2MuMu_PT',
        'Jpsi2MuMu_TisTosSpecs',
        
        # Tight Psi(2S) line
        'Psi2MuMu_Prescale',
        'Psi2MuMu_Postscale',
        'Psi2MuMu_checkPV',

        'Psi2MuMu_ParticleName', 
        'Psi2MuMu_MuonPT',
        'Psi2MuMu_MuonP',
        'Psi2MuMu_MuonPIDmu',
        'Psi2MuMu_MuonTRCHI2DOF', 
        'Psi2MuMu_MassWindow',
        'Psi2MuMu_VCHI2PDOF',
        'Psi2MuMu_PT',
        'Psi2MuMu_TisTosSpecs', 
        
        # DiMuonDetached line
        'DiMuonDetached_Prescale',
        'DiMuonDetached_Postscale',
        
        'DiMuonDetached_MuonPT',
        'DiMuonDetached_MuonP',
        'DiMuonDetached_MuonPIDmu',
        'DiMuonDetached_MuonTRCHI2DOF', 
        'DiMuonDetached_MinMass',
        'DiMuonDetached_VCHI2PDOF',
        'DiMuonDetached_PT',
        'DiMuonDetached_LTCuts',
        
        # Jpsi2MuMuDetached line
        'Jpsi2MuMuDetached_Prescale',
        'Jpsi2MuMuDetached_Postscale',
        
        'Jpsi2MuMuDetached_MuonPT',
        'Jpsi2MuMuDetached_MuonP',
        'Jpsi2MuMuDetached_MuonPIDmu',
        'Jpsi2MuMuDetached_MuonTRCHI2DOF', 
        'Jpsi2MuMuDetached_MinMass',
        'Jpsi2MuMuDetached_MaxMass',
        'Jpsi2MuMuDetached_VCHI2PDOF',
        'Jpsi2MuMuDetached_PT',
        'Jpsi2MuMuDetached_LTCuts',
#        'Jpsi2MuMuDetached_MinusLTCuts',
        
        # Psi2MuMuDetached line
        'Psi2MuMuDetached_Prescale',
        'Psi2MuMuDetached_Postscale',

        'Psi2MuMuDetached_ParticleName',  
        'Psi2MuMuDetached_MuonPT',
        'Psi2MuMuDetached_MuonP',
        'Psi2MuMuDetached_MuonPIDmu',
        'Psi2MuMuDetached_MuonTRCHI2DOF',
        'Psi2MuMuDetached_MassWindow',
        'Psi2MuMuDetached_VCHI2PDOF',
        'Psi2MuMuDetached_PT',
        'Psi2MuMuDetached_LTCuts',
#        'Psi2MuMuDetached_MinusLTCuts'       
        )
    



    
    def __init__(self, name, config):
        
        LineBuilder.__init__(self, name, config)

        # if name not set outside, set it to empty 
        if name == None:
            name = ""        

            
        """
        DiMuon line 
        """
        self.SelDiMuon = filterDiMuonWMax( name + 'DiMuon',
                                           MuonPT        = config['DiMuon_MuonPT'],
                                           MuonP         = config['DiMuon_MuonP'],
                                           MuonTRCHI2DOF = config['DiMuon_MuonTRCHI2DOF'],
                                           MuMuMinMass   = config['DiMuon_MinMass'],
                                           MuMuMaxMass   = config['DiMuon_MaxMass'],
                                           MuMuVCHI2PDOF = config['DiMuon_VCHI2PDOF'],
                                           MuMuPT        = config['DiMuon_PT']
                                       )

        self.DiMuonLine = StrippingLine( name + 'DiMuonInc' + 'Line',
                                         prescale  = config['DiMuon_Prescale'],
                                         postscale = config['DiMuon_Postscale'],
                                         checkPV   = config['DiMuon_checkPV'],
                                         selection = self.SelDiMuon
                                         )
# Line merged to DiMuon
#        """
#        DiMuonLowPT line (Complementary line for 2012 restripping, including only events with pT(Jpsi) < DiMuon_PT_oldTh
#        """
#        self.SelDiMuonLowPT = filterDiMuonWMax( name + 'DiMuonLowPT',
#                                           MuonPT        = config['DiMuon_MuonPT'],
#                                           MuonP         = config['DiMuon_MuonP'],
#                                           MuonTRCHI2DOF = config['DiMuon_MuonTRCHI2DOF'],
#                                           MuMuMinMass   = config['DiMuon_MinMass'],
#                                           MuMuMaxMass   = config['DiMuon_MaxMass'],
#                                           MuMuVCHI2PDOF = config['DiMuon_VCHI2PDOF'],
#                                           MuMuPT        = config['DiMuon_PT_oldTh'],
#                                           reversePtCut  = True
#                                       )
#
#        self.DiMuonLowPTLine = StrippingLine( name + 'DiMuonIncLowPT' + 'Line',
#                                         prescale  = config['DiMuon_Prescale'],
#                                         postscale = config['DiMuon_Postscale'],
#                                         checkPV   = config['DiMuon_checkPV'],
#                                         selection = self.SelDiMuonLowPT
#                                         )

        """
        DiMuon same sign line
        """
        self.SelDiMuonSameSign = filterDiMuonSameSign( name + 'DiMuonSameSign',
                                                       MuonPT        = config['DiMuon_MuonPT'],
                                                       MuonP         = config['DiMuon_MuonP'],
                                                       MuonTRCHI2DOF = config['DiMuon_MuonTRCHI2DOF'],
                                                       MuMuMinMass   = config['DiMuon_MinMass'],
                                                       MuMuVCHI2PDOF = config['DiMuon_VCHI2PDOF'],
                                                       MuMuPT        = config['DiMuon_PT']
                                                       )
        self.DiMuonSameSignLine = StrippingLine( name + 'DiMuonSameSign' + 'Line',
                                                 prescale  = config['DiMuonSameSign_Prescale'],
                                                 postscale = config['DiMuonSameSign_Postscale'],
                                                 checkPV   = config['DiMuonSameSign_checkPV'],
                                                 selection = self.SelDiMuonSameSign
                                                 )

        """
        DiMuonTIS
        """
        self.DiMuonTISSel = filterTisTos( name + "TISDiMuon",
                                          DiMuonInput = self.SelDiMuon,
                                          myTisTosSpecs = config['DiMuonTIS_TisTosSpecs']
                                          )
        
        self.DiMuonTISLine = StrippingLine( name + 'DiMuonTIS' + 'Line',
                                               prescale  = config['Jpsi2MuMu_Prescale'],
                                               postscale = config['Jpsi2MuMu_Postscale'],
                                               checkPV   = config['Jpsi2MuMu_checkPV'],
                                               selection = self.DiMuonTISSel
                                               )
        
        """
        DiMuonPrescaled line
        """
        self.SelDiMuonPrescaled = filterDiMuonWMax( name + 'DiMuonPrescaled',
                                                    MuonPT        = config['DiMuonPrescaled_MuonPT'],
                                                    MuonP         = config['DiMuonPrescaled_MuonP'],
                                                    MuonTRCHI2DOF = config['DiMuonPrescaled_MuonTRCHI2DOF'],
                                                    MuMuMinMass   = config['DiMuonPrescaled_MinMass'],
                                                    MuMuMaxMass   = config['DiMuonPrescaled_MaxMass'],
                                                    MuMuVCHI2PDOF = config['DiMuonPrescaled_VCHI2PDOF'],
                                                    MuMuPT        = config['DiMuonPrescaled_PT']
                                                    )
        
        self.DiMuonPrescaledLine = StrippingLine( name + 'DiMuonPrescaled' + 'Line',
                                                  prescale  = config['DiMuonPrescaled_Prescale'],
                                                  postscale = config['DiMuonPrescaled_Postscale'],
                                                  checkPV   = config['DiMuonPrescaled_checkPV'],
                                                  selection = self.SelDiMuonPrescaled
                                                  )
        
        
        """
        DiMuonExclusiveline
        """
        self.SelDiMuonExclusive = filterDiMuonAndDZ( name + 'DiMuonExclusive',
                                                     MuonPT        = config['DiMuonExclusive_MuonPT'],
                                                     MuonP         = config['DiMuonExclusive_MuonP'],
                                                     MuonTRCHI2DOF = config['DiMuonExclusive_MuonTRCHI2DOF'],
                                                     MuMuMinMass   = config['DiMuonExclusive_MinMass'],
                                                     MuMuVCHI2PDOF = config['DiMuonExclusive_VCHI2PDOF'],
                                                     MuMuPT        = config['DiMuonExclusive_PT'],
                                                     MuMuDZ        = config['DiMuonExclusive_DZ']
                                                     )
                
        self.DiMuonExclusiveLine = StrippingLine( name + 'DiMuonExclusive' + 'Line',
                                                  prescale  = config['DiMuonExclusive_Prescale'],
                                                  postscale = config['DiMuonExclusive_Postscale'],
                                                  checkPV   = config['DiMuonExclusive_checkPV'],
                                                  selection = self.SelDiMuonExclusive
                                                  )

        """
        DiMuonNoPVline
        """
        self.SelDiMuonNoPV = filterDiMuon( name + 'DiMuonNoPV',
                                           MuonPT        = config['DiMuonExclusive_MuonPT'],
                                           MuonP         = config['DiMuonExclusive_MuonP'],
                                           MuonTRCHI2DOF = config['DiMuonExclusive_MuonTRCHI2DOF'],
                                           MuonPIDCut    = config['DiMuonExclusive_MuonPIDCut'], 
                                           MuMuMinMass   = config['DiMuonExclusive_MinMass'],
                                           MuMuVCHI2PDOF = config['DiMuonExclusive_VCHI2PDOF'],
                                           MuMuPT        = config['DiMuonExclusive_PT']
                                           )

        self.DiMuonNoPVLine = StrippingLine( name + 'DiMuonNoPV' + 'Line',
                                             prescale  = config['DiMuonNoPV_Prescale'],
                                             postscale = config['DiMuonNoPV_Postscale'],
                                             checkPV   = False,
                                             FILTER = { 'Code'       : "recSummary(LHCb.RecSummary.nPVs, 'Rec/Vertex/Primary')<0.5" , 
                                                        'Preambulo'  : [ 'from LoKiTracks.decorators import *' ,
                                                                         'from LoKiNumbers.decorators    import *',
                                                                         'from LoKiCore.functions    import *' ]
                                                        },
                                             selection = self.SelDiMuonNoPV
                                             )       
        
        """
        DiMuon High Mass line
        """
        self.SelDiMuonHighMass = filterDiMuon( name + 'DiMuonHighMass',
                                               MuonPT        = config['DiMuonHighMass_MuonPT'],
                                               MuonP         = config['DiMuonHighMass_MuonP'],
                                               MuonTRCHI2DOF = config['DiMuonHighMass_MuonTRCHI2DOF'],
                                               MuonPIDCut    = config['DiMuonHighMass_MuonPIDCut'],
                                               MuMuMinMass   = config['DiMuonHighMass_MinMass'],
                                               MuMuVCHI2PDOF = config['DiMuonHighMass_VCHI2PDOF'],
                                               MuMuPT        = config['DiMuonHighMass_PT']                                               
                                               )
        self.DiMuonHighMassLine = StrippingLine( name + 'DiMuonHighMass' + 'Line',
                                                 prescale  = config['DiMuonHighMass_Prescale'],
                                                 postscale = config['DiMuonHighMass_Postscale'],
                                                 checkPV   = config['DiMuonHighMass_checkPV'],
                                                 selection = self.SelDiMuonHighMass
                                                 )


        
        """
        DiMuon High Mass Same Sign line
        """
        self.SelDiMuonHighMassSameSign = filterDiMuonSameSign( name + 'DiMuonHighMassSameSign',
                                                               MuonPT        = config['DiMuonHighMass_MuonPT'],
                                                               MuonP         = config['DiMuonHighMass_MuonP'],
                                                               MuonTRCHI2DOF = config['DiMuonHighMass_MuonTRCHI2DOF'],
                                                               MuMuMinMass   = config['DiMuonHighMass_MinMass'],
                                                               MuMuVCHI2PDOF = config['DiMuonHighMass_VCHI2PDOF'],
                                                               MuMuPT        = config['DiMuonHighMass_PT']
                                                               )
        
        self.DiMuonHighMassSameSignLine = StrippingLine( name + 'DiMuonHighMassSameSign' + 'Line',
                                                         prescale  = config['DiMuonHighMassSameSign_Prescale'],
                                                         postscale = config['DiMuonHighMassSameSign_Postscale'],
                                                         checkPV   = config['DiMuonHighMassSameSign_checkPV'],
                                                         selection = self.SelDiMuonHighMassSameSign
                                                         )


        
        """
        DiMuon Low Mass line
        """
        self.SelDiMuonLowMass = filterDiMuon( name + 'DiMuonLowMass',
                                              MuonPT        = config['DiMuonLowMass_MuonPT'],
                                              MuonP         = config['DiMuonLowMass_MuonP'],
                                              MuonTRCHI2DOF = config['DiMuonLowMass_MuonTRCHI2DOF'],
                                              MuonPIDCut    = config['DiMuonLowMass_MuonPIDCut'], 
                                              MuMuMinMass   = config['DiMuonLowMass_MinMass'],
                                              MuMuVCHI2PDOF = config['DiMuonLowMass_VCHI2PDOF'],
                                              MuMuPT        = config['DiMuonLowMass_PT']
                                              )
        self.DiMuonLowMassLine = StrippingLine( name + 'DiMuonLowMass' + 'Line',
                                                HLT = config['DiMuonLowMass_HLTFILTER'],
                                                prescale  = config['DiMuonLowMass_Prescale'],
                                                postscale = config['DiMuonLowMass_Postscale'],
                                                checkPV   = config['DiMuonLowMass_checkPV'],
                                                selection = self.SelDiMuonLowMass 
                                                )
        
        """
        Jpsi-> mumu tight line
        """
        self.SelJpsi2MuMu = filterJpsi2MuMu( name + 'Jpsi2MuMu',
                                             MuonPT        = config['Jpsi2MuMu_MuonPT'],
                                             MuonP         = config['Jpsi2MuMu_MuonP'],
                                             MuonPIDmu     = config['Jpsi2MuMu_MuonPIDmu'],
                                             MuonTRCHI2DOF = config['Jpsi2MuMu_MuonTRCHI2DOF'],
                                             MuMuMinMass   = config['Jpsi2MuMu_MinMass'],
                                             MuMuMaxMass   = config['Jpsi2MuMu_MaxMass'],
                                             MuMuVCHI2PDOF = config['Jpsi2MuMu_VCHI2PDOF'],
                                             MuMuPT        = config['Jpsi2MuMu_PT']
                                             )
        self.Jpsi2MuMuLine = StrippingLine( name + 'Jpsi2MuMu' + 'Line',
                                            prescale  = config['Jpsi2MuMu_Prescale'],
                                            postscale = config['Jpsi2MuMu_Postscale'],
                                            checkPV   = config['Jpsi2MuMu_checkPV'],
                                            selection = self.SelJpsi2MuMu
                                            )

        """
        Jpsi2MuMuTOS
        """
        self.TOSJpsi2MuMu = filterTisTos( name + "TOSJpsi2MuMu",
                                          DiMuonInput = self.SelJpsi2MuMu,
                                          myTisTosSpecs = config['Jpsi2MuMu_TisTosSpecs']
                                          )
        
        self.Jpsi2MuMuTOSLine = StrippingLine( name + 'Jpsi2MuMuTOS' + 'Line',
                                               prescale  = config['Jpsi2MuMu_Prescale'],
                                               postscale = config['Jpsi2MuMu_Postscale'],
                                               checkPV   = config['Jpsi2MuMu_checkPV'],
                                               selection = self.TOSJpsi2MuMu
                                               )

        """
        Psi(2S)->mumu tight line
        """
        self.SelPsi2MuMu = filterSignal( name + 'Psi2MuMu',
                                         ParticleName  = config['Psi2MuMu_ParticleName'],
                                         MuonPT        = config['Psi2MuMu_MuonPT'],
                                         MuonP         = config['Psi2MuMu_MuonP'],
                                         MuonPIDmu     = config['Psi2MuMu_MuonPIDmu'],  
                                         MuonTRCHI2DOF = config['Psi2MuMu_MuonTRCHI2DOF'],
                                         MuMuMassWindow= config['Psi2MuMu_MassWindow'],                                          
                                         MuMuVCHI2PDOF = config['Psi2MuMu_VCHI2PDOF'],
                                         MuMuPT        = config['Psi2MuMu_PT']
                                         )

        self.Psi2MuMuLine = StrippingLine( name + 'Psi2MuMu' + 'Line',
                                           prescale  = config['Psi2MuMu_Prescale'],
                                           postscale = config['Psi2MuMu_Postscale'],
                                           checkPV   = config['Psi2MuMu_checkPV'],
                                           selection = self.SelPsi2MuMu
                                           )

        
        """
        Psi2MuMuTOS
        """
        self.TOSPsi2MuMu = filterTisTos( name + "TOSPsi2MuMu",
                                         DiMuonInput = self.SelPsi2MuMu,
                                         myTisTosSpecs = config['Psi2MuMu_TisTosSpecs']
                                         )
        
        self.Psi2MuMuTOSLine = StrippingLine( name + 'Psi2MuMuTOS' + 'Line',
                                              prescale  = config['Psi2MuMu_Prescale'],
                                              postscale = config['Psi2MuMu_Postscale'],
                                              checkPV   = config['Psi2MuMu_checkPV'],
                                              selection = self.TOSPsi2MuMu
                                              )
        
        
        """
        DiMuonDetachedline
        """
        self.SelDiMuonDetached = filterDiMuonDetached( name + 'DiMuonDetached',
                                                       MuonPT        = config['DiMuonDetached_MuonPT'],
                                                       MuonP         = config['DiMuonDetached_MuonP'],
                                                       MuonPIDmu     = config['DiMuonDetached_MuonPIDmu'],
                                                       MuonTRCHI2DOF = config['DiMuonDetached_MuonTRCHI2DOF'],
                                                       MuMuMinMass   = config['DiMuonDetached_MinMass'],
                                                       MuMuVCHI2PDOF = config['DiMuonDetached_VCHI2PDOF'],
                                                       MuMuPT        = config['DiMuonDetached_PT'],
                                                       MuMuLTCuts    = config['DiMuonDetached_LTCuts']
                                                       )
                
        self.DiMuonDetachedLine = StrippingLine( name + 'DiMuonDetached' + 'Line',
                                                 prescale  = config['DiMuonDetached_Prescale'],
                                                 postscale = config['DiMuonDetached_Postscale'],
                                                 checkPV   = True,
                                                 selection = self.SelDiMuonDetached
                                                 )


        """
        Jpsi2MuMuDetached tight line
        """
        self.SelJpsi2MuMuDetached = filterJpsi2MuMuDetached( name + 'Jpsi2MuMuDetached',
                                                             MuonPT        = config['Jpsi2MuMuDetached_MuonPT'],
                                                             MuonP         = config['Jpsi2MuMuDetached_MuonP'],
                                                             MuonPIDmu     = config['Jpsi2MuMuDetached_MuonPIDmu'],
                                                             MuonTRCHI2DOF = config['Jpsi2MuMuDetached_MuonTRCHI2DOF'],
                                                             MuMuMinMass   = config['Jpsi2MuMuDetached_MinMass'],
                                                             MuMuMaxMass   = config['Jpsi2MuMuDetached_MaxMass'],
                                                             MuMuVCHI2PDOF = config['Jpsi2MuMuDetached_VCHI2PDOF'],
                                                             MuMuPT        = config['Jpsi2MuMuDetached_PT'],
                                                             MuMuLTCuts    = config['Jpsi2MuMuDetached_LTCuts']
                                                             )
        self.Jpsi2MuMuDetachedLine = StrippingLine( name + 'Jpsi2MuMuDetached' + 'Line',
                                                    prescale  = config['Jpsi2MuMuDetached_Prescale'],
                                                    postscale = config['Jpsi2MuMuDetached_Postscale'],
                                                    RequiredRawEvents = [ "Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker" ],
                                                    checkPV   = True,
                                                    selection = self.SelJpsi2MuMuDetached
                                                    )

#        """                               
#        Jpsi2MuMuDetached tight line. negative value for lifetime cut.      
#        """
#        self.SelJpsi2MuMuDetachedMinus = filterJpsi2MuMuDetached( name + 'Jpsi2MuMuDetachedMinus',
#                                                             MuonPT        = config['Jpsi2MuMuDetached_MuonPT'],
#                                                             MuonP         = config['Jpsi2MuMuDetached_MuonP'],
#                                                             MuonPIDmu     = config['Jpsi2MuMuDetached_MuonPIDmu'],
#                                                             MuonTRCHI2DOF = config['Jpsi2MuMuDetached_MuonTRCHI2DOF'],
#                                                             MuMuMinMass   = config['Jpsi2MuMuDetached_MinMass'],
#                                                             MuMuMaxMass   = config['Jpsi2MuMuDetached_MaxMass'],
#                                                             MuMuVCHI2PDOF = config['Jpsi2MuMuDetached_VCHI2PDOF'],
#                                                             MuMuPT        = config['Jpsi2MuMuDetached_PT'],
#                                                             MuMuLTCuts    = config['Jpsi2MuMuDetached_MinusLTCuts']
#                                                             )
#        self.Jpsi2MuMuDetachedMinusLine = StrippingLine( name + 'Jpsi2MuMuDetachedMinus' + 'Line',
#                                                    prescale  = config['Jpsi2MuMuDetached_Prescale'],
#                                                    postscale = config['Jpsi2MuMuDetached_Postscale'],
#                                                    checkPV   = True,
#                                                    selection = self.SelJpsi2MuMuDetachedMinus
#                                                    )

        """
        Psi2MuMuDetached line
        """
        self.SelPsi2MuMuDetached = filterSignalDetached( name + 'Psi2MuMuDetached',
                                                         ParticleName  = config['Psi2MuMuDetached_ParticleName'],
                                                         MuonPT        = config['Psi2MuMuDetached_MuonPT'],
                                                         MuonP         = config['Psi2MuMuDetached_MuonP'],
                                                         MuonPIDmu     = config['Psi2MuMuDetached_MuonPIDmu'],  
                                                         MuonTRCHI2DOF = config['Psi2MuMuDetached_MuonTRCHI2DOF'],
                                                         MuMuMassWindow= config['Psi2MuMuDetached_MassWindow'],
                                                         MuMuVCHI2PDOF = config['Psi2MuMuDetached_VCHI2PDOF'],
                                                         MuMuPT        = config['Psi2MuMuDetached_PT'],
                                                         MuMuLTCuts    = config['Psi2MuMuDetached_LTCuts']
                                                         )

        self.Psi2MuMuDetachedLine = StrippingLine( name + 'Psi2MuMuDetached' + 'Line',
                                                   prescale  = config['Psi2MuMuDetached_Prescale'],
                                                   postscale = config['Psi2MuMuDetached_Postscale'],
                                                   checkPV   = True,
                                                   RequiredRawEvents = [ "Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker" ],
                                                   selection = self.SelPsi2MuMuDetached
                                                   ) 


# 
#        """
#        Psi2MuMuDetached line. negative value for lifetime cut.
#        """                                      
#        self.SelPsi2MuMuDetachedMinus = filterSignalDetached( name + 'Psi2MuMuDetachedMinus',
#                                                         ParticleName  = config['Psi2MuMuDetached_ParticleName'],
#                                                         MuonPT        = config['Psi2MuMuDetached_MuonPT'],
#                                                         MuonP         = config['Psi2MuMuDetached_MuonP'],
#                                                         MuonPIDmu     = config['Psi2MuMuDetached_MuonPIDmu'],  
#                                                         MuonTRCHI2DOF = config['Psi2MuMuDetached_MuonTRCHI2DOF'],
#                                                         MuMuMassWindow= config['Psi2MuMuDetached_MassWindow'],
#                                                         MuMuVCHI2PDOF = config['Psi2MuMuDetached_VCHI2PDOF'],
#                                                         MuMuPT        = config['Psi2MuMuDetached_PT'],
##                                                         MuMuLTCuts    = config['Psi2MuMuDetached_MinusLTCuts']
#                                                         )
#           
#        self.Psi2MuMuDetachedMinusLine = StrippingLine( name + 'Psi2MuMuDetachedMinus' + 'Line',
#                                                   prescale  = config['Psi2MuMuDetached_Prescale'],
#                                                   postscale = config['Psi2MuMuDetached_Postscale'],
#                                                   checkPV   = True,
#                                                   selection = self.SelPsi2MuMuDetachedMinus
#                                                   )

        

        if config['MicroDST']:
            self.registerLine( self.DiMuonLine )
            #self.registerLine( self.DiMuonLowPTLine ) ## line merged to DiMuonLine
            self.registerLine( self.DiMuonSameSignLine )
            # self.registerLine( self.DiMuonPrescaledLine ) ## line covered by DiMuonLine
            self.registerLine( self.Jpsi2MuMuLine )
            self.registerLine( self.Psi2MuMuLine )
            self.registerLine( self.DiMuonTISLine )

        else:    
            #self.registerLine( self.DiMuonExclusiveLine )
            self.registerLine( self.DiMuonNoPVLine )
            self.registerLine( self.DiMuonHighMassLine )
            self.registerLine( self.DiMuonHighMassSameSignLine )
            #self.registerLine( self.DiMuonLowMassLine )
            #self.registerLine( self.Jpsi2MuMuLine )
            self.registerLine( self.Jpsi2MuMuTOSLine )
            #self.registerLine( self.Psi2MuMuLine )
            self.registerLine( self.Psi2MuMuTOSLine )
            #self.registerLine( self.DiMuonDetachedLine )
            self.registerLine( self.Jpsi2MuMuDetachedLine )
#            self.registerLine( self.Jpsi2MuMuDetachedMinusLine )
            self.registerLine( self.Psi2MuMuDetachedLine )
#            self.registerLine( self.Psi2MuMuDetachedMinusLine )

def filterDiMuon( name,
                  MuonPT,
                  MuonP,
                  MuonTRCHI2DOF,
                  MuonPIDCut,
                  MuMuMinMass,
                  MuMuVCHI2PDOF,
                  MuMuPT 
                  ):
    
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut + MuonPIDCut )

    return Selection( name + "_SelMuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )


def filterDiMuonSameSign( name,
                          MuonPT,
                          MuonP,
                          MuonTRCHI2DOF,
                          MuMuMinMass,
                          MuMuVCHI2PDOF,
                          MuMuPT
                          ):
    
    _StdLooseDiMuonSameSign = DataOnDemand( Location = 'Phys/StdLooseDiMuonSameSign/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s *MeV)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name + "_SelMuMuSS",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuonSameSign ]
                      )


def filterDiMuonWMax( name,
                      MuonPT,
                      MuonP,
                      MuonTRCHI2DOF,
                      MuMuMinMass,
                      MuMuMaxMass,
                      MuMuVCHI2PDOF,
                      MuMuPT,
                      reversePtCut = False
                      ):
    
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(in_range( %(MuMuMinMass)s *MeV, MM, %(MuMuMaxMass)s *MeV)) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()

    if (reversePtCut):
      MuMuCut = "(in_range( %(MuMuMinMass)s *MeV, MM, %(MuMuMaxMass)s *MeV)) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT < %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )

    return Selection( name + "_SelMuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )



def filterJpsi2MuMu( name,
                     MuonPT,
                     MuonP,
                     MuonPIDmu,
                     MuonTRCHI2DOF,
                     MuMuMinMass,
                     MuMuMaxMass, 
                     MuMuVCHI2PDOF,
                     MuMuPT
                     ):
    
    _StdLooseJpsi2MuMu = DataOnDemand( Location = 'Phys/StdLooseJpsi2MuMu/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MINTREE('mu+'==ABSID,PIDmu) > %(MuonPIDmu)s) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (MM < %(MuMuMaxMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name + "_SelJpsi2MuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseJpsi2MuMu ]
                      )


def filterSignal( name,
                  ParticleName, 
                  MuonPT,
                  MuonP,
                  MuonPIDmu,
                  MuonTRCHI2DOF,
                  MuMuMassWindow, 
                  MuMuVCHI2PDOF,
                  MuMuPT
                  ):
        
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MINTREE('mu+'==ABSID,PIDmu) > %(MuonPIDmu)s) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(ADMASS(%(ParticleName)s) < %(MuMuMassWindow)s *MeV) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s *MeV)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )
    
    return Selection( name + "_SelP2MuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )


def filterDZ( name,
              DZAnyPV, 
              MySelection ):
    
    return EventSelection (
        #
        LoKi__VoidFilter( name + 'filterDZ', 
                          Code =  " ( minMyZ - minPVZ ) < %(DZAnyPV)s*mm " % locals() ,
                          Preambulo = [ "from LoKiPhys.decorators import *",
                                        "minMyZ = SOURCE('%s') >> min_value( VFASPF(VZ) )"  %(MySelection.outputLocation()) ,
                                        "minPVZ = VSOURCE('Rec/Vertex/Primary') >> min_value(VZ) "
                                        ]
                          )
        )
        


def filterDiMuonAndDZ( name,
                       MuonPT,
                       MuonP,
                       MuonTRCHI2DOF,
                       MuMuMinMass,
                       MuMuVCHI2PDOF,
                       MuMuPT,
                       MuMuDZ
                       ):
    
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s) & (BPVVDZ < %(MuMuDZ)s*mm)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut )

    return Selection( name + "_SelMuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )


def filterDiMuonDetached( name,
                          MuonPT,
                          MuonP,
                          MuonPIDmu,
                          MuonTRCHI2DOF,
                          MuMuMinMass,
                          MuMuVCHI2PDOF,
                          MuMuPT,
                          MuMuLTCuts
                          ):
    
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s) & (MINTREE('mu+'==ABSID,PIDmu) > %(MuonPIDmu)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut + MuMuLTCuts )

    return Selection( name + "_SelMuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )


def filterJpsi2MuMuDetached( name,
                             MuonPT,
                             MuonP,
                             MuonPIDmu,
                             MuonTRCHI2DOF,
                             MuMuMinMass,
                             MuMuMaxMass, 
                             MuMuVCHI2PDOF,
                             MuMuPT,
                             MuMuLTCuts
                             ):
    
    _StdLooseJpsi2MuMu = DataOnDemand( Location = 'Phys/StdLooseJpsi2MuMu/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s) & (MINTREE('mu+'==ABSID,PIDmu) > %(MuonPIDmu)s)" % locals()
    
    MuMuCut = "(MM > %(MuMuMinMass)s) & (MM < %(MuMuMaxMass)s) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut + MuMuLTCuts  )
    
    return Selection( name + "_SelJpsi2MuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseJpsi2MuMu ]
                      )


def filterSignalDetached( name,
                          ParticleName, 
                          MuonPT,
                          MuonP,
                          MuonPIDmu,
                          MuonTRCHI2DOF,
                          MuMuMassWindow, 
                          MuMuVCHI2PDOF,
                          MuMuPT,
                          MuMuLTCuts
                          ):
    
    _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' )
    
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s *MeV) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s *MeV) & (MINTREE('mu+'==ABSID,PIDmu) > %(MuonPIDmu)s) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)" % locals()
    
    MuMuCut = "(ADMASS(%(ParticleName)s) < %(MuMuMassWindow)s *MeV) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s *MeV)" % locals()
    
    _MuMu = FilterDesktop( Code = MuonCut + " & " + MuMuCut + MuMuLTCuts )
    
    return Selection( name + "_SelP2MuMu",
                      Algorithm = _MuMu,
                      RequiredSelections = [ _StdLooseDiMuon ]
                      )



def filterTisTos(name,
                 DiMuonInput,
                 myTisTosSpecs ) :
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

