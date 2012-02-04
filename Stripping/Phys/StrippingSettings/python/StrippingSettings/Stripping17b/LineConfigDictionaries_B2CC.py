BetaS = {         
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiXforBeta_sConf',
    'CONFIG'    : {
                         'TRCHI2DOF'             :       5
                 ,       'BPVLTIME'              :       0.2
                 ,       'DaughterPT'            :       1000
                 ,       'VCHI2PDOF'             :       10
                 ,       'Jpsi2MuMuPrescale'     :       0.01
                 ,       'Bu2JpsiKPrescale'      :       0.05
                 ,       'Bd2JpsiKstarPrescale'  :       0.03
                 ,       'Bd2JpsiKsPrescale'     :       0.3
                 ,       'Bs2JpsiPhiPrescale'    :       0.15
                 ,       'Bs2JpsiEtaPrescale'    :       0.2
                         },
    'STREAMS' : [ 'Dimuon' ]
    }

B2EtacKst = {
    'WGs'       :       ['B2CC'],
    'BUILDERTYPE'       :       'B2EtacKstConf',
    'CONFIG'    :       {
    'Prescale'                : 1.0 ,
    'Postscale'               : 1.0 ,
    #K parameters
    'K_PT'                    : 600.,
    'K_TRCHI2'                : 3.7,
    'K_IPCHI2'                : 6.,
    #pi parameters
    'pi_PT'                   : 600., 
    'pi_TRCHI2'               : 3.7,
    'pi_IPCHI2'               : 6.,
    #kst parameters
    'kst_VDZ'                 : 0., 
    'kst_PT'                  : 1000., 
    'kst_IPCHI2'              : 6.,
    'kst_VCHI2_VDOF'          : 16.,
    #rho (for eta_c decay) parameters
    'rho_etac_VDZ'            : 0.,
    'rho_etac_VCHI2_VDOF'     : 16.,
    #phi (for eta_c decay) parameters
    'phi_etac_VDZ'            : 0.,
    'phi_etac_VCHI2_VDOF'     : 16.,
    #eta_c parameters
    'etac_VDZ'                : 0.,    
    'etac_PT'                 : 600.,
    'etac_IPCHI2'             : 6., 
    'etac_VCHI2_VDOF'         : 16.,
    #B parameters
    'B_VDZ'                  : 0.,
    'B_DIRA'                 : 0.999,
    'B_IPCHI2'               : 9.,
    'B_VCHI2_VDOF'           : 16.
    },
    'STREAMS'  : [ 'Bhadron' ]
    }

# New line since Stripping17b
B2ChicK = {
    'WGs'       :       ['B2CC'],
    'BUILDERTYPE'       :       'B2ChicKConf',
    'CONFIG'    :       {
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    #K parameters
    'K_PT'                   : 600.,
    'K_TRCHI2'               : 3.7,
    'K_IPCHI2'               : 6.,
    #pi parameters
    'pi_PT'                  : 600., 
    'pi_TRCHI2'              : 3.7,
    'pi_IPCHI2'              : 6.,
    #rho (for chi_c decay) parameters
    'rho_chic_VDZ'           : 0.,
    'rho_chic_VCHI2_VDOF'    : 16.,
    #phi (for chi_c decay) parameters
    'phi_chic_VDZ'           : 0.,
    'phi_chic_VCHI2_VDOF'    : 16.,
    #chi_c parameters
    'chic_VDZ'               : 0.,    
    'chic_PT'                : 1000.,
    'chic_IPCHI2'            : 6.,
    'chic_VCHI2_VDOF'        : 16.,
    #B parameters
    'B_VDZ'                 : 0.,
    'B_DIRA'                : 0.999,
    'B_IPCHI2'              : 9.,
    'B_VCHI2_VDOF'          : 16.
    },
    'STREAMS'  : [ 'Bhadron' ]
    }


