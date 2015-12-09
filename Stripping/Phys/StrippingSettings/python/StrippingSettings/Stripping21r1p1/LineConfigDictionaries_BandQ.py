##########################################################
##                 STRIPPING 21r1p1                     ##
##                                                      ##
##   Configuration for BandQ WG                         ##
##   contact person: roberta.cardinale@cern.ch          ##
##                                                      ##
##########################################################

from GaudiKernel.SystemOfUnits import *

##########################################################
##                                                      ##
##            StrippingCC2DD.py                         ##
##         Lucio Anderlini and Andrea Bizzetti          ##
##                                                      ##
##########################################################

CC2DD =  {
    'WGs'  : ['BandQ'],
    'BUILDERTYPE'       : 'CC2DDConf',
    'CONFIG'   :  {
######## D0 / D~0 -> K pi cuts
                   'D0MassWin'     : "60*MeV",
                   'D0PT'          : "1000*MeV",
                   'D0VtxChi2Ndof' : 10,
                   'D0Bpvdira'     : -10.,
                   'D0Bpvdls'      : 4.,
                   'D0daughterBpvIpChi2'    : 4.,
                   'D0daughterPT'           : "600*MeV",
                   'D0daughterP'            : "5*GeV",
                   'D0daughterTrkChi2'      : 3,
                   'D0daughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'D0daughterKaonProbNNk'  : 0.1,
                   'D0daughterPionProbNNpi' : 0.1,

######## Dplus/Dminus -> Kpipi cuts, used also for D_s+/D_s- ->KKpi
                   'DpmMassWin'     : "60*MeV",
                   'DpmPT'          : "1000*MeV",
                   'DpmVtxChi2Ndof' : 10,
                   'DpmBpvdira'     : -10.,
                   'DpmBpvdls'      : 4.,
                   'DpmdaughterBpvIpChi2'    : 4.,
                   'DpmdaughterPT'           : "500*MeV",
                   'DpmdaughterP'            : "5*GeV",
                   'DpmdaughterTrkChi2'      : 3,
                   'DpmdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'DpmdaughterKaonProbNNk'  : 0.1,
                   'DpmdaughterPionProbNNpi' : 0.1,

######## Lambda_c+, Xi_c+ -> p+ K- pi+ cuts
                   'LcMassWin'     : "60*MeV",
                   'LcPT'          : "1000*MeV",
                   'LcVtxChi2Ndof' : 10,
                   'LcBpvdira'     : -10.,
                   'LcBpvdls'      : 4.,               # standard cut
                   'LcdaughterBpvIpChi2'    : 4.,
                   'LcdaughterPT'           : "500*MeV",
                   'LcdaughterP'            : "5*GeV",
                   'LcdaughterTrkChi2'      : 3,
                   'LcdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'LcdaughterKaonProbNNk'  : 0.1,
                   'LcdaughterPionProbNNpi' : 0.1,
                   'LcdaughterProtonProbNNp': 0.1,

######## Xi_c0, Omega_c0 -> p+ K- K- pi+ cuts
                   'XcMassWin'     : "60*MeV",
                   'XcPT'          : "1000*MeV",
                   'XcVtxChi2Ndof' : 10,
                   'XcBpvdira'     : -10.,
                   'XcBpvdls'      : -10.,             # no cut
                   'XcdaughterBpvIpChi2'    : 2.,      # loose cut
                   'XcdaughterPT'           : "500*MeV",
                   'XcdaughterP'            : "5*GeV",
                   'XcdaughterTrkChi2'      : 3,
                   'XcdaughterTrkGhostProb' : 0.3,
### ProbNN conditions
                   'XcdaughterKaonProbNNk'  : 0.1,
                   'XcdaughterPionProbNNpi' : 0.1,
                   'XcdaughterProtonProbNNp': 0.1,

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


#########################################################################
##                                                                     ##
##                              Lb2EtacKp                              ##
##                            Liming Zhang                             ##
##                                                                     ##
#########################################################################


Lb2EtacKp = {
    'BUILDERTYPE'       :  'Lb2EtacKpConf',
    'CONFIG'    : {
        'KaonCuts'      : "(PROBNNk > 0.1) & (PT > 300*MeV) & (TRGHOSTPROB<0.4)",
        'ProtonCuts'    : "(PROBNNp > 0.1) & (PT > 300*MeV) & (P > 10*GeV) & (TRGHOSTPROB<0.4)",        
        'PionCuts'      : "(PROBNNpi > 0.1) & (PT > 250*MeV) & (TRGHOSTPROB<0.4)",
        'EtacComAMCuts' : "(AM<3.25*GeV)",
        'EtacComN4Cuts' : """
                          (in_range(2.75*GeV, AM, 3.25*GeV))
                          & ( (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) ) > 2.5 *GeV)
                          & ( (ACHILD(MIPCHI2DV(), 1) + ACHILD(MIPCHI2DV(), 2) + ACHILD(MIPCHI2DV(), 3) + ACHILD(MIPCHI2DV(), 4))>30)
                          """,
        'EtacMomN4Cuts' : """
                           (VFASPF(VCHI2/VDOF) < 9.) 
                           & (in_range(2.8*GeV, MM, 3.2*GeV)) 
                           & (MIPCHI2DV(PRIMARY) > 0.) 
                           & (BPVVDCHI2>10) 
                           & (BPVDIRA>0.9)
                           """,
        'EtacComCuts'   : "(in_range(2.75*GeV, AM, 3.25*GeV))",
        'LambdaSComCuts': "(ACHILD(PT,1)+ACHILD(PT,2) > 900.*MeV) & (AM < 4.0 *GeV) & (ADOCACHI2CUT(20., ''))",
        'LambdaSMomCuts': """
                          (MIPCHI2DV(PRIMARY) > 0.)
                          & (BPVVDCHI2 > 10.)
                          & (VFASPF(VCHI2) < 9.)
                          & (BPVDIRA>0.9)
                          """,
        'KsCuts'        : "(ADMASS('KS0') < 30.*MeV) & (BPVDLS>5)",
        'LbComCuts'     : "(ADAMASS('Lambda_b0') < 500 *MeV)",
        'LbMomCuts'     : """
                          (VFASPF(VCHI2/VDOF) < 10.) 
                          & (BPVDIRA> 0.9999) 
                          & (BPVIPCHI2()<25) 
                          & (BPVVDCHI2>250)
                          & (BPVVDRHO>0.1*mm) 
                          & (BPVVDZ>2.0*mm)
                          """,
        'Prescale'      : 1.,
        'RelatedInfoTools': [{
                      'Type'              : 'RelInfoVertexIsolation',
                      'Location'          : 'RelInfoVertexIsolation'
                  }, {
                      'Type'              : 'RelInfoVertexIsolationBDT',
                      'Location'          : 'RelInfoVertexIsolationBDT'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 1.0,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_1.0'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 1.5,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_1.5'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 2.0,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_2.0'
                  }]        
        },
    'STREAMS'           : ['Bhadron' ],
    'WGs'               : ['BandQ'],
    }

#############################################################################
##                                                                         ##
##                           XB2DPiP                                       ##
##                                                                         ##
##                        Liming Zhang                                     ##
##                                                                         ##
#############################################################################


XB2DPiP = {
    'WGs'               : ['BandQ'],
    'BUILDERTYPE'       : 'XB2DPiPConf',
    'STREAMS' : [ 'Bhadron' ],
    'CONFIG'    : {
                         'TRCHI2DOF'                 :       5
                 ,       'TRGHOSTPROB'               :       0.5
                 ,       'MIPCHI2'                   :       9
                 ,       'ProtonPIDp'                      :       5
                 ,       'ProtonPIDpK'                     :      -3
                 ,        'MomCuts'     : """
                          (VFASPF(VCHI2/VDOF) < 10.) 
                          & (BPVDIRA> 0.9999) 
                          & (BPVIPCHI2()<25) 
                          & (BPVVDCHI2>250)
                          & (BPVVDRHO>0.1*mm) 
                          & (BPVVDZ>2.0*mm)
                          & (MINTREE(((ABSID=='D+') | (ABSID=='D0') | (ABSID=='Lambda_c+')) , VFASPF(VZ))-VFASPF(VZ) > 0.0 *mm )
                          """
                  ,'RelatedInfoTools': [{
                      'Type'              : 'RelInfoVertexIsolation',
                      'Location'          : 'RelInfoVertexIsolation'
                  }, {
                      'Type'              : 'RelInfoVertexIsolationBDT',
                      'Location'          : 'RelInfoVertexIsolationBDT'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 1.0,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_1.0'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 1.5,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_1.5'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 2.0,
                      "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                      'Location'          : 'RelInfoConeVariables_2.0'
                  }]
                  }    
    }


############################################################################
##                                                                        ##
##                           Ccbar2PPPiPi                                 ##
##                                                                        ##
##                             Jibo He                                    ##
##                                                                        ##
############################################################################


Ccbar2PPPiPi = {
    'BUILDERTYPE'       :  'Ccbar2PPPiPiConf',
    'CONFIG'    : {
        'HLTCuts'       : "(HLT_PASS_RE('Hlt2Topo.*Decision'))",
        'ProtonCuts'    : "(PROBNNp  > 0.1) & (PT > 300*MeV) & (TRGHOSTPROB<0.4) & (MIPCHI2DV(PRIMARY) > 9.)",
        'PionCuts'      : "(PROBNNpi > 0.2) & (PT > 250*MeV) & (TRGHOSTPROB<0.4) & (MIPCHI2DV(PRIMARY) > 9.)",
        'EtacComAMCuts' : "(AM<4.3*GeV)",
        'EtacComN4Cuts' : """
                          (AM > 2.7 *GeV)
                          & ( (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) ) > 2.5 *GeV)
                          & ( (ACHILD(MIPCHI2DV(), 1) + ACHILD(MIPCHI2DV(), 2) + ACHILD(MIPCHI2DV(), 3) + ACHILD(MIPCHI2DV(), 4))>30)
                          """,
        'EtacMomN4Cuts' : "(VFASPF(VCHI2/VDOF) < 9.) & (in_range(2.8*GeV, MM, 4.2*GeV)) & (BPVDLS>5)",        
        'Prescale'      : 1.
        },
    'STREAMS'           : ['Bhadron' ],
    'WGs'               : ['BandQ'],
    }


############################################################################
##                                                                        ##
##                     Prompt Charm                                       ##
##                                                                        ##
##              Vanya Belyaev + Marco Pappagallo                          ##
############################################################################


PromptCharm = {
    #
    'WGs'         : [ 'BandQ' , 'Charm' ] ,
    'CONFIG'      : {
    #
    # use for simulation:
    'NOPIDHADRONS'   : False   , 
    #
    ## PT-cuts
    ## attention: with 1GeV pt-cut prescale is needed for D0,D+,D*+ and Ds
    #
    'pT(D0)'     :  1.0 * GeV ,    ## pt-cut for  prompt   D0
    'pT(D+)'     :  1.0 * GeV ,    ## pt-cut for  prompt   D+
    'pT(Ds+)'    :  1.0 * GeV ,    ## pt-cut for  prompt   Ds+
    'pT(Lc+)'    :  1.0 * GeV ,    ## pt-cut for  prompt   Lc+
    'pT(Xic0)'   :  1.0 * GeV ,    ## pt-cut for  prompt   Xic0/Omegac0
    'pT(Omgcc)'  :  1.0 * GeV ,    ## pt-cut for  prompt   Omegacc
    #
    'pT(D0->HH)' :  1.0 * GeV ,    ## pt-cut for  prompt   D0->KK,pipi models 
    #
    # Selection of basic particles
    #
    'TrackCut' : """
    ( CLONEDIST   > 5000      ) &
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2  , ETA , 4.9 ) &
    HASRICH
    """ ,
    # 
    'PionCut'   : """
    ( PT          > 250 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 9        )
    """ ,
    #
    'KaonCut'   : """
    ( PT          > 250 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 9        )
    """ ,
    #
    'ProtonCut'   : """
    ( PT           > 250 * MeV ) & 
    ( CLONEDIST    > 5000      ) & 
    ( TRGHOSTPROB  < 0.5       ) & 
    in_range ( 2         , ETA , 4.9       ) &
    in_range ( 10 * GeV  , P   , 150 * GeV ) &
    HASRICH                      &
    ( MIPCHI2DV()  > 9         ) 
    """ ,
    ##
    'MuonCut'   : """
    ISMUON &
    in_range ( 2 , ETA , 4.9     ) &
    ( PT            >  550 * MeV ) &
    ( PIDmu - PIDpi >    0       ) &
    ( CLONEDIST     > 5000       )     
    """ ,
    ## cust for prompt kaon
    'PromptKaonCut'   : """
    ( CLONEDIST   > 5000         ) & 
    ( TRGHOSTPROB < 0.5          ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     
    """ ,
    #
    ## PID-cuts for hadrons 
    #
    'PionPIDCut'   : " PROBNNpi > 0.1 " ,
    'KaonPIDCut'   : " PROBNNk  > 0.1 " ,
    'ProtonPIDCut' : " PROBNNp  > 0.1 " ,
    'PhotonCLCut'  : 0.05,
    ##
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : " ( PT > 400 * MeV ) & ( CL > 0.05 ) " ,
    #
    ## W+- selection
    #
    'WCuts'           : " ( 'mu+'== ABSID ) & ( PT > 15 * GeV )" ,
    #
    # Global Event cuts
    #
    'CheckPV'         : True ,
    #
    # Technicalities:
    #
    'Preambulo'       : [
    # the D0 decay channels
    "pipi   = DECTREE ('[D0]cc -> pi- pi+   ') " ,
    "kk     = DECTREE ('[D0]cc -> K-  K+    ') " ,
    "kpi    = DECTREE ('[D0    -> K-  pi+]CC') " ,
    # number of kaons in final state (as CombinationCuts)
    "ak2    = 2 == ANUM( 'K+' == ABSID ) "       ,
    # shortcut for chi2 of vertex fit
    'chi2vx = VFASPF(VCHI2) '                    ,
    # shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" ,
    "ctau     = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau_9   = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau_16  = BPVLTIME (  16 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<16
    "ctau_25  = BPVLTIME (  25 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<25
    "ctau_100 = BPVLTIME ( 100 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<100
    "ctau_400 = BPVLTIME ( 400 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<400
    "ctau_no  = BPVLTIME (     ) * c_light "  , ## no embedded cut for chi2(lifetimeFit)
    # dimuons:
    "psi           =   ADAMASS ('J/psi(1S)') < 150 * MeV"  ,
    "psi_prime     =   ADAMASS (  'psi(2S)') < 150 * MeV"  ,
    ] ,
    # Q Values:
    'QvalueXiCK'     :  600 * MeV ,
    'QvalueXiCprime' :  250 * MeV ,
    'QvalueXiCstar'  :  150 * MeV ,
    'QvalueXiCPiK'   :  600 * MeV ,
    'QvalueXiCprimeK':  600 * MeV ,
    'QvalueLcPiK'    :  700 * MeV ,
    'QvalueOmegaCC'  : 4500 * MeV ,
    ## monitoring ?
    'Monitor'     : False ,
    ## pescales
    'D0Prescale'               : 1.0 ,
    'D*Prescale'               : 1.0 ,
    'DsPrescale'               : 1.0 ,
    'D+Prescale'               : 1.0 ,
    'LambdaCPrescale'          : 1.0 ,
    'XiC0Prescale'             : 1.0 ,
    'OmegaC0Prescale'          : 1.0 ,
    'LambdaCpKKPrescale'       : 1.0 ,
    'LambdaC*Prescale'         : 1.0 ,
    'OmegaC*Prescale'          : 1.0 ,
    'XiCprimePrescale'         : 1.0 ,
    'XiC*Prescale'             : 1.0 ,
    'OmegaC*2XiCPiKPrescale'   : 1.0 ,
    'OmegaC*2XiCprimeKPrescale': 1.0 ,
    'XiC**2LcPiKPrescale'      : 1.0 ,
    #'OmegaCCPrescale'          : 1.0 ,
    'SigmaCPrescale'           : 1.0 ,
    'Xic02LcPiPrescale'        : 1.0 ,
    #
    'OmegaCC2XiCKpiPrescale'       : 1.0 ,
    'OmegaCC2XiCKKPrescale'        : 1.0 ,
    ##
    'D02KKPrescale'            : 1.0 ,
    'D02pipiPrescale'          : 1.0 ,
    'D*CPPrescale'             : 1.0 ,
    ##
    'DiCharmPrescale'          : 1.0 ,
    'DiMu&CharmPrescale'       : 1.0 ,
    'DoubleDiMuPrescale'       : 1.0 ,
    'Chi&CharmPrescale'        : 1.0 ,
    'Charm&WPrescale'          : 1.0 ,
    'DiMuon&WPrescale'         : 1.0 ,
    'Chi&WPrescale'            : 1.0 
    },
    'BUILDERTYPE' :   'StrippingPromptCharmConf'            ,
    'STREAMS'     : { 'Charm'    : [ #'StrippingD02KpiForPromptCharm'              , 
                                     #'StrippingDstarForPromptCharm'               , 
                                     #'StrippingDForPromptCharm'                   , 
                                     #'StrippingDsForPromptCharm'                  ,
                                     #'StrippingLambdaCForPromptCharm'             ,
                                     #'StrippingXiC0ForPromptCharm'                ,
#                                    # 'StrippingOmegaC0ForPromptCharm'             ,
                                     #'StrippingLambdaC2pKKForPromptCharm'         ,
                                     #'StrippingSigmaCForPromptCharm'              ,
                                     #'StrippingLambdaCstarForPromptCharm'         ,
                                     'StrippingOmegaCstarForPromptCharm'          ,
                                     'StrippingXiCprimeForPromptCharm'            ,
                                     'StrippingXiCstarForPromptCharm'             ,
                                     'StrippingOmegaCstar2XiCPiKForPromptCharm'   ,
                                     'StrippingOmegaCstar2XiCprimeKForPromptCharm',
                                     'StrippingXiCstarstar2LambdaCPiKForPromptCharm',
                                     'StrippingOmegaCC2XiCKpiForPromptCharm'      ,
                                     'StrippingOmegaCC2XiCKKForPromptCharm'       ,
                                     'StrippingXic02LcPiForPromptCharm'           #,
                                     #'StrippingDiCharmForPromptCharm'             , ## ? 
                                     #'StrippingChiAndCharmForPromptCharm'         ,
                                     #'StrippingCharmAndWForPromptCharm'           ,
                                     #'StrippingDiMuonAndCharmForPromptCharm'      ,
                                     ### for Eva
                                     #'StrippingD02KKForPromptCharm'               ,   ## prescale ?
                                     #'StrippingD02pipiForPromptCharm'             ,   ## prescale ?
                                     #'StrippingDstarCPForPromptCharm'
                                     ] , 
                      ##
                      'CharmCompleteEvent' : [ 'StrippingOmegaC0ForPromptCharm']#,
                      #'Leptonic' : [ 'StrippingDoubleDiMuonForPromptCharm'   , ## Full DST ?
                      #               'StrippingDiMuonAndWForPromptCharm'     , ## Full DST ? 
                      #               'StrippingChiAndWForPromptCharm'        ]
                      }
    }

###############################################################################
##                                                                           ##
##                        Omegab2XixKpi                                      ##
##                                                                           ##
##                       Marco Pappagallo                                    ##
##                                                                           ##
###############################################################################

Omegab2XicKpi= {
    'BUILDERTYPE'       : 'Omegab2XicKpiConf',
    'CONFIG'    : {
                   # Omegab
                    'MinOmegabMass'      : 5500.     # MeV
                   ,'MaxOmegabMass'      : 6700.     # MeV
                   ,'Omegab_PT'          : 3500.     # MeV 
#                   (ASUM(SUMTREE(PT,(ISBASIC | (ID=='gamma')),0.0))>5000*MeV)
                   ,'OmegabVertChi2DOF'  : 10        # Dimensionless 
                   ,'OmegabLT'           : 0.2       # ps
                   ,'OmegabIPCHI2'       : 25        # Dimensionless
                   ,'OmegabDIRA'         : 0.999     # Dimensionless
                   ,'KaonProbNN'          : 0.1       # Dimensionless
                   ,'PionProbNN'          : 0.1       # Dimensionless
                   ,'ProtProbNN'          : 0.1       # Dimensionless
                   ,'XicPT'               : 1800      # MeV
                   ,'XicBPVVDCHI2'        : 36
                   ,'XicDIRA'             : 0.        # Dimensionless
                   ,'XicDeltaMassWin'     : 100       # MeV
                   ,'MaxXicVertChi2DOF'   : 10        # Dimensionless
                   # Pre- and postscales
                   ,'Omegab2XicKpiPreScale'               : 1.0
                   ,'Omegab2XicKpiPostScale'              : 1.0
                  },
    'STREAMS' : [ 'Bhadron' ],
    'WGs'    : [ 'BandQ' ]
    }
